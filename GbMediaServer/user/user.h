

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

#ifndef _C_GB_MEDIA_SERVER_USER_H_
#define _C_GB_MEDIA_SERVER_USER_H_



#include <cstdint>
#include <memory>   
#include <vector>  
#include <string>

#include <atomic>

#include "rtc_base/copy_on_write_buffer.h"



namespace gb_media_server
{
	 

		enum  UserType
		{
			kUserTypePublishRtmp = 0,
			kuserTypePublishMpegts,
			kUserTypePublishPav,
			kUserTypePublishWebRtc,
			kUserTypePublishGB28181,
			//kUserTypePublishPav,
			kUserTypePlayerPav,
			kUserTypePlayerFlv,
			kUserTypePlayerHls,
			kUserTypePlayerRtmp,
			kUserTypePlayerWebRTC,
			kUserTypeUnknowed = 255,


		};


		enum  UserProtocol
		{
			kUserProtocolHttp = 0,
			kUserProtocolHttps,
			kUserProtocolQuic,
			kUserProtocolRtsp,
			kUserProtocolWebRTC,
			kUserProtocolUdp,
			kUserProtocolUnknowed = 255,

		};


		 



		 
		class Stream;
		//using StreamPtr = std::shared_ptr<Stream>;
		class Session;
		class Connection;
		//using SessionPtr = std::shared_ptr<Session>;
		//using std::string;
		class User : public std::enable_shared_from_this<User>
		{
		public:
			friend class Session;
			User(const std::shared_ptr<Connection>& ptr, const std::shared_ptr<Stream> & stream, const std::shared_ptr<Session> &s);
			virtual ~User() = default;
		public:


		 
			const std::string & AppName() const;
			void SetAppName(const std::string & app_name);
			const std::string & StreamName() const;
			void SetStreamName(const std::string & stream);
			const std::string & Param() const;
			void SetParam(const std::string & param);



			 


			virtual UserType GetUserType() const;
			void SetUserType(UserType t);
			virtual UserProtocol GetUserProtocol() const;
			void SetUserProtocol(UserProtocol p);



			std::shared_ptr<Session> GetSession() const
			{
				return session_;
			}
			std::shared_ptr < Stream> GetStream() const
			{
				return stream_;
			}
			std::shared_ptr < Stream> GetStream()
			{
				return stream_;
			}
			const std::string &UserId() const
			{
				return user_id_;
			}

			//接受不同协议上层处理 实现
			virtual  void OnRecv(const rtc::CopyOnWriteBuffer&  buffer) {}
		public:
			//网络层接口
			void Close();
			std::shared_ptr<Connection> GetConnection();
			uint64_t ElapsedTime();
			void Active();
			void Deactive();
		protected:
			std::shared_ptr < Stream> stream_;
			std::shared_ptr<Connection>   connection_;
			std::string     domain_name_; // 域名
			std::string     app_name_;
			std::string     stream_name_;
			std::string     param_;
			std::string		user_id_; 
			int64_t			start_timestamp_{ 0 }; // 启始时间
			UserType		type_{ UserType::kUserTypeUnknowed };
			UserProtocol	protocol_{ UserProtocol::kUserProtocolUnknowed };
			std::atomic_bool destroyed_{ false };
			std::shared_ptr < Session> session_;
		};
 
}

#endif // 