#include "stdafx.h"
#include "SymbolCon.h"

namespace proxy {

	SymbolCon::SymbolCon(const sk::stringa& SymbolsPath) :
		m_SymbolsPath(SymbolsPath),
		m_mutex(std::make_shared<std::mutex>()) {
		sk::Helper::CreateDirectoryA(m_SymbolsPath.c_str());
		Init();
	}

	SymbolCon::~SymbolCon() {
		UnInit();
	}

	int SymbolCon::Init()
	{
		Global::SharedGet()->m_SystemProductIDQ.clear();
		Global::SharedGet()->m_SystemProductFinalQ.clear();
		Global::SharedGet()->m_LPProductFinalQ.clear();
		Global::SharedGet()->m_FeederProductFinalQ.clear();
		Global::SharedGet()->m_GatewayProductFinalQ.clear();
		Global::SharedGet()->m_MarketDataFinalQ.clear();
		m_SymbolConBufferQ.clear();

		std::map<std::string, _finddata_t> m_Files;
		sk::Helper::EnumFile(m_SymbolsPath, m_Files, "*.*", false);
		for (const auto& file : m_Files)
		{
			auto fileBuffer = sk::Helper::LoadFile((m_SymbolsPath + file.first).c_str());
			if (!fileBuffer.empty())
			{
				shared::SystemProduct sysProd;
				sk::container::set<shared::LPProduct> lpProds;
				sk::container::set<shared::MTFeederProduct> feederProds;
				sk::container::set<shared::MTGatewayProduct> gatewayProds;
				if (!shared::ProductCon::DeconBuffer(
					sk::packet(&fileBuffer[0], fileBuffer.size()),
					sysProd,
					lpProds,
					feederProds,
					gatewayProds)) {
					if (sysProd.ID > 0)
					{
						Global::SharedGet()->m_SystemProductIDQ.push(sysProd.Symbol, sysProd.ID);
						Global::SharedGet()->m_SystemProductFinalQ.push(sysProd.ID, sysProd);
						Global::SharedGet()->m_LPProductFinalQ.push(sysProd.ID, lpProds);
						Global::SharedGet()->m_FeederProductFinalQ.push(sysProd.ID, feederProds);
						Global::SharedGet()->m_GatewayProductFinalQ.push(sysProd.ID, gatewayProds);
						//! 初始化聚合报价缓冲
						Global::SharedGet()->m_MarketDataFinalQ.push(sysProd.ID, shared::marketdata::MarketData());
						//! 备份映像文件树
						m_SymbolConBufferQ.push(sysProd.ID, sk::packet(&fileBuffer[0], fileBuffer.size()));
					}
				}
			}
		}
		if (Global::SharedGet()->m_SystemProductFinalQ.empty())
		{
			Global::Log(sk::EnLogType::EN_LOG_TYPE_ERROR)->error("{}", "请初始化系统产品树");
		}
		else
		{
			Global::Log(sk::EnLogType::EN_LOG_TYPE_WARN)->warn("载入系统产品({})", Global::SharedGet()->m_SystemProductFinalQ.size());
		}
		return 0;
	}

	int SymbolCon::UnInit() {
		return 0;
	}

	int SymbolCon::Reload()
	{
		std::lock_guard<std::mutex> lock(*m_mutex);
		return Init();
	}

	int SymbolCon::Clear() {
		std::lock_guard<std::mutex> lock(*m_mutex);
		Global::SharedGet()->m_SystemProductIDQ.clear();
		Global::SharedGet()->m_SystemProductFinalQ.clear();
		Global::SharedGet()->m_LPProductFinalQ.clear();
		Global::SharedGet()->m_FeederProductFinalQ.clear();
		Global::SharedGet()->m_GatewayProductFinalQ.clear();
		Global::SharedGet()->m_MarketDataFinalQ.clear();
		std::map<std::string, _finddata_t> m_Files;
		sk::Helper::EnumFile(m_SymbolsPath, m_Files, "*.*", false);
		for (const auto& file : m_Files) {
			::DeleteFileA((m_SymbolsPath + file.first).c_str());
		}

		m_SymbolConBufferQ.clear();


		return 0;
	}

