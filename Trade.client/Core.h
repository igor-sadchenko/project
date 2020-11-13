#if !defined(AFX_SK_H__083733DA_EE4E_4380_867B_0FDC7DA1FB6A__HEAD__)
#define AFX_SK_H__083733DA_EE4E_4380_867B_0FDC7DA1FB6A__HEAD__

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
		Client* ClientGet() const { return m_pClient; }
	private:
		Client* m_pClient = nullptr;

	};


}///namespace local

//! @/*新生联创®（上海）*/
//! @/*Fri Nov 13 15:09:00 UTC+0800 2020*/
//! @/*___www.skstu.com___*/
#endif/*AFX_SK_H__083733DA_EE4E_4380_867B_0FDC7DA1FB6A__HEAD__*/