#include "stdafx.h"
#include "Setup.h"

namespace cfh {

	Setup::Setup(const sk::stringa& objPath) :
		m_objPath(objPath) {
		if (Load()) { sk::Helper::ErrorFatal(__FUNCTION__, nullptr, "%s", "Module initialize failed !"); }
		if (LoadProducts()) { sk::Helper::ErrorFatal(__FUNCTION__, nullptr, "%s", "Module initialize failed !"); }
	}

	Setup::~Setup() {

	}

	int Setup::LoadProducts()
	{
		auto ProductBuf = sk::Helper::LoadFile(ProductSrcPathname().c_str());
		if (ProductBuf.size() <= 0) return -1;

		std::vector<sk::stringa> vrParse;
		sk::Helper::OneUnknownParseA(&ProductBuf[0], '\n', vrParse);
		if (vrParse.empty())
		{
			return -1;
		}
		size_t line = 0;
		for (auto& content : vrParse)
		{
			if (line >= 1)
			{
				if (content.empty())
				{
					continue;
				}
				content.pop_back();//! 去掉'\r'
				std::vector<sk::stringa> content_line;
				sk::Helper::OneUnknownParseA(content, ',', content_line);
				if (content_line.empty() || content_line.size() < 4)
				{
					continue;
				}
				shared::LPProduct inst;
				inst.ID = strtoull(content_line[0].c_str(), nullptr, 10);
				inst.Symbol = content_line[1].c_str();
				inst.SystemSymbol = content_line[2].c_str();
				inst.QuotedCCY = content_line[3].c_str();
				inst.ContractSize = std::stoi(content_line[4].c_str());
				inst.TickSize = std::stod(content_line[5].c_str());
				inst.MinOrderSize = std::stod(content_line[6].c_str());
				inst.MaxOrderSize = std::stod(content_line[7].c_str());
				inst.StepOrderSize = std::stod(content_line[8].c_str());
				if (inst.Verify())
				{
					m_ProductQ.push(inst.ID, inst);
					m_SymbolProductQ.push(inst.Symbol.symbol, inst);
				}
			}
			++line;
		}
		if (m_ProductQ.empty()) return -1;
		return 0;
	}

