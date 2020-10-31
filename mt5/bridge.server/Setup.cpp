#include "stdafx.h"
#include "Setup.h"

namespace proxy {

	Setup::Setup() {
		if (Init()) { sk::Helper::ErrorFatal(__FUNCTION__, nullptr, "%s", "Module initialize failed !"); }
	}

	Setup::~Setup() {
		UnInit();
	}

	int Setup::Init() {
		if (LoadSystemSetup())
		{
			return -1;
		}
		SystemSetupLoad();
		SystemAssignLoginsPostOrderLoad();
		return 0;
	}

	int Setup::UnInit() {
		return 0;
	}

	int Setup::Open() {
		return 0;
	}

	int Setup::Close() {
		return 0;
	}

	int Setup::LoadSystemSetup()
	{
		auto targetFile = GetGlobal()->GetCurrentProcessDirectory() + "bridge.server.setup.xml";
		if (!sk::Helper::CheckDirectory(targetFile.c_str()))
		{
			return -1;
		}
		auto stream = std::make_shared<std::basic_ifstream<char>>(targetFile.c_str(), std::ios::binary);
		if (!stream->is_open()) { return -1; }
		auto xmlfile = std::make_shared<rapidxml::file<>>(*stream.get());
		auto xmldoc = std::make_shared<rapidxml::xml_document<>>();
		try
		{
			xmldoc->parse<0>(xmlfile->data());
		}
		catch (rapidxml::parse_error& err)
		{
			auto what = err.what();
			return -1;
		}
		auto system = xmldoc->first_node("system");
		if (!system) { return -1; }

		auto system_node = system->first_node();
		do {
			if (!system_node) break;
			if (!strcmp("setup", system_node->name()))
			{
				auto first_node = system_node->first_node();
				do {
					if (!first_node) break;
					if (!strcmp("api", first_node->name()))
					{
						auto first_attribute = first_node->first_attribute();
						do {
							if (!first_attribute) break;
							if (!strcmp("path", first_attribute->name()))
							{
								m_ApiPath = first_attribute->value();//!@ 注意Api指向使用绝对路径
							}
							first_attribute = first_attribute->next_attribute();
						} while (1);
					}
					else if (!strcmp("symbols", first_node->name()))
					{
						auto first_attribute = first_node->first_attribute();
						do {
							if (!first_attribute) break;
							if (!strcmp("path", first_attribute->name()))
							{
								m_SymbolsPath = GetGlobal()->GetCurrentProcessDirectory() + first_attribute->value();
							}
							first_attribute = first_attribute->next_attribute();
						} while (1);
					}
					else if (!strcmp("system_setup", first_node->name()))
					{
						auto first_attribute = first_node->first_attribute();
						do {
							if (!first_attribute) break;
							if (!strcmp("pathname", first_attribute->name()))
							{
								m_SystemSetupPathName = GetGlobal()->GetCurrentProcessDirectory() + first_attribute->value();
							}
							first_attribute = first_attribute->next_attribute();
						} while (1);
					}
					else if (!strcmp("system_assign_logins_post_order", first_node->name()))
					{
						auto first_attribute = first_node->first_attribute();
						do {
							if (!first_attribute) break;
							if (!strcmp("pathname", first_attribute->name()))
							{
								m_SystemAssignLoginsPostOrderPathName = GetGlobal()->GetCurrentProcessDirectory() + first_attribute->value();
							}
							first_attribute = first_attribute->next_attribute();
						} while (1);
					}
					else if (!strcmp("trade_record", first_node->name()))
					{
						auto first_attribute = first_node->first_attribute();
						do {
							if (!first_attribute) break;
							if (!strcmp("path", first_attribute->name()))
							{
								m_LogRecordPath = GetGlobal()->GetCurrentProcessDirectory() + first_attribute->value();
							}
							first_attribute = first_attribute->next_attribute();
						} while (1);
					}
					first_node = first_node->next_sibling();
				} while (1);
			}
			else if (!strcmp("server", system_node->name()))
			{
				auto first_node = system_node->first_node();
				do {
					if (!first_node) break;
					if (!strcmp("tcp", first_node->name()))
					{
						auto first_attribute = first_node->first_attribute();
						do {
							if (!first_attribute) break;
							if (!strcmp("addr", first_attribute->name()))
							{
								m_ServerTcpAddress = first_attribute->value();
							}
							first_attribute = first_attribute->next_attribute();
						} while (1);
					}
					first_node = first_node->next_sibling();
				} while (1);
			}
			else if (!strcmp("proceses", system_node->name()))
			{
				auto first_node = system_node->first_node();
				do {
					if (!first_node) break;
					if (!strcmp("process", first_node->name()))
					{
						auto first_attribute = first_node->first_attribute();
						LPProcess* process = new LPProcess();
						do {
							if (!first_attribute) break;
							if (!strcmp("id", first_attribute->name()))
							{
								process->ID(strtoull(first_attribute->value(), nullptr, 16) << 4 * 8);
							}
							else if (!strcmp("enable", first_attribute->name()))
							{
								process->Enable(false);
								if (!strcmp("true", first_attribute->value())) process->Enable(true);
							}
							else if (!strcmp("show", first_attribute->name()))
							{
								process->Show(false);
								if (!strcmp("true", first_attribute->value())) process->Show(true);
							}
							else if (!strcmp("identify", first_attribute->name()))
							{
								process->Identify(first_attribute->value());
							}
							else if (!strcmp("pathname", first_attribute->name()))
							{
								process->ObjPathname(GetGlobal()->GetCurrentProcessDirectory() + first_attribute->value());
							}
							first_attribute = first_attribute->next_attribute();
						} while (1);

						if (process->ID() > 0)
						{
							m_SystemAssignLoginsPostOrderQ.push(shared::SystemIDType::ExchangeType(process->ID()), sk::container::set<UINT64>());
						}

						if (process->ID() > 0 && process->Enable())
						{
							m_LPProcesesMapQ.push(process->ID(), process);
						}
						else
						{
							SK_DELETE_PTR(process);
						}
					}
					first_node = first_node->next_sibling();
				} while (1);
			}
			else if (!strcmp("clients", system_node->name()))
			{
				auto first_node = system_node->first_node();
				do {
					if (!first_node) break;
					if (!strcmp("client", first_node->name()))
					{
						auto first_attribute = first_node->first_attribute();
						shared::BridgeLoginField field;
						do {
							if (!first_attribute) break;
							if (!strcmp("id", first_attribute->name()))
							{
								field.ID = strtoull(first_attribute->value(), nullptr, 16) << 4 * 8;
							}
							else if (!strcmp("password", first_attribute->name()))
							{
								sk::SafeCopyA(field.Password, first_attribute->value(), _countof(field.Password));
							}
							else if (!strcmp("identify", first_attribute->name()))
							{
								sk::SafeCopyA(field.Identify, first_attribute->value(), _countof(field.Identify));
							}
							else if (!strcmp("username", first_attribute->name()))
							{
								sk::SafeCopyA(field.Username, first_attribute->value(), _countof(field.Username));
							}
							first_attribute = first_attribute->next_attribute();
						} while (1);

						if (field.ID > 0)
						{
							m_ClientsMapQ.push(field.ID, field);
						}
					}
					first_node = first_node->next_sibling();
				} while (1);
			}
			else if (!strcmp("readys", system_node->name()))
			{
				auto first_node = system_node->first_node();
				do {
					if (!first_node) break;
					if (!strcmp("context", first_node->name()))
					{
						auto first_attribute = first_node->first_attribute();
						shared::SystemID ServiceObjFlag = 0;
						bool ServiceObjRequired = false;
						do {
							if (!first_attribute) break;
							if (!strcmp("id", first_attribute->name()))
							{
								ServiceObjFlag = strtoull(first_attribute->value(), nullptr, 16) << 4 * 8;
							}
							else if (!strcmp("required", first_attribute->name()))
							{
								ServiceObjRequired = false;
								if (!strcmp("true", first_attribute->value()))
								{
									ServiceObjRequired = true;
								}
							}
							first_attribute = first_attribute->next_attribute();
						} while (1);

						if (ServiceObjFlag > 0 && ServiceObjRequired)
						{
							m_ReadyContextMapQ.push(ServiceObjFlag, false);
						}
					}
					first_node = first_node->next_sibling();
				} while (1);
			}
			else if (!strcmp("commands", system_node->name()))
			{
				auto first_node = system_node->first_node();
				do {
					if (!first_node) break;
					if (!strcmp("command", first_node->name()))
					{
						auto first_attribute = first_node->first_attribute();
						shared::ConsoleCommand field;
						do {
							if (!first_attribute) break;
							if (!strcmp("id", first_attribute->name()))
							{
								field.ID = strtoull(first_attribute->value(), nullptr, 16);
							}
							else if (!strcmp("input", first_attribute->name()))
							{
								sk::SafeCopyA(field.Input, first_attribute->value(), _countof(field.Input));
							}
							else if (!strcmp("explain", first_attribute->name()))
							{
								sk::SafeCopyA(field.Explain, first_attribute->value(), _countof(field.Explain));
							}
							first_attribute = first_attribute->next_attribute();
						} while (1);

						if (field.ID > 0)
						{
							m_CommandsMapQ.push(field.ID, field);
						}
					}
					first_node = first_node->next_sibling();
				} while (1);
			}

			system_node = system_node->next_sibling();
		} while (1);

		xmldoc->clear();
		return 0;
	}

