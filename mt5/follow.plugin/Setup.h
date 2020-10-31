#if !defined(AFX_SKSTU_Setup_H__CAA53_3928139B_E9A2BF3389A6D7F_CCAF__HEAD__)
#define AFX_SKSTU_Setup_H__CAA53_3928139B_E9A2BF3389A6D7F_CCAF__HEAD__

namespace follow {

	class Setup final {
	public:
		Setup(const sk::stringa&);
		~Setup();
	public:
		int Load();
		int SystemSetupLoad();
		const sk::stringa& ApiPath() const { return m_ApiPath; }
		const sk::stringa& ObjPath() const { return m_ObjPath; }
		const sk::stringa& ServerTcpAddr() const { return m_ServerTcpAddr; };
		const sk::stringa& FollowRecordPath() const { return m_FollowRecordPath; }
		const sk::stringa& FollowconPathname() const { return m_FollowconPathname; }
		const sk::stringa& SystemSetupPathname() const { return m_SystemSetupPathname; }
		const bool EnableLog() const { return m_EnableLog.load(); }
		const bool EnableServerTcp() const { return m_EnableServerTcp.load(); }

		int SystemSetupSet(const shared::FollowSystemSetup&);
		const shared::FollowSystemSetup& SystemSetupGet() const;
	private:
		sk::stringa m_ObjPath;
		sk::stringa m_ApiPath;
		sk::stringa m_ServerTcpAddr;
		sk::stringa m_FollowRecordPath;
		sk::stringa m_SystemSetupPathname;
		sk::stringa m_FollowconPathname;
		std::atomic_bool m_EnableLog = true;
		std::atomic_bool m_EnableServerTcp = true;

		shared::FollowSystemSetup m_FollowSystemSetup;
	public:
		sk::container::map<shared::SystemID, shared::FollowObjLoginField> m_ClientsMapQ;
	private:
		std::shared_ptr<std::mutex> m_mutex = std::make_shared<std::mutex>();
		int SystemSetupFinalize();
	};



}///namespace follow




/// /*新生联创（上海）**/
/// /*2020年01月16日 14:5:20|645**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///AFX_SKSTU_Setup_H__CAA53_3928139B_E9A2BF3389A6D7F_CCAF__TAIL__