	int SymbolCon::Push
	(
		__in const sk::packet& xmlSingleBuffer,
		__out shared::SystemProduct& sysProd,
		__out sk::container::set<shared::LPProduct>& lpProds,
		__out sk::container::set<shared::MTFeederProduct>& feederProds,
		__out sk::container::set<shared::MTGatewayProduct>& gatewayProds
	)
	{
		lpProds.clear();
		feederProds.clear();
		gatewayProds.clear();
		memset(&sysProd, 0x00, shared::LENSYSTEMPRODUCT);

		if (!shared::ProductCon::DeconBuffer(
			xmlSingleBuffer,
			sysProd,
			lpProds,
			feederProds,
			gatewayProds))
		{
			if (sysProd.ID > 0) {
				Global::SharedGet()->m_SystemProductIDQ.push(sysProd.Symbol, sysProd.ID);
				Global::SharedGet()->m_SystemProductFinalQ.push(sysProd.ID, sysProd);
				Global::SharedGet()->m_LPProductFinalQ.push(sysProd.ID, lpProds);
				Global::SharedGet()->m_FeederProductFinalQ.push(sysProd.ID, feederProds);
				Global::SharedGet()->m_GatewayProductFinalQ.push(sysProd.ID, gatewayProds);
				//! 初始化聚合报价缓冲
				Global::SharedGet()->m_MarketDataFinalQ.push(sysProd.ID, shared::marketdata::MarketData());
				//! 备份映像文件树
				m_SymbolConBufferQ.push(sysProd.ID, xmlSingleBuffer);
			}
			return 0;
		}
		return -1;
	}

	int SymbolCon::Push(/*const shared::MarketType& sysProdID, */const sk::packet& xmlSingleBuffer) {
		std::lock_guard<std::mutex> lock(*m_mutex);

		shared::SystemProduct sysProd;
		sk::container::set<shared::LPProduct> lpProds;
		sk::container::set<shared::MTFeederProduct> feederProds;
		sk::container::set<shared::MTGatewayProduct> gatewayProds;
		if (!shared::ProductCon::DeconBuffer(
			xmlSingleBuffer,
			sysProd,
			lpProds,
			feederProds,
			gatewayProds)) {
			if (sysProd.ID > 0) {
				Global::SharedGet()->m_SystemProductIDQ.push(sysProd.Symbol, sysProd.ID);
				Global::SharedGet()->m_SystemProductFinalQ.push(sysProd.ID, sysProd);
				Global::SharedGet()->m_LPProductFinalQ.push(sysProd.ID, lpProds);
				Global::SharedGet()->m_FeederProductFinalQ.push(sysProd.ID, feederProds);
				Global::SharedGet()->m_GatewayProductFinalQ.push(sysProd.ID, gatewayProds);
				//! 初始化聚合报价缓冲
				Global::SharedGet()->m_MarketDataFinalQ.push(sysProd.ID, shared::marketdata::MarketData());
				//! 备份映像文件树
				m_SymbolConBufferQ.push(sysProd.ID, xmlSingleBuffer);
			}
			return 0;
		}
		return -1;
	}

	int SymbolCon::Pop(const shared::SystemID& sysProdID) {
		std::lock_guard<std::mutex> lock(*m_mutex);
		Global::SharedGet()->m_SystemProductFinalQ.search(
			sysProdID,
			[&](const auto&, auto& sysProdObj)
			{
				Global::SharedGet()->m_SystemProductIDQ.pop(sysProdObj.Symbol, [](const auto&, auto&) {});
				Global::SharedGet()->m_SystemProductFinalQ.pop(sysProdID, [](const auto&, auto&) {});
				Global::SharedGet()->m_LPProductFinalQ.pop(sysProdID, [](const auto&, auto&) {});
				Global::SharedGet()->m_FeederProductFinalQ.pop(sysProdID, [](const auto&, auto&) {});
				Global::SharedGet()->m_GatewayProductFinalQ.pop(sysProdID, [](const auto&, auto&) {});
				Global::SharedGet()->m_MarketDataFinalQ.pop(sysProdID, [](const auto&, auto&) {});
				m_SymbolConBufferQ.pop(sysProdID, [](const auto&, auto&) {});
			});
		return 0;
	}

