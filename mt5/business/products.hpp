#if !defined(AFX_SKSTU_ProductsMap_H__188C3_0292F006_FEA16204729545F_4C17__HEAD__)
#define AFX_SKSTU_ProductsMap_H__188C3_0292F006_FEA16204729545F_4C17__HEAD__

/**
* A basic definition of the type of product
**/


#include "systemid.hpp"

namespace shared {
#pragma pack(push,1)
	//! 货币
	typedef struct tagCurrency {
		char currency[3];
		tagCurrency() { memset(this, 0x00, sizeof(*this)); }
		tagCurrency(const char* obj) { *this = obj; }
		tagCurrency(const tagCurrency& obj) { *this = obj; }
		void operator=(const char* obj) { memcpy(currency, obj, _countof(currency)); }
		bool operator==(const char* obj) const { return strcmp(currency, obj) == 0; }
		bool operator<(const char* obj) const { return strcmp(currency, obj) < 0; }
		void operator=(const tagCurrency& obj) { memcpy(currency, obj.currency, _countof(currency)); }
		bool operator==(const tagCurrency& obj) const { return strcmp(currency, obj.currency) == 0; }
		bool operator<(const tagCurrency& obj) const { return strcmp(currency, obj.currency) < 0; }
	}Currency; const size_t LENCURRENCY = sizeof(Currency);
	//! 合约
	typedef struct tagInstrument {
		char symbol[32];
		tagInstrument() { memset(this, 0x00, sizeof(*this)); }
		tagInstrument(const char* sym) { *this = sym; }
		tagInstrument(const tagInstrument& obj) { *this = obj; }
		void operator=(const char* obj) { memcpy(symbol, obj, _countof(symbol)); }
		bool operator==(const char* obj) const { return strcmp(symbol, obj) == 0; }
		bool operator<(const char* obj) const { return strcmp(symbol, obj) < 0; }
		void operator=(const tagInstrument& obj) { memcpy(symbol, obj.symbol, _countof(symbol)); }
		bool operator==(const tagInstrument& obj) const { return strcmp(symbol, obj.symbol) == 0; }
		bool operator<(const tagInstrument& obj) const { return strcmp(symbol, obj.symbol) < 0; }
	}Instrument; const size_t LEINSTRUMENT = sizeof(Instrument);








	typedef struct tagProduct {
		SystemID ID;
		Instrument Symbol;

		tagProduct() { memset(this, 0x00, sizeof(*this)); }
		void operator=(const tagProduct& obj) { memcpy(this, &obj, sizeof(*this)); }
		bool Verify() const { return ID > 0; }
	}Product; const size_t LENPRODUCT = sizeof(tagProduct);
	__inline static bool operator<(const tagProduct& obj1, const tagProduct& obj2) { return obj1.ID < obj2.ID; }

	typedef struct tagLPProduct final : public Product {
		Instrument SystemSymbol;//! 交易所Symbol对应到系统Symbol
		Currency QuotedCCY;//! 报价货币
		double ContractSize;//! 合约乘数/合约大小
		double TickSize;//! 最小报价单位
		double MinOrderSize;//! 最小交易量
		double MaxOrderSize;//! 最大交易量
		double StepOrderSize;//! 步长

		tagLPProduct() { memset(this, 0x00, sizeof(*this)); }
		void operator=(const tagLPProduct& obj) { memcpy(this, &obj, sizeof(*this)); }
	}ExchangeProduct, LiquidityProviderProduct, LPProduct; const size_t LENLPPRODUCT = sizeof(tagLPProduct);


	typedef struct tagMTFeederProduct final : public Product {
		int SpreadAsk;//! 卖点差
		int SpreadBid;//! 买点差
		int SpreadTotal;
		bool EnableQuote;//! 启用/禁用 过桥报价

		double ContractSize;//! 合约乘数/合约大小
		UINT Digits; //! MT Symbol Digits
		double Point; //! MT Symbol Point 
		tagMTFeederProduct() { memset(this, 0x00, sizeof(*this)); EnableQuote = true; }
		void operator=(const tagMTFeederProduct& obj) { memcpy(this, &obj, sizeof(*this)); }
	}MetaQuotesFeederProduct, MTFeederProduct; const size_t LENMTFEEDERPRODUCT = sizeof(tagMTFeederProduct);
	__inline static bool operator<(const MTFeederProduct& obj1, const MTFeederProduct& obj2) { return obj1.Symbol < obj2.Symbol; }

	typedef struct tagMTGatewayProduct final : public Product {
		//!@ ID=14020000002D
		int SpreadAsk;//! 卖点差
		int SpreadBid;//! 买点差
		int SpreadTotal;
		bool EnableTrade;	//! 启用/禁用 过桥交易
		SystemID AssignOrderLP;//! 指定开仓LP

		double ContractSize;//! 合约乘数/合约大小
		UINT Digits; //! MT Symbol Digits
		double Point; //! MT Symbol Point 
		INT AutoAdjustPositionSpreadRange;//! 自动调仓点差范围
		tagMTGatewayProduct() { memset(this, 0x00, sizeof(*this)); EnableTrade = true;}
		void operator=(const tagMTGatewayProduct& obj) { memcpy(this, &obj, sizeof(*this)); }
	}MetaQuotesGatewayProduct, MTGatewayProduct,MTDealerProduct; const size_t LENMTGATEWAYPRODUCT = sizeof(tagMTGatewayProduct);
	__inline static bool operator<(const MTGatewayProduct& obj1, const MTGatewayProduct& obj2) { return obj1.Symbol < obj2.Symbol; }

	typedef struct tagSystemProduct final : public Product {
		unsigned int Decimals;//! 精度
		unsigned int SetupEnable;//! 0 禁用 !1 启用
		char Setup[2048];
		tagSystemProduct(const SystemID& id) { memset(this, 0x00, sizeof(*this)); ID = id; }
		tagSystemProduct() { memset(this, 0x00, sizeof(*this)); }
		void operator=(const tagSystemProduct& obj) { memcpy(this, &obj, sizeof(*this)); }
	}SystemProduct; const size_t LENSYSTEMPRODUCT = sizeof(tagSystemProduct);


#pragma pack(pop)







}///namespace shared




/// /*新生联创（上海）**/
/// /*2020年01月3日 9:40:26|836**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///AFX_SKSTU_ProductsMap_H__188C3_0292F006_FEA16204729545F_4C17__TAIL__
