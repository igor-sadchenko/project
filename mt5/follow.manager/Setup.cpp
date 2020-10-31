#include "stdafx.h"
#include "Setup.h"

namespace client {
	Setup::Setup(const sk::stringa& ObjPath) :
		m_ObjPath(ObjPath) {
		if (Init()) { sk::Helper::ErrorFatal(__FUNCTION__, nullptr, "%s", "Module initialize failed !"); }
	}
	Setup::~Setup() {
		UnInit();
	}
	int Setup::UnInit() {
		return 0;
	}
	int Setup::Init() {
		int result = -1;
		auto stream = std::make_shared<std::basic_ifstream<char>>((ObjPath() + "follow.manager.xml").c_str(), std::ios::binary);
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
		auto setup = xmldoc->first_node("setup");
		if (!setup) { return result; }
		result = 0;
		auto node_setup = setup->first_node();
		while (node_setup) {
			if (!strcmp("runtime", node_setup->name())) {
				auto first_attribute = node_setup->first_attribute();
				while (first_attribute) {
					if (!strcmp("path", first_attribute->name())) {
						m_ApiPath = m_ObjPath + first_attribute->value();
					}
					first_attribute = first_attribute->next_attribute();
				}///while (first_attribute)
			}
			else if (!strcmp("tcpto", node_setup->name())) {
				auto first_attribute = node_setup->first_attribute();
				while (first_attribute) {
					if (!strcmp("addr", first_attribute->name())) {
						m_ServerAddr = first_attribute->value();
					}
					first_attribute = first_attribute->next_attribute();
				}///while (first_attribute)
			}
			else if (!strcmp("login", node_setup->name())) {
				auto first_attribute = node_setup->first_attribute();
				while (first_attribute) {
					if (!strcmp("username", first_attribute->name())) {
						if (first_attribute->value_size())
						{
							sk::SafeCopyA(m_LoginField.Username, first_attribute->value(), _countof(m_LoginField.Username));
						}		
					}
					else if (!strcmp("password", first_attribute->name())) {
						if (first_attribute->value_size())
						{
							sk::SafeCopyA(m_LoginField.Password, first_attribute->value(), _countof(m_LoginField.Password));
						}
					}
					else if (!strcmp("identify", first_attribute->name())) {
						if (first_attribute->value_size())
						{
							sk::SafeCopyA(m_LoginField.Identify, first_attribute->value(), _countof(m_LoginField.Identify));
						}
					}
					else if (!strcmp("id", first_attribute->name())) {
						m_LoginField.ID = strtoull(first_attribute->value(), nullptr, 16) << 8 * 4;
					}
					first_attribute = first_attribute->next_attribute();
				}///while (first_attribute)
			}
			node_setup = node_setup->next_sibling();
		}///while (node_setup)
		return result;
	}







}///namespace client