	bool Setup::SystemAssignLoginsPostOrderSelect(const shared::SystemID& exchange, std::vector<UINT64>& out)
	{
		out.clear();
		std::lock_guard<std::mutex> lock{ *m_mutex };
		auto pUsers = m_SystemAssignLoginsPostOrderQ.search(exchange);
		if (pUsers)
		{
			out = pUsers->Vector();
		}
		return pUsers ? true : false;
	}
	bool Setup::SystemAssignLoginsPostOrderEmpty(const shared::SystemID& exchange)
	{
		bool result = false;
		std::lock_guard<std::mutex> lock{ *m_mutex };
		m_SystemAssignLoginsPostOrderQ.search(
			exchange,
			[&](const auto&, auto& logins)
			{
				logins.clear();
				result = true;
			});
		SystemAssignLoginsPostOrderFinalize();
		return result;
	}
	bool Setup::SystemAssignLoginsPostOrderDelete(const UINT64& login)
	{
		bool result = false;
		std::lock_guard<std::mutex> lock{ *m_mutex };
		m_SystemAssignLoginsPostOrderQ.iterate(
			[&](const auto&, auto& logins)
			{
				if (!result)
					result = logins.pop(login);
				else
					logins.pop(login);
			});
		SystemAssignLoginsPostOrderFinalize();
		return result;
	}
	bool Setup::SystemAssignLoginsPostOrderInsert(const shared::SystemID& exchange, const UINT64& login)
	{
		int result = false;
		std::lock_guard<std::mutex> lock{ *m_mutex };
		m_SystemAssignLoginsPostOrderQ.search(
			exchange,
			[&](const auto&, auto& users)
			{
				users.push(login);
				result = true;
			});
		if (result)
		{
			m_SystemAssignLoginsPostOrderQ.iterate(
				[&](const auto& exchangeid, auto& users)
				{
					if (exchangeid != exchange)
						users.pop(login);
				});

			SystemAssignLoginsPostOrderFinalize();
		}
		return result;
	}

