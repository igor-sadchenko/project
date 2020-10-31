#if !defined(AFX_SK_H__D2584F11_F722_4385_837F_FF2B2ABD6E0E__HEAD__)
#define AFX_SK_H__D2584F11_F722_4385_837F_FF2B2ABD6E0E__HEAD__

namespace proxy {
	class OrderDispatch final
	{
	public:
		OrderDispatch(const class System*);
		~OrderDispatch();
	public:
		void Ack(__in const shared::TradeTaskRouteBridge&);
		bool Req(__in const std::vector<shared::SystemOrder>&);
		bool Res(__in const std::vector<shared::SystemOrder>&);
	private:
		SKAPIRES VerifyOrder(__in const shared::SystemOrder&) const;
		void MadeOrder(__in const shared::SystemOrder&,__out std::vector<shared::TradeTaskRouteBridge>&) const;
	private:
		const System* m_pSystem = nullptr;
		std::shared_ptr<std::mutex> m_mutex = std::make_shared<std::mutex>();
		sk::container::map<shared::Serial, shared::TradeTaskRouteBridge> m_OrderMappingQ;
	public:
		sk::container::queue<shared::TradeTaskRouteBridge> m_ReplyFailedQ;
		sk::container::queue<shared::TradeTaskRouteBridge> m_ReplyQ;
		sk::container::queue<shared::TradeTaskRouteBridge> m_RequestQ;
	};



}///namespace proxy



//!@ /*新生联创®（上海）*/
//!@ /*Sat Sep 12 09:17:25 UTC+0800 2020*/
//!@ /*___www.skstu.com___*/
#endif /*AFX_SK_H__D2584F11_F722_4385_837F_FF2B2ABD6E0E__HEAD__*/