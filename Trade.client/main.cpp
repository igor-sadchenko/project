#include "stdafx.h"

int WINAPI wWinMain
(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nShowCmd
)
{
#if defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	/*_CrtSetBreakAlloc(743045);*/
#else
	sk::Dump __Dump;
#endif
	HANDLE hToken;
	if (::OpenProcessToken(::GetCurrentProcess(), TOKEN_ALL_ACCESS, &hToken))
	{
		sk::Helper::SetPrivilege(hToken, SE_DEBUG_NAME, TRUE);
	}

#if defined(_DEBUG)
	HINSTANCE hInstanceRes = ::LoadLibraryA(R"(..\..\..\..\shared\x64\debug\shared.resource.dll)");
#else
	HINSTANCE hInstanceRes = ::LoadLibraryA((proxy::Global::GetSetup()->GetApiPath() + "shared.resource.dll").c_str());
#endif


#if 1
	client::FrameWnd* pFrameWnd = new client::FrameWnd(
		hInstance,
		hInstanceRes,
		sk::Log::Format(L"XS® Bridge Server ***Build - [{}]", \
			sk::Helper::GetProgramVersion(sk::Helper::GetCurrentProcessName())));

	pFrameWnd->CreateFrame(
		{
			{::GlobalAddAtom(sk::StringConvert::MBytesToTString(sk::UUID().uuid()).c_str()) - 0xC000,MAKELPARAM(INT('H'),MOD_ALT | MOD_CONTROL | MOD_NOREPEAT)},
			{::GlobalAddAtom(sk::StringConvert::MBytesToTString(sk::UUID().uuid()).c_str()) - 0xC000,MAKELPARAM(INT('T'),MOD_ALT | MOD_CONTROL | MOD_NOREPEAT)},
			{::GlobalAddAtom(sk::StringConvert::MBytesToTString(sk::UUID().uuid()).c_str()) - 0xC000,MAKELPARAM(INT('E'),MOD_ALT | MOD_CONTROL | MOD_NOREPEAT)},
		}

	);

	pFrameWnd->MessageLoop();

	std::this_thread::sleep_for(std::chrono::milliseconds(3888));
	SK_DELETE_PTR(pFrameWnd);
#endif

	return 0;
}