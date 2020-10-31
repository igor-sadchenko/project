#if !defined(AFX_SKSTU_Setup_H__62D08_6870AF48_0B74266B39BEEAD_323F__HEAD__)
#define AFX_SKSTU_Setup_H__62D08_6870AF48_0B74266B39BEEAD_323F__HEAD__

namespace proxy {

	class Setup final : public sk::object::IObject {
	public:
		Setup();
		~Setup();
	private:
		int Init() override final;
		int UnInit() override final;
	public:
		int Open() override final;
		int Close() override final;

		int LoadSystemSetup();
		int SystemSetupSet(const shared::BridgeSystemSetup&);
		const shared::BridgeSystemSetup& SystemSetupGet() const;
		bool SystemAssignLoginsPostOrderInsert(const shared::SystemID&, const UINT64&);
		bool SystemAssignLoginsPostOrderSelect(const shared::SystemID&, std::vector<UINT64>&);
		bool SystemAssignLoginsPostOrderDelete(const UINT64&);
		bool SystemAssignLoginsPostOrderEmpty(const shared::SystemID&);

		const sk::stringa& GetApiPath() const { return m_ApiPath; }
		const sk::stringa& GetServerTcpAddress() const { return m_ServerTcpAddress; }

		const sk::stringa& GetSystemSetupPathName() const { return m_SystemSetupPathName; }
		const sk::stringa& GetSymbolsPath() const { return m_SymbolsPath; }
		const sk::stringa& GetLogRecordPath() const { return m_LogRecordPath; }
		const sk::stringa& ProcessMapPathName() const { return m_ProcessMapPathName; }
		const sk::stringa& ContextMapPathName() const { return m_ContextMapPathName; }
		const sk::stringa& MDCacheMapPathName() const { return m_MDCacheMapPathName; }
		const sk::stringa& HedgeCacheMapPathName() const { return m_HedgeCacheMapPathName; }
		const sk::stringa& MDCacheMapViewPathName() const { return m_MDCacheMapViewPathName; }
		const sk::stringa& HedgeCacheMapViewPathName() const { return m_HedgeCacheMapViewPathName; }
		const sk::stringa& SystemAssignLoginsPostOrderPathName() const { return m_SystemAssignLoginsPostOrderPathName; }

		sk::container::map<shared::SystemID/*ExchangeID*/, LPProcess*> m_LPProcesesMapQ;
		sk::container::map<shared::SystemID, shared::BridgeLoginField> m_ClientsMapQ;
		sk::container::map<shared::SystemID, shared::ConsoleCommand> m_CommandsMapQ;

		sk::container::map<shared::SystemID/*FlagID*/,bool/*status*/> m_ReadyContextMapQ;
		//!@ 指定帐户抛单缓存队列
		sk::container::map<shared::SystemID/*ExchangeID*/, sk::container::set<UINT64/*MT5Login*/>> m_SystemAssignLoginsPostOrderQ;
	private:
		sk::stringa m_SystemSetupPathName;
		sk::stringa m_SystemAssignLoginsPostOrderPathName;
		sk::stringa m_ServerTcpAddress;
		sk::stringa m_ApiPath;
		sk::stringa m_SymbolsPath;
		sk::stringa m_LogRecordPath;
		sk::stringa m_ProcessMapPathName;
		sk::stringa m_ContextMapPathName;
		sk::stringa m_MDCacheMapPathName;
		sk::stringa m_HedgeCacheMapPathName;
		sk::stringa m_MDCacheMapViewPathName;
		sk::stringa m_HedgeCacheMapViewPathName;

		shared::BridgeSystemSetup m_BridgeSystemSetup;
	private:
		std::shared_ptr<std::mutex> m_mutex = std::make_shared<std::mutex>();
		int SystemSetupLoad();
		int SystemSetupFinalize();

		int SystemAssignLoginsPostOrderLoad();
		int SystemAssignLoginsPostOrderFinalize();
	};




}///namespace proxy




/// /*新生联创（上海）**/
/// /*2019年11月30日 16:10:09|844**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///AFX_SKSTU_Setup_H__62D08_6870AF48_0B74266B39BEEAD_323F__TAIL__