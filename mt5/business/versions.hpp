#if !defined(AFX_SKSTU_versions_H__855AF_ED98A2F8_2AD71FBAE0E0169_98AB__HEAD__)
#define AFX_SKSTU_versions_H__855AF_ED98A2F8_2AD71FBAE0E0169_98AB__HEAD__

#define VERSION_IDENTIFICATION	"新生联创(R) mttrade(c)"
#define VERSION_NUMBER_MAJOR					0	//! 主版本号
#define VERSION_NUMBER_MINOR				1	//! 次要版本号
#define VERSION_NUMBER_UPDATE				0	//! 补丁或更新版本号

namespace shared {
	/***
	* Major ：具有相同名称但不同主版本号的程序集不可互换。例如，这适用于对产品的大量重写，这些重写使得无法实现向后兼容性。
	* Minor ：如果两个程序集的名称和主版本号相同，而次版本号不同，这指示显著增强，但照顾到了向后兼容性。例如，这适用于产品的修正版或完全向后兼容的新版本。
	* Build ：内部版本号的不同表示对相同源所作的重新编译。这适合于更改处理器、平台或编译器的情况。
	* Revision ：名称、主版本号和次版本号都相同但修订号不同的程序集应是完全可互换的。这适用于修复以前发布的程序集中的安全漏洞。
	*/

	class Versions final {
	public:
		Versions() {}
		~Versions() = delete;
	public:
		sk::string GetVersionsStr() const {
			return tostring(m_Major).append(tostring(m_Minor)).append(tostring(m_Build)).append(tostring(m_Revision));
		}
		sk::uint64 SetVersions(const sk::uint64& major, const sk::uint64& minor, const sk::uint64& build, const sk::uint64& revision) {
			m_Major = major;
			m_Minor = minor;
			m_Build = build;
			m_Revision = revision;
			return m_Major + m_Minor + m_Build + m_Revision;
		}
	private:
		sk::uint64 m_Major = 0;
		sk::uint64 m_Minor = 0;
		sk::uint64 m_Build = 0;
		sk::uint64 m_Revision = 0;
	};




}///namespace shared





/// /*新生联创（上海）**/
/// /*2019年08月12日 19:25:28|011**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///AFX_SKSTU_versions_H__855AF_ED98A2F8_2AD71FBAE0E0169_98AB__TAIL__