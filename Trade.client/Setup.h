#if !defined(AFX_SK_H__F3C6AE73_A015_4ADD_8FFC_1B6B5B0CF453__HEAD__)
#define AFX_SK_H__F3C6AE73_A015_4ADD_8FFC_1B6B5B0CF453__HEAD__

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

}///namespace lcoal

//! @/*新生联创®（上海）*/
//! @/*Fri Nov 13 15:15:22 UTC+0800 2020*/
//! @/*___www.skstu.com___*/
#endif/*AFX_SK_H__F3C6AE73_A015_4ADD_8FFC_1B6B5B0CF453__HEAD__*/