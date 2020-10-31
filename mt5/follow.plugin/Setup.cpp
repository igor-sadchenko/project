#include "stdafx.h"
#include "Setup.h"

namespace follow {

	Setup::Setup(const sk::stringa& ObjPath) :
		m_ObjPath(ObjPath) {
		auto sk = 0;
	}
	Setup::~Setup() {

	}


	int Setup::Load() {
		auto targetFile = m_ObjPath + "follow.setup.xml";
		if (!sk::Helper::CheckDirectory(targetFile.c_str())) 
		{
			return -1;
		}
		int result = -1;
		auto stream = std::make_shared<std::basic_ifstream<char>>(targetFile.c_str(), std::ios::binary);
		if (!stream->is_open()) { return result; }
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
		auto setup = xmldoc->first_node("setup");
		if (!setup) { return result; }
		result = 0;
		auto node_setup = setup->first_node();
		while (node_setup)
		{
			if (!strcmp("server", node_setup->name()))
			{
				auto first_node = node_setup->first_node();
				do {
					if (!first_node) break;
					if (!strcmp("tcp", first_node->name()))
					{
						auto first_attribute = first_node->first_attribute();
						while (first_attribute)
						{
							if (!strcmp("addr", first_attribute->name())) {
								m_ServerTcpAddr = first_attribute->value();
							}
							else if (!strcmp("enable", first_attribute->name()))
							{
								m_EnableServerTcp = false;
								if (!strcmp("true", first_attribute->value()))
								{
									m_EnableServerTcp = true;
								}
							}
							first_attribute = first_attribute->next_attribute();
						}
					}
					first_node = first_node->next_sibling();
				} while (1);
			}
			else if (!strcmp("system", node_setup->name()))
			{
				auto first_node = node_setup->first_node();
				do {
					if (!first_node) break;
					if (!strcmp("api", first_node->name()))
					{
						auto first_attribute = first_node->first_attribute();
						while (first_attribute)
						{
							if (!strcmp("path", first_attribute->name())) {
								m_ApiPath = first_attribute->value();
							}
							first_attribute = first_attribute->next_attribute();
						}
					}
					else if (!strcmp("log", first_node->name()))
					{
						auto first_attribute = first_node->first_attribute();
						while (first_attribute)
						{
							if (!strcmp("enable", first_attribute->name()))
							{
								m_EnableLog = false;
								if (!strcmp("true", first_attribute->value()))
								{
									m_EnableLog = true;
								}
							}
							first_attribute = first_attribute->next_attribute();
						}
					}
					else if (!strcmp("followcon", first_node->name()))
					{
						auto first_attribute = first_node->first_attribute();
						while (first_attribute)
						{
							if (!strcmp("pathname", first_attribute->name()))
							{
								m_FollowconPathname = m_ObjPath + first_attribute->value();
							}
							first_attribute = first_attribute->next_attribute();
						}
					}
					else if (!strcmp("followrecord", first_node->name()))
					{
						auto first_attribute = first_node->first_attribute();
						while (first_attribute)
						{
							if (!strcmp("path", first_attribute->name()))
							{
								m_FollowRecordPath = m_ObjPath + first_attribute->value();
							}
							first_attribute = first_attribute->next_attribute();
						}
					}
					else if (!strcmp("system_setup", first_node->name()))
					{
						auto first_attribute = first_node->first_attribute();
						while (first_attribute)
						{
							if (!strcmp("pathname", first_attribute->name()))
							{
								m_SystemSetupPathname = m_ObjPath + first_attribute->value();
							}
							first_attribute = first_attribute->next_attribute();
						}
					}
					first_node = first_node->next_sibling();
				} while (1);
			}
			else if (!strcmp("clients", node_setup->name()))
			{
				auto first_node = node_setup->first_node();
				do {
					if (!first_node) break;
					if (!strcmp("client", first_node->name()))
					{
						shared::FollowObjLoginField loginField;
						auto first_attribute = first_node->first_attribute();
						while (first_attribute)
						{
							if (!strcmp("id", first_attribute->name()))
							{
								loginField.ID = ::strtoull(first_attribute->value(), nullptr, 16) << 4 * 8;
							}
							else if (!strcmp("username", first_attribute->name()))
							{
								sk::SafeCopyA(loginField.Username, first_attribute->value(), _countof(loginField.Username));
							}
							else if (!strcmp("password", first_attribute->name()))
							{
								sk::SafeCopyA(loginField.Password, first_attribute->value(), _countof(loginField.Password));
							}
							else if (!strcmp("identify", first_attribute->name()))
							{
								sk::SafeCopyA(loginField.Identify, first_attribute->value(), _countof(loginField.Identify));
							}
							first_attribute = first_attribute->next_attribute();
						}
						if (loginField.ID > 0)
						{
							m_ClientsMapQ.push(loginField.ID, loginField);
						}
					}	
					first_node = first_node->next_sibling();
				} while (1);
			}
			node_setup = node_setup->next_sibling();
		}///while (node_setup)

		xmldoc->clear();
		return result;
	}