	int Setup::SystemAssignLoginsPostOrderLoad()
	{
		int result = -1;
		if (m_SystemAssignLoginsPostOrderPathName.empty())
			return result;
		if (!sk::Helper::CheckDirectory(m_SystemAssignLoginsPostOrderPathName.c_str()))
		{
			result = SystemAssignLoginsPostOrderFinalize();
		}
		else
		{
			auto stream = std::make_shared<std::basic_ifstream<char>>(m_SystemAssignLoginsPostOrderPathName.c_str(), std::ios::binary);
			if (!stream->is_open()) { return result; }
			auto xmlfile = std::make_shared<rapidxml::file<>>(*stream.get());
			auto xmldoc = std::make_shared<rapidxml::xml_document<>>();
			try
			{
				xmldoc->parse<0>(xmlfile->data());
			}
			catch (rapidxml::parse_error& err) {
				auto what = err.what();
				return -1;
			}
			auto setup = xmldoc->first_node("setup");
			if (!setup) { return result; }

			result = 0;

			auto node_setup = setup->first_node();
			while (node_setup)
			{
				if (!strcmp("Exchange", node_setup->name()))
				{
					shared::SystemID ExchangeID = 0;
					auto first_attribute = node_setup->first_attribute();
					while (first_attribute)
					{
						if (!strcmp("ID", first_attribute->name()))
						{
							if (first_attribute->value_size() > 0)
								ExchangeID = strtoull(first_attribute->value(), nullptr, 16);
						}
						first_attribute = first_attribute->next_attribute();
					}///while (first_attribute)

					if (ExchangeID > 0)
					{
						m_SystemAssignLoginsPostOrderQ.push(ExchangeID, sk::container::set<UINT64>());

						auto node_user = node_setup->first_node();
						while (node_user)
						{
							if (!strcmp("User", node_user->name()))
							{
								auto first_attribute = node_setup->first_attribute();
								while (first_attribute)
								{
									if (!strcmp("Login", first_attribute->name()))
									{
										if (first_attribute->value_size() > 0)
										{
											shared::UserField userField;
											userField.Login = strtoull(first_attribute->value(), nullptr, 10);
											m_SystemAssignLoginsPostOrderQ.pushpush(ExchangeID, [&](auto& users) {users.push(userField.Login); });
										}
									}
									first_attribute = first_attribute->next_attribute();
								}///while (first_attribute)
							}
							node_user = node_user->next_sibling();
						}
					}
				}
				node_setup = node_setup->next_sibling();
			}
			xmldoc->clear();
		}
		return 0;
	}

