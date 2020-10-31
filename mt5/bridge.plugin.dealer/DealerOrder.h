#if !defined(AFX_SK_H__399F6680_365C_481A_8A62_019C5059E507__HEAD__)
#define AFX_SK_H__399F6680_365C_481A_8A62_019C5059E507__HEAD__


namespace dealer {
	class DealerOrder final
	{
	public:
		DealerOrder(const shared::Serial&);
		~DealerOrder();
	public:
		void UpdateTime(const time_t& at) { m_UpdateTime.store(at); }
		time_t UpdateTime() const { return m_UpdateTime.load(); }
		const time_t& CreateTime() const { return m_CreateTime; }
		const shared::Serial& Serial() const { return m_Serial; }
		void Push(const shared::TradeTaskRouteGateway&);
		void Packet(__out std::vector<shared::SystemOrder>&) const;
		bool Response(__in const shared::SystemOrder&, __out std::vector<shared::ConfirmField>&,__out std::vector<shared::TradeTaskRouteGateway>&);
		std::vector<shared::ConfirmField> Finish(__in const EnMTAPIRetcode&);
		void Timeout(const std::function<bool/*true->clear*/(const shared::TradeTaskRouteGateway&)>&);
		void Search(const shared::HedgeKey&,const UINT&,const std::function<void(shared::TradeTaskRouteGateway&)>&);
	private:
		void Clear();
		void ClearAt(const shared::HedgeKey&);
		size_t Count() const;
		size_t CountAt(const shared::HedgeKey&) const;
		size_t Total() const;
		std::vector<shared::TradeTaskRouteGateway> Pop(const shared::HedgeKey&);
	private:	
		const shared::Serial m_Serial;
		std::atomic<time_t> m_UpdateTime = sk::Helper::TickCountGet<std::chrono::milliseconds>();
		const time_t m_CreateTime = sk::Helper::TickCountGet<std::chrono::milliseconds>();
		sk::container::map<shared::HedgeKey, sk::container::map<UINT, shared::TradeTaskRouteGateway>> m_DealerOrderQ;
		std::shared_ptr<std::mutex> m_mutex = std::make_shared<std::mutex>();
	};
}///namespace dealer







///!@ 新生联创®（上海）
///!@ Fri Jun 12 17:10:30 UTC+0800 2020
///!@ ___www.skstu.com___
#endif///AFX_SK_H__399F6680_365C_481A_8A62_019C5059E507__HEAD__