	int Setup::SystemSetupSet(const shared::FollowSystemSetup& setup)
	{
		std::lock_guard<std::mutex> lock{ *m_mutex };
		m_FollowSystemSetup = setup;
		SystemSetupFinalize();
		return 0;
	}
	const shared::FollowSystemSetup& Setup::SystemSetupGet() const
	{
		std::lock_guard<std::mutex> lock{ *m_mutex };
		return m_FollowSystemSetup;
	}

	int Setup::SystemSetupLoad()
	{
		int result = -1;
		if (!sk::Helper::CheckDirectory(m_SystemSetupPathname.c_str()))
		{
			result = SystemSetupFinalize();
		}
		else
		{
			auto stream = std::make_shared<std::basic_ifstream<char>>(m_SystemSetupPathname.c_str(), std::ios::binary);
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
				if (!strcmp("SystemSwitch", node_setup->name()))
				{
					if (node_setup->value_size())
						m_FollowSystemSetup.SystemSwitch = decltype(m_FollowSystemSetup.SystemSwitch)(strtol(node_setup->value(), nullptr, 10));
				}
				else if (!strcmp("PatchInAction", node_setup->name()))
				{
					if (node_setup->value_size())
						m_FollowSystemSetup.PatchInAction = decltype(m_FollowSystemSetup.PatchInAction)(strtol(node_setup->value(), nullptr, 10));
				}
				else if (!strcmp("AutoPositionCloseSwitch", node_setup->name()))
				{
					if (node_setup->value_size())
						m_FollowSystemSetup.AutoPositionCloseSwitch = decltype(m_FollowSystemSetup.AutoPositionCloseSwitch)(strtol(node_setup->value(), nullptr, 10));
				}
				else if (!strcmp("FollowToSignalExchangeSwitch", node_setup->name()))
				{
					if (node_setup->value_size())
						m_FollowSystemSetup.FollowToSignalExchangeSwitch = decltype(m_FollowSystemSetup.FollowToSignalExchangeSwitch)(strtol(node_setup->value(), nullptr, 10));
				}
				else if (!strcmp("CorrectionSLTPSwitch", node_setup->name()))
				{
					if (node_setup->value_size())
						m_FollowSystemSetup.CorrectionSLTPSwitch = decltype(m_FollowSystemSetup.CorrectionSLTPSwitch)(strtol(node_setup->value(), nullptr, 10));
				}
				else if (!strcmp("TimeAutoPositionCloseCheck", node_setup->name()))
				{
					if (node_setup->value_size())
						m_FollowSystemSetup.TimeAutoPositionCloseCheck = strtoll(node_setup->value(), nullptr, 10);
				}
				else if (!strcmp("TimeSLTPCorrectionInterval", node_setup->name()))
				{
					if (node_setup->value_size())
						m_FollowSystemSetup.TimeSLTPCorrectionInterval = strtoll(node_setup->value(), nullptr, 10);
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
			<< '\t' << R"(<SystemSwitch>)" << UINT(m_FollowSystemSetup.SystemSwitch) << R"(</SystemSwitch>)" << std::endl
			<< '\t' << R"(<PatchInAction>)" << UINT(m_FollowSystemSetup.PatchInAction) << R"(</PatchInAction>)" << std::endl
			<< '\t' << R"(<AutoPositionCloseSwitch>)" << UINT(m_FollowSystemSetup.AutoPositionCloseSwitch) << R"(</AutoPositionCloseSwitch>)" << std::endl
			<< '\t' << R"(<FollowToSignalExchangeSwitch>)" << UINT(m_FollowSystemSetup.FollowToSignalExchangeSwitch) << R"(</FollowToSignalExchangeSwitch>)" << std::endl
			<< '\t' << R"(<CorrectionSLTPSwitch>)" << UINT(m_FollowSystemSetup.CorrectionSLTPSwitch) << R"(</CorrectionSLTPSwitch>)" << std::endl
			<< '\t' << R"(<TimeSLTPCorrectionInterval>)" << time_t(m_FollowSystemSetup.TimeSLTPCorrectionInterval) << R"(</TimeSLTPCorrectionInterval>)" << std::endl
			<< '\t' << R"(<TimeAutoPositionCloseCheck>)" << time_t(m_FollowSystemSetup.TimeAutoPositionCloseCheck) << R"(</TimeAutoPositionCloseCheck>)" << std::endl;

		os << R"(</setup>)" << std::endl;

		std::ofstream of(m_SystemSetupPathname, std::ios::out | std::ios::trunc);
		if (of.is_open())
		{
			of << os.str();
			of.close();
		}
		return 0;
	}


}///namespace follow