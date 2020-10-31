#if !defined(AFX_SKSTU_SymbolCon_H__CA089_57909D93_892C4587B625670_26A6__HEAD__)
#define AFX_SKSTU_SymbolCon_H__CA089_57909D93_892C4587B625670_26A6__HEAD__

namespace proxy {

	typedef struct tagSymbolConField {
		shared::SystemProduct systemProduct;
		shared::LPProduct lpProduct;
		shared::MTFeederProduct feederProduct;
		shared::MTGatewayProduct gatewayProduct;

		tagSymbolConField() { memset(this, 0x00, sizeof(*this)); }
		void operator=(const tagSymbolConField& obj) { memcpy(this, &obj, sizeof(*this)); }
	}SymbolConField;

	class SymbolCon final {
	public:
		SymbolCon(const sk::stringa&);
		~SymbolCon();
	private:
		int Init();
		int UnInit();
	public:
		int Reload();//! ���������Ʒ��
		int Initialize();
		int Push
		(
			__in const sk::packet&,
			__out shared::SystemProduct&,
			__out sk::container::set<shared::LPProduct>&,
			__out sk::container::set<shared::MTFeederProduct>&,
			__out sk::container::set<shared::MTGatewayProduct>&
		);
		int Push(/*const shared::MarketType&,*/const sk::packet&);
		int Pop(const shared::SystemID&);
		int Finalize(); //! ���
		int Finalize(const shared::SystemID&);
		int Refresh();//! �������뱾�ػ����ļ�
		int Refresh(const shared::SystemID&);//! ����ָ����Ʒ����
		int Clear();
		size_t Count();
	public:
		std::shared_ptr<shared::SystemProduct> GetSystemProduct(const shared::SystemID&);
		std::shared_ptr<shared::LPProduct> GetLPProduct(const shared::SystemID&/*ע���������������*/);
		std::shared_ptr<shared::MTFeederProduct> GetMTFeederProduct(const shared::SystemID&);
		std::shared_ptr<shared::MTGatewayProduct> GetMTGatewayProduct(const shared::SystemID&);

		bool SetFeederProductByMTSymbol(const shared::SystemID&,const shared::ConSymbolField&);
		bool SetGatewayProductByMTSymbol(const shared::SystemID&, const shared::ConSymbolField&);

		shared::SystemID GetSystemProductID(const shared::Instrument&);
		std::vector<shared::SystemProduct> GetSystemProducts();
		std::vector<shared::LPProduct> GetLPProducts(const shared::SystemID&);
		std::vector<shared::MTFeederProduct> GetFeederProducts(const shared::SystemID&);
		std::vector<shared::MTGatewayProduct> GetGatewayProducts(const shared::SystemID&);
		std::shared_ptr<sk::packet> GetSystemSymbolConBuffer(const shared::SystemID&);
	private:
		sk::stringa m_SymbolsPath;
		std::shared_ptr<std::mutex> m_mutex;
		sk::container::map<shared::SystemID/*��ϵͳ��ƷID*/, sk::packet/*Symbol���û���*/> m_SymbolConBufferQ;
	};




}///namespace proxy




/// /*�����������Ϻ���**/
/// /*2020��01��10�� 8:50:56|096**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///AFX_SKSTU_SymbolCon_H__CA089_57909D93_892C4587B625670_26A6__TAIL__