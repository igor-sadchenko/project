#if !defined(AFX_SKSTU_SYSTEM_H__E0AB7_A8BC6F7D_AD4BC6C5E1723AB_7257__HEAD__)
#define AFX_SKSTU_SYSTEM_H__E0AB7_A8BC6F7D_AD4BC6C5E1723AB_7257__HEAD__

namespace proxy {
	//!  聚合结算桥接系统® 新生联创（上海）
	class System final : public sk::object::IObject {
		friend class Server;
	public:
		System();
		~System();
	private:
		int Init() override final;
		int UnInit() override final;
	public:
		int Open() override final;
		int Close() override final;
	public:
		bool Ready() const;
		bool Suspend() const;
		Server* GetServer() const { return m_pServer; }
		QuoteFinal* FinalQuoteGet() const { return m_pQuoteFinal; }
		SymbolCon* SymbolConGet() const { return m_pSymbolCon; }
		OrderDispatch* OrderDispatchGet() const { return m_pOrderDispatch; }
		shared::DiskLogEx* RecordGet() const { return m_pRecord; }
	private:
		QuoteFinal* m_pQuoteFinal = nullptr;
		SymbolCon* m_pSymbolCon = nullptr;
		OrderDispatch* m_pOrderDispatch = nullptr;
		Server* m_pServer = nullptr;
		shared::DiskLogEx* m_pRecord = nullptr;
		std::atomic_bool m_LPProcessShowFlag = false;
	public:
		void ShowLPProcess();
	private:
		int OpenLPProceses();
		int CloseLPProceses();

		void ProcessT();
		void RecordT();
		void TradeOrderT();
		void MarketDataProcessT();
	public:
		//! 系统命令 特约命令
		int ExecSystemCommand(const sk::stringa& cmdStr);
		//! 初始化系统产品树目录/缓冲
		int ExecSystemProductsInit();
	};



}///namespace proxy




/// /*新生联创（上海）**/
/// /*2019年11月25日 18:2:41|346**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///AFX_SKSTU_SYSTEM_H__E0AB7_A8BC6F7D_AD4BC6C5E1723AB_7257__TAIL__