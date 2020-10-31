#pragma once

namespace manager {

	class Core final : public sk::object::IObject {
	public:
		Core();
		~Core();
	private:
		int Init() override final;
		int UnInit() override final;
	public:
		int Open() override final;
		int Close() override final;
	public:
		System* SystemGet() const { return m_pSystem; }
		Manager* ManagerGet() const { return m_pManager; }
		void PositionCloseWhile(const bool& flag) { m_PositionCloseWhile.store(flag); }
		const bool PositionCloseWhile() const { return m_PositionCloseWhile.load(); }
		void StatusManager(const sk::network::EnNetStatus& status) { m_StatusManager.store(status); }
		sk::network::EnNetStatus StatusManager() const { return m_StatusManager.load(); }
	private:
		std::atomic<sk::network::EnNetStatus> m_StatusManager = sk::network::EnNetStatus::EN_NETSTATUS_DISCONNECTED;
		void ManagerT();
		void ManagerProcessT();
		Manager* m_pManager = nullptr;
		System* m_pSystem = nullptr;
		sk::Timer m_Timer;
		std::atomic_bool m_PositionCloseWhile = false;
		static int OnTimer(const sk::int64&, const sk::intptr&, const sk::int64&);
	};


}///namespace manager