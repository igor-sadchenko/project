#include "stdafx.h"
#include "Setup.h"

namespace dealer {

	Setup::Setup(const sk::stringa& ObjPath) :
		m_ObjPath(ObjPath) 
	{
	}
	Setup::~Setup() 
	{

	}


	int Setup::Load() 
	{
		auto targetFile = m_ObjPath + "dealer.setup.xml";
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
					if(!strcmp("tcpto", first_node->name()))
					{
						auto first_attribute = first_node->first_attribute();
						while (first_attribute)
						{
							if (!strcmp("addr", first_attribute->name())) {
								m_TcptoAddr = first_attribute->value();
							}
							else if (!strcmp("enable", first_attribute->name()))
							{
								m_EnableTcpto.store(false);
								if (!strcmp("true", first_attribute->value()))
								{
									m_EnableTcpto.store(true);
								}
							}
							first_attribute = first_attribute->next_attribute();
						}
					}
					else if (!strcmp("login", first_node->name()))
					{
						auto first_attribute = first_node->first_attribute();
						while (first_attribute)
						{
							if (!strcmp("id", first_attribute->name())) 
							{
								m_BridgeLoginFiled.ID = strtoull(first_attribute->value(), nullptr, 16)<<(4*8);
							}
							else if (!strcmp("username", first_attribute->name()))
							{
								sk::SafeCopyA(m_BridgeLoginFiled.Username, first_attribute->value(), _countof(m_BridgeLoginFiled.Username));
							}
							else if (!strcmp("password", first_attribute->name()))
							{
								sk::SafeCopyA(m_BridgeLoginFiled.Password, first_attribute->value(), _countof(m_BridgeLoginFiled.Password));
							}
							else if (!strcmp("identify", first_attribute->name()))
							{
								sk::SafeCopyA(m_BridgeLoginFiled.Identify, first_attribute->value(), _countof(m_BridgeLoginFiled.Identify));
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
					else if (!strcmp("proxy", first_node->name()))
					{
						auto first_attribute = first_node->first_attribute();
						while (first_attribute)
						{
							if (!strcmp("pathname", first_attribute->name())) {
								m_ProxyPathname = first_attribute->value();
							}
							else if (!strcmp("enable", first_attribute->name()))
							{
								m_ProxyEnable.store(false);
								if (!strcmp("true", first_attribute->value()))
								{
									m_ProxyEnable.store(true);
								}
							}
							else if (!strcmp("interval", first_attribute->name())) {
								auto val = strtoll(first_attribute->value(), nullptr, 10);
								if (val < 5000)
									val = 5000;
								m_ProxyInterval.store(val);
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
}///namespace dealer