

/***********************************************************************************************
created: 		2025-04-26

author:			chensong

purpose:		GOPMGR






��Ӯ����Ҫ���𰸶�������ʲô�������Ҫ��

�����ߣ��ٴ�֮����Ҳ��Ψ�з������ܣ�����������ʱ����ʱ����Ӣ�ۣ�Ӣ�۴�����ʱ�����������˵�����񣬿��㱾�����ٰ��� �����ã��Լ�������ͼ��顣


�ҿ��ܻ������ܶ���ˣ������ǽ���2��Ĺ��£�����д�ɹ��»��ɸ裬����ѧ���ĸ���������������
Ȼ�󻹿�����һ����������һ�������ҵĹ�������ܻᱻ��˧����������ֻᱻ��������ں���������ĳ�������ҹ������ȫ����͸Ȼ��Ҫ������С��ס�������ϵ�ʪ�·���
3Сʱ���������������ʵ��������ҵĹ�������Ը���򸸻���顣�Ҳ����Ѹ��������Ǵ�Խ�����������ǣ���ʼ��Ҫ�ص��Լ������硣
Ȼ���ҵ���Ӱ������ʧ���ҿ������������ˮ������ȴû���κ�ʹ�࣬�Ҳ�֪����ԭ���ҵ��ı������ˣ�������ȫ�����ԭ�򣬾���Ҫ�һ��Լ��ı��ġ�
�����ҿ�ʼ����Ѱ�Ҹ��ָ���ʧȥ�ĵ��ˣ��ұ��һ��שͷ��һ������һ��ˮ��һ����ƣ�ȥ�����Ϊʲô��ʧȥ�Լ��ı��ġ�
�ҷ��֣��ճ����ı��������Ļ��ڣ������ģ����ǵı��ľͻ���ʧ���յ��˸��ֺڰ�֮�����ʴ��
��һ�����ۣ������ʺͱ��ߣ�����ί����ʹ�࣬�ҿ���һֻֻ���ε��֣������ǵı��ĳ��飬�ɱΣ�͵�ߣ���Ҳ�ز������˶���ߡ�
�ҽ����������֡��������Ǻ�����ͬ�ڵļ��� �����Ҳ������£�����ϸ�����Լ�ƽ����һ�� Ѱ�ұ������ֵĺۼ���
�����Լ��Ļ��䣬һ�����ĳ�����������������֣��ҵı��ģ�����д�����ʱ�򣬻������
��������Ȼ����������ҵ�һ�У�д��������ұ��Ļع����÷�ʽ���һ�û�ҵ��������֣��������ţ�˳�������������һ����˳�����ϣ�������������
************************************************************************************************/
#include "user/user.h"
#include "rtc_base/time_utils.h"
#include "server/connection.h"
#include "server/session.h"
#include "server/connection.h"
namespace gb_media_server
{ 
		User::User(const std::shared_ptr<Connection>& ptr, const std::shared_ptr<Stream> & stream, const std::shared_ptr<Session> &s)
			: stream_(stream), connection_(ptr), session_(s)
		{

			start_timestamp_ = rtc::TimeMillis(); // tmms::base::TTime::NowMS();
			user_id_ = "1892.2332.23.23";// ptr->LocalAddr().ToString();// ptr->PeerAddr().ToIpPort();
		}
		 
		const std::string & User::AppName() const
		{
			return app_name_;
		}
		void User::SetAppName(const std::string & app_name)
		{
			app_name_ = app_name;
		}
		const std::string & User::StreamName() const
		{
			return stream_name_;
		}
		void User::SetStreamName(const std::string & stream)
		{
			stream_name_ = stream;
		}
		const std::string & User::Param() const
		{
			return param_;
		}
		void User::SetParam(const std::string & param)
		{
			param_ = param;
		}


 


		UserType  User::GetUserType() const
		{
			return type_;
		}
		void  User::SetUserType(UserType t)
		{
			type_ = t;
		}
		UserProtocol  User::GetUserProtocol() const
		{
			return protocol_;
		}
		void  User::SetUserProtocol(UserProtocol p)
		{
			protocol_ = p;
		}


		void User::Close()
		{
			if (connection_)
			{
				connection_->ForceClose();
			}
		}
		std::shared_ptr<Connection> User::GetConnection()
		{
			return connection_;
		}
		uint64_t User::ElapsedTime()
		{
			return  rtc::TimeMillis() - start_timestamp_;
		}
		void User::Active()
		{
			if (connection_)
			{
				connection_->Active();
			}
		}
		void User::Deactive()
		{
			if (connection_)
			{
				connection_->Deactive();
			}
		}
	 
}