#if !defined(AFX_SK_H__7B3C3AA0_92FF_4182_8D29_225077A270EE__HEAD__)
#define AFX_SK_H__7B3C3AA0_92FF_4182_8D29_225077A270EE__HEAD__

namespace local {

	class Core final : public sk::object::IObject
	{
	public:
		Core();
		virtual ~Core();
	private:
		int Init() override final;
		int UnInit() override final;
	public:
		int Open() override final;
		int Close() override final;
	public:
		Server* ServerGet() const { return m_pServer; }
	private:
		Server* m_pServer = nullptr;

	};

}///namespace local


//!@ /*新生联创®（上海）*/
//!@ /*Thu Nov 12 11:53:55 UTC+0800 2020*/
//!@ /*___www.skstu.com___*/
#endif /*AFX_SK_H__7B3C3AA0_92FF_4182_8D29_225077A270EE__HEAD__*/