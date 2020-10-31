#if !defined(AFX_SKSTU_UIFollowEdit_H__740E9_E57D5533_FF9BAC5C4BB37A3_2E9D__HEAD__)
#define AFX_SKSTU_UIFollowEdit_H__740E9_E57D5533_FF9BAC5C4BB37A3_2E9D__HEAD__

namespace client {

	class IFollowTreeNodeUI final : public CListContainerElementUI {
	public:
		void Set(const shared::FollowField&);
	};

	class IFollowConfigureEditUI final : public CListContainerElementUI {
	public:
		void Set(const shared::FollowField&);
		std::shared_ptr<shared::FollowField> Get();
	};

	class IFollowEditUI final : public CListUI {
	public:
		void Parse();
		sk::packet Packet();
	};

}///namespace client





/// /*新生联创（上海）**/
/// /*2020年01月19日 9:53:44|337**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///AFX_SKSTU_UIFollowEdit_H__740E9_E57D5533_FF9BAC5C4BB37A3_2E9D__TAIL__
