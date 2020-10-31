#if !defined(AFX_SKSTU_SERVICEOBJ_H__89F14_11BB24A6_1EA1D66A5C8AEB9_E4C6__HEAD__)
#define AFX_SKSTU_SERVICEOBJ_H__89F14_11BB24A6_1EA1D66A5C8AEB9_E4C6__HEAD__


namespace proxy {

	class IServiceObj {
	protected:
		IServiceObj(sk::network::INetworkApi*, sk::network::INetworkContext*);
		~IServiceObj();
	protected:
		virtual int Write(const sk::network::EnNetCmd&, const sk::packet&) const;
		virtual int WriteEx(const sk::network::EnNetCmd&, const sk::packet&,const sk::network::Header::ExternData&, const SKAPIRES&) const;
		virtual int WriteEx(const sk::network::EnNetCmd&, const sk::packet&, const sk::network::HEADER::EnDataProcessFlag&, const sk::network::Header::ExternData&) const;
	public:
		const shared::EnServiceObjectFlag& ServiceObjFlagGet() const { return m_ServiceObjFlag; }
		sk::network::INetworkApi* NetApi() const { return m_pNetApi; }
		virtual int SendLoginResult(const SKAPIRES&) const;
		virtual int SendShutdown() const;
		virtual int SendShowWindow(const bool&) const;
		virtual int SendSystemTime() const;
		virtual int SendBridgeSystemSetup(const shared::BridgeSystemSetup&) const;
		virtual int SendMarketDataUpdate(const sk::packet&) const;
		virtual int SendMarketDataCache(const sk::packet&) const;
		virtual int SendMTUsers(const sk::network::HEADER::EnDataProcessFlag& ,const sk::packet&) const;
		virtual int SendMTSymbols(const sk::packet&) const;
		virtual int SendSystemAssignLoginsPostOrder() const;
		virtual int SendSystemAssignLoginsPostOrder(const shared::SystemID&/*ExchangeID*/, const sk::network::HEADER::EnDataProcessFlag&, const sk::packet&/*logins*/) const;
	public:
		virtual int ProductsRequest() const;
	protected:
		shared::EnServiceObjectFlag m_ServiceObjFlag;
		std::string m_Address;
		std::atomic_bool m_Enable = false;
		std::atomic<time_t> m_ActiveTime = sk::Helper::TickCountGet<std::chrono::milliseconds>();
	protected:
		sk::network::INetworkApi* m_pNetApi = nullptr;
		sk::network::INetworkContext* m_pContext = nullptr;
	public:
		virtual void Release() = 0;
		shared::SystemID LoginFlag() const { return m_pContext ? m_pContext->Flag() : 0; }
		time_t ActiveTime() const { return m_ActiveTime.load(); }
		const std::string& Address() const { return m_Address; }
		sk::network::INetworkContext* ContextGet() const { return m_pContext; }
		bool Enable() const { return m_Enable.load(); }
		void Enable(const bool& enable) { m_Enable.store(enable); }
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

}///namespace proxy



/// /*新生联创（上海）**/
/// /*2019年11月17日 23:8:01|212**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///AFX_SKSTU_SERVICEOBJ_H__89F14_11BB24A6_1EA1D66A5C8AEB9_E4C6__TAIL__
