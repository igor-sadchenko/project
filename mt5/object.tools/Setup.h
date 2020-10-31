#if !defined(AFX_SKSTU_SETUP_H__B1DCD_829B60FF_AE9A4481809C23B_7F3A__HEAD__)
#define AFX_SKSTU_SETUP_H__B1DCD_829B60FF_AE9A4481809C23B_7F3A__HEAD__

namespace manager {

	class Setup final {
	public:
		Setup(const sk::stringa& SystemPath);
		~Setup();
	public:
		int Load();

		const sk::stringa ManagerAddress() const { return m_ManagerAddress; }
		const sk::stringa ManagerPassword() const { return m_ManagerPassword; }
		const sk::stringa ManagerLogin() const { return m_ManagerLogin; }
		const sk::stringa ManagerConnectTimeout() const{ return m_ManagerConnectTimeout; }
	private:
		sk::stringa m_ManagerAddress;
		sk::stringa m_ManagerPassword;
		sk::stringa m_ManagerLogin;
		sk::stringa m_ManagerConnectTimeout;

		sk::stringa m_SystemPath;
	};


}///namespace manager




/// /*新生联创（上海）**/
/// /*2020年04月29日 13:46:54|150**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///AFX_SKSTU_SETUP_H__B1DCD_829B60FF_AE9A4481809C23B_7F3A__TAIL__