	int SymbolCon::Finalize() {
		std::lock_guard<std::mutex> lock(*m_mutex);
		//! 清空文件夹
		std::map<std::string, _finddata_t> m_Files;
		sk::Helper::EnumFile(m_SymbolsPath, m_Files, "*.*", false);
		for (const auto& file : m_Files) {
			::DeleteFileA((m_SymbolsPath + file.first).c_str());
		}
		m_SymbolConBufferQ.clear();
		Global::SharedGet()->m_SystemProductFinalQ.iterate(
			[&](const auto& sysProdID, auto& sysProdObj)
			{
				shared::ProductCon prodConObj(\
					Global::GetSetup()->GetSymbolsPath() + shared::ProductCon::ProductConFileName(sysProdObj), sysProdObj);
				auto lpProds = Global::SharedGet()->m_LPProductFinalQ.search(sysProdID);
				if (lpProds) {
					lpProds->iterate([&](auto& prod) {prodConObj.EnconProd(prod); });
				}
				auto mtFeederProds = Global::SharedGet()->m_FeederProductFinalQ.search(sysProdID);
				if (mtFeederProds) {
					mtFeederProds->iterate([&](auto& prod) {prodConObj.EnconProd(prod); });
				}
				auto mtGatewayProds = Global::SharedGet()->m_GatewayProductFinalQ.search(sysProdID);
				if (mtGatewayProds) {
					mtGatewayProds->iterate([&](auto& prod) {prodConObj.EnconProd(prod); });
				}
				prodConObj.EnconEnd();
				m_SymbolConBufferQ.push(sysProdObj.ID, prodConObj.BufferGet());
			});
		return 0;
	}

	int SymbolCon::Finalize(const shared::SystemID& sysProdID) {
		int result = -1;
		std::lock_guard<std::mutex> lock(*m_mutex);
		Global::SharedGet()->m_SystemProductFinalQ.search(
			sysProdID,
			[&](const auto&, auto& sysProdObj)
			{
				::DeleteFileA((m_SymbolsPath + shared::ProductCon::ProductConFileName(sysProdObj)).c_str());
				shared::ProductCon prodConObj(m_SymbolsPath + shared::ProductCon::ProductConFileName(sysProdObj), sysProdObj);
				auto lpProds = Global::SharedGet()->m_LPProductFinalQ.search(sysProdID);
				if (lpProds) {
					lpProds->iterate([&](auto& prod) {prodConObj.EnconProd(prod); });
				}
				auto mtFeederProds = Global::SharedGet()->m_FeederProductFinalQ.search(sysProdID);
				if (mtFeederProds) {
					mtFeederProds->iterate([&](auto& prod) {prodConObj.EnconProd(prod); });
				}
				auto mtGatewayProds = Global::SharedGet()->m_GatewayProductFinalQ.search(sysProdID);
				if (mtGatewayProds) {
					mtGatewayProds->iterate([&](auto& prod) {prodConObj.EnconProd(prod); });
				}
				prodConObj.EnconEnd();
				m_SymbolConBufferQ.push(sysProdObj.ID, prodConObj.BufferGet());

				result = 0;
			});
		return result;
	}

	std::vector<shared::SystemProduct>
		SymbolCon::GetSystemProducts() {
		std::vector<shared::SystemProduct> result;
		std::lock_guard<std::mutex> lock(*m_mutex);
		Global::SharedGet()->m_SystemProductFinalQ.iterate(
			[&](const auto& sysProdID, auto& sysProdObj) {
				result.emplace_back(sysProdObj);
			});
		return result;
	}

	int SymbolCon::Refresh() {
		std::lock_guard<std::mutex> lock(*m_mutex);
		Init();
		return 0;
	}

