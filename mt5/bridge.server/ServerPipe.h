#if !defined(AFX_SK_H__E2B15DCC_412A_4E63_83F8_F6C78340D37F__HEAD__)
#define AFX_SK_H__E2B15DCC_412A_4E63_83F8_F6C78340D37F__HEAD__

namespace proxy {
	class ServerPipe final : public sk::object::IObject, public sk::network::IPipeSpi
	{
	public:
		ServerPipe();
		~ServerPipe();
	private:
		int Init() override final;
		int UnInit() override final;
	public:
		int Open() override final;
		int Close() override final;
	};
}///namespace proxy



//!@ /*新生联创®（上海）*/
//!@ /*Sun Oct 11 06:55:47 UTC+0800 2020*/
//!@ /*___www.skstu.com___*/
#endif /*AFX_SK_H__E2B15DCC_412A_4E63_83F8_F6C78340D37F__HEAD__*/
