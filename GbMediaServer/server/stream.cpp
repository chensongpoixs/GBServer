

/***********************************************************************************************
created: 		2025-04-26

author:			chensong

purpose:		GOPMGR


实时流

1. 一路实时输入的艺术品数据流
2. 服务器被动接收推流
3. 服务器自动拉流




- 处理输入音视频数据
	1. 修正时间戳， 处理index
	2. 处理CodecHeader
	3. 处理GOP
	4. 保存音视频帧

- 输出音视频数据



1. 定位GOP， 查找CodecHeader
2. 必须先发送CodecHeader， 再发送音视频数据
3. 跳帧，重新查找CodecHeader
4. 输出一定数量的音视频帧






										   流程图


			  请求音视频帧



			已经输出音视频序列头                     否定 ===>          |          定位GOP
																	|
																	|
				  是												    |
																	|
			音视频帧落后太多?                       是   ===>          |          定位GOP

				  否定                                                           ||

																			查找音视频序列头

																				 ||

																			输出音视频序列头


			输出一定数量的音视频帧




流超时


- 长时间没有收到实时数据， 记录收到数据的时间 当前时间收到数据的时间差， 超过一定的时间， 流就算超时
- 流超时是被动检测




流准备好

- 收到关键帧


输赢不重要，答案对你们有什么意义才重要。

光阴者，百代之过客也，唯有奋力奔跑，方能生风起时，是时代造英雄，英雄存在于时代。或许世人道你轻狂，可你本就年少啊。 看护好，自己的理想和激情。


我可能会遇到很多的人，听他们讲好2多的故事，我来写成故事或编成歌，用我学来的各种乐器演奏它。
然后还可能在一个国家遇到一个心仪我的姑娘，她可能会被我帅气的外表捕获，又会被我深邃的内涵吸引，在某个下雨的夜晚，她会全身淋透然后要在我狭小的住处换身上的湿衣服。
3小时候后她告诉我她其实是这个国家的公主，她愿意向父皇求婚。我不得已告诉她我是穿越而来的男主角，我始终要回到自己的世界。
然后我的身影慢慢消失，我看到她眼里的泪水，心里却没有任何痛苦，我才知道，原来我的心被丢掉了，我游历全世界的原因，就是要找回自己的本心。
于是我开始有意寻找各种各样失去心的人，我变成一块砖头，一颗树，一滴水，一朵白云，去听大家为什么会失去自己的本心。
我发现，刚出生的宝宝，本心还在，慢慢的，他们的本心就会消失，收到了各种黑暗之光的侵蚀。
从一次争论，到嫉妒和悲愤，还有委屈和痛苦，我看到一只只无形的手，把他们的本心扯碎，蒙蔽，偷走，再也回不到主人都身边。
我叫他本心猎手。他可能是和宇宙同在的级别 但是我并不害怕，我仔细回忆自己平淡的一生 寻找本心猎手的痕迹。
沿着自己的回忆，一个个的场景忽闪而过，最后发现，我的本心，在我写代码的时候，会回来。
安静，淡然，代码就是我的一切，写代码就是我本心回归的最好方式，我还没找到本心猎手，但我相信，顺着这个线索，我一定能顺藤摸瓜，把他揪出来。
************************************************************************************************/
#include "server/stream.h"
#include "server/session.h"
#include "rtc_base/time_utils.h"
 
namespace gb_media_server
{
	 

		Stream::Stream( Session &s, const std::string & session_name)
			:session_(s), session_name_(session_name) , hls_muxer_(session_name)
		{
			 
		}
		void Stream::AddVideoFrame(  libmedia_codec::EncodedImage&& frame)
		{ 
			// TODO@chensong 2025-11-17 实现 HLS的协议
			//hls_muxer_.OnPacket(packet);
			session_.AddVideoFrame(std::move(frame));
		}
		void Stream::AddAudioFrame(  rtc::CopyOnWriteBuffer&&frame, int64_t  pts)
		{
			// TODO@chensong 2025-11-17 实现 HLS的协议
			//hls_muxer_.OnPacket(packet);
			session_.AddAudioFrame(std::move(frame), pts);
		}
		 
		// 流信息函数
		const std::string & Stream::SessionName() const
		{
			return session_name_;
		}
		 
	 
}

