#if !defined(AFX_SK_H__8CAE1FA8_6B68_48EE_8296_870E8F18A0B5__HEAD__)
#define AFX_SK_H__8CAE1FA8_6B68_48EE_8296_870E8F18A0B5__HEAD__

namespace local {

	class Client final : public fix::IFixSpi, public sk::object::IObject
	{
	public:
		Client();
		virtual ~Client();
	private:
		int Init() override final;
		int UnInit() override final;
	public:
		int Open() override final;
		int Close() override final;

	};

}///namespace local

//! @/*新生联创®（上海）*/
//! @/*Fri Nov 13 15:10:55 UTC+0800 2020*/
//! @/*___www.skstu.com___*/
#endif/*AFX_SK_H__8CAE1FA8_6B68_48EE_8296_870E8F18A0B5__HEAD__*/


