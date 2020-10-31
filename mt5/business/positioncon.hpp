#if !defined(AFX_SKSTU_POSITION_H__3FB48_EB15ADC1_8FFD5D9C9C99422_4E64__HEAD__)
#define AFX_SKSTU_POSITION_H__3FB48_EB15ADC1_8FFD5D9C9C99422_4E64__HEAD__

#include "../common/File.hpp"

namespace shared {






	class Positioncon final {
	public:
		Positioncon(const char* _FilePathName) {
			m_pMapFile = new sk::file::MapFile(_FilePathName, "mapping_position", false);
			if (Open()) { sk::Helper::ErrorFatal(__FUNCTION__, nullptr, "%s", "Module initialize failed !"); }
		}
		~Positioncon() {
			Close();
			SK_DELETE_PTR(m_pMapFile);
		}
	public:
		sk::uint64 Size() { return m_Position.size(); }
		int Insert(const sk::uint64& position_follow, const DiskPositinField& position_signal) {
			std::lock_guard<std::mutex> lock(m_mutex);

			//! 先入主业务队列
			if (!m_Position.push(position_follow, position_signal)) {
				//! 是更新
			}
			//! 再落地存盘
			auto pIdle = m_IdleOffsetQ.pop();
			if (pIdle) {
				m_pMapFile->Write((unsigned long)*pIdle, (char*)&position_signal, (unsigned long)sizeof(position_signal));
				//! 更新偏移量
				m_PositionOffsetQ.push(position_follow, *pIdle);
			}
			else {
				m_pMapFile->Write((char*)&position_signal, (unsigned long)sizeof(position_signal));
				m_PositionOffsetQ.push(position_follow, (m_Position.size() - 1) * sizeof(position_signal));
			}
			return 0;//! 是新建
		}

		int Delete(const sk::uint64& position_follow) {
			std::lock_guard<std::mutex> lock(m_mutex);
			m_Position.pop(position_follow,
				[&](const auto& key, auto& followcon) {
					m_PositionOffsetQ.pop(key, [&](const auto&, auto& pos) {
						m_IdleOffsetQ.push(pos);
						});
				});
			return 0;
		}
		//! 与系统持仓同步
		__inline void Sync(sk::container::map<sk::uint64/*login*/, IMTPosition*>& mtPositionSrc, \
			const std::function<void(const RealPositionField&)>& changed_cb
		);
		__inline void Sync(sk::container::map<sk::uint64/*login*/, PositionField>& mtPositionSrc, \
			const std::function<void(const RealPositionField&)>& changed_cb
		);

		bool Search(const sk::uint64& position_signal, sk::container::map<sk::uint64, DiskPositinField>& _OutQ) {
			if (m_Position.empty()) { return false; }
			std::lock_guard<std::mutex> lock(m_mutex);
			m_Position.iterate(
				[&](const auto& key, auto& val) {
					if (val.PositionSignal.Position == position_signal) {
						_OutQ.push(key, val);
					}});
			return true;
		}
		auto Search(const sk::uint64& position_signal) {
			std::shared_ptr<sk::container::map<sk::uint64, DiskPositinField>> result;
			if (m_Position.empty()) { return result; }

			std::lock_guard<std::mutex> lock(m_mutex);
			m_Position.iterate(
				[&](const auto& key, auto& obj) {
					if (obj.PositionSignal.Position == position_signal) {
						if (!result) { result = std::make_shared<sk::container::map<sk::uint64, DiskPositinField>>(); }
						result->push(key, obj);
					}});
			return result;
		}
		auto Search(const sk::uint64& position_signal, const std::function<void(const DiskPositinField&)>& search_cb) {
			std::shared_ptr<sk::container::map<sk::uint64, DiskPositinField>> result;
			if (m_Position.empty()) { return result; }

			std::lock_guard<std::mutex> lock(m_mutex);
			m_Position.iterate(
				[&](const auto& key, auto& obj) {
					if (search_cb && key == position_signal) {
						search_cb(obj);
					}
					if (obj.PositionSignal.Position == position_signal) {
						if (!result) { result = std::make_shared<sk::container::map<sk::uint64, DiskPositinField>>(); }
						result->push(key, obj);
					}});
			return result;
		}
	protected:
		void Reset() {
			std::lock_guard<std::mutex> lock(m_mutex);
			m_PositionOffsetQ.clear();
			m_IdleOffsetQ.clear();
			m_Position.clear();
			if (m_pMapFile) {
				m_pMapFile->Clear();
			}
		}
	private:
		int Open();
		int Close();
	private:
		std::mutex m_mutex;
		sk::file::MapFile* m_pMapFile = nullptr;
		sk::container::map<sk::uint64/*psoition follow*/, DiskPositinField> m_Position;
		sk::container::queue<sk::uint64/*offset*/> m_IdleOffsetQ;
		sk::container::map<sk::uint64/*position*/, sk::uint64> m_PositionOffsetQ;
	};

