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
 *
 * Producer 端 NACK 生成器：
 *   - 服务端作为 WebRTC 接收方时，检测推流端的丢包并产生 NACK 列表
 *   - 参考 libwebrtc `NackModule2` 与 mediasoup `NackGenerator`
 *   - 纯算法类，不含线程与 IO：调用方负责把它放到单一线程（worker_thread）执行
 *
 ******************************************************************************/

#ifndef _C_GB_MEDIA_SERVER_NACK_GENERATOR_H_
#define _C_GB_MEDIA_SERVER_NACK_GENERATOR_H_

#include <cstdint>
#include <map>
#include <vector>
#include <iostream>
#include <stdbool.h>
#include <cstdlib>

namespace gb_media_server
{
	
	/**
	 * @brief NACK 生成器（单 SSRC）。
	 *
	 * **线程模型（重要）**：本类**不**加内部锁，所有方法必须在**同一线程**调用
	 * （项目中固定为 RtcService 的 worker_thread）。OnReceivedPacket、GetNackBatch、
	 * SetRtt 都在 worker_thread 串行执行。禁止跨线程直接访问成员。
	 *
	 * 典型用法：
	 *   worker_thread_->PostTask([this, seq, now_ms] {
	 *       video_nack_generator_->OnReceivedPacket(seq, now_ms);
	 *       auto batch = video_nack_generator_->GetNackBatch(now_ms);
	 *       if (!batch.empty()) { // 构造 rtcp::Nack 并 SendSrtpRtcp }
	 *   });
	 */
	class NackGenerator
	{
	public:
	

		NackGenerator() = default;
		~NackGenerator() = default;

		NackGenerator(const NackGenerator&) = delete;
		NackGenerator& operator=(const NackGenerator&) = delete;

		/**
		 * @brief 新 RTP 到达时调用：更新 last_seq_ 与 nack_list_
		 *
		 * @param seq RTP 16 位序号（保留原值，内部负责 unwrap）
		 * @param now_ms 当前单调时间（rtc::TimeMillis()）
		 */
		void OnReceivedPacket(uint16_t seq, int64_t now_ms);

		/**
		 * @brief 取出本次待发的 NACK seq 列表
		 *
		 * 决策规则（对齐 mediasoup / libwebrtc）：
		 *   - age > kMaxNackAgeMs      → 放弃该 seq
		 *   - send_count >= kMaxNackRetries → 放弃
		 *   - 首次未到 first_nack_delay → 跳过
		 *   - 非首次且距上次 NACK < max(rtt, kMinNackIntervalMs) → 跳过（冷却）
		 *   - 通过 → 纳入返回列表，send_at_ms=now, send_count++
		 *
		 * 批量节流：两次 GetNackBatch 间隔 < kProcessIntervalMs 时直接返回空。
		 *
		 * @param now_ms 当前单调时间
		 * @return 待 NACK 的 seq 列表（16 位），空表示无
		 */
		std::vector<uint16_t> GetNackBatch(int64_t now_ms);

		/**
		 * @brief 更新 RTT（供冷却窗使用）。0 值会被 clamp 到 kDefaultRttMs。
		 */
		void SetRtt(int64_t rtt_ms);

		/// 当前待补表大小（监控/日志用）
		size_t PendingSize() const { return nack_list_.size(); }

		/// 获取最近一次收到的 seq（16 位截取，单测/日志用）
		uint16_t LastSeq() const
		{
			return initialized_ ? static_cast<uint16_t>(last_seq_unwrap_ & 0xFFFF) : 0;
		}

		/// 重置状态（流重启或 producer 析构前调用）
		void Reset();

	private:
		struct NackEntry {
			int64_t  first_missed_at_ms = 0;
			int64_t  send_at_ms         = 0;   // 0 = 尚未 NACK 过
			uint16_t send_count         = 0;
		};

		/// 把 16 位 seq 展开到 64 位单调空间（保留相对顺序）
		int64_t Unwrap(uint16_t seq);

		/// 增加区间 (from_unwrap+1, to_unwrap-1] ∩ [from_unwrap+1, to_unwrap) 的 seq 到 nack_list_
		void AddMissingRange(int64_t from_unwrap_exclusive,
		                     int64_t to_unwrap_exclusive,
		                     int64_t now_ms);

		/// 待补表尺寸溢出时，按 first_missed_at_ms 最老 erase
		void TrimBySize();

		std::map<uint16_t, NackEntry>  nack_list_;    ///< 待补 seq → 元数据
		int64_t  last_seq_unwrap_     = -1;           ///< 已收到的最大 unwrap seq
		uint16_t last_seq_raw_        = 0;            ///< 辅助 unwrap 的上一次 16 位值
		bool     initialized_         = false;
		int64_t  rtt_ms_ = 100;// = kDefaultRttMs;
		int64_t  last_process_at_ms_  = 0;            ///< 上次 GetNackBatch 时间（节流）
	};

} // namespace gb_media_server

#endif // _C_GB_MEDIA_SERVER_NACK_GENERATOR_H_
