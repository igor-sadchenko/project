#include "stdafx.h"

int APIENTRY _tWinMain
(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nShowCmd
)
{
	sk::Dump __Dump;
#if defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(268);
#endif

	client::extern_global_hinstance = hInstance;

	if (!::HeapSetInformation(nullptr, HeapEnableTerminationOnCorruption, nullptr, 0)) { return -1; }
	auto other = ::OleInitialize(NULL);
	if (FAILED(::CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE))) { return -1; }

	HICON hIcon = ::LoadIcon(hInstance, _TEXT("IDI_ICON1"));

	client::Global::GetCore()->Open();
	std::thread([](){do {if (client::Global::IsSystemExit()){client::Global::GetCore()->Close();break;}std::this_thread::sleep_for(std::chrono::seconds(1));} while (1);}).join();

	::CoUninitialize();
	::OleUninitialize();

	return 0;


}