#if !defined(AFX_SK_H__46C81C75_1E1C_4034_8908_1DCF36D88273__HEAD__)
#define AFX_SK_H__46C81C75_1E1C_4034_8908_1DCF36D88273__HEAD__

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
		LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override final;
		LRESULT OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override final;
		LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override final;

		void OnMinimized(CControlUI*);
		void OnUIClose(CControlUI*);
		void OnUILogon(CControlUI*);
		void OnUILogout(CControlUI* crtl = nullptr);
		void OnUIFollowConsUpdate(CControlUI* ctrl = nullptr);
		void OnUIFollowConsRefresh(CControlUI*);
		void OnUIFollowConsCreate(CControlUI*);
		void OnUIFollowConsDelete(CControlUI*);
		void OnUIFollowRecordRefresh(CControlUI* ctrl = nullptr);
		void OnUIFollowRecordInitialize(CControlUI* ctrl = nullptr);
		void OnUIFollowRecordUpdate(CControlUI* ctrl=nullptr);
		void OnUIFollowRecordClear(CControlUI* ctrl = nullptr);
		void OnUIPlatfomPatchPosition(CControlUI* ctrl = nullptr);
		void OnUIPlatfomSwitchFollow(CControlUI* ctrl = nullptr);
		void OnUIPlatfomSwitchSLTP(CControlUI* ctrl = nullptr);
		void OnUIPlatfomSwitchPatch(CControlUI* ctrl = nullptr);
		void OnUIServerTimeRefresh(CControlUI* ctrl = nullptr);
		void OnUIServerTimeUpdate(const time_t&);
		void OnUISystemTradeRecordHead(const sk::packet&);
		void OnUIPlaformPatchPositions(const SKAPIRES&);
		void OnUIClickListHead(CControlUI*);
	private:
		void UISystemSetupSet();
		void UISystemSetupGet();
	private:
		void OnNetSystemSetup(const sk::network::Header&, const sk::packet&);
		void OnNetSystemQuickFunctionSwitchFollow(const sk::network::Header&, const sk::packet&);
		void OnNetSystemQuickFunctionSwitchPatch(const sk::network::Header&, const sk::packet&);
		void OnNetSystemQuickFunctionSwitchSLTP(const sk::network::Header&, const sk::packet&);
		void OnNetPlaformPatchPositions(const sk::network::Header&, const sk::packet&);
		void OnNetMTUsers(const sk::network::Header&, const sk::packet&);
		void OnNetMTSymbols(const sk::network::Header&, const sk::packet&);
		void OnNetFollowCons(const sk::network::Header&, const sk::packet&);
		void OnNetFollowUpdate(const sk::network::Header&, const sk::packet&);
		void OnNetFollowDelete(const sk::network::Header&, const sk::packet&);
		void OnNetFollowRecordInitialize(const sk::network::Header&, const sk::packet&);
		void OnNetFollowRecordUpdate(const sk::network::Header&, const sk::packet&);
		void OnNetServerTimeUpdate(const sk::network::Header&, const sk::packet&);
		void OnNetSystemTradeRecordHead(const sk::network::Header&, const sk::packet&);
		void OnNetSystemTradeRecordUpdate(const sk::network::Header&, const sk::packet&);
	public:
		int NetLoginRequest(const shared::FollowObjLoginField&) const;
		int NetFollowConsRequest() const;
		int NetFollowConsUpdateRequest(const sk::packet&) const;
		int NetFollowConsDeleteRequest(const sk::packet&) const;
		int NetFollowRecordRequet() const;
		int NetPlatfomPatchPositionRequest() const;
		int NetServerTimeRequest() const;
		int NetSystemTradeRecordHead() const;
		int NetSystemTradeRecordUpdate(int count = 100) const;
		int NetSystemSetupSend(const shared::FollowSystemSetup&) const;
		int NetSystemSetupRequest() const;
		int NetQuickFunctionSwitchFollow() const;
		int NetQuickFunctionSwitchPatch() const;
		int NetQuickFunctionSwitchSLTP() const;
	protected:
		void OnTcpClientConnect() override final;
		void OnTcpClientDisconnect() override final;
		void OnTcpClientRead(const sk::network::Header&, const sk::packet&) override final;
		void OnTcpClientWelcome(const sk::network::Header&, const sk::packet&) override final;

	};

}///namespace client






//!@ /*新生联创®（上海）*/
//!@ /*Sun Aug 23 21:43:09 UTC+0800 2020*/
//!@ /*___www.skstu.com___*/
#endif /*AFX_SK_H__46C81C75_1E1C_4034_8908_1DCF36D88273__HEAD__*/
