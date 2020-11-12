#if !defined(AFX_SK_H__79B37FFF_799A_4EE7_8E25_92C418649259__HEAD__)
#define AFX_SK_H__79B37FFF_799A_4EE7_8E25_92C418649259__HEAD__

namespace local {

	class Setup final : public sk::object::IObject
	{
	public:
		Setup();
		virtual ~Setup();
	private:
		int Init() override final;
		int UnInit() override final;
	public:
		int Open() override final;
		int Close() override final;
	public:
		const sk::stringa& SetupPathname() const { return m_SetupPathname; }
		const sk::stringa& RuntimePath() const { return m_RuntimePath; }
		const sk::stringa& PluginPath() const { return m_PluginPath; }
		const sk::stringa& ServerAddress() const { return m_ServerAddress; }
		const sk::stringa& FixObjPath() const { return m_FixObjPath; }
		const sk::stringa& FixObjInitiatorPathname() const { return m_FixObjInitiatorPathname; }
		const sk::stringa& FixObjAcceptorPathname() const { return m_FixObjAcceptorPathname; }
	private:
		sk::stringa m_SetupPathname;
		sk::stringa m_RuntimePath;
		sk::stringa m_PluginPath;
		sk::stringa m_ServerAddress;
		sk::stringa m_FixObjPath;
		sk::stringa m_FixObjInitiatorPathname;
		sk::stringa m_FixObjAcceptorPathname;
	};


}///namespace local

//! @/*新生联创®（上海）*/
//! @/*Thu Nov 12 13:24:51 UTC+0800 2020*/
//! @/*___www.skstu.com___*/
#endif/*AFX_SK_H__79B37FFF_799A_4EE7_8E25_92C418649259__HEAD__*/
