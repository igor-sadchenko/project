#if !defined(AFX_SKSTU_Server_H__E5485_6A5654B4_475C15B1EAB1F7A_DC7D__HEAD__)
#define AFX_SKSTU_Server_H__E5485_6A5654B4_475C15B1EAB1F7A_DC7D__HEAD__

namespace proxy {
	class System;
	class Server final : public sk::object::IObject, public sk::network::INetworkSpi 
	{
		friend System;
	public:
		Server();
		~Server();
	private:
		int Init() override final;
		int UnInit() override final;
	public:
		int Open() override final;
		int Close() override final;
		System* SystemGet() const;
		SymbolCon* SymbolConGet() const;
		QuoteFinal* QuoteFinalGet() const;
	protected:
		void OnTcpServerLogon(sk::network::INetworkContext*) override final;
		void OnTcpServerLogout(sk::network::INetworkContext*) override final;
		void OnTcpServerRead(sk::network::INetworkContext*, const sk::network::Header&, const sk::packet&) override final;
	public:
		sk::container::map<shared::SystemID, IServiceObj*> m_ServiceObjQ;
	private:
		void OnNetLoginProcess(sk::network::INetworkContext*, const sk::network::Header&, const sk::packet&);
		void OnNetProductConProcess(sk::network::INetworkContext*, const sk::network::Header&, const sk::packet&);
		void OnNetProductsProcess(sk::network::INetworkContext*, const sk::network::Header&, const sk::packet&);
		void OnNetProductsGetProcess(sk::network::INetworkContext*, const sk::network::Header&, const sk::packet&);
		void OnNetTradeOrderProcess(sk::network::INetworkContext*, const sk::network::Header&, const sk::packet&);
		void OnNetExchangePositionProcess(sk::network::INetworkContext*, const sk::network::Header&, const sk::packet&);
		void OnNetMTUsersProcess(sk::network::INetworkContext*, const sk::network::Header&, const sk::packet&);
		void OnNetMTSymbolsProcess(sk::network::INetworkContext*, const sk::network::Header&, const sk::packet&);
		void OnNetSystemTimeProcess(sk::network::INetworkContext*, const sk::network::Header&, const sk::packet&);
		void OnNetSystemSetupProcess(sk::network::INetworkContext*, const sk::network::Header&, const sk::packet&);
		void OnNetMarketDataUpdate(sk::network::INetworkContext*, const sk::network::Header&, const sk::packet&);
		void OnNetMarketDataCache(sk::network::INetworkContext*, const sk::network::Header&, const sk::packet&);
		void OnNetTradeRecordHeadProcess(sk::network::INetworkContext*, const sk::network::Header&, const sk::packet&);
		void OnNetTradeRecordUpdateProcess(sk::network::INetworkContext*, const sk::network::Header&, const sk::packet&);	
		void OnNetExchangeOnlineProcess(sk::network::INetworkContext*, const sk::network::Header&, const sk::packet&);
		void OnNetOrderTimeoutProcess(sk::network::INetworkContext*, const sk::network::Header&, const sk::packet&);
		void OnNetOrderFailedProcess(sk::network::INetworkContext*, const sk::network::Header&, const sk::packet&);
		void OnNetLogpakProcess(sk::network::INetworkContext*, const sk::network::Header&, const sk::packet&);
		void OnNetSystemPositionProcess(sk::network::INetworkContext*, const sk::network::Header&, const sk::packet&);
		void OnNetSystemAssignLoginsPostOrderProcess(sk::network::INetworkContext*, const sk::network::Header&, const sk::packet&);
		SKAPIRES NetTradeOrderRequest(const shared::SystemOrder&);
		SKAPIRES NetTradeOrderResSend(const shared::SystemOrder&);
	};


}///namespace proxy




/// /*新生联创（上海）**/
/// /*2019年12月8日 19:4:16|617**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///AFX_SKSTU_Server_H__E5485_6A5654B4_475C15B1EAB1F7A_DC7D__TAIL__