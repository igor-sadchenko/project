#pragma once

namespace manager {

	class Manager final :
		public IMTManagerSink,
		public IMTDealerSink,
		public sk::object::IObject {
	public:
		Manager();
		~Manager();
	private:
		int Init() override final;
		int UnInit() override final;
	public:
		int Open() override final;
		int Close() override final;
		IMTManagerAPI* Api() const { return m_pApi; }
	private:
		INT64 m_ServerTimeMsc = 0;
		IMTManagerAPI* m_pApi = nullptr;
		CMTManagerAPIFactory m_ApiFactory;
		MTAPIRES Connect();
	private:
		void OnConnect(void) override final;
		void OnDisconnect(void) override final;
		void OnTradeAccountSet(const MTAPIRES           /*retcode*/,
			const INT64              /*request_id*/,
			const IMTUser*           /*user*/,
			const IMTAccount*        /*account*/,
			const IMTOrderArray*     /*orders*/,
			const IMTPositionArray*  /*positions*/) override final;

		void OnDealerResult(const IMTConfirm* result) final override;
		void OnDealerAnswer(const IMTRequest* request) final override;
	public:
		MTAPIRES CreateUsers();
		MTAPIRES SymbolsGet(sk::container::map<sk::stringw, shared::ConSymbolField>&);
		MTAPIRES GroupsGet(sk::container::map<sk::stringw, shared::ConGroupField>&) ;
		MTAPIRES UsersGet(sk::container::map<UINT64, shared::UserField>&) ;
		MTAPIRES PositionsGet(sk::container::map<UINT64,shared::PositionField>&) ;
		MTAPIRES PositionCloseAt(const UINT64& login) ;

		MTAPIRES PositionNetVolumeGet(sk::container::map<shared::SystemID, sk::container::map<sk::stringw, INT64>>&);
	};



}///namespace manager