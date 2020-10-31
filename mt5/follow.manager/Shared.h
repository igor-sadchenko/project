#if !defined(AFX_SKSTU_Shared_H__71054_EF40B005_502F65870A2A6B4_2685__HEAD__)
#define AFX_SKSTU_Shared_H__71054_EF40B005_502F65870A2A6B4_2685__HEAD__

namespace client {
	class Shared {
	public:
		Shared() {
			m_FollowLogQ.SetMaxSize(100);
		}
		~Shared() {}
	public:
		sk::container::map<shared::SystemID/*ϵͳ��ƷID*/, shared::SystemProduct> m_SystemProductQ;
		sk::container::map<shared::SystemID/*ϵͳ��ƷID*/, sk::stringa/*ϵͳ��Ʒ���û���*/> m_SystemSymbolConQ;
		sk::container::queue<shared::marketdata::MarketData> m_MarketDataUpdateQ;
		sk::container::map<shared::SystemID/*ȫ·��*/, shared::marketdata::MarketData> m_MarketDataQ;


		sk::container::map<shared::FollowKey, shared::FollowField> m_FollowFieldQ;
		sk::container::map<sk::uint64/*login*/, shared::UserSimpleField> m_MTUserQ;
		sk::container::map<sk::stringw/*symbol*/, shared::ConSymbolField> m_MTConSymbolQ;

		sk::container::multimap<time_t, shared::FollowLogField> m_FollowLogQ;

		shared::NetSync m_NetSync;
	};



}





/// /*�����������Ϻ���**/
/// /*2019��12��30�� 11:51:48|304**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///AFX_SKSTU_Shared_H__71054_EF40B005_502F65870A2A6B4_2685__TAIL__