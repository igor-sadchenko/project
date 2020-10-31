#if !defined(AFX_SKSTU_CORE_H__19D14_91E2B4AE_8A3D5B1611ACD89_D47E__HEAD__)
#define AFX_SKSTU_CORE_H__19D14_91E2B4AE_8A3D5B1611ACD89_D47E__HEAD__

namespace follow {

	class Core final : public sk::object::IObject
	{
	public:
		Core();
		~Core();
	private:
		int Init() override final;
		int UnInit() override final;
	public:
		int Open() override final;
		int Close() override final;
		Server* ServerGet() const { return m_pServer; }
	private:
		Server* m_pServer = nullptr;
	};



}///namespace follow




/// /*新生联创（上海）**/
/// /*2019年09月10日 9:31:40|695**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///AFX_SKSTU_CORE_H__19D14_91E2B4AE_8A3D5B1611ACD89_D47E__TAIL__