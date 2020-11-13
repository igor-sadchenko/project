#include "stdafx.h"
#include "Setup.h"

namespace local {

	Setup::Setup()
	{
		if (Init()) { sk::Helper::ErrorFatal(__FUNCTION__, nullptr, "%s", "Module initialize failed !"); }
	}

	Setup::~Setup()
	{
		UnInit();
	}

	int Setup::Init()
	{
		m_SetupPathname = GlobalGet()->GetCurrentProcessDirectory() + \
			sk::Helper::RemoveProcessNameSuffix(GlobalGet()->GetCurrentHostProcessName()) + \
			".xml";
		if (m_SetupPathname.empty())
			return -1;


		int result = -1;
		auto stream = std::make_shared<std::basic_ifstream<char>>(m_SetupPathname.c_str(), std::ios::binary);
		if (!stream->is_open()) { return result; }
		auto xmlfile = std::make_shared<rapidxml::file<>>(*stream.get());
		auto xmldoc = std::make_shared<rapidxml::xml_document<>>();
		try {
			xmldoc->parse<0>(xmlfile->data());
		}
		catch (rapidxml::parse_error& err) {
			auto what = err.what();
			return -1;
		}
		auto setup = xmldoc->first_node("Setup");
		if (!setup) { return result; }
		result = 0;
		auto node_setup = setup->first_node();
		while (node_setup)
		{
			if (!strcmp("Runtime", node_setup->name()))
			{
				auto first_attribute = node_setup->first_attribute();
				while (first_attribute)
				{
					if (!strcmp("path", first_attribute->name()))
					{
						m_RuntimePath = GlobalGet()->GetCurrentProcessDirectory() + first_attribute->value();
					}
					first_attribute = first_attribute->next_attribute();
				}///while (first_attribute)
			}//Runtime
			if (!strcmp("Plugin", node_setup->name()))
			{
				auto first_attribute = node_setup->first_attribute();
				while (first_attribute)
				{
					if (!strcmp("path", first_attribute->name()))
					{
						m_PluginPath = GlobalGet()->GetCurrentProcessDirectory() + first_attribute->value();
					}
					first_attribute = first_attribute->next_attribute();
				}///while (first_attribute)
			}//Plugin
			else if (!strcmp("Server", node_setup->name()))
			{
				auto first_attribute = node_setup->first_attribute();
				while (first_attribute)
				{
					if (!strcmp("address", first_attribute->name()))
					{
						m_ServerAddress = first_attribute->value();
					}
					first_attribute = first_attribute->next_attribute();
				}///while (first_attribute)
			}//Server
			else if (!strcmp("FixObj", node_setup->name()))
			{
				auto first_attribute = node_setup->first_attribute();
				while (first_attribute)
				{
					if (!strcmp("path", first_attribute->name()))
					{
						m_FixObjPath = GlobalGet()->GetCurrentProcessDirectory() + first_attribute->value();
					}
					first_attribute = first_attribute->next_attribute();
				}///while (first_attribute)

				auto node_fixobj = node_setup->first_node();
				while (node_fixobj)
				{
					if (!strcmp("initiator", node_fixobj->name()))
					{
						auto first_attribute = node_fixobj->first_attribute();
						while (first_attribute)
						{
							if (!strcmp("pathname", first_attribute->name()))
							{
								m_FixObjInitiatorPathname = m_FixObjPath + first_attribute->value();
							}
							first_attribute = first_attribute->next_attribute();
						}///while (first_attribute)
					}
					else if (!strcmp("acceptor", node_fixobj->name()))
					{
						auto first_attribute = node_fixobj->first_attribute();
						while (first_attribute)
						{
							if (!strcmp("pathname", first_attribute->name()))
							{
								m_FixObjAcceptorPathname = m_FixObjPath + first_attribute->value();
							}
							first_attribute = first_attribute->next_attribute();
						}///while (first_attribute)
					}
					node_fixobj = node_fixobj->next_sibling();
				}//while



			}//FixObj
			node_setup = node_setup->next_sibling();
		}///while (node_setup)



		xmldoc->clear();
		return result;
	}

	int Setup::UnInit()
	{
		return 0;
	}

	int Setup::Open()
	{
		return 0;
	}

	int Setup::Close()
	{
		return 0;
	}

}///namespace lcoal