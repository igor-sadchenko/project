#if !defined(AFX_SK_H__67674D28_D1C1_4321_8A9E_1C5AF847D473__HEAD__)
#define AFX_SK_H__67674D28_D1C1_4321_8A9E_1C5AF847D473__HEAD__

namespace local {

	class ChildDraw : public sk::IWindowChild
	{
	public:
		ChildDraw(const long&, const HINSTANCE&, const HINSTANCE&, const sk::string&);
		~ChildDraw();
	public:
		void Release() const override final { delete this; }
	protected:
		LRESULT OnChildMessage(HWND, UINT, WPARAM, LPARAM, BOOL&) override final;
	private:
		HDC m_hDC = nullptr;
		HGLRC m_hRC = nullptr;
	};

}///namespace local


//!@ /*新生联创®（上海）*/
//!@ /*Sun Nov 8 06:34:46 UTC+0800 2020*/
//!@ /*___www.skstu.com___*/
#endif /*AFX_SK_H__67674D28_D1C1_4321_8A9E_1C5AF847D473__HEAD__*/
