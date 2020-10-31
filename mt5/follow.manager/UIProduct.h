#if !defined(AFX_SKSTU_UIPRODUCT_H__F641F_C67BF4B0_5D0641478E5A45A_F092__HEAD__)
#define AFX_SKSTU_UIPRODUCT_H__F641F_C67BF4B0_5D0641478E5A45A_F092__HEAD__

namespace client {

	enum struct EnProductEditUIType : __int64 {
		EN_PRODUCT_EDIT_UI_TYPE_SYSTEM = 0x2,
		EN_PRODUCT_EDIT_UI_TYPE_EXCHANGE = 0x4,
		EN_PRODUCT_EDIT_UI_TYPE_MTFEEDER = 0x6,
		EN_PRODUCT_EDIT_UI_TYPE_MTGATEWAY = 0x8,
	};

	class IProductTreeNodeUI final : public CListContainerElementUI {
	public:
		void SetSymbol(const sk::string&);
		void SetQuotes(const shared::marketdata::MarketData&, const unsigned int&);
	};

	class IProductSystemEditUI final : public CListContainerElementUI {
	public:
		void Set(const shared::SystemProduct&,sk::container::set<shared::LPProduct>&);
		std::shared_ptr<shared::SystemProduct> Get();
	};

	class IProductExchangeEditUI final : public CListContainerElementUI {
	public:
		void Set(const shared::LPProduct&);
		std::shared_ptr<shared::LPProduct> Get();
	};

	class IProductMTGatewayEditUI final : public CListContainerElementUI {
	public:
		void Set(const shared::MTGatewayProduct&);
		std::shared_ptr<shared::MTGatewayProduct> Get();
	};

	class IProductMTFeederEditUI final : public CListContainerElementUI {
	public:
		void Set(const shared::MTFeederProduct&);
		std::shared_ptr<shared::MTFeederProduct> Get();
	};

	class IProductEditUI final : public CListUI {
	public:
		void Parse();
		sk::packet Packet();
	};

}///namespace client




/// /*新生联创（上海）**/
/// /*2020年01月12日 0:18:50|820**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///AFX_SKSTU_UIPRODUCT_H__F641F_C67BF4B0_5D0641478E5A45A_F092__TAIL__