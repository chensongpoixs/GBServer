 /*****************************************************************************
 *  Copyright (c) 2025 The CRTC project authors . All Rights Reserved.
 *
 *  Please visit https://chensongpoixs.github.io for detail
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 *
 *
 *
 *				   Author: chensong
 *				   date:  2025-10-14
 *
 * 输赢不重要，答案对你们有什么意义才重要。
 *
 * 光阴者，百代之过客也，唯有奋力奔跑，方能生风起时，是时代造英雄，英雄存在于时代。或许世人道你轻狂，可你本就年少啊。 看护好，自己的理想和激情。
 *
 *
 *我可能会遇到很多的人，听他们讲好2多的故事，我来写成故事或编成歌，用我学来的各种乐器演奏它。
 *然后还可能在一个国家遇到一个心仪我的姑娘，她可能会被我帅气的外表捕获，又会被我深邃的内涵吸引，在某个下雨的夜晚，她会全身淋透然后要在我狭小的住处换身上的湿衣服。
 *3小时候后她告诉我她其实是这个国家的公主，她愿意向父皇求婚。我不得已告诉她我是穿越而来的男主角，我始终要回到自己的世界。
 *然后我的身影慢慢消失，我看到她眼里的泪水，心里却没有任何痛苦，我才知道，原来我的心被丢掉了，我游历全世界的原因，就是要找回自己的本心。
 *于是我开始有意寻找各种各样失去心的人，我变成一块砖头，一颗树，一滴水，一朵白云，去听大家为什么会失去自己的本心。
 *我发现，刚出生的宝宝，本心还在，慢慢的，他们的本心就会消失，收到了各种黑暗之光的侵蚀。
 *从一次争论，到嫉妒和悲愤，还有委屈和痛苦，我看到一只只无形的手，把他们的本心扯碎，蒙蔽，偷走，再也回不到主人都身边。
 *我叫他本心猎手。他可能是和宇宙同在的级别 但是我并不害怕，我仔细回忆自己平淡的一生 寻找本心猎手的痕迹。
 *沿着自己的回忆，一个个的场景忽闪而过，最后发现，我的本心，在我写代码的时候，会回来。
 *安静，淡然，代码就是我的一切，写代码就是我本心回归的最好方式，我还没找到本心猎手，但我相信，顺着这个线索，我一定能顺藤摸瓜，把他揪出来。
 *
 *******************************************************************************/
#ifndef _C_RTC_SERVICE_MGR_H_
#define _C_RTC_SERVICE_MGR_H_

#include "server/session.h"
#include "server/stream.h"
#include "libmedia_transfer_protocol/librtc/rtc_server.h"
#include "libmedia_transfer_protocol/libhttp/http_server.h"
#include "producer/gb28181_producer.h"
#include "consumer/rtc_consumer.h"
//#include "swagger/dto/RtcApiDto.hpp"
#include "share/rtc_interface.h"

namespace gb_media_server
{

