#if !defined(AFX_SKSTU_SHARED_H__347E0_11F5B0C6_B6CA8AFC668080F_086A__HEAD__)
#define AFX_SKSTU_SHARED_H__347E0_11F5B0C6_B6CA8AFC668080F_086A__HEAD__

namespace follow {

	class Shared final {
	public:
		Shared();
		~Shared();
	public:
		//! ���͵��۲��ߵİ�
#if 0
		sk::container::queue<sk::packet> m_PacketFormViewQ;
		sk::container::queue<sk::packet> m_PacketFormViewLogQ;/*��������ʱ��־ר��*/
#endif
		//! ���͵������ߵİ�
		/*sk::container::queue<sk::packet> m_PacketFormManagerQ;*/
		//! ����ʱ��������
		/*sk::container::map<sk::network::NetCmd, sk::packet> m_RuntimeNetCMDQ;*/
	public:
		sk::container::queue<sk::packet> m_FollowLogQ;
		sk::container::map<UINT64, shared::UserField> m_OnUserFieldQ;
		sk::container::map<UINT64, shared::DealField> m_OnDealFieldQ;
		sk::container::map<UINT64, shared::OrderField> m_OnOrderFieldQ;
		sk::container::map<UINT64, IMTPosition*> m_OnPositionPtrQ;
		sk::container::map<UINT64, shared::PositionField> m_OnPositionFieldQ;
		sk::container::map<sk::stringw, shared::ConGroupField> m_OnConGroupFieldQ;
		sk::container::map<sk::stringw, shared::ConSymbolField> m_OnConSymbolFieldQ;
		sk::container::map<UINT, shared::serverplugin::OnTradeRequestField> m_OnTradeRequestQ;
		//! �û�Login/Group ���ٶ���
		sk::container::map<UINT64, sk::stringw> m_LoginGroupQ;
		//! ��Լ/����/���Ҷ� ��·��
		//! symbol(USDCAD.lmx) path(LMX-FX\)
		sk::container::map<sk::stringw/*symbol*/, sk::stringw/*symbol path*/> m_SymbolPathQ;
		//! ����/������/������
		//! managers\administrators->"*"->"LMX-FX\*"->"houzhui-2\*"
		sk::container::map<sk::stringw/*group*/, sk::container::map<sk::stringw/*group symbol path*/, shared::ConGroupSymbolField>> m_ConGroupGroupSymbolQ;
		//! �ֲֹ�ϵ����̬��
		sk::container::map<shared::PositionKey/*signal*/, sk::container::map<shared::PositionKey,shared::DealKey>/*follows*/> m_PositionRelationQ;
		//! �ֲֵ������Dealӳ��
		sk::container::map<shared::PositionKey, shared::DealKey/*Last/now*/> m_PositionDealQ;

	public:
		shared::Serial Serial() { return ::_InterlockedExchangeAdd64(&m_Serial, 1ll); }
	private:
		//!ǩ�� : m_OrderSerial
		//!˵�� : ������ģ�鶨�����к�
		//!ע�� : �����ģ����������ڱ���һ��
		//!���� : Fri Jun 12 01:05:41 UTC+0800 2020
		volatile shared::Serial m_Serial = 1ll;
	};





}///namespace follow




/// /*�����������Ϻ���**/
/// /*2020��01��17�� 11:33:26|995**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///AFX_SKSTU_SHARED_H__347E0_11F5B0C6_B6CA8AFC668080F_086A__TAIL__