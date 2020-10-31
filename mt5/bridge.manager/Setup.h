#if !defined(AFX_SKSTU_SETUP_H__D616C_0CDB1D23_F11B32E9A17F765_36F3__HEAD__)
#define AFX_SKSTU_SETUP_H__D616C_0CDB1D23_F11B32E9A17F765_36F3__HEAD__

namespace client {

	class Setup final {
	public:
		Setup(const sk::stringa&);
		~Setup();
	public:
		const sk::stringa ServerAddr() const { return m_ServerAddr; }
		const sk::stringa ApiPath() const { return m_ApiPath; }
		const sk::stringa ObjPath() const { return m_ObjPath; }
		const shared::BridgeSystemSetup& BridgeSystemSetup();
		void BridgeSystemSetup(const shared::BridgeSystemSetup&);
		const __int64& BridgeSystemTime();
		void BridgeSystemTime(const __int64&);
		const shared::BridgeLoginField& LoginField() const { return m_LoginField; }
	private:
		int Init();
		int UnInit();
	private:
		sk::stringa m_ServerAddr;
		sk::stringa m_ApiPath;

		sk::stringa m_ObjPath;
		__int64 m_BridgeSystemTime = 0;
		shared::BridgeSystemSetup m_BridgeSystemSetup;
		shared::BridgeLoginField m_LoginField;
	private:
		std::mutex m_mutex;
	};





}///namespace client





/// /*新生联创（上海）**/
/// /*2020年01月11日 20:56:10|862**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///AFX_SKSTU_SETUP_H__D616C_0CDB1D23_F11B32E9A17F765_36F3__TAIL__
