#if !defined(AFX_SKSTU_PRODUCTSCON_H__6FDE3_EB259BA0_B036DA5A84A18AD_62B1__HEAD__)
#define AFX_SKSTU_PRODUCTSCON_H__6FDE3_EB259BA0_B036DA5A84A18AD_62B1__HEAD__

#include "../../../gpl/rapidjson/reader.h"
#include "../../../gpl/rapidjson/document.h"
#include "../../../gpl/rapidjson/rapidjson.h"
#include "../../../gpl/rapidjson/stringbuffer.h"
#include "../../../gpl/rapidjson/writer.h"
#include "../../../gpl/rapidjson/stream.h"

namespace shared {

	const char JsonKeyQuoteSource[] = "QuoteSource";
	const char JsonKeyQuoteSourceLP[] = "LP";

	class ProductCon final {
	private:
		sk::stringa file;
		std::ostringstream os;
	public:
		static sk::stringa ProductConFileName(const shared::SystemProduct& sysProd) {
			return sk::Log::Format("{:X}.{}", sysProd.ID, sysProd.Symbol.symbol);
		}
		ProductCon(const sk::stringa& filePath, const shared::SystemProduct& sysProd) :
			file(filePath) {
			os << R"(<?xml version="1.0" encoding="ANSI" ?>)" << std::endl;
			os << R"(<SystemProduct)"
				<< R"( ID=")" << sk::Log::Format("{:X}", sysProd.ID) << R"(")"
				<< R"( Symbol=")" << sysProd.Symbol.symbol << R"(")"
				<< R"( Decimals=")" << tostringa(sysProd.Decimals) << R"(")"
				<< R"( SetupEnable=")" << sysProd.SetupEnable << R"(")"
				<< R"(>)"
				<< std::endl
				<< "\t" << R"(<![CDATA[)" << sysProd.Setup << R"(]]>)"
				<< std::endl;
		}
		~ProductCon() {
			if (!file.empty()) {
				std::ofstream of(file, std::ios::out | std::ios::trunc);
				if (of.is_open()) {
					of << os.str();
					of.close();
				}
			}
		}
		sk::stringa BufferGet() const { return os.str(); }
	public:
		void EnconEnd() {
			os << R"(</SystemProduct>)" << std::endl;
		}
		void EnconProd(const shared::LPProduct& lpProd) {
			os << '\t'
				<< R"(<LPProduct)"
				<< R"( ID=")" << sk::Log::Format("{:X}", lpProd.ID) << R"(")"
				<< R"( Symbol=")" << lpProd.Symbol.symbol << R"(")"
				<< R"( SystemSymbol=")" << lpProd.SystemSymbol.symbol << R"(")"
				<< R"( QuotedCCY=")" << lpProd.QuotedCCY.currency << R"(")"
				<< R"( ContractSize=")" << tostringa(lpProd.ContractSize) << R"(")"
				<< R"( TickSize=")" << tostringa(lpProd.TickSize) << R"(")"
				<< R"( MinOrderSize=")" << tostringa(lpProd.MinOrderSize) << R"(")"
				<< R"( MaxOrderSize=")" << tostringa(lpProd.MaxOrderSize) << R"(")"
				<< R"( StepOrderSize=")" << tostringa(lpProd.StepOrderSize) << R"(")"
				<< R"(/>)"
				<< std::endl;
		}
		void EnconProd(const shared::MTFeederProduct& feederProd) {
			os << '\t'
				<< R"(<FeederProduct)"
				<< R"( ID=")" << sk::Log::Format("{:X}", feederProd.ID) << R"(")"
				<< R"( Symbol=")" << feederProd.Symbol.symbol << R"(")"
				<< R"( SpreadAsk=")" << tostringa(feederProd.SpreadAsk) << R"(")"
				<< R"( SpreadBid=")" << tostringa(feederProd.SpreadBid) << R"(")"
				<< R"( SpreadTotal=")" << tostringa(feederProd.SpreadTotal) << R"(")"
				<< R"( EnableQuote=")" << (feederProd.EnableQuote ? "true" : "false") << R"(")"
				<< R"(/>)"
				<< std::endl;
		}
		void EnconProd(const shared::MTGatewayProduct& gatewayProd) {
			os << '\t'
				<< R"(<GatewayProduct)"
				<< R"( ID=")" << sk::Log::Format("{:X}", gatewayProd.ID) << R"(")"
				<< R"( Symbol=")" << gatewayProd.Symbol.symbol << R"(")"
				<< R"( ContractSize=")" << tostringa(gatewayProd.ContractSize) << R"(")"
				<< R"( SpreadAsk=")" << tostringa(gatewayProd.SpreadAsk) << R"(")"
				<< R"( SpreadBid=")" << tostringa(gatewayProd.SpreadBid) << R"(")"
				<< R"( SpreadTotal=")" << tostringa(gatewayProd.SpreadTotal) << R"(")"
				<< R"( Digits=")" << tostringa(gatewayProd.Digits) << R"(")"
				<< R"( Point=")" << tostringa(gatewayProd.Point) << R"(")"
				<< R"( AutoAdjustPositionSpreadRange=")" << tostringa(gatewayProd.AutoAdjustPositionSpreadRange) << R"(")"
				<< R"( AssignOrderLP=")" << sk::Log::Format("{:X}", gatewayProd.AssignOrderLP) << R"(")"
				<< R"( EnableTrade=")" << (gatewayProd.EnableTrade ? "true" : "false") << R"(")"
				<< R"(/>)"
				<< std::endl;
		}

