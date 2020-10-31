#if !defined(AFX_SKSTU_MARKETDATA_H__44768_01AC0E09_075A5DEBC0E8D2E_6297__HEAD__)
#define AFX_SKSTU_MARKETDATA_H__44768_01AC0E09_075A5DEBC0E8D2E_6297__HEAD__

#include "systemid.hpp"

namespace shared {
	namespace marketdata {
#pragma pack(push,1)
		typedef __int64 MDReqID;
		typedef __int64 MDTime;
		typedef double MDPrice;
		typedef double MDVolume;

		const unsigned int MD_QUOTE_LEVEL = 5;//! 五档报价


		enum struct EnCFHMDRejectReason : CHAR //!@ 281
		{
			EN_CFH_MD_REJECT_REASON_UNKNOWN_SYMBOL = '0',
			EN_CFH_MD_REJECT_REASON_DUPLICATE_MDREQID = '1',
			EN_CFH_MD_REJECT_REASON_INSUFFICIENT_PERMISSIONS = '3',
			EN_CFH_MD_REJECT_REASON_UNSUPPORTED_SUBSCRIPTIONREQUESTTYPE = '4',
		};


		typedef struct tagFinalPrice final
		{
			MDPrice Price;
			SystemID Source;/*LPID*/
			tagFinalPrice(const MDPrice& price, const SystemID& mtype)
			{
				Price = price;
				Source = mtype;
			}
			tagFinalPrice() { memset(this, 0x00, sizeof(*this)); }
			void operator=(const tagFinalPrice& obj) { memcpy(this, &obj, sizeof(*this)); }
			bool operator<(const tagFinalPrice& obj) const { return Source < obj.Source; }
		}FinalPrice;

		typedef struct tagFinalQuote final
		{
			FinalPrice Ask;
			FinalPrice Bid;
			tagFinalQuote() { memset(this, 0x00, sizeof(*this)); }
			void operator=(const tagFinalQuote& obj) { memcpy(this, &obj, sizeof(*this)); }
		}FinalQuote;

		typedef struct tagMarketData final
		{
			unsigned short head;
			MDTime GMTQuoteTimestamp;
			SystemID ID;//! 系统代码（平台唯一）/*ID全路径*/
			Instrument Symbol;/*桥系统Symbol*/
			MDPrice PriceBid[MD_QUOTE_LEVEL];//! 买价/ 出价/竞拍价
			MDVolume VolumeBid[MD_QUOTE_LEVEL];//! 买量
			MDPrice PriceOffer[MD_QUOTE_LEVEL];//! 卖价 卖家报价/报盘价
			MDVolume VolumeOffer[MD_QUOTE_LEVEL];//! 卖量
			MDPrice PriceBidFinal; //! 最优价
			MDPrice PriceOfferFinal; //! 最优价
			char Reserved[14];
			unsigned short tail;

			bool Verify() const
			{
				return head == 0xfeff && tail == 0x200b;
			}
			void operator=(const tagMarketData& obj)
			{
				memcpy(this, &obj, sizeof(*this));
				head = 0xfeff;
				tail = 0x200b;
			}
			bool operator<(const tagMarketData& obj) const { return ID < obj.ID; }
			tagMarketData() { memset(this, 0x00, sizeof(*this)); head = 0xfeff; tail = 0x200b; }
		}MarketData; const size_t LENMARKETDATA = sizeof(MarketData);
#if 0
		__inline static bool operator<(const tagMarketData& obj1, const tagMarketData& obj2) { return obj1.ID < obj2.ID; }
#endif

#if 0
		typedef struct tagQuote {
			MDPrice Buy;
			MDPrice Sell;
			MDVolume BuyVolume;
			MDVolume SellVolume;
			tagQuote() { memset(this, 0x00, sizeof(*this)); }
			void operator=(const tagQuote& obj) { memcpy(this, &obj, sizeof(*this)); }
		}QuoteNode, Quote;
#endif

		typedef struct tagMarketDataRequest final
		{
			enum struct EnMDSubscribeFlag : UINT
			{
				EN_MD_SUBSCRIBE_FLAG_SUBSCRIBE = 0,
				EN_MD_SUBSCRIBE_FLAG_UNSUBSCRIBE = 1,
			};
			EnMDSubscribeFlag MDSubscribeFlag;
			tagMarketDataRequest() { memset(this, 0x00, sizeof(*this)); }
			void operator=(const tagMarketDataRequest* pObj) { if (pObj)memcpy(this, pObj, sizeof(*this)); }
			void operator=(const tagMarketDataRequest& obj) { memcpy(this, &obj, sizeof(*this)); }
		}MarketDataRequest, MDRequest; const size_t LENMDREQUEST = sizeof(MDRequest);


		typedef struct tagExchangeMarketData final
		{
			unsigned short head;
			SystemID ID;
			MarketData MD;
			char Reserved[46];
			unsigned short tail;
			
			void operator=(const tagExchangeMarketData& obj) { memcpy(this, &obj, sizeof(*this)); head = 0xfeff; tail = 0x200b; }
			bool operator<(const tagExchangeMarketData& obj) const { return ID < obj.ID; }
			bool Verify() const { return head == 0xfeff && tail == 0x200b; }
			tagExchangeMarketData() { memset(this, 0x00, sizeof(*this)); head = 0xfeff; tail = 0x200b; }
		}ExchangeMarketData, ExchangeMarketDataDisk; const size_t LENEXCHANGEMARKETDATA = sizeof(ExchangeMarketData);







#pragma pack(pop)
	}///namespace marketdata
}///namespace shared




/// /*新生联创（上海）**/
/// /*2020年01月9日 16:40:39|145**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///AFX_SKSTU_MARKETDATA_H__44768_01AC0E09_075A5DEBC0E8D2E_6297__TAIL__