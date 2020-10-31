#if !defined(AFX_SKSTU_UIProductEdit_H__D28FF_2EADA978_0A0D849A5D59D26_087F__HEAD__)
#define AFX_SKSTU_UIProductEdit_H__D28FF_2EADA978_0A0D849A5D59D26_087F__HEAD__

namespace client {
	class IUIProductEdit {
	public:
		IUIProductEdit();
		~IUIProductEdit();
	protected:
		virtual int Init();
	};
	class UISYSProductEdit final : public IUIProductEdit {
	public:
		UISYSProductEdit();
		~UISYSProductEdit();
	};
	class UILPProductEdit final : public IUIProductEdit {
	public:
		UILPProductEdit();
		~UILPProductEdit();
	};
}///namespace client





/// /*新生联创（上海）**/
/// /*2019年12月31日 12:23:57|029**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///AFX_SKSTU_UIProductEdit_H__D28FF_2EADA978_0A0D849A5D59D26_087F__TAIL__
