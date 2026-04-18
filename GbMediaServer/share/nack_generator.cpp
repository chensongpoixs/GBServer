/******************************************************************************
 *  Copyright (c) 2025 The CRTC project authors . All Rights Reserved.
 *
 *  Please visit https://chensongpoixs.github.io for detail
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 ******************************************************************************/
/*****************************************************************************
 *                Author: chensong
 *                Date:   2025-11-12
 ******************************************************************************/

#include "share/nack_generator.h"

#include <algorithm>

namespace gb_media_server
{


	namespace {
		/// 丢包超过此时间不再 NACK（对齐 mediasoup MaxNackRetries * interval 语义）
		static const int64_t kMaxNackAgeMs = 1000;
		/// 同一 seq 最多 NACK 次数
		static const uint16_t kMaxNackRetries = 10;
		/// 首次 NACK 延迟：0 = 尽快发（对齐 libwebrtc NackModule2）
		static const int64_t kFirstNackDelayMs = 0;
		/// RTT=0 时的最小重发间隔，也是 batch 节流窗
		static const int64_t kMinNackIntervalMs = 20;
		/// 两次批量处理最小间隔（事件驱动节流，防每 RTP 包扫全表）
		static const int64_t kProcessIntervalMs = 20;
		/// 待补表尺寸上限，超过按最老 erase（防推流端严重乱序时无限增长）
		static const size_t  kMaxPendingSeq = 1000;
		/// seq 回退超过此值视为流重启，重置 last_seq_
		static const int64_t kMaxReorder = 1000;
		/// 默认 RTT
		static const int64_t kDefaultRttMs = 100;
	}
	// ---------------------------------------------------------------------------
	// Unwrap：把 16 位 seq 延展到 64 位单调空间
	// 与 libwebrtc SequenceNumberUnwrapper 思想一致：
	//   diff = (int16_t)(seq - last_seq_raw_)   // -32768 ~ +32767
	//   unwrap = last_unwrap + diff
	// 首次调用需单独初始化，由外层 OnReceivedPacket 保证。
	// ---------------------------------------------------------------------------
	int64_t NackGenerator::Unwrap(uint16_t seq)
	{
		if (!initialized_) {
			// 未初始化时不应调用；防御性返回
			return static_cast<int64_t>(seq);
		}
		const int16_t diff = static_cast<int16_t>(seq - last_seq_raw_);
		return last_seq_unwrap_ + diff;
	}

	void NackGenerator::AddMissingRange(int64_t from_unwrap_exclusive,
	                                    int64_t to_unwrap_exclusive,
	                                    int64_t now_ms)
	{
		// 填充 (from, to) 开区间；典型场景：上次收到 A，现在收到 A+5 → 填 A+1..A+4
		for (int64_t s = from_unwrap_exclusive + 1; s < to_unwrap_exclusive; ++s) {
			const uint16_t key = static_cast<uint16_t>(s & 0xFFFF);
			// 若已存在（例如回绕重访同 key），保留原条目不重置 first_missed_at_ms
			if (nack_list_.find(key) == nack_list_.end()) {
				NackEntry e;
				e.first_missed_at_ms = now_ms;
				e.send_at_ms         = 0;
				e.send_count         = 0;
				nack_list_.emplace(key, e);
			}
		}
		TrimBySize();
	}

	void NackGenerator::TrimBySize()
	{
		while (nack_list_.size() > kMaxPendingSeq) {
			// 线性扫描找最老 first_missed_at_ms（典型 size ≤ 1000，O(N) 可接受）
			auto oldest = nack_list_.begin();
			for (auto it = std::next(nack_list_.begin()); it != nack_list_.end(); ++it) {
				if (it->second.first_missed_at_ms < oldest->second.first_missed_at_ms) {
					oldest = it;
				}
			}
			nack_list_.erase(oldest);
		}
	}

	void NackGenerator::OnReceivedPacket(uint16_t seq, int64_t now_ms)
	{
		if (!initialized_) {
			initialized_      = true;
			last_seq_raw_     = seq;
			last_seq_unwrap_  = static_cast<int64_t>(seq);
			return;
		}

		const int64_t unwrapped = Unwrap(seq);

		if (unwrapped == last_seq_unwrap_) {
			// 重复包，忽略
			return;
		}

		if (unwrapped > last_seq_unwrap_) {
			// 有 gap 则填充待补表
			AddMissingRange(last_seq_unwrap_, unwrapped, now_ms);
			last_seq_unwrap_ = unwrapped;
			last_seq_raw_    = seq;
			return;
		}

		// unwrapped < last：乱序/重传到达，或推流端重启
		const int64_t reorder = last_seq_unwrap_ - unwrapped;
		if (reorder > kMaxReorder) {
			// 视为流重启，重置
			nack_list_.clear();
			last_seq_raw_    = seq;
			last_seq_unwrap_ = static_cast<int64_t>(seq);
			return;
		}

		// 正常乱序：从待补表 erase（迟到但到达）
		nack_list_.erase(seq);
	}

	std::vector<uint16_t> NackGenerator::GetNackBatch(int64_t now_ms)
	{
		std::vector<uint16_t> result;
		if (nack_list_.empty()) {
			last_process_at_ms_ = now_ms;
			return result;
		}
		// 节流：相邻处理间隔 < kProcessIntervalMs 直接返回空
		if (last_process_at_ms_ != 0
		    && (now_ms - last_process_at_ms_) < kProcessIntervalMs) {
			return result;
		}
		last_process_at_ms_ = now_ms;

		const int64_t rtt_for_cooldown = std::max<int64_t>(rtt_ms_, kMinNackIntervalMs);

		for (auto it = nack_list_.begin(); it != nack_list_.end(); /* inc in body */) {
			NackEntry& e = it->second;
			const int64_t age = now_ms - e.first_missed_at_ms;

			// 规则 1：超龄放弃
			if (age > kMaxNackAgeMs) {
				it = nack_list_.erase(it);
				continue;
			}
			// 规则 2：重传次数上限放弃
			if (e.send_count >= kMaxNackRetries) {
				it = nack_list_.erase(it);
				continue;
			}

			if (e.send_at_ms == 0) {
				// 首次：是否过了 first_nack_delay
				if (age < kFirstNackDelayMs) {
					++it;
					continue;
				}
			} else {
				// 非首次：冷却判断
				if ((now_ms - e.send_at_ms) < rtt_for_cooldown) {
					++it;
					continue;
				}
			}

			// 通过 → 加入 batch 并更新
			result.push_back(it->first);
			e.send_at_ms   = now_ms;
			e.send_count  += 1;
			++it;
		}
		return result;
	}

	void NackGenerator::SetRtt(int64_t rtt_ms)
	{
		if (rtt_ms <= 0) {
			rtt_ms_ = kDefaultRttMs;
		} else {
			rtt_ms_ = rtt_ms;
		}
	}

	void NackGenerator::Reset()
	{
		nack_list_.clear();
		last_seq_unwrap_    = -1;
		last_seq_raw_       = 0;
		initialized_        = false;
		last_process_at_ms_ = 0;
	}

} // namespace gb_media_server
