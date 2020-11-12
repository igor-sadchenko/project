#if !defined(AFX_SK_H__7ADB7632_9FF8_4015_9F46_FB23A74A8C43__HEAD__)
#define AFX_SK_H__7ADB7632_9FF8_4015_9F46_FB23A74A8C43__HEAD__

namespace local {

	class Server final : public fix::IFixSpi , public sk::object::IObject
	{
	public:
		Server();
		virtual ~Server();
	private:
		int Init() override final;
		int UnInit() override final;
	public:
		int Open() override final;
		int Close() override final;

	};


}///namespace local



//!@ /*新生联创®（上海）*/
//!@ /*Thu Nov 12 11:49:50 UTC+0800 2020*/
//!@ /*___www.skstu.com___*/
#endif /*AFX_SK_H__7ADB7632_9FF8_4015_9F46_FB23A74A8C43__HEAD__*/