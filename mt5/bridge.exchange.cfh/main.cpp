#include "stdafx.h"


int wmain(int argc, wchar_t* argv[])
{
	HANDLE hToken;
	if (::OpenProcessToken(::GetCurrentProcess(), TOKEN_ALL_ACCESS, &hToken)) {
		sk::Helper::SetPrivilege(hToken, SE_DEBUG_NAME, TRUE);
	}

	::SetConsoleTitleW(sk::Log::Format(L"交易所组件® cfh ***内部版本 - [{}]", sk::Helper::GetProgramVersion(sk::Helper::GetCurrentProcessName())).c_str());

#if 0
	//!@ 本程序逻辑流程在同一台硬件系统仅支持一个实例运行.
	if (!sk::Helper::ProgramInstanceMark(sk::Helper::GetCurrentProcessNameA()))
	{
		std::cout << "本程序逻辑流程在同一台硬件系统,仅支持一个实例运行." << std::endl;
		std::cout << "This program logic flow in the same hardware system to support only one instance to run" << std::endl;
		std::this_thread::sleep_for(std::chrono::seconds(5));
		return 0;
	}
#endif

#if defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	/*_CrtSetBreakAlloc(743045);*/
	if (argc > 1)
	{
		cfh::Global::GetCore()->BridgeProcessName(argv[1]);
	}
#else
	sk::Dump __Dump;
	if (argc <= 1)
	{
		return 0;
	}
	cfh::Global::GetCore()->BridgeProcessName(argv[1]);

	HMENU hmenu = GetSystemMenu(cfh::Global::GetHwnd(), false);
	RemoveMenu(hmenu, SC_CLOSE, MF_BYCOMMAND);
	DestroyMenu(hmenu);
	//ReleaseDC(hwnd, NULL);
#endif

	cfh::Global::GetCore()->Open();

	std::thread process(
		[&]() {
			do {
				sk::stringa _input;
				char c;
				while (std::cin >> std::noskipws >> c) { if ('\n' == c) break; _input.push_back(c); }
				std::cin >> std::skipws;
				sk::StringConvert::ToLowerA(_input);

				if (_input == "q" || _input == "quit" || _input == "exit")
				{
					cfh::Global::GetCore()->Close();
					cfh::Global::SystemExit(1);
					break;
				}
				else if (_input.find("order ")!=sk::stringa::npos)
				{
					std::vector<sk::stringa> parses;
					sk::Helper::OneUnknownParseA(_input, ' ', parses);
					if (parses.size() >= 2)
					{
						auto eid = cfh::Global::GetSetup()->m_SymbolProductQ.search("XAUUSD");

						shared::SystemOrder order;
						order.Key.MTLogin = 0;
						order.Key.OrdSide = shared::tradeorder::EnOrdSide::EN_ORD_SIDE_BUY;
						order.GatewayPID = 0x424020000006D;
						order.LPOrderQty = strtod(parses[1].c_str(), nullptr);
						order.OrdType = shared::tradeorder::EnOrdType::EN_ORD_TYPE_MARKET;
						order.OrdTimeInForce = shared::tradeorder::EnOrdTimeInForce::EN_ORD_TIMEINFORCE_FOK;
						order.OrderID = shared::SystemIDType::SystemOrderIDMade(0xB00000000);
						order.ExchangePID = 0x150B00000063;
						cfh::Global::GetCore()->TradeOrderGet()->OnNetTradeOrderSignal(sk::network::Header(), sk::packet((char*)&order, shared::LENSYSTEMORDER));
					}

				}
				else if (_input == "order_result")
				{
				}
				else if (_input == "order_status")
				{
					cfh::Global::GetCore()->TestOrderStatus();
				}
				else if (_input == "order_record")
				{
					cfh::Global::GetCore()->TestOrderRecord();
				}
				else if (_input == "test_request")
				{
					cfh::Global::GetCore()->TestTestRequest();
				}
				else if (_input == "order_resend_request")
				{
					cfh::Global::GetCore()->TestTestRequest();
				}
			} while (1);
		});

	do {
		if (cfh::Global::SystemExit()) break;
		std::this_thread::sleep_for(std::chrono::seconds(1));
	} while (1);

	sk::Helper::WriteConsoleInputQ();
	process.join();

	return 0;
}