	int Setup::SystemAssignLoginsPostOrderFinalize()
	{
		std::ostringstream os;
		os.clear();
		os << R"(<?xml version="1.0" encoding="ANSI" ?>)" << std::endl;
		os << R"(<setup>)" << std::endl;
		m_SystemAssignLoginsPostOrderQ.iterate(
			[&](const auto& exchangeid, auto& users)
			{
				os << "\t" << sk::Log::Format(R"(<Exchange ID="{:X}">)", exchangeid) << std::endl;
				users.iterate(
					[&](auto& login)
					{
						os << "\t\t" << sk::Log::Format(R"(<User Login="{}"/>)", login) << std::endl;
					});
				os << "\t" << R"(</Exchange>)" << std::endl;
			});
		os << R"(</setup>)" << std::endl;
		std::ofstream of(m_SystemAssignLoginsPostOrderPathName, std::ios::out | std::ios::trunc);
		if (of.is_open())
		{
			of << os.str();
			of.close();
		}
		return 0;
	}

	int Setup::SystemSetupLoad()
	{
		int result = -1;
		if (m_SystemSetupPathName.empty())
			return result;
		if (!sk::Helper::CheckDirectory(m_SystemSetupPathName.c_str()))
		{
			result = SystemSetupFinalize();
		}
		else
		{
			auto stream = std::make_shared<std::basic_ifstream<char>>(m_SystemSetupPathName.c_str(), std::ios::binary);
			if (!stream->is_open()) { return result; }
			auto xmlfile = std::make_shared<rapidxml::file<>>(*stream.get());
			auto xmldoc = std::make_shared<rapidxml::xml_document<>>();
			try
			{
				xmldoc->parse<0>(xmlfile->data());
			}
			catch (rapidxml::parse_error& err) {
				auto what = err.what();
				return -1;
			}
			auto setup = xmldoc->first_node("setup");
			if (!setup) { return result; }

			result = 0;

			auto node_setup = setup->first_node();
			while (node_setup) {
				if (!strcmp("SystemPlatformMode", node_setup->name()))
				{
					if (node_setup->value_size())
						m_BridgeSystemSetup.SystemPlatformMode = decltype(m_BridgeSystemSetup.SystemPlatformMode)(strtol(node_setup->value(), nullptr, 10));
				}
				else if (!strcmp("SystemOrderMode", node_setup->name()))
				{
					if (node_setup->value_size())
						m_BridgeSystemSetup.SystemOrderMode = decltype(m_BridgeSystemSetup.SystemOrderMode)(strtol(node_setup->value(), nullptr, 10));
				}
				else if (!strcmp("FeederSpreadMode", node_setup->name()))
				{
					if (node_setup->value_size())
						m_BridgeSystemSetup.FeederSpreadMode = decltype(m_BridgeSystemSetup.FeederSpreadMode)(strtol(node_setup->value(), nullptr, 10));
				}
				else if (!strcmp("TimeoutQuoteActive", node_setup->name()))
				{
					if (node_setup->value_size())
						m_BridgeSystemSetup.TimeoutQuoteActive = strtoll(node_setup->value(), nullptr, 10);
				}
				else if (!strcmp("TimeoutDealerOrder", node_setup->name()))
				{
					if (node_setup->value_size())
						m_BridgeSystemSetup.TimeoutDealerOrder = strtoll(node_setup->value(), nullptr, 10);
				}
				else if (!strcmp("DealerContinueToRequestDelay", node_setup->name()))
				{
					if (node_setup->value_size())
						m_BridgeSystemSetup.DealerContinueToRequestDelay = strtoll(node_setup->value(), nullptr, 10);
				}
				else if (!strcmp("DealerContinueToRequestSwitch", node_setup->name()))
				{
					if (node_setup->value_size())
						m_BridgeSystemSetup.DealerContinueToRequestSwitch = decltype(m_BridgeSystemSetup.DealerContinueToRequestSwitch)(strtol(node_setup->value(), nullptr, 10));
				}
				else if (!strcmp("DealerOrdType", node_setup->name()))
				{
					if (node_setup->value_size())
						m_BridgeSystemSetup.DealerOrdType = decltype(m_BridgeSystemSetup.DealerOrdType)(node_setup->value()[0]);
				}
				else if (!strcmp("DealerOrdTimeInForce", node_setup->name()))
				{
					if (node_setup->value_size())
						m_BridgeSystemSetup.DealerOrdTimeInForce = decltype(m_BridgeSystemSetup.DealerOrdTimeInForce)(node_setup->value()[0]);
				}
				else if (!strcmp("DealerTradePxType", node_setup->name()))
				{
					if (node_setup->value_size())
						m_BridgeSystemSetup.DealerTradePxType = decltype(m_BridgeSystemSetup.DealerTradePxType)(strtol(node_setup->value(), nullptr, 10));
				}
				else if (!strcmp("DevelopDebugSwitch", node_setup->name()))
				{
					if (node_setup->value_size())
						m_BridgeSystemSetup.DevelopDebugSwitch = decltype(m_BridgeSystemSetup.DevelopDebugSwitch)(strtol(node_setup->value(), nullptr, 10));
				}
				else if (!strcmp("AutoAdjustPositionSwitch", node_setup->name()))
				{
					if (node_setup->value_size())
						m_BridgeSystemSetup.AutoAdjustPositionSwitch = decltype(m_BridgeSystemSetup.AutoAdjustPositionSwitch)(strtol(node_setup->value(), nullptr, 10));
				}
				else if (!strcmp("AggregateQuoteSwitch", node_setup->name()))
				{
					if (node_setup->value_size())
						m_BridgeSystemSetup.AggregateQuoteSwitch = decltype(m_BridgeSystemSetup.AggregateQuoteSwitch)(strtol(node_setup->value(), nullptr, 10));
				}
				else if (!strcmp("SystemOperatingTimePeriod", node_setup->name()))
				{
					std::map<INT, shared::TimePeriod> TimePeriods;
					auto node_sotp = node_setup->first_node();
					while (node_sotp)
					{
						if (!strcmp("TimePeriod", node_sotp->name())) {
							INT wday = -1;
							shared::TimePeriod period;
							auto first_attribute = node_sotp->first_attribute();
							while (first_attribute) {
								if (!strcmp("wday", first_attribute->name())) {
									wday = strtol(first_attribute->value(), nullptr, 10);
								}
								else if (!strcmp("begin", first_attribute->name())) {
									period.TimeBegin = strtoul(first_attribute->value(), nullptr, 10);
								}
								else if (!strcmp("end", first_attribute->name())) {
									period.TimeEnd = strtoul(first_attribute->value(), nullptr, 10);
								}
								first_attribute = first_attribute->next_attribute();
							}///while (first_attribute)
							if (wday >= 0 && wday < _countof(m_BridgeSystemSetup.SystemOperatingTimePeriod))
							{
								TimePeriods.insert({ {wday,period} });
							}
						}
						node_sotp = node_sotp->next_sibling();
					}

					for (auto it = TimePeriods.begin(); it != TimePeriods.end(); ++it)
					{
						m_BridgeSystemSetup.SystemOperatingTimePeriod[it->first] = it->second;
					}
				}
				else if (!strcmp("SingleOrderLogins", node_setup->name()))
				{
					auto node_login = node_setup->first_node();
					while (node_login)
					{
						if (!strcmp("Login", node_login->name()))
						{
							m_BridgeSystemSetup.OneByOneOrderAssignLoginAppend(strtoull(node_login->value(), nullptr, 10));
						}
						node_login = node_login->next_sibling();
					}
				}
				node_setup = node_setup->next_sibling();
			}
			xmldoc->clear();
		}

		return 0;
	}

