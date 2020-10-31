#if !defined(AFX_SKSTU_DEALER_DISPATCH_H__B82D8_735C26F8_20A4AAEFA47E73B_1835__HEAD__)
#define AFX_SKSTU_DEALER_DISPATCH_H__B82D8_735C26F8_20A4AAEFA47E73B_1835__HEAD__

namespace shared {
	typedef struct tagDealerSendPacket {
		sk::uint64 serial;
		sk::uint64 batch;
		FollowRouteField route;
		sk::int64 time_create;
		tagDealerSendPacket() {
			time_create = ::time(0);
			serial = 0;
			batch = 0;
		}
	}DealerSendPacket, DealerSendField;

	class DealerBatch final {
		friend class DealerDispatch;
	public:
		DealerBatch() {}
		~DealerBatch() {}
	public:
		void EntryMOD(const FollowRouteField& Field) {
			std::lock_guard<std::mutex> lock(m_mutex);
			EntryMOD_.push(Field);
		}
		void EntryIN(const FollowRouteField& Field) {
			std::lock_guard<std::mutex> lock(m_mutex);
			EntryIN_.push(Field);
		}
		void EntryOUT(const FollowRouteField& Field) {
			std::lock_guard<std::mutex> lock(m_mutex);
			EntryOUT_.push(Field);
		}
		auto Total() {
			std::lock_guard<std::mutex> lock(m_mutex);
			return EntryIN_.size() + EntryOUT_.size() + EntryMOD_.size();
		}
	private:
		std::mutex m_mutex;
		sk::uint64 m_signal_data_key = 0;
		std::queue<FollowRouteField> EntryIN_;
		std::queue<FollowRouteField> EntryOUT_;
		std::queue<FollowRouteField> EntryMOD_;
	};

	class DealerDispatch final {
	private:
		struct Batch {
			sk::uint64 batch;
			std::queue<DealerSendPacket> EntryIN_;
			std::queue<DealerSendPacket> EntryOUT_;
			std::queue<DealerSendPacket> EntryMOD_;
			bool Empty() const {
				return EntryIN_.empty() && EntryOUT_.empty() && EntryMOD_.empty();
			}
			auto Size() const {
				return EntryIN_.size() + EntryOUT_.size() + EntryMOD_.size();
			}
			Batch() { batch = 0; }
		};
	public:
		DealerDispatch(sk::uint64 serial = 0, sk::uint64 batch = 0, sk::uint16 max_node = 127) :
			m_serial_number(serial),
			m_batch_number(batch),
			m_max_node(max_node) {
		}
		~DealerDispatch() {
		}
	public:
		/**
		*	按先平仓、后开仓、的顺序排好序入栈
		*/
		sk::uint64 Push(DealerBatch& batch) {
			std::lock_guard<std::mutex> lock(m_mutex);

			if (!batch.EntryMOD_.empty())
			{
				Batch Batch_;
				do {
					if (batch.EntryMOD_.empty())
					{
						break;
					}
					DealerSendPacket req;
					req.serial = m_serial_number.load();
					req.batch = m_batch_number.load();
					req.time_create = ::time(0);
					req.route = batch.EntryMOD_.front();
					Batch_.EntryMOD_.push(req);
					batch.EntryMOD_.pop();
					++m_serial_number;
				} while (1);

				Batch_.batch = m_batch_number.load();
				m_BatchQ.push(Batch_.batch, Batch_);
				++m_batch_number;
			}

			if (!batch.EntryIN_.empty())
			{
				Batch Batch_;
				do {
					if (batch.EntryIN_.empty())
					{
						break;
					}
					DealerSendPacket req;
					req.serial = m_serial_number.load();
					req.batch = m_batch_number.load();
					req.time_create = ::time(0);
					req.route = batch.EntryIN_.front();
					Batch_.EntryIN_.push(req);
					batch.EntryIN_.pop();
					++m_serial_number;
				} while (1);

				Batch_.batch = m_batch_number.load();
				m_BatchQ.push(Batch_.batch, Batch_);
				++m_batch_number;
			}


			if (!batch.EntryOUT_.empty())
			{
				Batch Batch_;
				do {
					if (batch.EntryOUT_.empty())
					{
						break;
					}
					DealerSendPacket req;
					req.serial = m_serial_number.load();
					req.batch = m_batch_number.load();
					req.time_create = ::time(0);
					req.route = batch.EntryOUT_.front();
					Batch_.EntryOUT_.push(req);
					batch.EntryOUT_.pop();
					++m_serial_number;
				} while (1);

				Batch_.batch = m_batch_number.load();
				m_BatchQ.push(Batch_.batch, Batch_);
				++m_batch_number;
			}

			return m_batch_number.load();
		}
		/**
		*	按先平仓、后开仓、每次只取一个序列、每次取大包数127、出栈
		*	平仓的请求先出栈，如果未有平仓请求则开仓、出栈
		*/
		std::shared_ptr<sk::container::list<DealerSendPacket>> Pop() {

			std::lock_guard<std::mutex> lock(m_mutex);
			std::shared_ptr<sk::container::list<DealerSendPacket>> result;
			if (m_BatchQ.empty())
			{
				return result;
			}

			std::vector<sk::uint64> clear;
			m_BatchQ.iterate(
				[&](const auto& batch_id, auto& batch, const auto& iterate_index, auto& iterate_break)
				{
					if (!batch.Empty())
					{
						if (!result)
						{
							result = std::make_shared<sk::container::list<DealerSendPacket>>();
						}
						std::size_t total_pop = 0;

						while (!batch.EntryMOD_.empty())
						{
							if (total_pop >= m_max_node)
							{
								break;
							}
							auto node = batch.EntryMOD_.front();
							result->push_back(node);
							batch.EntryMOD_.pop();
							total_pop++;
						}

						if (result->empty())
						{
							while (!batch.EntryOUT_.empty())
							{
								if (total_pop >= m_max_node)
								{
									break;
								}
								auto node = batch.EntryOUT_.front();
								result->push_back(node);
								batch.EntryOUT_.pop();
								total_pop++;
							}
						}

						if (result->empty()) {
							while (!batch.EntryIN_.empty())
							{
								if (total_pop >= m_max_node)
								{
									break;
								}
								auto node = batch.EntryIN_.front();
								result->push_back(node);
								batch.EntryIN_.pop();
								total_pop++;
							}
						}

						if (batch.Empty())
						{
							clear.emplace_back(batch_id);
						}
						iterate_break = true;
					}
					else
					{
						clear.emplace_back(batch_id);
					}
				});

			for (const auto& node : clear)
			{
				m_BatchQ.pop(node, [](const auto&, auto&) {});
			}
			return result;
		}

		bool Empty() {
			std::lock_guard<std::mutex> lock(m_mutex);
			return m_BatchQ.empty();
		}


		std::size_t Size() {
			std::lock_guard<std::mutex> lock(m_mutex);
			return m_BatchQ.size();
		}
	private:
		std::mutex m_mutex;
		std::atomic<sk::uint64> m_serial_number;
		std::atomic<sk::uint64> m_batch_number;
		sk::uint16 m_max_node;
		sk::container::map<sk::uint64, Batch> m_BatchQ;
	};





}///namespace shared




/// /*新生联创（上海）**/
/// /*2019年08月7日 9:31:43|493**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///AFX_SKSTU_DEALER_DISPATCH_H__B82D8_735C26F8_20A4AAEFA47E73B_1835__TAIL__