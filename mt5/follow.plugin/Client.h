#if !defined(AFX_SKSTU_Client_H__AC7C0_3E5435ED_05C802527A456C0_63AA__HEAD__)
#define AFX_SKSTU_Client_H__AC7C0_3E5435ED_05C802527A456C0_63AA__HEAD__

namespace follow {

	class IServiceObj {
	protected:
		IServiceObj(sk::network::INetworkApi*, sk::network::INetworkContext*);
		~IServiceObj();
	protected:
		virtual int Write(const sk::network::EnNetCmd& cmd, const sk::packet& pak) const;
	public:
		sk::network::INetworkApi* NetApi() const { return m_pNetApi; }
		const shared::EnServiceObjectFlag& ServiceObjFlag() const { return m_ServiceObjFlag; }
	protected:
		sk::network::INetworkApi* m_pNetApi = nullptr;
		sk::network::INetworkContext* m_pContext = nullptr;
		shared::EnServiceObjectFlag m_ServiceObjFlag;
	public:
		virtual void Release() = 0;
		int SendLoginResult(const shared::FollowObjLoginField&);
		int SendTradeRequestAck(const sk::packet&);
		int SendPositioncons();
	};

	template<typename T>
	static T* ServiceObjType(IServiceObj* obj) { return (T*)obj; }

	class AdminObj final : public IServiceObj {
	public:
		AdminObj(sk::network::INetworkApi*, sk::network::INetworkContext*);
		~AdminObj();
	public:
		int SendTradeRecordInit(const sk::packet& pak);
		int SendTradeRecordUpdate(const sk::packet& pak);
		int SendMTUsers();
		int SendMTSymbols();
		int SendFollowcons();	
		int SendFollowconCreateAck(const sk::packet&);
		int SendFollowconDeleteAck(const sk::packet&);
		int SendPlaformPatchPositionsAck(const sk::packet&);
		int SendPlaformPatchPositionsResult(const sk::packet&);
	public:
		void Release() override final { delete this; }
	};

	class DeveloperObj final : public IServiceObj {
	public:
		DeveloperObj(sk::network::INetworkApi*, sk::network::INetworkContext*);
		~DeveloperObj();
	public:
		int SendOnMsgMT(const sk::packet&);
		int SendRuntimLog(const sk::packet&);
	public:
		void Release() override final { delete this; }
	};

	class GuestObj final : public IServiceObj {
	public:
		GuestObj(sk::network::INetworkApi*, sk::network::INetworkContext*);
		~GuestObj();
	public:
		int SendOnMsgMT(const sk::packet&);
		int SendRuntimLog(const sk::packet&);
	public:
		void Release() override final { delete this; }
	};



}///namespace follow





/// /*新生联创（上海）**/
/// /*2020年01月16日 16:11:39|953**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///AFX_SKSTU_Client_H__AC7C0_3E5435ED_05C802527A456C0_63AA__TAIL__