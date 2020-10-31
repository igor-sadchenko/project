#include "stdafx.h"
#include "Setup.h"

namespace manager {

	Setup::Setup(const sk::stringa& SystemPath) :
		m_SystemPath(SystemPath)
	{

	}

	Setup::~Setup()
	{


	}

	int Setup::Load()
	{
		auto targetFile = m_SystemPath + "manager.xml";
		if (!sk::Helper::CheckDirectory(targetFile.c_str())) {
			Global::Log()->error("{}", "缺少系统配置,无法启动.");
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
		catch (rapidxml::parse_error& err) {
			auto what = err.what();
			return -1;
		}
		auto system = xmldoc->first_node("system");
		if (!system) { return result; }
		result = 0;
		auto node_sys = system->first_node();
		while (node_sys) {
			if (!strcmp("manager", node_sys->name())) {
				auto first_attribute = node_sys->first_attribute();
				while (first_attribute) {
					if (!strcmp("address", first_attribute->name())) {
						m_ManagerAddress = first_attribute->value();
					}
					else if (!strcmp("password", first_attribute->name())) {
						m_ManagerPassword = first_attribute->value();
					}
					else if (!strcmp("login", first_attribute->name())) {
						m_ManagerLogin = first_attribute->value();
					}
					else if (!strcmp("timeout", first_attribute->name())) {
						m_ManagerConnectTimeout = first_attribute->value();
					}
					first_attribute = first_attribute->next_attribute();
				}///while (first_attribute)
			}

			node_sys = node_sys->next_sibling();
		}///while (node_setup)
		return result;
	}



}///namespace manager