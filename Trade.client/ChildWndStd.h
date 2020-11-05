#if !defined(AFX_SKSTU_1604538404_H__1010C_8B9D3030_A31A621B466B582_A49C__HEAD__)
#define AFX_SKSTU_1604538404_H__1010C_8B9D3030_A31A621B466B582_A49C__HEAD__

namespace client {

	class ChildWndStd : public sk::IWindowChild
	{
	public:
		ChildWndStd(const long&, const HINSTANCE&, const HINSTANCE&, const sk::string&);
		~ChildWndStd();
	public:
		void Release() const override final { delete this; }
	protected:
		LRESULT OnChildMessage(HWND, UINT, WPARAM, LPARAM, BOOL&) override final;
		void DoPaint();
	private:
		BOOL GLUTSetupPixelFormat(const HDC&);
		BOOL GLUTInit();
		void GLUTChangeSize(const int&, const int&);
	private:
		HDC m_hDC = nullptr;
		HDC m_hMemDC = nullptr;
		HGLRC m_hGLRC = nullptr;
	};






}///namespace client




/// /*新生联创（上海）**/
/// /*2020年11月5日 10:29:27|200**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///AFX_SKSTU_1604538404_H__1010C_8B9D3030_A31A621B466B582_A49C__TAIL__