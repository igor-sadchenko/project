#include "stdafx.h"


int wmain(int argc, wchar_t* argv[])
{
	HANDLE hToken;
	if (::OpenProcessToken(::GetCurrentProcess(), TOKEN_ALL_ACCESS, &hToken)) {
		sk::Helper::SetPrivilege(hToken, SE_DEBUG_NAME, TRUE);
	}

	::SetConsoleTitleW(sk::Log::Format(L"交易所组件® lmax ***内部版本 - [{}]", sk::Helper::GetProgramVersion(sk::Helper::GetCurrentProcessName())).c_str());

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
		lmax::Global::GetCore()->BridgeProcessName(argv[1]);
	}
#else
	sk::Dump __Dump;
	if (argc <= 1)
	{
		return 0;
	}
	lmax::Global::GetCore()->BridgeProcessName(argv[1]);

	HMENU hmenu = GetSystemMenu(lmax::Global::GetHwnd(), false);
	RemoveMenu(hmenu, SC_CLOSE, MF_BYCOMMAND);
	DestroyMenu(hmenu);
	//ReleaseDC(hwnd, NULL);
#endif

	lmax::Global::GetCore()->Open();

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
					lmax::Global::GetCore()->Close();
					lmax::Global::SystemExit(1);
					break;
				}
				else if (_input.find("order ") != sk::stringa::npos)
				{
					std::vector<sk::stringa> parses;
					sk::Helper::OneUnknownParseA(_input, ' ', parses);
					if (parses.size() < 3)
						break;

#if 0
					for (int i = 0; i < 100; ++i)
					{
						//ExchangeProdID(4200A0001891D),GatewayProdID(424020000007E)
						//lmax::Global::GetCore()->TestOrder();
						shared::SystemOrder order;
						order.Key.MTLogin = 0;
						order.Key.OrdSide = shared::tradeorder::EnOrdSide::EN_ORDER_SIDE_BUY;
						order.GatewayPID = 0x424020000007E;
						order.LPOrderQty = strtod(parses[1].c_str(), nullptr);
						order.OrderID = shared::SystemIDType::SystemOrderIDMade(0xA00000000);
						order.ExchangePID = 0x4200A0001891D;
						lmax::Global::GetCore()->TradeOrderGet()->OnNetTradeOrderSignal(sk::network::Header(), sk::packet((char*)&order, shared::LENSYSTEMORDER));
					}

					for (int i = 0; i < 100; ++i)
					{
						//ExchangeProdID(4200A0001891D),GatewayProdID(424020000007E)
						//lmax::Global::GetCore()->TestOrder();
						shared::SystemOrder order;
						order.Key.MTLogin = 0;
						order.Key.OrdSide = shared::tradeorder::EnOrdSide::EN_ORDER_SIDE_SELL;
						order.GatewayPID = 0x424020000007E;
						order.LPOrderQty = strtod(parses[1].c_str(), nullptr);
						order.OrderID = shared::SystemIDType::SystemOrderIDMade(0xA00000000);
						order.ExchangePID = 0x4200A0001891D;
						lmax::Global::GetCore()->TradeOrderGet()->OnNetTradeOrderSignal(sk::network::Header(), sk::packet((char*)&order, shared::LENSYSTEMORDER));
					}
#endif

					libfix::_D__ v;
					v._11 = shared::SystemIDType::SystemOrderIDToStr(shared::SystemIDType::SystemOrderIDMade(0xA00000000));
					//!@ BTCUSD 100934  
					//!@ EURUSD 4001  
					//!@ XRPUSD 100938
					//!@ XLCJPY 100943
					v._48 = tostringa(100943);

					double ask = 0;
					double bid = 0;
					//double cumPx = lmax::Global::GetCore()->MarketDataGet()->MDCacheGet()->Search(
					//	100943,
					//	[&](const shared::marketdata::MarketData& md,auto&)
					//	{
					//		ask = md.PriceOffer[0];
					//		bid = md.PriceBid[0];
					//	});

					v._22 = "8";
					v._38 = tostringa(strtod(parses[1].c_str(), nullptr));
					v._54 = parses[2][0];
					v._40 = CHAR(shared::tradeorder::EnOrdType::EN_ORD_TYPE_LIMIT);

					switch (shared::tradeorder::EnOrdType(v._40[0]))
					{
					case shared::tradeorder::EnOrdType::EN_ORD_TYPE_MARKET:
					{

					}break;
					case shared::tradeorder::EnOrdType::EN_ORD_TYPE_LIMIT:
					{
						switch (shared::tradeorder::EnOrdSide(v._54[0]))
						{
						case shared::tradeorder::EnOrdSide::EN_ORD_SIDE_BUY:
						{
							v._44 = tostringa(ask);
						}break;
						case shared::tradeorder::EnOrdSide::EN_ORD_SIDE_SELL:
						{
							v._44 = tostringa(bid);
						}break;
						}
					}break;
					case shared::tradeorder::EnOrdType::EN_ORD_TYPE_STOP:
					{
						switch (shared::tradeorder::EnOrdSide(v._54[0]))
						{
						case shared::tradeorder::EnOrdSide::EN_ORD_SIDE_BUY:
						{
							v._99 = tostringa(ask+1);
						}break;
						case shared::tradeorder::EnOrdSide::EN_ORD_SIDE_SELL:
						{
							v._99 = tostringa(bid+1);
						}break;
						}
					}break;
					case shared::tradeorder::EnOrdType::EN_ORD_TYPE_STOPLIMIT:
					{
						switch (shared::tradeorder::EnOrdSide(v._54[0]))
						{
						case shared::tradeorder::EnOrdSide::EN_ORD_SIDE_BUY:
						{
							v._44 = tostringa(ask);
							v._99 = tostringa(ask +1);
						}break;
						case shared::tradeorder::EnOrdSide::EN_ORD_SIDE_SELL:
						{
							v._44 = tostringa(bid);
							v._99 = tostringa(bid+1);
						}break;
						}
					}break;
					default:
					{

					}break;
					}
					v._60 = lmax::Global::GetCore()->TradeOrderGet()->FixApi()->TransactTimeNow();
					v._59 = CHAR(shared::tradeorder::EnOrdTimeInForce::EN_ORD_TIMEINFORCE_IOC);
					UINT FixSeqNum = 0;
					if (!lmax::Global::GetCore()->TradeOrderGet()->FixApi()->FixRequest(libfix::EnFixMessageType::EN_FIX_MESSAGE_TYPE_D, TypeToPtr(&v), FixSeqNum))
					{
						auto success = 0;
					}
					else 
					{
						auto failed = 0;
					}



				}
				else if (_input == "order_result")
				{
				}
				else if (_input == "order_status")
				{
					lmax::Global::GetCore()->TestOrderStatus();
				}
				else if (_input == "order_record")
				{
					lmax::Global::GetCore()->TestOrderRecord();
				}
				else if (_input == "test_request")
				{
					lmax::Global::GetCore()->TestTestRequest();
				}
				else if (_input == "order_resend_request")
				{
					lmax::Global::GetCore()->TestTestRequest();
				}
			} while (1);
		});

	do {
		if (lmax::Global::SystemExit()) break;
		std::this_thread::sleep_for(std::chrono::seconds(1));
	} while (1);

	sk::Helper::WriteConsoleInputQ();
	process.join();

	return 0;
}
