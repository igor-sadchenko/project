#if !defined(AFX_SKSTU_Setup_H__CAA53_3928139B_E9A2BF3389A6D7F_CCAF__HEAD__)
#define AFX_SKSTU_Setup_H__CAA53_3928139B_E9A2BF3389A6D7F_CCAF__HEAD__

namespace dealer {

	class Setup final {
	public:
		Setup(const sk::stringa&);
		~Setup();
	public:
		int Load();
		const sk::stringa& ApiPath() const { return m_ApiPath; }
		const sk::stringa& ObjPath() const { return m_ObjPath; }
		const sk::stringa& TcptoAddr() const { return m_TcptoAddr; }
		const bool EnableLog() const { return m_EnableLog.load(); }
		const bool EnableTcpto() const { return m_EnableTcpto.load(); }

		std::int64_t BridgeSystemTime() const { return m_BridgeSystemTime.load(); }
		void BridgeSystemTime(const std::int64_t& time) { m_BridgeSystemTime.store(time); }
		const shared::BridgeSystemSetup& BridgeSystemSetup() const { return m_BridgeSystemSetup; }
		void BridgeSystemSetup(const shared::BridgeSystemSetup& setup) { m_BridgeSystemSetup = setup; }
		const shared::BridgeLoginField& BridgeLoginField() const { return m_BridgeLoginFiled; }

		const bool ProxyEnable() const { return m_ProxyEnable.load(); }
		const time_t ProxyInterval() const { return m_ProxyInterval.load(); }
		const sk::stringa& ProxyPathname() const { return m_ProxyPathname; }
	private:
		sk::stringa m_ObjPath;
		sk::stringa m_ApiPath;
		sk::stringa m_TcptoAddr;
		std::atomic_bool m_EnableLog = true;
		std::atomic_bool m_EnableTcpto = true;

		std::atomic_bool m_ProxyEnable = false;
		std::atomic_int64_t m_ProxyInterval = 5000;
		sk::stringa m_ProxyPathname;

		std::atomic_int64_t m_BridgeSystemTime = 0;
		shared::BridgeSystemSetup m_BridgeSystemSetup;
		shared::BridgeLoginField m_BridgeLoginFiled;
	public:
		sk::container::map<shared::SystemID, shared::FollowObjLoginField> m_ClientsMapQ;
	};



}///namespace dealer




/// /*新生联创（上海）**/
/// /*2020年01月16日 14:5:20|645**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///AFX_SKSTU_Setup_H__CAA53_3928139B_E9A2BF3389A6D7F_CCAF__TAIL__