	int Setup::SystemSetupFinalize()
	{
		std::ostringstream os;
		os.clear();
		os << R"(<?xml version="1.0" encoding="ANSI" ?>)" << std::endl;
		os << R"(<setup>)" << std::endl
			<< '\t' << R"(<SystemPlatformMode>)" << UINT(m_BridgeSystemSetup.SystemPlatformMode) << R"(</SystemPlatformMode>)" << std::endl
			<< '\t' << R"(<TimeoutDealerOrder>)" << time_t(m_BridgeSystemSetup.TimeoutDealerOrder) << R"(</TimeoutDealerOrder>)" << std::endl
			<< '\t' << R"(<TimeoutQuoteActive>)" << time_t(m_BridgeSystemSetup.TimeoutQuoteActive) << R"(</TimeoutQuoteActive>)" << std::endl
			<< '\t' << R"(<DealerOrdType>)" << CHAR(m_BridgeSystemSetup.DealerOrdType) << R"(</DealerOrdType>)" << std::endl
			<< '\t' << R"(<DealerOrdTimeInForce>)" << CHAR(m_BridgeSystemSetup.DealerOrdTimeInForce) << R"(</DealerOrdTimeInForce>)" << std::endl
			<< '\t' << R"(<AggregateQuoteSwitch>)" << UINT(m_BridgeSystemSetup.AggregateQuoteSwitch) << R"(</AggregateQuoteSwitch>)" << std::endl
			<< '\t' << R"(<DealerTradePxType>)" << UINT(m_BridgeSystemSetup.DealerTradePxType) << R"(</DealerTradePxType>)" << std::endl
			<< '\t' << R"(<AutoAdjustPositionSwitch>)" << UINT(m_BridgeSystemSetup.AutoAdjustPositionSwitch) << R"(</AutoAdjustPositionSwitch>)" << std::endl
			<< '\t' << R"(<SystemOrderMode>)" << UINT(m_BridgeSystemSetup.SystemOrderMode) << R"(</SystemOrderMode>)" << std::endl
			<< '\t' << R"(<FeederSpreadMode>)" << UINT(m_BridgeSystemSetup.FeederSpreadMode) << R"(</FeederSpreadMode>)" << std::endl
			<< '\t' << R"(<DealerContinueToRequestDelay>)" << time_t(m_BridgeSystemSetup.DealerContinueToRequestDelay) << R"(</DealerContinueToRequestDelay>)" << std::endl
			<< '\t' << R"(<DealerContinueToRequestSwitch>)" << UINT(m_BridgeSystemSetup.DealerContinueToRequestSwitch) << R"(</DealerContinueToRequestSwitch>)" << std::endl
			<< '\t' << R"(<DevelopDebugSwitch>)" << UINT(m_BridgeSystemSetup.DevelopDebugSwitch) << R"(</DevelopDebugSwitch>)" << std::endl;

		os << "\t" << R"(<AssignLogins>)" << std::endl;
		if (m_BridgeSystemSetup.OneByOneOrderAssignLoginCount() > 0)
		{
			for (size_t i = 0; i < 200; ++i)
			{
				if (m_BridgeSystemSetup.OneByOneOrderAssignLogin[i] > 0)
				{
					os << "\t\t"
						<< R"(<Login>)"
						<< m_BridgeSystemSetup.OneByOneOrderAssignLogin[i]
						<< R"(</Login>)"
						<< std::endl;
				}
				else
				{
					break;
				}
			}
		}
		os << "\t" << R"(</AssignLogins>)" << std::endl;

		os << "\t" << R"(<SystemOperatingTimePeriod>)" << std::endl;
		for (size_t i = 0; i < _countof(m_BridgeSystemSetup.SystemOperatingTimePeriod); ++i)
		{
			os << "\t\t"
				<< R"(<TimePeriod )"
				<< R"(wday=")"
				<< i
				<< R"(")"
				<< R"( begin=")"
				<< m_BridgeSystemSetup.SystemOperatingTimePeriod[i].TimeBegin
				<< R"(")"
				<< R"( end=")"
				<< m_BridgeSystemSetup.SystemOperatingTimePeriod[i].TimeEnd
				<< R"(")"
				<< R"(/>)"
				<< std::endl;
		}
		os << "\t" << R"(</SystemOperatingTimePeriod>)" << std::endl;

		os << R"(</setup>)" << std::endl;

		std::ofstream of(m_SystemSetupPathName, std::ios::out | std::ios::trunc);
		if (of.is_open())
		{
			of << os.str();
			of.close();
		}
		return 0;
	}
	int Setup::SystemSetupSet(const shared::BridgeSystemSetup& setup)
	{
		std::lock_guard<std::mutex> lock{ *m_mutex };
		m_BridgeSystemSetup = setup;
		SystemSetupFinalize();
		return 0;
	}
	const shared::BridgeSystemSetup& Setup::SystemSetupGet() const
	{
		std::lock_guard<std::mutex> lock{ *m_mutex };
		return m_BridgeSystemSetup;
	}
}///namespace proxy