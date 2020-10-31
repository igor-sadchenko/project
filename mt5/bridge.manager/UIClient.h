#if !defined(AFX_SK_H__E052147E_CA4D_49C0_B9D3_D20F324EC3CC__HEAD__)
#define AFX_SK_H__E052147E_CA4D_49C0_B9D3_D20F324EC3CC__HEAD__

namespace client {

	class UIClient final : public sk::object::IObject, public sk::ui::UIFrame, public sk::network::INetworkSpi
	{
		friend class Core;
	public:
		UIClient();
		virtual ~UIClient();
	private:
		int Init() override final;
		int UnInit() override final;

		void UIProcessT();
		void MDUpdateT();
	public:
		int Open() override final;
		int Close() override final;
		static void MsgBox(HWND, const sk::string&, const sk::string&, const UINT&);
	protected:
		void InitWindow() override final;
		void Notify(TNotifyUI& msg) override final;
		CControlUI* CreateControl(LPCTSTR pstrClassName) override final;
		LPCTSTR GetWindowClassName() const override final { return _TEXT("UIMain"); }
		LRESULT OnChar(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override final;
		LRESULT OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override final;
		LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override final;
		LRESULT OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override final;
		LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override final;

		void OnMinimized(CControlUI*);
		void OnUIClose(CControlUI*);
		void OnUILogon(CControlUI*);
		void OnUILogout(CControlUI* crtl = nullptr);
		void OnUISystemSetupUpdate(CControlUI* crtl = nullptr);
		void OnUISystemSetupRefresh(CControlUI* crtl = nullptr);
		void OnUISystemTimeUpdate();
		void OnUISystemTimeRefresh(CControlUI*);
		void OnUISystemProductsRefresh(CControlUI*);
		void OnUISystemProductsUpdate();
		void OnUINetworkStatusUpdate(sk::network::EnNetStatus& NetStatusPrev) const;
		void OnUISystemProductConRefresh(CControlUI* ctrl = nullptr);
		void OnUISystemProductConUpdate(CControlUI* ctrl = nullptr);
		void OnUISystemTradeRecordInit(const sk::packet& headStr);
		void OnUISystemTradeRecordUpdate(const sk::packet& records);
		void OnUISystemTradeRecordRefresh(CControlUI* ctrl = nullptr);
		void OnUISystemTradeRecordClear(CControlUI* ctrl = nullptr);
		void OnUISystemExchangeOnlineUpdate();
		void OnUISystemExchangeOnlineRefresh(CControlUI* ctrl = nullptr);
		void OnUISystemExchangePositionUpdate();
		void OnUISystemExchangePositionRefresh(CControlUI* ctrl = nullptr);
		void OnUISystemPositionRefresh(CControlUI* ctrl = nullptr);
		void OnUISystemAssignLoginsPostOrderAppend();
		void OnUISystemAssignLoginsPostOrderRemove();
		void OnUISystemAssignLoginsPostOrderRefresh();
		void OnUISystemAssignLoginsPostOrderUpdate(const shared::SystemID&);
		void OnUISystemAssignLoginsPostOrderSearch(CControlUI* ctrl = nullptr);

		void OnUIListSort(CControlUI*);
	public:
		void OnUISystemMDUpdate(const shared::marketdata::MarketData&);
	private:
		int NetLoginRequest() const;
		int NetSystemTimeRequest() const;
		int NetSystemSetupUpdateRequest(const shared::BridgeSystemSetup&) const;
		int NetSystemProductsRequest() const;
		int NetSystemProductConsRequest(
			shared::SystemID reqID = 0,
			sk::network::Header::EnDataProcessFlag ProcessFlag = sk::network::Header::EnDataProcessFlag::EN_DATA_PROCESS_FLAG_SELECT,
			sk::packet update = "") const;
		int NetSystemTradeRecordHead() const;
		int NetSystemTradeRecordUpdate(int count = 100) const;
		int NetSystemMDCacheRequest() const;
		int NetSystemExchangePositionRequest(const shared::SystemID&) const;
		int NetSystemExchangeOnline() const;
		int NetSystemPositionRequest(const shared::SystemID&) const;
		int NetSystemMTUsersRequest() const;
		int NetSystemAssignLoginsPostOrder(const shared::SystemID&/*ExchangeID*/, const sk::network::HEADER::EnDataProcessFlag&, const sk::packet&) const;
	private:
		void OnNetSystemMTUsers(const sk::network::Header&, const sk::packet&);
		void OnNetSystemPosition(const sk::network::Header&, const sk::packet&);
		void OnNetSystemSetup(const sk::network::Header&, const sk::packet&);
		void OnNetSystemTime(const sk::network::Header&, const sk::packet&);
		void OnNetSystemExchanges(const sk::network::Header&, const sk::packet&);
		void OnNetSystemProducts(const sk::network::Header&, const sk::packet&);
		void OnNetSystemProductCons(const sk::network::Header&, const sk::packet&);
		void OnNetSystemMDUpdate(const sk::network::Header&, const sk::packet&);
		void OnNetSystemMDCache(const sk::network::Header&, const sk::packet&);
		void OnNetSystemTradeRecordHead(const sk::network::Header&, const sk::packet&);
		void OnNetSystemTradeRecordUpdate(const sk::network::Header&, const sk::packet&);
		void OnNetSystemExchangePositionUpdate(const sk::network::Header&, const sk::packet&);
		void OnNetSystemAssignLoginsPostOrder(const sk::network::Header&, const sk::packet&);
	protected:
		void OnTcpClientConnect() override final;
		void OnTcpClientDisconnect() override final;
		void OnTcpClientRead(const sk::network::Header&, const sk::packet&) override final;
		void OnTcpClientWelcome(const sk::network::Header&, const sk::packet&) override final;
	};


}///namespace client








//!@ /*新生联创®（上海）*/
//!@ /*Tue Aug 25 15:01:33 UTC+0800 2020*/
//!@ /*___www.skstu.com___*/
#endif /*AFX_SK_H__E052147E_CA4D_49C0_B9D3_D20F324EC3CC__HEAD__*/