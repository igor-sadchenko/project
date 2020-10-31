#if !defined(AFX_SK_H__4CD59B4D_A319_4040_8A8B_09C10846EF1F__HEAD__)
#define AFX_SK_H__4CD59B4D_A319_4040_8A8B_09C10846EF1F__HEAD__

namespace manager {

	class System final
	{
	public:
		System();
		~System();
	public:		
		//!签名 : 
		//!说明 : 卸载指定进程名的指定DLL名字的DLL模块
		//!注意 : 
		//!日期 : Mon Aug 31 18:20:19 UTC+0800 2020
		BOOL UnLoadDllAt(const sk::string&, const sk::string&) const;
	};

}///namespace manager


//!@ /*新生联创®（上海）*/
//!@ /*Mon Aug 31 18:18:39 UTC+0800 2020*/
//!@ /*___www.skstu.com___*/
#endif /*AFX_SK_H__4CD59B4D_A319_4040_8A8B_09C10846EF1F__HEAD__*/