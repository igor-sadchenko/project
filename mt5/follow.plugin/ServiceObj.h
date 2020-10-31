#if !defined(AFX_SKSTU_Client_H__AC7C0_3E5435ED_05C802527A456C0_63AA__HEAD__)
#define AFX_SKSTU_Client_H__AC7C0_3E5435ED_05C802527A456C0_63AA__HEAD__

namespace follow {

	class IServiceObj {
	public:
		IServiceObj(sk::network::INetworkApi*, sk::network::INetworkContext*);
		~IServiceObj();
	protected:
		virtual int Write(const sk::network::EnNetCmd& cmd, const sk::packet& pak) const;
		virtual int WriteEx(const sk::network::EnNetCmd& cmd, const sk::packet& pak,const SKAPIRES&) const;
		virtual int WriteEx(const sk::network::EnNetCmd& cmd, const INT&, const SKAPIRES&) const;
	public:
		sk::network::INetworkContext* NetContext() const { return m_pContext; }
		sk::network::INetworkApi* NetApi() const { return m_pNetApi; }
		const shared::EnServiceObjectFlag& ServiceObjFlag() const { return m_ServiceObjFlag; }
	protected:
		sk::network::INetworkApi* m_pNetApi = nullptr;
		sk::network::INetworkContext* m_pContext = nullptr;
		shared::EnServiceObjectFlag m_ServiceObjFlag;
	public:
		virtual void Release() { delete this; };
		int SendLoginResult(const SKAPIRES&);
	};

	class ServiceObj final : public IServiceObj {
	public:
		ServiceObj(sk::network::INetworkApi* pNetworkApi, sk::network::INetworkContext* pNetworkContext) :
			IServiceObj(pNetworkApi, pNetworkContext)
		{
			m_ServiceObjFlag = shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_UNDEFINED;
		}
		~ServiceObj() {}
	public:
		void Release() override final { delete this; }
	};

	template<typename T>
	static T* ServiceObjType(IServiceObj* obj) { return (T*)obj; }

	class AdminObj final : public IServiceObj {
	public:
		AdminObj(sk::network::INetworkApi*, sk::network::INetworkContext*);
		~AdminObj();
	public:
		int SendTradeRecordHead(const sk::packet&) const;
		int SendTradeRecordUpdate(const sk::packet&) const;
		int SendMTUsers() const;
		int SendMTSymbols() const;
		int SendFollowconInitialize() const;
		int SendFollowconUpdate(const SKAPIRES&,const sk::packet&) const;
		int SendFollowconDelete(const SKAPIRES&, const sk::packet&) const;
		int SendPlaformPatchPositions(const SKAPIRES&) const;
		int SendServerTimeS() const;
		int SendSystemSetup(const sk::packet&) const;
		int SendQuickFunctionSwitchFollow(const UINT&, const SKAPIRES&) const;
		int SendQuickFunctionSwitchPatch(const UINT&, const SKAPIRES&) const;
		int SendQuickFunctionSwitchSLTP(const UINT&, const SKAPIRES&) const;
	public:
		void Release() override final { delete this; }
	};

	class DeveloperObj final : public IServiceObj {
	public:
		DeveloperObj(sk::network::INetworkApi*, sk::network::INetworkContext*);
		~DeveloperObj();
	public:
		void Release() override final { delete this; }
	};

	class GuestObj final : public IServiceObj {
	public:
		GuestObj(sk::network::INetworkApi*, sk::network::INetworkContext*);
		~GuestObj();
	public:
		void Release() override final { delete this; }
	};



}///namespace follow





/// /*新生联创（上海）**/
/// /*2020年01月16日 16:11:39|953**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///AFX_SKSTU_Client_H__AC7C0_3E5435ED_05C802527A456C0_63AA__TAIL__