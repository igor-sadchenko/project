#if !defined(AFX_SKSTU_ServerHttp_H__AD268_AD9FD392_DA15F544CB157DD_3973__HEAD__)
#define AFX_SKSTU_ServerHttp_H__AD268_AD9FD392_DA15F544CB157DD_3973__HEAD__



namespace followplugin {

	class ServerHttp final : public sk::object::IObject, public sk::network::INetworkSpi {
	public:
		ServerHttp();
		~ServerHttp();
	private:
		int Init() override final;
		int UnInit() override final;
	public:
		int Open() override final;
		int Close() override final;
	public:
		sk::network::INetworkApi* NtApi() const { return m_pNtApi; }
	private:
		sk::network::INetworkApi* m_pNtApi = nullptr;
		void OnHttpRead(const  sk::network::HttpPacket&, sk::packet&) override final;
	};




}///namespace followplugin



/// /*新生联创（上海）**/
/// /*2020年01月16日 17:2:15|851**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///AFX_SKSTU_ServerHttp_H__AD268_AD9FD392_DA15F544CB157DD_3973__TAIL__