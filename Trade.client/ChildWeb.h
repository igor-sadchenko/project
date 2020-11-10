#if !defined(AFX_SK_H__4F8D7A5E_93A0_41CA_8CE8_802DE24539A5__HEAD__)
#define AFX_SK_H__4F8D7A5E_93A0_41CA_8CE8_802DE24539A5__HEAD__

namespace local {

	class ChildWeb : public sk::IWindowChild
	{
	public:
		ChildWeb(const long&, const HINSTANCE&, const HINSTANCE&, const sk::string&);
		~ChildWeb();
	public:
		void Release() const override final { delete this; }
	protected:
		LRESULT OnChildMessage(HWND, UINT, WPARAM, LPARAM, BOOL&) override final;
	private:
		std::vector<std::thread> m_Threads;
	};



}///namespace local

//!@ /*新生联创®（上海）*/
//!@ /*Sun Nov 8 14:24:26 UTC+0800 2020*/
//!@ /*___www.skstu.com___*/
#endif /*AFX_SK_H__4F8D7A5E_93A0_41CA_8CE8_802DE24539A5__HEAD__*/
