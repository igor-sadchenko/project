#if !defined(AFX_SKSTU_GLOBAL_H__32B6A_1C619967_31A657A50DD5248_A3A5__HEAD__)
#define AFX_SKSTU_GLOBAL_H__32B6A_1C619967_31A657A50DD5248_A3A5__HEAD__

namespace manager {
	class Global final : public sk::object::IGlobal {
	public:
		Global();
		~Global();
	private:
		virtual int Init() override final;
		virtual int UnInit() override final;
		virtual const char* GetModuleName() const { return "manager"; }
	public:
		static Core* CoreGet();
		static Shared* SharedGet();
		static Setup* SetupGet();
		static std::shared_ptr<spdlog::logger> Log(sk::EnLogType logType = sk::EnLogType::EN_LOG_TYPE_SYSTEM);
	private:
		sk::stringa m_LogKey;
		sk::Log* m_pLog = nullptr;

		Setup* m_pSetup = nullptr;
		Core* m_pCore = nullptr;
		Shared* m_pShared = nullptr;
	};
}///namespace manager




/// /*新生联创（上海）**/
/// /*2019年07月30日 14:7:07|120**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///AFX_SKSTU_GLOBAL_H__32B6A_1C619967_31A657A50DD5248_A3A5__TAIL__