	/**
	*  @author chensong
	*  @date 2025-10-14
	*  @brief RTC 服务管理器（RtcService Manager）
	*
	*  RtcServiceMgr 是 GBMediaServer 中 **多路 RTC 工作线程/监听端口** 的集中入口，采用 **Meyers 单例**，
	*  与 `RtcService`（单路 UDP 收发与 RtcInterface 路由）配合使用：本类持有 `num_workers` 个 `RtcService` 实例，
	*  在 `init()` 中按配置创建，在 `startup()` 中为每个实例绑定 **递增 UDP 端口**（`udp_port + i`），
	*  供推流/拉流会话按索引选取，实现 **多端口并行** 分担 ICE/DTLS/RTP 负载。
	*
	*  职责概要：
	*  1. 从 `YamlConfig::GetRtcServerConfig()` 读取 `num_workers`、`udp_port` 等；
	*  2. `init()`：构造 `num_workers` 个 `RtcService` 并放入 `rtc_services_`；
	*  3. `startup()`：依次对每个 `RtcService` 调用 `Startup(udp_port + i)`，启动底层 `RtcServer` 监听；
	*  4. `GetRtcService(index)`：按会话或哈希索引选取一路 `RtcService`（下标对 `size()` 取模，便于负载分散）。
	*
	*  典型调用顺序（进程启动）：
	*  @code
	*  RtcServiceMgr::GetInstance().init();
	*  RtcServiceMgr::GetInstance().startup();
	*  // 会话创建后：
	*  RtcService* svc = RtcServiceMgr::GetInstance().GetRtcService(session_index);
	*  @endcode
	*
	*  @note 与 `RtcService` 单例不同：本管理器是 **多实例** 容器；`GetRtcService` 返回的指针在进程存活期内有效。
	*  @note `destroy()` 当前为空实现，若后续需要优雅停机，应在此停止各 `RtcService` 并释放资源。
	*  @see RtcService
	*  @see YamlConfig::GetRtcServerConfig()
	*/
	class RtcServiceMgr
	{
	public:
		/**
		*  @author chensong
		*  @date 2025-10-14
		*  @brief 构造函数（Constructor）
		*
		*  初始化空的 `rtc_services_` 向量，不创建 `RtcService`；实际 worker 数量在 `init()` 中按配置分配。
		*/
		explicit RtcServiceMgr();

		/**
		*  @author chensong
		*  @date 2025-10-14
		*  @brief 析构函数（Destructor）
		*
		*  释放管理器对象；当前未在析构中显式停止各 `RtcService`，与 `destroy()` 行为一致，依赖进程退出清理。
		*/
		virtual ~RtcServiceMgr();

		/**
		*  @author chensong
		*  @date 2025-10-14
		*  @brief 获取全局唯一管理器实例（Meyers Singleton）
		*
		*  @return RtcServiceMgr& 进程内单例引用
		*/
		static RtcServiceMgr& GetInstance()
		{
			static RtcServiceMgr instance;
			return instance;
		}

		/**
		*  @author chensong
		*  @date 2025-10-14
		*  @brief 初始化 RTC 工作池（init）
		*
		*  根据配置 `num_workers` 创建对应数量的 `RtcService`，并加入 `rtc_services_`。
		*  此时仅完成对象构造，**尚未**绑定端口或启动网络线程，需再调用 `startup()`。
		*
		*  @return true 表示分配成功；当前实现恒为 true
		*/
		bool init();

		/**
		*  @author chensong
		*  @date 2025-10-14
		*  @brief 启动所有 RTC 服务监听（startup）
		*
		*  对 `rtc_services_[i]` 调用 `Startup(udp_port + i)`，使第 i 路服务监听独立 UDP 端口。
		*  若某路启动失败会打 WARNING 日志，但函数仍返回 true（与现有实现一致）。
		*
		*  @return true 表示流程走完；单路失败请查看日志
		*/
		bool startup();

		/**
		*  @author chensong
		*  @date 2025-10-14
		*  @brief 销毁/停机占位（destroy）
		*
		*  预留接口：用于将来统一停止各 `RtcService`、释放端口与线程。当前为空实现。
		*/
		void destroy();

		/**
		*  @author chensong
		*  @date 2025-10-14
		*  @brief 按索引选取一路 RtcService（GetRtcService）
		*
		*  使用 `index % rtc_services_.size()` 将任意无符号索引映射到有效下标，便于会话 ID、哈希或轮询分散到多路 worker。
		*
		*  @param index 逻辑索引（可与 Session 编号、哈希等相关联）
		*  @return RtcService* 非空指针；若 `rtc_services_` 为空则取模未定义（配置应保证 `num_workers >= 1`）
		*/
		RtcService* GetRtcService(uint32_t index);

	private:
		/// 多路 RTC 服务实例，下标 i 对应 UDP 端口 `udp_port + i`（由 startup 传入）
		std::vector<std::unique_ptr<RtcService>> rtc_services_;
	};

}  // namespace gb_media_server

#endif  // _C_RTC_SERVICE_MGR_H_
