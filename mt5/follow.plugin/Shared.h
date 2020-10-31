#if !defined(AFX_SKSTU_SHARED_H__347E0_11F5B0C6_B6CA8AFC668080F_086A__HEAD__)
#define AFX_SKSTU_SHARED_H__347E0_11F5B0C6_B6CA8AFC668080F_086A__HEAD__

namespace follow {

	class Shared final {
	public:
		Shared();
		~Shared();
	public:
		//! 推送到观察者的包
#if 0
		sk::container::queue<sk::packet> m_PacketFormViewQ;
		sk::container::queue<sk::packet> m_PacketFormViewLogQ;/*推送运行时日志专用*/
#endif
		//! 推送到管理者的包
		/*sk::container::queue<sk::packet> m_PacketFormManagerQ;*/
		//! 运行时网络命令
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
		//! 用户Login/Group 快速队列
		sk::container::map<UINT64, sk::stringw> m_LoginGroupQ;
		//! 合约/符号/货币对 的路径
		//! symbol(USDCAD.lmx) path(LMX-FX\)
		sk::container::map<sk::stringw/*symbol*/, sk::stringw/*symbol path*/> m_SymbolPathQ;
		//! 组名/符号组/组配置
		//! managers\administrators->"*"->"LMX-FX\*"->"houzhui-2\*"
		sk::container::map<sk::stringw/*group*/, sk::container::map<sk::stringw/*group symbol path*/, shared::ConGroupSymbolField>> m_ConGroupGroupSymbolQ;
		//! 持仓关系表（动态）
		sk::container::map<shared::PositionKey/*signal*/, sk::container::map<shared::PositionKey,shared::DealKey>/*follows*/> m_PositionRelationQ;
		//! 持仓到最近的Deal映射
		sk::container::map<shared::PositionKey, shared::DealKey/*Last/now*/> m_PositionDealQ;

	public:
		shared::Serial Serial() { return ::_InterlockedExchangeAdd64(&m_Serial, 1ll); }
	private:
		//!签名 : m_OrderSerial
		//!说明 : 本程序模块定单序列号
		//!注意 : 与程序模块的生命周期保存一致
		//!日期 : Fri Jun 12 01:05:41 UTC+0800 2020
		volatile shared::Serial m_Serial = 1ll;
	};





}///namespace follow




/// /*新生联创（上海）**/
/// /*2020年01月17日 11:33:26|995**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///AFX_SKSTU_SHARED_H__347E0_11F5B0C6_B6CA8AFC668080F_086A__TAIL__