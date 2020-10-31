#if !defined(AFX_SKSTU_REQUESTID_H__788E9_B124E384_FAC4F628A49893D_1B89__HEAD__)
#define AFX_SKSTU_REQUESTID_H__788E9_B124E384_FAC4F628A49893D_1B89__HEAD__

namespace shared {

	class RequestID final {
	public:
		RequestID() {}
		~RequestID() {}
	public:
		void operator=(const unsigned long long& obj) {
			::InterlockedExchange(&m_ReqID, obj);
		}
		unsigned long long operator++(int) {
			return ::InterlockedExchangeAdd(&m_ReqID, 1);
		}
		unsigned long long get() {
			return ::InterlockedExchangeAdd(&m_ReqID, 0);
		}
	private:
		unsigned long long m_ReqID = 0;
	};



}




/// /*新生联创（上海）**/
/// /*2019年11月20日 10:6:31|580**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///AFX_SKSTU_REQUESTID_H__788E9_B124E384_FAC4F628A49893D_1B89__TAIL__