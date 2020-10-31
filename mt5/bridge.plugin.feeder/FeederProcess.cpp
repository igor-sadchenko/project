#include "stdafx.h"
#include "Feeder.h"

namespace feeder {

	MTAPIRES Feeder::MTSymbolSync()
	{
		MTAPIRES result = MT_RET_OK;
		SharedGet()->m_MTSymbolQ.clear();
		SharedGet()->m_OnConSymbolFieldQ.clear();

		UINT nTotalConSymbol = ServerApi()->SymbolTotal();
		for (UINT i = 0; i < nTotalConSymbol; ++i)
		{
			auto pSymbol = ServerApi()->SymbolCreate();
			if (MT_RET_OK == ServerApi()->SymbolNext(i, pSymbol))
			{
				shared::ConSymbolField Symbol;
				shared::Business::MakeConSymbolField(pSymbol, Symbol);
				SharedGet()->m_OnConSymbolFieldQ.push(pSymbol->Symbol(), Symbol);
				SharedGet()->m_MTSymbolQ.push(pSymbol->Symbol(), Symbol);
			}
			pSymbol->Release();
		}
		Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->info("Symbol total({})", SharedGet()->m_OnConSymbolFieldQ.size());
		return result;
	}

	MTAPIRES Feeder::MTServerTimeSync()
	{
		MTAPIRES result = MT_RET_OK;
		m_ServerTime.store(ServerApi()->TimeCurrent());
		m_ServerTimeDiff.store(m_ServerTime.load() - ::time(0));
		auto conTime = ServerApi()->TimeCreate();
		result = ServerApi()->TimeGet(conTime);
		if (MT_RET_OK == result)
		{
			Log(sk::EnLogType::EN_LOG_TYPE_SYSTEM)->info(
				"系统时间: 时区[{}],时差[{}],时间[{}]",
				shared::Business::MTGMT(conTime->TimeZone()),
				m_ServerTimeDiff.load(),
				sk::time::Time::TimestampToTimeStr(m_ServerTime.load()));
		}
		conTime->Release();
		License(ServerApi()->TimeCurrent());
		return result;
	}

	MTAPIRES Feeder::ParamentersGet()
	{
		MTAPIRES result = MT_RET_OK;
		IMTConPlugin* pCurrPlugin = ServerApi()->PluginCreate();
		if (!pCurrPlugin)
		{
			return MT_RET_ERR_MEM;
		}
		result = ServerApi()->PluginCurrent(pCurrPlugin);
		if (MT_RET_OK != result)
		{
			pCurrPlugin->Release();
			return result;
		}
		auto PathObj = shared::Business::MTParamenterGet<IMTServerAPI, IMTConPlugin>(m_pServerApi, pCurrPlugin, PLUGIN_PARAM_KEY_PATH_OBJ);
		m_PathObj = sk::StringConvert::WStringToMBytes(PathObj);

		pCurrPlugin->Release();
		if (!m_PathObj.empty())
		{
			return MT_RET_OK;
		}
		return MT_RET_ERR_PARAMS;
	}

	void Feeder::ProcessT()
	{

	}

}///namespace gateway