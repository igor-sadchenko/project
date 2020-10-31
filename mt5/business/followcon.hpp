#if !defined(AFX_SKSTU_FOLLOWCON_H__A13F6_722375DB_0344DDED7BD2BDC_BB2C__HEAD__)
#define AFX_SKSTU_FOLLOWCON_H__A13F6_722375DB_0344DDED7BD2BDC_BB2C__HEAD__

#include "../common/File.hpp"

namespace shared{

	class Followcon final {
	public:
		Followcon(const char* pFileFullPath) :
			m_FileFullPath(pFileFullPath) {
			m_pMapFile = new sk::file::MapFile(m_FileFullPath.c_str(), "followcon mapping", false);
			if (Open()) { sk::Helper::ErrorFatal(__FUNCTION__, nullptr, "%s", "Module initialize failed !"); }
		}
		~Followcon() {
			Close();
			SK_DELETE_PTR(m_pMapFile);
		}
	public:
		//! 获取配置镜像
		auto GetMirror() {
			std::lock_guard<std::mutex> lock(m_mutex);
			return std::make_shared<sk::container::map<sk::stringw, FollowField>>(m_FollowconQ);
		}

		//! 针对分户系统
		//! 查询指定跟单者的配置
		auto Select(const sk::uint64& login/*user login*/) {
			std::shared_ptr<sk::container::map<shared::FollowKey, FollowField>> result;
			std::lock_guard<std::mutex> lock(m_mutex);
			m_FollowconQ.iterate(
				[&](const auto& key, auto& con) {
					if (!result) result = std::make_shared<sk::container::map<shared::FollowKey, FollowField>>();
					if (con.LoginFollow == login) {
						result->push(con.GetKey(), con);
					}
				});
			return result;
		}