	__inline void
		Positioncon::Sync
		(
			sk::container::map<sk::uint64/*login*/, PositionField>& mtPositionSrc, \
			const std::function<void(const RealPositionField&)>& changed_cb
		)
	{
		if (mtPositionSrc.empty()) { Reset(); return; }
		std::lock_guard<std::mutex> lock(m_mutex);
		sk::container::map<sk::uint64, RealPositionField> ChangedQ;
		m_Position.iterate(
			[&](const auto& position_follow, auto& position_signal) {
				if (!mtPositionSrc.search(position_follow, [&](const auto&, auto& mtposition) {
					if (position_signal.PositionSignal.Volume != mtposition.Volume) {
						RealPositionField RealPosition;
						RealPosition = position_signal;
						RealPosition.Status = RealPositionField::EnDiskPositionFieldStatusType::EN_DISKPOSITION_FIELD_STATUS_TYPE_CHANGE;
						ChangedQ.push(position_follow, RealPosition);
					}
					}))
				{
					RealPositionField RealPosition;
					RealPosition = position_signal;
					RealPosition.Status = RealPositionField::EnDiskPositionFieldStatusType::EN_DISKPOSITION_FIELD_STATUS_TYPE_DISCARD;
					ChangedQ.push(position_follow, RealPosition);
				}
			});
		ChangedQ.iterate([&](const auto& position, auto& RealPositionObj) {
			if (changed_cb) { changed_cb(RealPositionObj); }
			if (RealPositionField::EnDiskPositionFieldStatusType::EN_DISKPOSITION_FIELD_STATUS_TYPE_DISCARD == \
				RealPositionObj.Status) {
				m_Position.pop(position, [](const auto&, auto&) {});
			}
			});

		m_pMapFile->Clear();
		m_IdleOffsetQ.clear();
		m_PositionOffsetQ.clear();
		m_Position.iterate(
			[&](const auto& position_follow, auto& position_signal) {
				if (m_pMapFile->Write((char*)&position_signal, sizeof(position_signal))) {
					auto index = m_PositionOffsetQ.size();
					m_PositionOffsetQ.push(position_follow, index);
				}
			});
		return;
	}
	__inline void
		Positioncon::Sync
		(
			sk::container::map<sk::uint64, IMTPosition*>& mtPositionSrc,
			const std::function<void(const RealPositionField&)>& changed_cb
		)
	{

		if (mtPositionSrc.empty()) { Reset(); return; }

		std::lock_guard<std::mutex> lock(m_mutex);

		sk::container::map<sk::uint64, RealPositionField> ChangedQ;

		m_Position.iterate(
			[&](const auto& position_follow, auto& position_signal) {
				if (!mtPositionSrc.search(position_follow, [&](const auto&, auto& mtposition) {
					if (position_signal.PositionSignal.Volume != mtposition->Volume()) {
						RealPositionField RealPosition;
						RealPosition = position_signal;
						RealPosition.Status = RealPositionField::EnDiskPositionFieldStatusType::EN_DISKPOSITION_FIELD_STATUS_TYPE_CHANGE;
						ChangedQ.push(position_follow, RealPosition);
					}
					}))
				{
					RealPositionField RealPosition;
					RealPosition = position_signal;
					RealPosition.Status = RealPositionField::EnDiskPositionFieldStatusType::EN_DISKPOSITION_FIELD_STATUS_TYPE_DISCARD;
					ChangedQ.push(position_follow, RealPosition);
				}
			});

		ChangedQ.iterate([&](const auto& position, auto& RealPositionObj) {
			if (changed_cb) { changed_cb(RealPositionObj); }
			if (RealPositionField::EnDiskPositionFieldStatusType::EN_DISKPOSITION_FIELD_STATUS_TYPE_DISCARD == \
				RealPositionObj.Status) {
				m_Position.pop(position, [](const auto&, auto&) {});
			}
			});

		m_pMapFile->Clear();
		m_IdleOffsetQ.clear();
		m_PositionOffsetQ.clear();
		m_Position.iterate(
			[&](const auto& position_follow, auto& position_signal) {
				if (m_pMapFile->Write((char*)&position_signal, sizeof(position_signal))) {
					auto index = m_PositionOffsetQ.size();
					m_PositionOffsetQ.push(position_follow, index);
				}
			});
		return;
	}

	__inline int Positioncon::Open() {
		if (!m_pMapFile->Open()) {
			return -1;
		}
		m_Position.clear();
		auto pData = m_pMapFile->Read();
#if 1//! 有备用方法
		auto nData = m_pMapFile->GetDataLength();
		if (pData && nData) {
			decltype(nData) total = nData;
			decltype(nData) nNode = static_cast<decltype(nData)>(sizeof(DiskPositinField));
			decltype(nData) pos = 0;
			while (total >= nNode) {
				auto pNode = (DiskPositinField*)(pData + nData - total);
				if (pNode && pNode->Verify()) {
					m_Position.push(pNode->PositionFollow.Position, *pNode);
				}
				else {
					break;
				}
				total -= nNode;
			}///while
		}///if
#else
		if (pData) {
			size_t pos = 0;
			do {
				auto pNode = (DiskPositinField*)(pData + pos);
				if (pNode && pNode->Verify()) {
					m_Position.push(pNode->PositionFollow.Position, *pNode);
				}
				else {
					break;
				}
				pos += sizeof(DiskPositinField);
			} while (1);
		}
#endif
		return 0;
	}
	__inline int Positioncon::Close() {
		if (m_pMapFile) {
			m_pMapFile->Clear();
			m_Position.iterate(
				[&](const auto& position_follow, auto& position_signal) {
					m_pMapFile->Write((char*)&position_signal, sizeof(position_signal));
				});
			m_pMapFile->Close();
		}
		return 0;
	}

}///namesapce shared




/// /*新生联创（上海）**/
/// /*2019年08月5日 20:57:51|654**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///AFX_SKSTU_POSITION_H__3FB48_EB15ADC1_8FFD5D9C9C99422_4E64__TAIL__
