#if !defined(AFX_SK_H__2B76E365_904A_437B_8552_C9B09301D424__HEAD__)
#define AFX_SK_H__2B76E365_904A_437B_8552_C9B09301D424__HEAD__

namespace dealer {

	class Exchange final : public sk::object::IObject, public sk::network::INetworkSpi {
	public:
		Exchange();
		virtual ~Exchange();
	private:
		int Init() override final;
		int UnInit() override final;
	public:
		int Open() override final;
		int Close() override final;
	protected:
		void OnTcpClientConnect() override final;
		void OnTcpClientDisconnect() override final;
		void OnTcpClientRead(const sk::network::Header&, const sk::packet&) override final;
		void OnTcpClientWelcome(const sk::network::Header&, const sk::packet&) override final;
	public:
		int NetLoginRequest() const;
		void OnNetLoginResult(const sk::network::Header&, const sk::packet&);
		void OnNetMTUsersGet(const sk::network::Header&, const sk::packet&);
		void OnNetMTSymbolsGet(const sk::network::Header&, const sk::packet&);
		void OnNetSystemPositionsGet(const sk::network::Header&, const sk::packet&);
		void OnNetProductsGet(const sk::network::Header&, const sk::packet&);
		void OnNetSystemSetupUpdate(const sk::network::Header&, const sk::packet&);
		void OnNetSystemProducts(const sk::network::Header&, const sk::packet&);
		void OnNetSystemTimeUpdate(const sk::network::Header&, const sk::packet&);
		void OnNetTradeOrderRes(const sk::network::Header&, const sk::packet&);
		void OnNetSystemAssignLoginsPostOrder(const sk::network::Header&, const sk::packet&);

		int NetProductsSend() const;
		int NetSystemProductRequest() const;
		int NetMTUsersSend(const sk::network::Header::EnDataProcessFlag&, const sk::packet&) const;
		int NetMTSymbolsSend(const sk::network::Header::EnDataProcessFlag&, const sk::packet&) const;
		int NetSystemPositiionSend(const sk::network::Header::ExternData&, const sk::packet&) const;
		int NetTradeOrderRequest(const sk::packet&) const;	
		int NetTradeOrderFailedSend(const shared::SystemOrder&) const;
		int NetTradeOrderTimeoutSend(const shared::SystemOrder&) const;
		int NetSystemLogpakSend(const sk::packet&) const;
	private:
		std::atomic_bool ReadyLoginVeirfy = false;
		std::atomic_bool ReadySystemSetup = false;
		std::atomic_bool ReadyProductCon = false;
	public:
		bool Ready() const;

	};










}///namespace dealer









///!@ 新生联创®（上海）
///!@ Thu May 28 14:14:57 UTC+0800 2020
///!@ ___www.skstu.com___
#endif///AFX_SK_H__2B76E365_904A_437B_8552_C9B09301D424__HEAD__