		static
			int DeconBuffer
			(
				__in const sk::packet& buffer,
				__out shared::SystemProduct& sysProd,
				__out sk::container::set<shared::LPProduct>& lpProds,
				__out sk::container::set<shared::MTFeederProduct>& mtFeederProds,
				__out sk::container::set<shared::MTGatewayProduct>& mtGatewayProds
			)
		{
			memset(&sysProd, 0x00, shared::LENSYSTEMPRODUCT);
			lpProds.clear();
			mtFeederProds.clear();
			mtGatewayProds.clear();
			if (buffer.empty()) return -1;
			std::stringstream gangplank;
			std::iostream isetting(gangplank.rdbuf());
			isetting.write(buffer.data(), buffer.size());
			auto xmlfile = std::make_shared<rapidxml::file<>>(isetting);
			auto xmldoc = std::make_shared<rapidxml::xml_document<>>();
			try {
				xmldoc->parse<0>(xmlfile->data());
			}
			catch (rapidxml::parse_error& err) {
				auto what = err.what();
				return -1;
			}

			auto beginNode = xmldoc->first_node("SystemProduct");
			//! 系统产品结构
			auto first_attribute = beginNode->first_attribute();
			while (first_attribute) {
				if (!strcmp("ID", first_attribute->name())) {
					sysProd.ID = shared::SystemIDType::SystemProductType(std::stoll(first_attribute->value(), nullptr, 16));
				}
				else if (!strcmp("Symbol", first_attribute->name())) {
					sysProd.Symbol = first_attribute->value();
				}
				else if (!strcmp("Decimals", first_attribute->name())) {
					sysProd.Decimals = strtol(first_attribute->value(), nullptr, 10);
				}
				else if (!strcmp("SetupEnable", first_attribute->name())) {
					sysProd.SetupEnable = strtol(first_attribute->value(), nullptr, 10);
				}
				first_attribute = first_attribute->next_attribute();
			}

			auto nextNode = beginNode->first_node();
			while (nextNode) {
				if (nextNode->value_size() > 0)
				{
					sk::SafeCopyA(sysProd.Setup, nextNode->value(), _countof(sysProd.Setup));
				}
				else if (!strcmp("LPProduct", nextNode->name())) {
					shared::LPProduct lpProd;
					auto first_attribute = nextNode->first_attribute();
					while (first_attribute) {
						if (!strcmp("ID", first_attribute->name())) {
							lpProd.ID = strtoull(first_attribute->value(), nullptr, 16);
						}
						else if (!strcmp("Symbol", first_attribute->name())) {
							lpProd.Symbol = first_attribute->value();
						}
						else if (!strcmp("SystemSymbol", first_attribute->name())) {
							lpProd.SystemSymbol = first_attribute->value();
						}
						else if (!strcmp("QuotedCCY", first_attribute->name())) {
							lpProd.QuotedCCY = first_attribute->value();
						}
						else if (!strcmp("ContractSize", first_attribute->name())) {
							lpProd.ContractSize = strtod(first_attribute->value(), nullptr);
						}
						else if (!strcmp("TickSize", first_attribute->name())) {
							lpProd.TickSize = strtod(first_attribute->value(), nullptr);
						}
						else if (!strcmp("MinOrderSize", first_attribute->name())) {
							lpProd.MinOrderSize = strtod(first_attribute->value(), nullptr);
						}
						else if (!strcmp("MaxOrderSize", first_attribute->name())) {
							lpProd.MaxOrderSize = strtod(first_attribute->value(), nullptr);
						}
						else if (!strcmp("StepOrderSize", first_attribute->name())) {
							lpProd.StepOrderSize = strtod(first_attribute->value(), nullptr);
						}
						first_attribute = first_attribute->next_attribute();
					}///while
					if (lpProd.ID > 0)
					{
						lpProds.push(lpProd);
					}
				}
				else if (!strcmp("FeederProduct", nextNode->name())) {
					shared::MTFeederProduct mtFeederProd;
					auto first_attribute = nextNode->first_attribute();
					while (first_attribute) {
						if (!strcmp("ID", first_attribute->name())) {
							mtFeederProd.ID = strtoull(first_attribute->value(), nullptr, 16);
						}
						else if (!strcmp("Symbol", first_attribute->name())) {
							mtFeederProd.Symbol = first_attribute->value();
						}
						else if (!strcmp("SpreadAsk", first_attribute->name())) {
							mtFeederProd.SpreadAsk = strtol(first_attribute->value(), nullptr, 10);
						}
						else if (!strcmp("SpreadBid", first_attribute->name())) {
							mtFeederProd.SpreadBid = strtol(first_attribute->value(), nullptr, 10);
						}
						else if (!strcmp("SpreadTotal", first_attribute->name())) {
							mtFeederProd.SpreadTotal = strtol(first_attribute->value(), nullptr, 10);
						}
						else if (!strcmp("Point", first_attribute->name())) {
							mtFeederProd.Point = strtod(first_attribute->value(), nullptr);
						}
						else if (!strcmp("EnableQuote", first_attribute->name())) {
							mtFeederProd.EnableQuote = false;
							if (!strcmp("true", first_attribute->value())) {
								mtFeederProd.EnableQuote = true;
							}
						}
						first_attribute = first_attribute->next_attribute();
					}///while

					if (mtFeederProd.ID > 0)
					{
						mtFeederProds.push(mtFeederProd);
					}
				}
				else if (!strcmp("GatewayProduct", nextNode->name())) {
					shared::MTGatewayProduct mtGatewayProd;
					auto first_attribute = nextNode->first_attribute();
					while (first_attribute) {
						if (!strcmp("ID", first_attribute->name())) {
							mtGatewayProd.ID = strtoull(first_attribute->value(), nullptr, 16);
						}
						else if (!strcmp("Symbol", first_attribute->name())) {
							mtGatewayProd.Symbol = first_attribute->value();
						}
						else if (!strcmp("SpreadAsk", first_attribute->name())) {
							mtGatewayProd.SpreadAsk = strtol(first_attribute->value(), nullptr, 10);
						}
						else if (!strcmp("SpreadBid", first_attribute->name())) {
							mtGatewayProd.SpreadBid = strtol(first_attribute->value(), nullptr, 10);
						}
						else if (!strcmp("SpreadTotal", first_attribute->name())) {
							mtGatewayProd.SpreadTotal = strtol(first_attribute->value(), nullptr, 10);
						}
						else if (!strcmp("ContractSize", first_attribute->name())) {
							mtGatewayProd.ContractSize = strtod(first_attribute->value(), nullptr);
						}
						else if (!strcmp("Digits", first_attribute->name())) {
							mtGatewayProd.Digits = strtol(first_attribute->value(), nullptr, 10);
						}
						else if (!strcmp("Point", first_attribute->name())) {
							mtGatewayProd.Point = std::stod(first_attribute->value());
						}
						else if (!strcmp("AutoAdjustPositionSpreadRange", first_attribute->name())) {
							mtGatewayProd.AutoAdjustPositionSpreadRange = strtol(first_attribute->value(), nullptr, 10);
						}
						else if (!strcmp("AssignOrderLP", first_attribute->name())) {
							mtGatewayProd.AssignOrderLP = strtoull(first_attribute->value(), nullptr, 16);
						}
						else if (!strcmp("EnableTrade", first_attribute->name())) {
							mtGatewayProd.EnableTrade = false;
							if (!strcmp("true", first_attribute->value())) {
								mtGatewayProd.EnableTrade = true;
							}
						}
						first_attribute = first_attribute->next_attribute();
					}///while

					if (mtGatewayProd.ID > 0)
					{
						mtGatewayProds.push(mtGatewayProd);
					}
				}

				nextNode = nextNode->next_sibling();
			}

			xmldoc->clear();
			return 0;
		}


