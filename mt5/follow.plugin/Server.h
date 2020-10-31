#if !defined(AFX_SKSTU_SERVER_H__3F7C1_A8486F84_5D73FAF5A796250_B1C8__HEAD__)
#define AFX_SKSTU_SERVER_H__3F7C1_A8486F84_5D73FAF5A796250_B1C8__HEAD__

namespace follow {

	class Server final : public sk::object::IObject, public sk::network::INetworkSpi {
	public:
		Server();
		~Server();
	private:
		int Init() override final;
		int UnInit() override final;
	public:
		int Open() override final;
		int Close() override final;
	private:
		void ProcessT();
		
		//!ǩ�� : ProcessRuntimeNetCMDT
		//!˵�� : ����ʱ����������߳�
		//!ע�� : �ظ�����/����ռ��
		//!���� : Fri May 29 08:51:04 UTC+0800 2020
		void ProcessRuntimeNetCMDT();
	public:
		sk::container::map<shared::SystemID, IServiceObj*> m_NetClientQ;
	protected:
		void OnTcpServerLogon(sk::network::INetworkContext*) override final;
		void OnTcpServerLogout(sk::network::INetworkContext*) override final;
		void OnTcpServerRead(sk::network::INetworkContext*, const sk::network::Header&, const sk::packet&) override final;
	private:
		void OnNetLoginProcess(sk::network::INetworkContext*, const sk::network::Header&, const sk::packet&);
		void OnNetSystemSetupProcess(sk::network::INetworkContext*, const sk::network::Header&, const sk::packet&);
		void OnNetFollowconRequest(sk::network::INetworkContext*, const sk::network::Header&, const sk::packet&);
		void OnNetFollowconUpdate(sk::network::INetworkContext*, const sk::network::Header&, const sk::packet&);
		void OnNetFollowconDelete(sk::network::INetworkContext*, const sk::network::Header&, const sk::packet&);
		void OnNetMTUsersRequest(sk::network::INetworkContext*, const sk::network::Header&, const sk::packet&);
		void OnNetMTSymbolsRequest(sk::network::INetworkContext*, const sk::network::Header&, const sk::packet&);
		void OnNetTradeRecordRequest(sk::network::INetworkContext*, const sk::network::Header&, const sk::packet&);
		void OnNetPlaformPatchPositionsProcess(sk::network::INetworkContext*, const sk::network::Header&, const sk::packet&);
		void OnNetServerTimeRequest(sk::network::INetworkContext*, const sk::network::Header&, const sk::packet&);
		void OnNetTradeRecordHeadProcess(sk::network::INetworkContext*, const sk::network::Header&, const sk::packet&);
		void OnNetTradeRecordUpdateProcess(sk::network::INetworkContext*, const sk::network::Header&, const sk::packet&);
		void OnNetQuickFunctionSwitchFollow(sk::network::INetworkContext*, const sk::network::Header&, const sk::packet&);
		void OnNetQuickFunctionSwitchPatch(sk::network::INetworkContext*, const sk::network::Header&, const sk::packet&);
		void OnNetQuickFunctionSwitchSLTP(sk::network::INetworkContext*, const sk::network::Header&, const sk::packet&);
	};



}///namespace follow




/// /*�����������Ϻ���**/
/// /*2020��01��16�� 10:37:02|301**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///AFX_SKSTU_SERVER_H__3F7C1_A8486F84_5D73FAF5A796250_B1C8__TAIL__