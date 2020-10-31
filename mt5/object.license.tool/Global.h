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
	private:
		sk::stringa m_LogKey;
		sk::Log* m_pLog = nullptr;
	public:
		static std::shared_ptr<spdlog::logger> Log(sk::EnLogType logType = sk::EnLogType::EN_LOG_TYPE_SYSTEM);
	};
	Global* GetGlobal();
}///namespace local




/// /*新生联创（上海）**/
/// /*2019年07月30日 14:7:07|120**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///AFX_SKSTU_GLOBAL_H__32B6A_1C619967_31A657A50DD5248_A3A5__TAIL__