		static sk::packet MadeSystemSetup(const sk::container::set<shared::SystemID>& inQuoteSourceSel)
		{
			sk::packet result;
			if (inQuoteSourceSel.empty())
				return result;
			rapidjson::Document doc;
			doc.SetObject();
			rapidjson::Document::AllocatorType& JsonAllocator = doc.GetAllocator();
			rapidjson::Value Actions(rapidjson::kArrayType);
			Actions.SetArray();
			inQuoteSourceSel.iterate(
				[&](const auto& exchange, auto& itbreak)
				{
					rapidjson::Value Node(rapidjson::kObjectType);
					Node.SetObject();
					Node.AddMember(
						rapidjson::Value().SetString(JsonKeyQuoteSourceLP, JsonAllocator).Move(),
						exchange,
						JsonAllocator);
					Actions.PushBack(Node, JsonAllocator);
				});
			doc.AddMember(rapidjson::Value().SetString(JsonKeyQuoteSource, JsonAllocator).Move(), Actions, JsonAllocator);
			rapidjson::StringBuffer JsonBuffer;
			if (doc.IsObject()) {
				rapidjson::Writer<rapidjson::StringBuffer> JsonWriter(JsonBuffer);
				doc.Accept(JsonWriter);
			}
			return JsonBuffer.GetString();
		}