	int SymbolCon::Refresh(const shared::SystemID& sysProdID) {
		std::lock_guard<std::mutex> lock(*m_mutex);
		Global::SharedGet()->m_SystemProductFinalQ.search(
			sysProdID,
			[&](const auto&, auto& sysProdObj)
			{
				auto fileBuffer = sk::Helper::LoadFile((m_SymbolsPath + shared::ProductCon::ProductConFileName(sysProdObj)).c_str());
				if (!fileBuffer.empty()) {
					shared::SystemProduct sysProd;
					sk::container::set<shared::LPProduct> lpProds;
					sk::container::set<shared::MTFeederProduct> feederProds;
					sk::container::set<shared::MTGatewayProduct> gatewayProds;
					if (!shared::ProductCon::DeconBuffer(
						sk::packet(&fileBuffer[0], fileBuffer.size()),
						sysProd,
						lpProds,
						feederProds,
						gatewayProds)) {
						if (sysProd.ID > 0) {
							Global::SharedGet()->m_SystemProductIDQ.push(sysProd.Symbol, sysProd.ID);
							Global::SharedGet()->m_SystemProductFinalQ.push(sysProd.ID, sysProd);
							Global::SharedGet()->m_LPProductFinalQ.push(sysProd.ID, lpProds);
							Global::SharedGet()->m_FeederProductFinalQ.push(sysProd.ID, feederProds);
							Global::SharedGet()->m_GatewayProductFinalQ.push(sysProd.ID, gatewayProds);
							//! 初始化聚合报价缓冲
							Global::SharedGet()->m_MarketDataFinalQ.push(sysProd.ID, shared::marketdata::MarketData());
							//! 备份映像文件树
							m_SymbolConBufferQ.push(sysProd.ID, sk::packet(&fileBuffer[0], fileBuffer.size()));
						}
					}
				}
			});
		return 0;
	}

	size_t SymbolCon::Count() {
		std::lock_guard<std::mutex> lock(*m_mutex);
		return m_SymbolConBufferQ.size();
	}








	std::shared_ptr<sk::packet> SymbolCon::GetSystemSymbolConBuffer(const shared::SystemID& sysProdID) {
		std::shared_ptr<sk::packet> result;
		std::lock_guard<std::mutex> lock(*m_mutex);
		return m_SymbolConBufferQ.search(sysProdID);
	}
	shared::SystemID SymbolCon::GetSystemProductID(const shared::Instrument& symbol) {
		shared::SystemID result = 0;
		std::lock_guard<std::mutex> lock(*m_mutex);
		Global::SharedGet()->m_SystemProductIDQ.search(
			symbol,
			[&](const auto& sysSymbol, auto& sysProdID)
			{
				result = sysProdID;
			});
		return result;
	}


	std::shared_ptr<shared::MTFeederProduct> 
		SymbolCon::GetMTFeederProduct(const shared::SystemID& feederProdID)
	{
		std::shared_ptr<shared::MTFeederProduct> result;
		std::lock_guard<std::mutex> lock(*m_mutex);
		Global::SharedGet()->m_FeederProductFinalQ.search(
			shared::SystemIDType::SystemProductType(feederProdID),
			[&](const auto&, auto& prods)
			{
				prods.iterate(
					[&](auto& prod, auto& itbreak)
					{
						if (prod.ID == feederProdID)
						{
							result = std::make_shared<shared::MTFeederProduct>(prod);
							itbreak = true;
						}
					});
			});
		return result;
	}

	std::shared_ptr<shared::SystemProduct>
		SymbolCon::GetSystemProduct(const shared::SystemID& sysProdID)
	{
		std::lock_guard<std::mutex> lock(*m_mutex);
		return Global::SharedGet()->m_SystemProductFinalQ.search(shared::SystemIDType::SystemProductType(sysProdID));
	}

	std::shared_ptr<shared::MTGatewayProduct> 
		SymbolCon::GetMTGatewayProduct(const shared::SystemID& gatewayProdID)
	{
		std::shared_ptr<shared::MTGatewayProduct> result;
		std::lock_guard<std::mutex> lock(*m_mutex);
		Global::SharedGet()->m_GatewayProductFinalQ.search(
			shared::SystemIDType::SystemProductType(gatewayProdID),
			[&](const auto&, auto& prods)
			{
				prods.iterate(
					[&](auto& prod, auto& itbreak)
					{
						if (prod.ID == gatewayProdID)
						{
							result = std::make_shared<shared::MTGatewayProduct>(prod);
							itbreak = true;
						}
					});
			});
		return result;
	}


