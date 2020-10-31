#if !defined(AFX_SKSTU_CORE_H__EBBCB_B45804B6_079BC9FEEF98D8F_2DCF__HEAD__)
#define AFX_SKSTU_CORE_H__EBBCB_B45804B6_079BC9FEEF98D8F_2DCF__HEAD__

namespace local {

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
	};



}///namespace local




/// /*新生联创（上海）**/
/// /*2019年08月10日 20:24:38|432**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///AFX_SKSTU_CORE_H__EBBCB_B45804B6_079BC9FEEF98D8F_2DCF__TAIL__