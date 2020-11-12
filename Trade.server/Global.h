#if !defined(AFX_SKSTU_GLOBAL_H__32B6A_1C619967_31A657A50DD5248_A3A5__HEAD__)
#define AFX_SKSTU_GLOBAL_H__32B6A_1C619967_31A657A50DD5248_A3A5__HEAD__

namespace local {
	class Global final : public sk::object::IGlobal {
	public:
		Global();
		~Global();
	private:
		int Init() override final;
		int UnInit() override final;
		virtual const char* GetModuleName() const { return "Trade.server"; }
	public:
		static Core* CoreGet();
		static Setup* SetupGet();
		static shared::SharedApi* SharedApiGet();
	private:
		Core* m_pCore = nullptr;
		Setup* m_pSetup = nullptr;
		shared::SharedApi* m_pSharedApi = nullptr;
	};
	extern Global* GlobalGet();
}///namespace local




/// /*新生联创（上海）**/
/// /*2019年07月30日 14:7:07|120**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///AFX_SKSTU_GLOBAL_H__32B6A_1C619967_31A657A50DD5248_A3A5__TAIL__