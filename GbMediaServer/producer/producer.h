

/***********************************************************************************************
created: 		2025-04-26

author:			chensong

purpose:		GOPMGR






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

#ifndef _C_GB_MEDIA_SERVER_PRODUCER_H_
#define _C_GB_MEDIA_SERVER_PRODUCER_H_



#include <cstdint>
#include <memory>   
#include <vector>  
#include <string>

#include <atomic>

#include "rtc_base/copy_on_write_buffer.h"
#include "rtc_base/socket_address.h"
#include "libmedia_codec/encoded_image.h"
//#include "server/session.h"
#include "share/share_resource.h"
namespace gb_media_server
{
	 

		 


	 


		 

	 
		 
		 
		class Producer : public ShareResource
		{
		public: 
			Producer(  const std::shared_ptr<Stream> & stream, const std::shared_ptr<Session> &s);
			virtual ~Producer() = default;
		public:

			 

			//���ܲ�ͬЭ���ϲ㴦�� ʵ��
			//virtual  void OnRecv(const rtc::CopyOnWriteBuffer&  buffer) {}

			/**
			*  @author chensong
			*  @date 2025-04-26
			*  @brief 请求关键帧（Request Key Frame）
			*  
			*  该方法用于请求生产者发送一个关键帧（IDR帧）。当消费者需要快速恢复视频播放时，
			*  可以通过调用此方法请求一个关键帧，以便快速重建视频解码器状态。
			*  
			*  关键帧是视频编码中的重要概念：
			*  - 关键帧（I帧/IDR帧）是独立编码的帧，不需要参考其他帧即可解码
			*  - 非关键帧（P帧/B帧）需要参考其他帧才能解码
			*  - 当消费者刚开始播放或需要快速跳转时，需要从关键帧开始解码
			*  
			*  @note 该方法是虚函数，由具体的生产者实现类（如RtcProducer、RtspProducer等）重写
			*        来实现特定的关键帧请求逻辑。不同协议的关键帧请求机制可能不同。
			*  
			*  使用场景：
			*  - 新消费者加入时，需要快速获取关键帧以开始播放
			*  - 网络丢包导致解码失败时，需要请求关键帧恢复
			*  - 视频质量切换时，需要请求关键帧重新编码
			*  
			*  @note 对于某些协议（如RTC），请求关键帧是异步的，可能需要等待一段时间才能收到
			*/
			 virtual void RequestKeyFrame() {}
		


			/**
			*  stream    ��û���˹ۿ�ʱֹͣ����  ����������ʱ���Զ�������
			* @param bool: �Ƿ�������� 
			* return ����ֵ
			*/
			 virtual  void SetStreamStatus(bool status) {}
		public: 
		protected:
			 
		};
 
}

#endif // 