	std::shared_ptr<shared::LPProduct>
		SymbolCon::GetLPProduct(const shared::SystemID& lpID) {
		std::shared_ptr<shared::LPProduct> result;
		std::lock_guard<std::mutex> lock(*m_mutex);
		Global::SharedGet()->m_LPProductFinalQ.search(
			shared::SystemIDType::SystemProductType(lpID),
			[&](const auto&, auto& lpProds)
			{
				lpProds.iterate(
					[&](const auto& lpProd, auto& itbreak)
					{
						if (shared::SystemIDType::ExchangeType(lpID) == shared::SystemIDType::ExchangeType(lpProd.ID)) {
							result = std::make_shared<shared::LPProduct>(lpProd);
							itbreak = true;
						}
					});
			});
		return result;
	}

	std::vector<shared::LPProduct> SymbolCon::GetLPProducts(const shared::SystemID& exchangeID)
	{
		std::vector<shared::LPProduct> result;
		std::lock_guard<std::mutex> lock(*m_mutex);
		Global::SharedGet()->m_LPProductFinalQ.iterate(
			[&](const auto&, auto& lpProds)
			{
				lpProds.iterate(
					[&](auto& lpProdObj) {
						if (shared::SystemIDType::ExchangeType(lpProdObj.ID) == shared::SystemIDType::ExchangeType(exchangeID))
						{
							result.emplace_back(lpProdObj);
						}
					});
			});
		return result;
	}
	std::vector<shared::MTFeederProduct> SymbolCon::GetFeederProducts(const shared::SystemID& feederID) {
		std::vector<shared::MTFeederProduct> result;
		std::lock_guard<std::mutex> lock(*m_mutex);
		Global::SharedGet()->m_FeederProductFinalQ.iterate(
			[&](const auto&, auto& feederProds) {
				feederProds.iterate(
					[&](auto& feederProdObj) {
						if (shared::SystemIDType::ServiceObjectType(feederProdObj.ID) == shared::SystemIDType::ServiceObjectType(feederID))
						{
							result.emplace_back(feederProdObj);
						}
					});
			});
		return result;
	}
	std::vector<shared::MTGatewayProduct> SymbolCon::GetGatewayProducts(const shared::SystemID& gatewayID) {
		std::vector<shared::MTGatewayProduct> result;
		std::lock_guard<std::mutex> lock(*m_mutex);
		Global::SharedGet()->m_GatewayProductFinalQ.iterate(
			[&](const auto&, auto& gatewayProds) {
				gatewayProds.iterate(
					[&](auto& gatewayProdObj) {
						if (shared::SystemIDType::ServiceObjectType(gatewayProdObj.ID) == shared::SystemIDType::ServiceObjectType(gatewayID))
						{
							result.emplace_back(gatewayProdObj);
						}
					});
			});
		return result;
	}