	int Setup::Load() {
		auto targetFile = m_objPath + "setup.xml";
		if (!sk::Helper::CheckDirectory(targetFile.c_str())) {
			return -1;
		}
		auto stream = std::make_shared<std::basic_ifstream<char>>(targetFile.c_str(), std::ios::binary);
		if (!stream->is_open()) { return -1; }
		auto xmlfile = std::make_shared<rapidxml::file<>>(*stream.get());
		auto xmldoc = std::make_shared<rapidxml::xml_document<>>();
		try {
			xmldoc->parse<0>(xmlfile->data());
		}
		catch (rapidxml::parse_error& err) {
			auto what = err.what();
			return -1;
		}
		auto node_setup = xmldoc->first_node("setup");
		if (!node_setup) { return -1; }
		auto node_lp = node_setup->first_node();
		while (node_lp) {
			if (!strcmp("MarketData", node_lp->name())) {
				auto first_attribute = node_lp->first_attribute();
				while (first_attribute) {
					if (!strcmp("enable", first_attribute->name())) {
						if (!strcmp("true", first_attribute->value())) {
							m_EnableMarketData = true;
						}
					}
					first_attribute = first_attribute->next_attribute();
				}///while (first_attribute)
				auto node_lp_quote = node_lp->first_node();
				while (node_lp_quote) {
					if (!strcmp("fixcon", node_lp_quote->name())) {
						auto first_attribute = node_lp_quote->first_attribute();
						while (first_attribute) {
							if (!strcmp("pathname", first_attribute->name())) {
								m_MarketDataFixcon = m_objPath + first_attribute->value();
							}
							first_attribute = first_attribute->next_attribute();
						}///while (first_attribute)
					}
					else if (!strcmp("tcpto", node_lp_quote->name())) {
						auto first_attribute = node_lp_quote->first_attribute();
						while (first_attribute) {
							if (!strcmp("addr", first_attribute->name())) {
								m_MarketDataTcptoAddress = first_attribute->value();
							}
							first_attribute = first_attribute->next_attribute();
						}///while (first_attribute)
					}
					else if (!strcmp("cache", node_lp_quote->name())) {
						auto first_attribute = node_lp_quote->first_attribute();
						while (first_attribute) {
							if (!strcmp("pathname", first_attribute->name())) {
								m_MarketDataCachePathname = m_objPath + first_attribute->value();
							}
							first_attribute = first_attribute->next_attribute();
						}///while (first_attribute)
					}
					else if (!strcmp("login", node_lp_quote->name())) {
						auto first_attribute = node_lp_quote->first_attribute();
						while (first_attribute) {
							if (!strcmp("id", first_attribute->name())) {
								if (first_attribute->value_size()) {
									m_LoginFieldMarketData.ID = strtoull(first_attribute->value(), nullptr, 16) << 4 * 8;
								}
							}
							else if (!strcmp("password", first_attribute->name())) {
								if (first_attribute->value_size()) {
									sk::SafeCopyA(m_LoginFieldMarketData.Password, first_attribute->value(), _countof(m_LoginFieldMarketData.Password));
								}
							}
							else if (!strcmp("username", first_attribute->name())) {
								if (first_attribute->value_size()) {
									sk::SafeCopyA(m_LoginFieldMarketData.Username, first_attribute->value(), _countof(m_LoginFieldMarketData.Username));
								}
							}
							else if (!strcmp("identify", first_attribute->name())) {
								if (first_attribute->value_size()) {
									sk::SafeCopyA(m_LoginFieldMarketData.Identify, first_attribute->value(), _countof(m_LoginFieldMarketData.Identify));
								}
							}
							first_attribute = first_attribute->next_attribute();
						}///while (first_attribute)
					}
					node_lp_quote = node_lp_quote->next_sibling();
				}
			}
			else if (!strcmp("TradeOrder", node_lp->name())) {
				auto first_attribute = node_lp->first_attribute();
				while (first_attribute) {
					if (!strcmp("enable", first_attribute->name())) {
						if (!strcmp("true", first_attribute->value())) {
							m_EnableTradeOrder = true;
						}
					}
					first_attribute = first_attribute->next_attribute();
				}///while (first_attribute)

				auto node_lp_trade = node_lp->first_node();
				while (node_lp_trade) {
					if (!strcmp("fixcon", node_lp_trade->name())) {
						auto first_attribute = node_lp_trade->first_attribute();
						while (first_attribute) {
							if (!strcmp("pathname", first_attribute->name())) {
								m_TradeOrderFixcon = m_objPath + first_attribute->value();
							}
							first_attribute = first_attribute->next_attribute();
						}///while (first_attribute)
					}
					else if (!strcmp("tcpto", node_lp_trade->name())) {
						auto first_attribute = node_lp_trade->first_attribute();
						while (first_attribute) {
							if (!strcmp("addr", first_attribute->name())) {
								m_TradeOrderTcptoAddress = first_attribute->value();
							}
							first_attribute = first_attribute->next_attribute();
						}///while (first_attribute)
					}
					else if (!strcmp("cache", node_lp_trade->name())) {
						auto first_attribute = node_lp_trade->first_attribute();
						while (first_attribute) {
							if (!strcmp("pathname", first_attribute->name())) {
								m_TradeOrderCachePathname = m_objPath + first_attribute->value();
							}
							first_attribute = first_attribute->next_attribute();
						}///while (first_attribute)
					}
					else if (!strcmp("login", node_lp_trade->name())) {
						auto first_attribute = node_lp_trade->first_attribute();
						while (first_attribute) {
							if (!strcmp("id", first_attribute->name())) {
								if (first_attribute->value_size()) {
									m_LoginFieldTradeOrder.ID = strtoull(first_attribute->value(), nullptr, 16) << 4 * 8;
								}
							}
							else if (!strcmp("password", first_attribute->name())) {
								if (first_attribute->value_size()) {
									sk::SafeCopyA(m_LoginFieldTradeOrder.Password, first_attribute->value(), _countof(m_LoginFieldTradeOrder.Password));
								}
							}
							else if (!strcmp("username", first_attribute->name())) {
								if (first_attribute->value_size()) {
									sk::SafeCopyA(m_LoginFieldTradeOrder.Username, first_attribute->value(), _countof(m_LoginFieldTradeOrder.Username));
								}
							}
							else if (!strcmp("identify", first_attribute->name())) {
								if (first_attribute->value_size()) {
									sk::SafeCopyA(m_LoginFieldTradeOrder.Identify, first_attribute->value(), _countof(m_LoginFieldTradeOrder.Identify));
								}
							}
							first_attribute = first_attribute->next_attribute();
						}///while (first_attribute)
					}
					node_lp_trade = node_lp_trade->next_sibling();
				}
			}
			else if (!strcmp("obj", node_lp->name())) {
				auto node_lp_obj = node_lp->first_node();
				while (node_lp_obj) {
					if (!strcmp("productssrc", node_lp_obj->name())) {
						auto first_attribute = node_lp_obj->first_attribute();
						while (first_attribute) {
							if (!strcmp("pathname", first_attribute->name())) {
								m_ProductSrcPathname = m_objPath + first_attribute->value();
							}
							first_attribute = first_attribute->next_attribute();
						}///while (first_attribute)
					}
					else if (!strcmp("api", node_lp_obj->name())) {
						auto first_attribute = node_lp_obj->first_attribute();
						while (first_attribute) {
							if (!strcmp("path", first_attribute->name())) {
								m_ApiPath = first_attribute->value();//!@ 注意Api指向使用绝对路径
							}
							first_attribute = first_attribute->next_attribute();
						}///while (first_attribute)
					}
					node_lp_obj = node_lp_obj->next_sibling();
				}

			}
			node_lp = node_lp->next_sibling();
		}///while (node_setup)
		return 0;
	}

	const shared::BridgeSystemSetup& Setup::BridgeSystemSetup()
	{
		std::lock_guard<std::mutex> lock{ m_mutex };
		return m_BridgeSystemSetup;
	}
	void Setup::BridgeSystemSetup(const shared::BridgeSystemSetup& setup)
	{
		std::lock_guard<std::mutex> lock{ m_mutex };
		m_BridgeSystemSetup = setup;
	}
}///namespace cfh