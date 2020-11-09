#if !defined(AFX_SK_H__A00BD58E_7CCF_4949_821D_C108DB0E7F7B__HEAD__)
#define AFX_SK_H__A00BD58E_7CCF_4949_821D_C108DB0E7F7B__HEAD__

namespace local
{
	enum struct EnWindowFunCbType : std::uint64_t
	{
		EN_FUNCB_TYPE_NULL = 0x0000,
		EN_FUNCB_TYPE_TIMER = 0x0010,
		EN_FUNCB_TYPE_IDLE = 0x0020,
		EN_FUNCB_TYPE_KEYBOARD = 0x0030,
		EN_FUNCB_TYPE_SPECIAL = 0x0040,
		EN_FUNCB_TYPE_RESHAPE = 0x0050,
		EN_FUNCB_TYPE_VISIBILITY = 0x0060,
		EN_FUNCB_TYPE_DISPLAY = 0x0070,
		EN_FUNCB_TYPE_MOUSE = 0x0080,
		EN_FUNCB_TYPE_MOTION = 0x0090,
		EN_FUNCB_TYPE_PASSIVEMOTION = 0x00A0,
		EN_FUNCB_TYPE_ENTRY = 0x00B0,
		EN_FUNCB_TYPE_KEYBOARDUP = 0x00C0,
		EN_FUNCB_TYPE_SPECIALUP = 0x00D0,
		EN_FUNCB_TYPE_JOYSTICK = 0x00E0,
		EN_FUNCB_TYPE_MENUSTATE = 0x00F0,
		EN_FUNCB_TYPE_MENUSTATUS = 0x0100,
		EN_FUNCB_TYPE_OVERLAYDISPLAY = 0x0110,
		EN_FUNCB_TYPE_WINDOWSTATUS = 0x0120,
		EN_FUNCB_TYPE_SPACEBALLMOTION = 0x0130,
		EN_FUNCB_TYPE_SPACEBALLBUTTON = 0x0140,
		EN_FUNCB_TYPE_BUTTONBOX = 0x0150,
		EN_FUNCB_TYPE_TABLEMOTION = 0x0160,
		EN_FUNCB_TYPE_TABLEBUTTON = 0x0170,
	};

	using tfWindowDisplayCb = void(__cdecl*)();
	using tfWindowTimerCb = std::function<void(unsigned int time, void (*callback)(int), int value)>;
	using tfWindowIdleCb = std::function<void(void (*callback)(void))>;
	using tfWindowKeyboardCb = std::function<void(void (*callback)(unsigned char, int, int))>;
	using tfWindowSpecialCb = std::function<void(void (*callback)(int, int, int))>;
	using tfWindowReshapeCb = std::function<void(void (*callback)(int, int))>;
	using tfWindowVisibilityCb = std::function<void(void (*callback)(int))>;
	using tfWindowMouseCb = std::function<void(void (*callback)(int, int, int, int))>;
	using tfWindowMotionCb = std::function<void(void (*callback)(int, int))>;
	using tfWindowPassiveMotionCb = std::function<void(void (*callback)(int, int))>;
	using tfWindowEntryCb = std::function<void(void (*callback)(int))>;
	using tfWindowKeyboardUpCb = std::function<void(void (*callback)(unsigned char, int, int))>;
	using tfWindowSpecialUpCb = std::function<void(void (*callback)(int, int, int))>;
	using tfWindowJoystickCb = std::function<void(void (*callback)(unsigned int, int, int, int), int pollInterval)>;
	using tfWindowMenuStateCb = std::function<void(void (*callback)(int))>;
	using tfWindowMenuStatusCb = std::function<void(void (*callback)(int, int, int))>;
	using tfWindowOverlayDisplayCb = std::function<void(void (*callback)(void))>;
	using tfWindowWindowStatusCb = std::function<void(void (*callback)(int))>;
	using tfWindowSpaceballMotionCb = std::function<void(void (*callback)(int, int, int))>;
	using tfWindowSpaceballRotateCb = std::function<void(void (*callback)(int, int, int))>;
	using tfWindowSpaceballButtonCb = std::function<void(void(*callback)(int, int))>;
	using tfWindowButtonBoxCb = std::function<void(void (*callback)(int, int))>;
	using tfWindowDialsCb = std::function<void(void (*callback)(int, int))>;
	using tfWindowTabletMotionCb = std::function<void(void (*callback)(int, int))>;
	using tfWindowTabletButtonCb = std::function<void(void (*callback)(int, int, int, int))>;

	class Window final
	{
	public:
		Window();
		~Window();
	private:
		void Init() const;
		void UnInit() const;
	public:
		void Open();
		void Close();
	private:
		std::atomic_bool m_IsOpen = false;
		std::vector<std::thread> m_Threads;
	public:
		void PostRedisplay() const;
	};
}///namespace local



//!@ /*新生联创®（上海）*/
//!@ /*Sat Nov 7 09:27:36 UTC+0800 2020*/
//!@ /*___www.skstu.com___*/
#endif /*AFX_SK_H__A00BD58E_7CCF_4949_821D_C108DB0E7F7B__HEAD__*/