#if !defined(AFX_SKSTU_CORE_H__EBBCB_B45804B6_079BC9FEEF98D8F_2DCF__HEAD__)
#define AFX_SKSTU_CORE_H__EBBCB_B45804B6_079BC9FEEF98D8F_2DCF__HEAD__

namespace proxy {

	class Core final : public sk::object::IObject {
	public:
		Core();
		~Core();
	private:
		int Init() override final;
		int UnInit() override final;
	public:
		int Open() override final;
		int Close() override final;
		bool OpenIs() const { return m_IsOpen.load(); }
	public:
		System* GetSystem() const { return m_pSystem; }
	private:
		sk::License m_License;
		System* m_pSystem = nullptr;
		sk::Timer m_Timer;
		static int OnTimer(const sk::int64&, const sk::intptr&, const sk::int64&);
	};



}///namespace proxy




/// /*新生联创（上海）**/
/// /*2019年08月10日 20:24:38|432**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///AFX_SKSTU_CORE_H__EBBCB_B45804B6_079BC9FEEF98D8F_2DCF__TAIL__