		static void UnMadeSystemSetup(const sk::packet& in, sk::container::set<shared::SystemID>& out)
		{
			if (in.empty())
				return;
			rapidjson::Document doc;
			rapidjson::Document::AllocatorType& Allocator = doc.GetAllocator();
			if (doc.Parse(in.c_str()).HasParseError()) { return; }
			if (!doc.HasMember(rapidjson::Value().SetString(JsonKeyQuoteSource, Allocator).Move()))
			{
				return;
			}
			auto JsonKeyQuoteSourceObj = doc.FindMember(rapidjson::Value().SetString(JsonKeyQuoteSource, Allocator).Move());
			if (!JsonKeyQuoteSourceObj->value.IsArray())
				return;
			for (auto itAynode = JsonKeyQuoteSourceObj->value.Begin(); itAynode != JsonKeyQuoteSourceObj->value.End(); ++itAynode)
			{
				if (!itAynode->HasMember(rapidjson::Value().SetString(JsonKeyQuoteSourceLP, Allocator).Move()))
				{
					return;
				}
				auto val = itAynode->FindMember(rapidjson::Value().SetString(JsonKeyQuoteSourceLP, Allocator).Move());
				if (val->value.IsNumber() && !val->value.IsDouble())
				{
					out.push(val->value.GetUint64());
				}
			}
		}


	};



}///namespace shared




/// /*新生联创（上海）**/
/// /*2019年12月1日 12:11:21|569**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///AFX_SKSTU_PRODUCTSCON_H__6FDE3_EB259BA0_B036DA5A84A18AD_62B1__TAIL__