		//! 搜索信号源跟单用户
		auto Search(const sk::uint64& login/*signal login*/) {
			std::lock_guard<std::mutex> lock(m_mutex);
			return m_FollowFieldQ.search(login);
		}
		//! 搜索定位到指定配置
		auto Search(const sk::uint64& loginSignal, const sk::uint64& loginFollow) {
			std::shared_ptr<FollowField> result;
			std::lock_guard<std::mutex> lock(m_mutex);
			m_FollowFieldQ.search(loginSignal,
				[&](const auto&, auto& objq) {objq.search(loginFollow,
					[&](const auto&, auto& obj) {
						result = std::make_shared<FollowField>(obj);
					}); });
			return result;
		}
		size_t GetDataSize() const {
			return m_pMapFile->GetDataLength();
		}
		int Insert(const FollowField& _Followcon,const std::function<void(const FollowField&)>& udpate_cb) {
			if (!_Followcon.Verify()) { return -1; }
			std::lock_guard<std::mutex> lock(m_mutex);
			//! 先入主业务队列
			m_FollowFieldQ.pushpush(_Followcon.LoginSignal, [&](auto& obj) {obj.push(_Followcon.LoginFollow, _Followcon); });

			if (!m_FollowconQ.push(_Followcon.GetKey(), _Followcon)) {
				if (udpate_cb) udpate_cb(_Followcon);
			}
			//! 再落地存盘
			auto pIdle = m_IdleOffsetQ.pop();
			if (pIdle) {
				m_pMapFile->Write((unsigned long)* pIdle, (char*)& _Followcon, (unsigned long)sizeof(_Followcon));
				//! 更新偏移量
				m_KeyOffsetQ.push(_Followcon.GetKeyW(), *pIdle);
			}
			else {
				m_pMapFile->Write((char*)& _Followcon, sizeof(_Followcon));
				m_KeyOffsetQ.push(_Followcon.GetKeyW(), (m_FollowconQ.size() - 1) * sizeof(FollowField));
			}
			return 0;//! 是新建
		}
		int Delete(const wchar_t* _FollowconKey) {
			std::lock_guard<std::mutex> lock(m_mutex);
			sk::uint64 signal = 0;
			sk::uint64 follow = 0;
			m_FollowconQ.pop(_FollowconKey,
				[&](const auto& key, auto& followcon) {
					follow = followcon.LoginFollow;
					signal = followcon.LoginSignal;
					m_KeyOffsetQ.pop(key,
						[&](const auto&, auto& pos) {
							m_IdleOffsetQ.push(pos);
						});
				});
			bool IsClearAt = false;
			m_FollowFieldQ.search(signal,
				[&](const auto&, auto& cons) {
					cons.pop(follow, [](const auto&, auto&) {});
					if (cons.empty()) {
						IsClearAt = true;
					}
				});
			if (IsClearAt) {
				m_FollowFieldQ.pop(signal, [](const auto&,auto&) {});
			}
			return 0;
		}
		int Reset() {
			std::lock_guard<std::mutex> lock(m_mutex);
			m_FollowconQ.clear();
			m_FollowFieldQ.clear();
			m_IdleOffsetQ.clear();
			m_KeyOffsetQ.clear();
			m_pMapFile->Clear();
			return 0;
		}
		int Clear() {
			std::lock_guard<std::mutex> lock(m_mutex);
			m_FollowconQ.clear();
			m_FollowFieldQ.clear();
			m_IdleOffsetQ.clear();
			m_KeyOffsetQ.clear();
			m_pMapFile->Clear();
			return 0;
		}
		size_t Size() {
			std::lock_guard<std::mutex> lock(m_mutex);
			return m_FollowconQ.size();
		}
	private:
		int Open() {
			if (!m_pMapFile->Open()) {
				return -1;
			}
			std::lock_guard<std::mutex> lock(m_mutex);
#if 1//! 有备用方法
			const auto Total = m_pMapFile->GetDataLength();
			if (Total % sizeof(FollowField) != 0) {
				//! 数据损坏的
				m_pMapFile->Clear();
				return 0;
			}
			if (Total > 0 && Total < sizeof(FollowField)) {
				//! 数据损坏的
				m_pMapFile->Clear();
				return 0;
			}

			auto pBegin = m_pMapFile->Read();

			auto Remain = m_pMapFile->GetDataLength();
			do {
				if (Remain <= 0) break;
				auto pField = (FollowField*)(pBegin+Total - Remain);
				if (!pField || !pField->Verify()) {
					break;
				}
				m_FollowconQ.push(pField->GetKeyW(), *pField);
				m_FollowFieldQ.pushpush(pField->LoginSignal, [&](auto& obj) {obj.push(pField->LoginFollow, *pField); });
				m_KeyOffsetQ.push(pField->GetKeyW(), Total - Remain);
				Remain -= sizeof(FollowField);
			} while (1);
#else
			auto pBegin = m_pMapFile->Read();
			if (pBegin) {
				size_t pos = 0;
				do {
					auto pField = (FollowField*)(pBegin + pos);
					if (!pField || !pField->Verify()) {
						break;
					}
					m_FollowconQ.push(pField->GetKey(), *pField);
					m_FollowFieldQ.pushpush(pField->LoginSignal, [&](auto& obj) {obj.push(pField->LoginFollow, *pField); });
					m_KeyOffsetQ.push(pField->GetKey(), pos);
					pos += sizeof(FollowField);
				} while (1);
			}
#endif
			return 0;
		}
		int Close() {
			std::lock_guard<std::mutex> lock(m_mutex);
			m_pMapFile->Clear();
			m_FollowconQ.iterate(
				[&](const auto&, auto& obj) {
					m_pMapFile->Write((char*)& obj, sizeof(obj));
				});
			m_pMapFile->Close();
			return 0;
		}
	private:
		sk::stringa m_FileFullPath;
		std::mutex m_mutex;
		sk::file::MapFile* m_pMapFile = nullptr;
		sk::container::map<sk::stringw/*key followcon*/, sk::uint64/*offset*/> m_KeyOffsetQ;/*偏移量*/
		sk::container::queue<sk::uint64/*offset*/> m_IdleOffsetQ;/*空闲位置偏移量*/
		sk::container::map<sk::stringw/*key followcon*/, FollowField> m_FollowconQ;/*配置映射*/
		sk::container::map<sk::uint64/*login signal*/, sk::container::map<sk::uint64/*login follow*/, FollowField>> m_FollowFieldQ;/*业务配置映射*/
	};


}///namespace shared




/// /*新生联创（上海）**/
/// /*2019年08月30日 13:55:46|747**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///AFX_SKSTU_FOLLOWCON_H__A13F6_722375DB_0344DDED7BD2BDC_BB2C__TAIL__