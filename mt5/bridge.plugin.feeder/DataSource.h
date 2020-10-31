#if !defined(AFX_SK_H__2B76E365_904A_437B_8552_C9B09301D424__HEAD__)
#define AFX_SK_H__2B76E365_904A_437B_8552_C9B09301D424__HEAD__

namespace feeder {

	class DataSource final : public sk::network::INetworkSpi, public sk::object::IObject
	{
	public:
		DataSource();
		~DataSource();
	private:
		int Init() override final;
		int UnInit() override final;
	public:
		int Open() override final;
		int Close() override final;
	private:
		void ProcessT();
	protected:
		void OnTcpClientConnect() override final;
		void OnTcpClientDisconnect() override final;
		void OnTcpClientRead(const sk::network::Header&, const sk::packet&) override final;
		void OnTcpClientWelcome(const sk::network::Header&, const sk::packet&) override final;
	private:
		void OnNetProductsGet(const sk::network::Header&, const sk::packet&);
		void OnNetLoginResult(const sk::network::Header&, const sk::packet&);
		void OnSystemProducts(const sk::network::Header&, const sk::packet&);
		void OnNetSystemSetupUpdate(const sk::network::Header&, const sk::packet&);
		void OnNetMarketDataUpdate(const sk::network::Header&, const sk::packet&);
		void OnNetSystemTimeUpdate(const sk::network::Header&, const sk::packet&);
		void ONetProductConUpdate(const sk::network::Header&, const sk::packet&);

		int NetLoginRequest() const;
		int NetProductsSend() const;
		int NetSystemProductsRequest() const;
	private:
		std::atomic_bool ReadyLoginVeirfy = false;
	public:
		bool Ready() const;
	};




}///namespace feeder









///!@ 新生联创®（上海）
///!@ Thu May 28 14:14:57 UTC+0800 2020
///!@ ___www.skstu.com___
#endif///AFX_SK_H__2B76E365_904A_437B_8552_C9B09301D424__HEAD__