	bool SymbolCon::SetFeederProductByMTSymbol(const shared::SystemID& mtFeederProductID, const shared::ConSymbolField& mtSymbol)
	{
		bool result = false;
		auto symbol = sk::StringConvert::WStringToMBytes(mtSymbol.Symbol);

		Global::SharedGet()->m_SystemProductIDQ.iterate(
			[&](const auto& sysSymbol, auto& sysProdID, const bool&, bool& itbreak)
			{
				if (symbol.find(sysSymbol.symbol) != sk::stringa::npos)
				{
					Global::SharedGet()->m_SystemProductFinalQ.search(
						sysProdID,
						[&](const auto&, auto& sysProd)
						{
							Global::SharedGet()->m_FeederProductFinalQ.pushpush(
								sysProdID,
								[&](auto& setObj) {
									shared::MTFeederProduct feederProduct;
									feederProduct.Symbol = symbol.c_str();
									auto feederProductAt = setObj.search(feederProduct);
									if (!feederProductAt)
									{
										feederProduct.ID = mtFeederProductID | sysProdID | (setObj.size() + 1);
										setObj.push(feederProduct);
									}
									else
									{

									}

									result = true;
								});
						});
					itbreak = true;
				}
			});
		return result;
	}
	bool SymbolCon::SetGatewayProductByMTSymbol(const shared::SystemID& mtGatewayProductID, const shared::ConSymbolField& mtSymbol)
	{
		bool result = false;
		auto symbol = sk::StringConvert::WStringToMBytes(mtSymbol.Symbol);

		Global::SharedGet()->m_SystemProductIDQ.iterate(
			[&](const auto& sysSymbol, auto& sysProdID, const bool&, bool& itbreak)
			{
				if (symbol.find(sysSymbol.symbol) != sk::stringa::npos)
				{
					Global::SharedGet()->m_SystemProductFinalQ.search(
						sysProdID,
						[&](const auto&, auto& sysProd)
						{
							Global::SharedGet()->m_GatewayProductFinalQ.pushpush(
								sysProdID,
								[&](auto& setObj) {
									shared::MTGatewayProduct gatewayProduct;
									gatewayProduct.Symbol = symbol.c_str();
									auto gatewayProductAt = setObj.search(gatewayProduct);
									if (!gatewayProductAt)
									{
										gatewayProduct.ID = mtGatewayProductID | sysProdID | (setObj.size() + 1);
										gatewayProduct.ContractSize = mtSymbol.ContractSize;
										gatewayProduct.Digits = mtSymbol.Digits;
										gatewayProduct.Point = mtSymbol.Point;
										setObj.push(gatewayProduct);
									}
									else
									{
										gatewayProduct.ID = gatewayProductAt->ID;
										gatewayProduct.ContractSize = mtSymbol.ContractSize;
										gatewayProduct.Digits = mtSymbol.Digits;
										gatewayProduct.Point = mtSymbol.Point;
										setObj.push(gatewayProduct);
									}
									result = true;
								});
						});
					itbreak = true;
				}
			});
		return result;
	}
	int SymbolCon::Initialize() {
		std::lock_guard<std::mutex> lock(*m_mutex);
		//! 如果LP缓冲是空的就不能初始化
		if (Global::SharedGet()->m_ExchangeProductQ.empty())
		{
			Global::Log()->error("{} {}", __FUNCTION__, "LP产品数据缓冲为空,未有支持的LP数据");
			return -1;
		}

		Global::Log()->warn("{}", "初始化系统产品树 . . . ");

		//! 清空运行时装备数据缓冲
		Global::SharedGet()->m_LPProductFinalQ.clear();
		Global::SharedGet()->m_SystemProductFinalQ.clear();
		Global::SharedGet()->m_FeederProductFinalQ.clear();
		Global::SharedGet()->m_GatewayProductFinalQ.clear();
		Global::SharedGet()->m_MarketDataFinalQ.clear();

		shared::SystemID nextSystemID = 1;
		sk::container::map<shared::Instrument, sk::container::set<shared::LPProduct>> tempMapping;
		Global::SharedGet()->m_ExchangeProductQ.iterate(
			[&](const auto& lpProductID/*0xA00000000*/, auto& lpProds)
			{
				lpProds.iterate(
					[&](const auto, auto& lpProd)
					{
						//! Make System Product Field
						if (!tempMapping.search(
							lpProd.SystemSymbol,
							[&](const auto&, auto& objSet)
							{
								Global::SharedGet()->m_SystemProductIDQ.search(
									lpProd.SystemSymbol,
									[&](const auto&, auto& sysID)
									{
										lpProd.ID = lpProd.ID | sysID | lpProductID;
										objSet.push(lpProd);
									});
							}))
						{
							lpProd.ID = lpProd.ID | (nextSystemID << (11 * 4)) | lpProductID;
							tempMapping.pushpush(lpProd.SystemSymbol, [&](auto& objSet) {objSet.push(lpProd); });
							Global::SharedGet()->m_SystemProductIDQ.push(lpProd.SystemSymbol, (nextSystemID << (11 * 4)));
							shared::SystemProduct systemProduct;
							systemProduct.ID = (nextSystemID << (11 * 4));
							systemProduct.Symbol = lpProd.SystemSymbol;
							Global::SharedGet()->m_SystemProductFinalQ.push(systemProduct.ID, systemProduct);
							Global::SharedGet()->m_FeederProductFinalQ.push(systemProduct.ID, sk::container::set<shared::MTFeederProduct>());
							Global::SharedGet()->m_GatewayProductFinalQ.push(systemProduct.ID, sk::container::set<shared::MTGatewayProduct>());
							//! 初始化聚合报价缓冲
							Global::SharedGet()->m_MarketDataFinalQ.push(systemProduct.ID, shared::marketdata::MarketData());
							++nextSystemID;
						}
					});
			});

		//! 初始化交易所(LP)缓冲队列
		tempMapping.iterate(
			[&](const auto& sysSymbol, auto& lpPords)
			{
				Global::SharedGet()->m_SystemProductIDQ.search(
					sysSymbol,
					[&](const auto& sysSymbol, auto& sysPordID)
					{
						Global::SharedGet()->m_LPProductFinalQ.push(sysPordID, lpPords);
						auto begin = lpPords.begin();
						if (begin) {
							//! 初始化系统产品的精度
							Global::SharedGet()->m_SystemProductFinalQ.search(sysPordID,
								[&](const auto&, auto& obj) {obj.Decimals = sk::Helper::DoubleToDecimals(begin->TickSize); });
						}

					});
			});

		//! 初始化Feeder符号表
		Global::SharedGet()->m_MTDataFeedProductsQ.iterate(
			[&](const auto& mtFeederProductID, auto& mtFeederProducts) {
				auto vtFeederSymbols = mtFeederProducts.Vector();
				if (!vtFeederSymbols.empty()) {
					Global::SharedGet()->m_SystemProductFinalQ.iterate(
						[&](const auto& systemProductID, auto& systemProductObj) {
							for (auto it = vtFeederSymbols.begin(); it != vtFeederSymbols.end();) {
								if (sk::stringa(it->Symbol.symbol).find(systemProductObj.Symbol.symbol) != sk::stringa::npos) {
									it->ID = mtFeederProductID | systemProductID | it->ID;
									Global::SharedGet()->m_FeederProductFinalQ.pushpush(
										systemProductID,
										[&](auto& setObj) {
											setObj.push(*it);
										});
									it = vtFeederSymbols.erase(it);
									continue;
								}
								++it;
							}
						});
				}
				else {
					Global::Log()->error("{:X} {}", mtFeederProductID, "Feeder symbols data is empty");
				}
			});

		//! 初始化Gateway符号表
		Global::SharedGet()->m_MTGatewayProductsQ.iterate(
			[&](const auto& mtGatewayProdustID, auto& mtGatewayProducts) {
				auto vtGatewaySymbols = mtGatewayProducts.Vector();
				if (!vtGatewaySymbols.empty()) {
					Global::SharedGet()->m_SystemProductFinalQ.iterate(
						[&](const auto& systemProductID, auto& systemProductObj) {
							for (auto it = vtGatewaySymbols.begin(); it != vtGatewaySymbols.end();) {
								if (sk::stringa(it->Symbol.symbol).find(systemProductObj.Symbol.symbol) != sk::stringa::npos) {
									shared::MTGatewayProduct gatewayProduct;
									it->ID = mtGatewayProdustID | systemProductID | it->ID;
									Global::SharedGet()->m_GatewayProductFinalQ.pushpush(
										systemProductID,
										[&](auto& setObj) {
											setObj.push(*it);
										});
									it = vtGatewaySymbols.erase(it);
									continue;
								}
								++it;
							}
						});
				}
				else {
					Global::Log()->error("{:X} {}", mtGatewayProdustID, "Gateway symbols data is empty");
				}
			});

		//auto view0 = &Global::SharedGet()->m_LPProductFinalQ;
		//auto view1 = &Global::SharedGet()->m_FeederProductFinalQ;
		//auto view2 = &Global::SharedGet()->m_GatewayProductFinalQ;

		Global::Log()->warn("初始化系统产品树完成({}) . . . ", Global::SharedGet()->m_SystemProductFinalQ.size());
		return -1;
	}

}///namespace proxy