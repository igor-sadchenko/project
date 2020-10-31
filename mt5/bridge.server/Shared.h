#if !defined(AFX_SKSTU_SHARED_H__D8CCC_96B3D1F0_D7D86A320C9B079_3384__HEAD__)
#define AFX_SKSTU_SHARED_H__D8CCC_96B3D1F0_D7D86A320C9B079_3384__HEAD__

namespace proxy {
	class Shared
	{
		friend class SymbolCon;
	public:
		Shared();
		~Shared();
	public:
		//!@ 测试使用
		sk::container::map<INT64, shared::SystemOrder> m_TestOrderRequestQ;

		//!@ 交易记录
		sk::container::queue<sk::packet> m_RecordQ;
		//!说明 : MTUser
		//!注意 : 
		//!日期 : Wed Jun 17 14:31:34 UTC+0800 2020
		sk::container::map<UINT64, shared::UserField> m_MTUserQ;
		//!说明 : MTSymbol
		//!注意 : 
		//!日期 : Sun Aug 30 01:41:29 UTC+0800 2020
		sk::container::map<sk::stringw, shared::ConSymbolField> m_MTSymbolQ;
		//! 任务报价队列(聚合后的报价)
		sk::container::queue<shared::marketdata::MarketData> m_MarketDataUpdateFormTaskQ;
		sk::container::queue<shared::marketdata::MarketData> m_MarketDataUpdateQ;
		sk::container::map<shared::SystemID/*FullID*/, shared::marketdata::MarketData> m_MarketDataQ;

		//! ---------------------------------预初始化数据缓冲---------------------------------
		//! Feeder Symbols
		sk::container::map<shared::SystemID/*MTGatewayID*/, sk::container::map<sk::stringa,shared::ConSymbolField/*MTSymobl*/>> m_FeederSymbolQ;
		//! Gateway Symbols
		sk::container::map<shared::SystemID/*MTGatewayID*/, sk::container::map<sk::stringa,shared::ConSymbolField/*MTSymobl*/>> m_GatewaySymbolQ;
		//! LP Products
		sk::container::map<shared::SystemID/*ExchangeID*/, sk::container::set<shared::LPProduct>> m_LPProductQ;


		//! Exchange Products
		sk::container::map<shared::SystemID/*ExchangeID*/, sk::container::map<shared::SystemID/*ProductID*/, shared::ExchangeProduct>> m_ExchangeProductQ;
		//! MTDataFeed Products
		sk::container::map<shared::SystemID/*DataFeedID*/, sk::container::map<shared::SystemID/*ProductID*/, shared::MTFeederProduct>> m_MTDataFeedProductsQ;
		//! MTGateway Products
		sk::container::map<shared::SystemID/*GatewayID*/, sk::container::map<shared::SystemID/*ProductID*/, shared::MTGatewayProduct>> m_MTGatewayProductsQ;
	public:
		//! ---------------------------------运行时聚合报价缓冲---------------------------------
		sk::container::map<shared::SystemID/*桥系统ID*/, shared::marketdata::MarketData> m_MarketDataFinalQ;
		//! ---------------------------------运行时数据缓冲---------------------------------
		//! 桥系统Symbol At ID
		sk::container::map<shared::Instrument/*桥系统Symbol*/, shared::SystemID/*桥系统ID*/> m_SystemProductIDQ;
		//! 桥系统品种
		sk::container::map<shared::SystemID/*桥系统ID*/, shared::SystemProduct> m_SystemProductFinalQ;
		//! Exchange(LP)品种映射
		sk::container::map<shared::SystemID/*桥系统ID*/, sk::container::set<shared::LPProduct>> m_LPProductFinalQ;
		//! Feeder品种映射
		sk::container::map<shared::SystemID/*桥系统ID*/, sk::container::set<shared::MTFeederProduct>> m_FeederProductFinalQ;
		//! Gateway品种映射
		sk::container::map<shared::SystemID/*桥系统ID*/, sk::container::set<shared::MTGatewayProduct>> m_GatewayProductFinalQ;

		//! Symbol映射关系
		sk::container::map<shared::Instrument/*MTSymbol*/, shared::MTGatewayProduct> m_MTSymbolProdconQ;
	public:
		shared::Serial Serial() { return ::_InterlockedExchangeAdd64(&m_Serial, 1ll); }
	private:
		//!签名 : m_OrderSerial
		//!说明 : 本程序模块定单序列号
		//!注意 : 与程序模块的生命周期保存一致
		//!日期 : Fri Jun 12 01:05:41 UTC+0800 2020
		volatile shared::Serial m_Serial = 1ll;
	};



}///namespace proxy




/// /*新生联创（上海）**/
/// /*2019年11月18日 8:48:17|365**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///AFX_SKSTU_SHARED_H__D8CCC_96B3D1F0_D7D86A320C9B079_3384__TAIL__