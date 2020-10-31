#include "stdafx.h"

enum struct EnCmdType : __int64
{
	EN_CMD_TYPE_POSITION_OPEN = 0x10001,
	EN_CMD_TYPE_POSITION_CLOSE = 0x10002,
	EN_CMD_TYPE_POSITION_NET_VOLUME = 0x10003,
	EN_CMD_TYPE_POSITION_CLOSE_WHILE = 0x10004,
	EN_CMD_TYPE_FOLLOW_CREATE = 0x10005,
	EN_CMD_TYPE_FOLLOW_CREATE_USERS = 0x10006,
	EN_CMD_TYPE_PARSING_MT5_POSITION_DAT = 0x10007,
	EN_CMD_TYPE_PARSING_MT5_ORDER_DAT = 0x10008,
	EN_CMD_TYPE_PARSING_UNLOAD_DLL = 0x10009,
};




int _tmain(int argc, char* argv[])
{

	HANDLE hToken;
	if (::OpenProcessToken(::GetCurrentProcess(), TOKEN_ALL_ACCESS, &hToken)) {
		sk::Helper::SetPrivilege(hToken, SE_DEBUG_NAME, TRUE); 
	}

#if defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(268);
#endif

	sk::Dump __Dump;

	manager::Global::Log(sk::EnLogType::EN_LOG_TYPE_WARN)->warn
	(
		"\n命令行使用说明:\n{}",
		"'/10002'\t\t指定Login平仓(例: /10002 10001(回车))\n\
'/10003'\t\t统计净额持仓(例: /10003(回车))\n\
'/10009'\t\t卸载MT5交易服务器占用的团队项目通用库(例: /10009(回车))\n\
'q'\t\t\t退出本系统\n\
");


	manager::Global::CoreGet()->Open();


	do {
		sk::stringa cin_;
		char c;
		while (std::cin >> std::noskipws >> c)
		{
			if ('\n' == c) break;
			cin_.push_back(c);
		}
		std::cin >> std::skipws;

		sk::StringConvert::ToLowerA(cin_);

		std::vector<sk::stringa> cmds;

		sk::Helper::OneUnknownParseA(cin_, ' ', cmds);

		if (cmds.empty())
			continue;

		sk::stringa cmd = cmds[0];
		sk::stringa param;
		if (cmds.size() > 1)
		{
			param = cmds[1];
		}
		
		if (cmd == "q" || cmd == "/q" || cmd == "/quit")
		{
			manager::Global::CoreGet()->Close();
			break;
		}
		else
		{
			auto nCmd = ::strtoll(cmd.c_str()+1,nullptr,16);

			switch (EnCmdType(nCmd))
			{
			case EnCmdType::EN_CMD_TYPE_PARSING_UNLOAD_DLL:
			{
				for (int i = 0; i < 5; ++i)
				{
					manager::Global::CoreGet()->SystemGet()->UnLoadDllAt(_TEXT("mt5trade64.exe"), _TEXT("skapi.network.dll"));
					std::this_thread::sleep_for(std::chrono::milliseconds(100));
				}

				for (int i = 0; i < 5; ++i)
				{
					manager::Global::CoreGet()->SystemGet()->UnLoadDllAt(_TEXT("mt5history64.exe"), _TEXT("skapi.network.dll"));
					std::this_thread::sleep_for(std::chrono::milliseconds(100));
				}
			}break;
			case EnCmdType::EN_CMD_TYPE_POSITION_CLOSE:
			{
				if (param.empty()) break;
				UINT64 login = std::stoull(param);
				manager::Global::CoreGet()->ManagerGet()->PositionCloseAt(login);
			}break;
			case EnCmdType::EN_CMD_TYPE_POSITION_NET_VOLUME:
			{
				sk::container::map<shared::SystemID,sk::container::map<sk::stringw, INT64>> ExchangePositionVolume;
				manager::Global::CoreGet()->ManagerGet()->PositionNetVolumeGet(ExchangePositionVolume);

				sk::container::map<sk::stringw, shared::ConSymbolField> Symbols;
				manager::Global::CoreGet()->ManagerGet()->SymbolsGet(Symbols);
#if 1
				sk::stringa output;
				if (ExchangePositionVolume.empty())
				{
					manager::Global::Log(sk::EnLogType::EN_LOG_TYPE_WARN)->warn("MT没有持仓.");
					break;
				}

				ExchangePositionVolume.iterate(
					[&](const auto& exchangeID, auto& positionVolumes)
					{
						output.append("\n");
						output.append(sk::Log::Format("{:X}\n", exchangeID));
						positionVolumes.iterate(
							[&](const auto& symbol, auto& totalVol)
							{
								if (totalVol != 0)
								{
									Symbols.search(
										symbol,
										[&](const auto&, shared::ConSymbolField& symbolField)
										{
											auto sNetVol = sk::Helper::FloatToStrA(totalVol / 10000.0 * (symbolField.ContractSize / 10000.0), 3, false);
											output.append(sk::StringConvert::WStringToMBytes(symbol)).append("\t\t\t").append(sNetVol).append("\n");
										});
								}
							});
					});


				if (output.empty())
				{
					manager::Global::Log(sk::EnLogType::EN_LOG_TYPE_WARN)->warn("没有符合条件的持仓({})", param);
				}
				else
				{
					output.insert(0,"\n");
					manager::Global::Log(sk::EnLogType::EN_LOG_TYPE_SUCCESS)->info(output);
				}
#endif
			}break;
			case EnCmdType::EN_CMD_TYPE_POSITION_CLOSE_WHILE:
			{
				if (manager::Global::CoreGet()->PositionCloseWhile())
				{
					manager::Global::CoreGet()->PositionCloseWhile(false);
				}
				else
				{
					manager::Global::CoreGet()->PositionCloseWhile(true);
				}
			}break;
			case EnCmdType::EN_CMD_TYPE_FOLLOW_CREATE:
			{
				auto pFollowConObjQ = new shared::Memmap<shared::FollowKey, shared::FollowField>("D:\\follow.con.bin", "follow.con.bin", false);
				pFollowConObjQ->Clear();
				auto alsdfkjadlskjf = &manager::Global::SharedGet()->m_MTUsersQ;
				//manager::Global::CoreGet()->ManagerGet()->
				shared::FollowField follow;
				follow.Default();
				follow.LoginSignal = 10001;
	
				manager::Global::SharedGet()->m_MTUsersQ.iterate(
					[&](const auto& login, auto& user, const bool&, bool& itbreak)
					{
						if (login >= 10010)
						{
							follow.LoginFollow = login;
							follow.FollowFlag = shared::FollowField::EnFollowFlag::EN_TRADE_FLAG_ENABLE;
							pFollowConObjQ->Push(shared::FollowKey(tostringa(10001).append("@").append(tostringa(login)).c_str()),follow);
						}
						if (pFollowConObjQ->Size() >= 49)
						{
							itbreak = true;
						}
					});


				SK_DELETE_PTR(pFollowConObjQ);
			}break;
			case EnCmdType::EN_CMD_TYPE_FOLLOW_CREATE_USERS:
			{
				for (int i = 0; i < 1000; ++i)
					manager::Global::CoreGet()->ManagerGet()->CreateUsers();
			}break;
			case EnCmdType::EN_CMD_TYPE_PARSING_MT5_POSITION_DAT:
			{
				auto postion_data_buf = sk::Helper::LoadFile(R"(C:\Users\k34ub\Desktop\mt5tradeserver_position\positions.idx)");

				const size_t head_len = 0x1E4;
#pragma pack(push,1)
				struct PositionIdx
				{
					wchar_t Symbol[32] = { 0 };


					PositionIdx() { memset(this,0x00,sizeof(*this)); }
				};
#pragma pack(pop)
				
				PositionIdx posidx;
				memcpy((char*)&posidx, &postion_data_buf[0]+head_len, sizeof(PositionIdx));
				auto sk = 0;




			}break;
			case EnCmdType::EN_CMD_TYPE_PARSING_MT5_ORDER_DAT:
			{

			}break;
			default:
			{
				manager::Global::Log(sk::EnLogType::EN_LOG_TYPE_ERROR)->error("系统不支持的命令({}).", cmd);
				continue;
			}break;
			}///switch
		}
	} while (1);

	return 0;
}