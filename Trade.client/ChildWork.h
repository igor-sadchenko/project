#if !defined(AFX_SK_H__0CC3AFF8_EF0C_46D2_A5F2_C38AF22CA639__HEAD__)
#define AFX_SK_H__0CC3AFF8_EF0C_46D2_A5F2_C38AF22CA639__HEAD__

namespace local {

	class UIWork : public sk::ui::UIFrame
	{
	public:
		UIWork();
		virtual ~UIWork();
	public:
		void AdjustSize(const int&,const int&,const int&, const int&);
	protected:
		void InitWindow() override final;
		void Notify(TNotifyUI& msg) override final;
		CControlUI* CreateControl(LPCTSTR pstrClassName) override final;
		LPCTSTR GetWindowClassName() const override final { return _TEXT("uiwork"); }
		LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override final;
		LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override final;
		LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override final;
	};


	class ChildWork : public sk::IWindowChild
	{
	public:
		ChildWork(const long&, const HINSTANCE&, const HINSTANCE&, const sk::string&);
		~ChildWork();
	public:
		void Release() const override final { delete this; }
	protected:
		LRESULT OnChildMessage(HWND, UINT, WPARAM, LPARAM, BOOL&) override final;
	private:
		UIWork* m_pUIWork = nullptr;
		std::vector<std::thread> m_Threads;
	};


}///namespace local


//!@ /*新生联创®（上海）*/
//!@ /*Mon Nov 9 09:55:00 UTC+0800 2020*/
//!@ /*___www.skstu.com___*/
#endif /*AFX_SK_H__0CC3AFF8_EF0C_46D2_A5F2_C38AF22CA639__HEAD__*/