#include "stdafx.h"
#include "RouteObj.h"

namespace follow {

	RouteObj::RouteObj(const shared::Serial& serial, const shared::serverplugin::OnTradeSignalField& signalObj) :
		m_Serial(serial),
		m_InitialSignal(signalObj),
		m_ProcessTaskType(EnProcessTaskType(signalObj.SignalType))
	{
		m_FollowPathOutQ.push(signalObj.SignalDeal.PositionID);
	}
	RouteObj::~RouteObj()
	{

	}

	//!@ {"时间","任务类型","帐户","数量","价格","持仓","品种","多空","开平","SL","TP","信号源","信号持仓","信号SL","信号TP","运行时间","返回代码","原因"}
	std::vector<sk::packet> RouteObj::Print() const
	{
		std::vector<sk::packet> result;
		CHAR szLogFormat[2048] = { 0 };
		int nLogFormat = 0;

		switch (ProcessTaskType())
		{
		case EnProcessTaskType::EN_PROCESS_TASK_TYPE_FOLLOW_POSITION_MODIFY:
		{
			m_ProcessFinishQ.iterate(
				[&](const UINT64& reqid, const TaskNode& route_node, auto& itbreak)
				{
					memset(szLogFormat, 0x00, _countof(szLogFormat));
					nLogFormat = sprintf_s(
						szLogFormat,
						"%s\t%s\t%I64d\t%I64d\t%.5f\t%I64d\t%s\t%s\t%s\t%.5f\t%.5f\t%I64d\t%I64d\t%.5f\t%.5f\t%I64d\t%d\t%s",
						sk::time::Time::TimestampToTimeStr(route_node.TimeBegin, true).c_str(),
						ProcessTaskTypeStr(),
						route_node.m_Response.SignalPosition.Login,
						route_node.m_Response.SignalPosition.Volume,
						route_node.m_Response.SignalPosition.PriceOpen,
						route_node.m_Response.SignalPosition.Position,
						sk::StringConvert::WStringToMBytes(route_node.m_Response.SignalPosition.Symbol).c_str(),
						shared::Business::DealActionStrA(route_node.m_Response.SignalPosition.Action),
						"-",
						route_node.m_Response.SignalPosition.PriceSL,
						route_node.m_Response.SignalPosition.PriceTP,
						route_node.m_Request.SignalPosition.Login,
						route_node.m_Request.SignalPosition.Position,
						route_node.m_Request.SignalPosition.PriceSL,
						route_node.m_Request.SignalPosition.PriceTP,
						route_node.TimeEnd - route_node.TimeBegin,
						route_node.m_Response.SignalRequest.ResultRetcode,
						sk::StringConvert::WStringToMBytes(SMTFormat::FormatError(route_node.m_Response.SignalRequest.ResultRetcode)).c_str()
					);
					if (nLogFormat > 0)
					{
						result.emplace_back(sk::packet(szLogFormat, nLogFormat));
					}
				});
		}break;
		case EnProcessTaskType::EN_PROCESS_TASK_TYPE_FOLLOW:
		{
			nLogFormat = sprintf_s(
				szLogFormat,
				"%s\t%s\t%I64d\t%I64d\t%.5f\t%I64d\t%s\t%s\t%s\t%.5f\t%.5f\t%s\t%s\t%s\t%s\t%I64d\t%d\t%s",
				sk::time::Time::TimestampToTimeStr(TimeBegin(), true).c_str(),
				ProcessTaskTypeStr(),
				InitialSignal().SignalRequest.Login,
				InitialSignal().SignalDeal.Volume,
				InitialSignal().SignalDeal.Price,
				InitialSignal().SignalDeal.PositionID,
				sk::StringConvert::WStringToMBytes(InitialSignal().SignalDeal.Symbol).c_str(),
				shared::Business::DealActionStrA(InitialSignal().SignalDeal.Action),
				shared::Business::DealEntryStrA(InitialSignal().SignalDeal.Entry),
				InitialSignal().SignalDeal.PriceSL,
				InitialSignal().SignalDeal.PriceTP,
				"-",
				"-",
				"-",
				"-",
				TimeEnd() - TimeBegin(),
				InitialSignal().SignalRequest.ResultRetcode,
				"-"
			);

			if (nLogFormat > 0)
			{
				result.emplace_back(sk::packet(szLogFormat, nLogFormat));
			}

			m_ProcessFinishQ.iterate(
				[&](const UINT64& reqid, const TaskNode& route_node, auto& itbreak)
				{
					memset(szLogFormat, 0x00, _countof(szLogFormat));

					nLogFormat = sprintf_s(
						szLogFormat,
						"%s\t%s\t%I64d\t%I64d\t%.5f\t%I64d\t%s\t%s\t%s\t%.5f\t%.5f\t%I64d\t%I64d\t%.5f\t%.5f\t%I64d\t%d\t%s",
						sk::time::Time::TimestampToTimeStr(route_node.TimeBegin, true).c_str(),
						ProcessTaskTypeStr(),
						route_node.m_Response.SignalRequest.Login,
						route_node.m_Response.SignalDeal.Volume,
						route_node.m_Response.SignalDeal.Price,
						route_node.m_Response.SignalDeal.PositionID,
						sk::StringConvert::WStringToMBytes(route_node.m_Response.SignalDeal.Symbol).c_str(),
						shared::Business::DealActionStrA(route_node.m_Response.SignalDeal.Action),
						shared::Business::DealEntryStrA(route_node.m_Response.SignalDeal.Entry),
						route_node.m_Response.SignalDeal.PriceSL,
						route_node.m_Response.SignalDeal.PriceTP,
						route_node.m_Request.SignalDeal.Login,
						route_node.m_Request.SignalDeal.PositionID,
						route_node.m_Response.SignalDeal.PriceSL,
						route_node.m_Response.SignalDeal.PriceTP,
						route_node.TimeEnd - route_node.TimeBegin,
						route_node.m_Response.SignalRequest.ResultRetcode,
						sk::StringConvert::WStringToMBytes(SMTFormat::FormatError(route_node.m_Response.SignalRequest.ResultRetcode)).c_str()
					);
					if (nLogFormat > 0)
					{
						result.emplace_back(sk::packet(szLogFormat, nLogFormat));
					}
				});
		}break;
		case EnProcessTaskType::EN_PROCESS_TASK_TYPE_FOLLOW_PATCH_CLOSE:
		{
			m_ProcessFinishQ.iterate(
				[&](const UINT64& reqid, const TaskNode& route_node, auto& itbreak)
				{
					memset(szLogFormat, 0x00, _countof(szLogFormat));
					nLogFormat = sprintf_s(
						szLogFormat,
						"%s\t%s\t%I64d\t%I64d\t%.5f\t%I64d\t%s\t%s\t%s\t%.5f\t%.5f\t%I64d\t%I64d\t%s\t%s\t%I64d\t%d\t%s",
						sk::time::Time::TimestampToTimeStr(route_node.TimeBegin, true).c_str(),
						ProcessTaskTypeStr(),
						route_node.m_Response.SignalRequest.Login,
						route_node.m_Response.SignalDeal.Volume,
						route_node.m_Response.SignalDeal.Price,
						route_node.m_Response.SignalDeal.PositionID,
						sk::StringConvert::WStringToMBytes(route_node.m_Response.SignalDeal.Symbol).c_str(),
						shared::Business::DealActionStrA(route_node.m_Response.SignalDeal.Action),
						shared::Business::DealEntryStrA(route_node.m_Response.SignalDeal.Entry),
						route_node.m_Response.SignalDeal.PriceSL,
						route_node.m_Response.SignalDeal.PriceTP,
						route_node.m_Request.SignalPosition.Login,
						route_node.m_Request.SignalPosition.Position,
						"-",
						"-",
						route_node.TimeEnd - route_node.TimeBegin,
						route_node.m_Response.SignalRequest.ResultRetcode,
						sk::StringConvert::WStringToMBytes(SMTFormat::FormatError(route_node.m_Response.SignalRequest.ResultRetcode)).c_str()
					);
					if (nLogFormat > 0)
					{
						result.emplace_back(sk::packet(szLogFormat, nLogFormat));
					}
				});
		}break;
		case EnProcessTaskType::EN_PROCESS_TASK_TYPE_FOLLOW_PATHC_OPEN:
		{
			m_ProcessFinishQ.iterate(
				[&](const UINT64& reqid, const TaskNode& route_node, auto& itbreak)
				{
					memset(szLogFormat, 0x00, _countof(szLogFormat));
					nLogFormat = sprintf_s(
						szLogFormat,
						"%s\t%s\t%I64d\t%I64d\t%.5f\t%I64d\t%s\t%s\t%s\t%.5f\t%.5f\t%I64d\t%I64d\t%.5f\t%.5f\t%I64d\t%d\t%s",
						sk::time::Time::TimestampToTimeStr(route_node.TimeBegin, true).c_str(),
						ProcessTaskTypeStr(),
						route_node.m_Response.SignalRequest.Login,
						route_node.m_Response.SignalDeal.Volume,
						route_node.m_Response.SignalDeal.Price,
						route_node.m_Response.SignalDeal.PositionID,
						sk::StringConvert::WStringToMBytes(route_node.m_Response.SignalDeal.Symbol).c_str(),
						shared::Business::DealActionStrA(route_node.m_Response.SignalDeal.Action),
						shared::Business::DealEntryStrA(route_node.m_Response.SignalDeal.Entry),
						route_node.m_Response.SignalDeal.PriceSL,
						route_node.m_Response.SignalDeal.PriceTP,
						route_node.m_Request.SignalDeal.Login,
						route_node.m_Request.SignalDeal.PositionID,
						route_node.m_Response.SignalDeal.PriceSL,
						route_node.m_Response.SignalDeal.PriceTP,
						route_node.TimeEnd - route_node.TimeBegin,
						route_node.m_Response.SignalRequest.ResultRetcode,
						sk::StringConvert::WStringToMBytes(SMTFormat::FormatError(route_node.m_Response.SignalRequest.ResultRetcode)).c_str()
					);
					if (nLogFormat > 0)
					{
						result.emplace_back(sk::packet(szLogFormat, nLogFormat));
					}
				});
		}break;
		default:
		{

		}break;
		}///switch

		return result;
	}


	bool RouteObj::PerformTask(__in const shared::serverplugin::OnTradeSignalField& signal, __in const std::vector<shared::serverplugin::OnTradeSignalField>& reqFields)
	{
		switch (ProcessTaskType())
		{
		case EnProcessTaskType::EN_PROCESS_TASK_TYPE_FOLLOW:
		{
			m_ProcessFinishQ.search(
				signal.SignalRequest.ID,
				[&](const auto&, auto& taskNode)
				{
					taskNode.TimeEnd = sk::Helper::TickCountGet<std::chrono::milliseconds>();
					taskNode.m_Response = signal;

					switch (taskNode.m_Response.SignalDeal.Entry)
					{
					case IMTDeal::EnDealEntry::ENTRY_IN:
					{
						//!@ 记住当前持仓到前一笔缓冲 对冲时使用
						FollowGet()->PositionApiDataAt(
							signal.SignalPosition.Position,
							[&](IMTPosition* positionObj)->MTAPIRES
							{
								Follow::ApiDataSet(positionObj, shared::EnMTApiParamType::EN_MTAPI_PARAMTYPE_DEAL, taskNode.m_Request.SignalDeal.Deal);
								Follow::ApiDataSet(positionObj, shared::EnMTApiParamType::EN_MTAPI_PARAMTYPE_LOGIN, taskNode.m_Request.SignalDeal.Login);
								Follow::ApiDataSet(positionObj, shared::EnMTApiParamType::EN_MTAPI_PARAMTYPE_ORDER, taskNode.m_Request.SignalDeal.Order);
								Follow::ApiDataSet(positionObj, shared::EnMTApiParamType::EN_MTAPI_PARAMTYPE_POSITION, taskNode.m_Request.SignalDeal.PositionID);
								Follow::ApiDataSet(positionObj, shared::EnMTApiParamType::EN_MTAPI_PARAMTYPE_POSITION_PREV, signal.SignalPosition.Position);
								return MT_RET_OK;
							});
						//!@ 跟单关系落实到磁盘
						FollowGet()->DealApiDataAt(
							signal.SignalDeal.Deal,
							[&](IMTDeal* dealObj)->MTAPIRES
							{
								Follow::ApiDataSet(dealObj, shared::EnMTApiParamType::EN_MTAPI_PARAMTYPE_DEAL, taskNode.m_Request.SignalDeal.Deal);
								Follow::ApiDataSet(dealObj, shared::EnMTApiParamType::EN_MTAPI_PARAMTYPE_LOGIN, taskNode.m_Request.SignalDeal.Login);
								Follow::ApiDataSet(dealObj, shared::EnMTApiParamType::EN_MTAPI_PARAMTYPE_ORDER, taskNode.m_Request.SignalDeal.Order);
								Follow::ApiDataSet(dealObj, shared::EnMTApiParamType::EN_MTAPI_PARAMTYPE_POSITION, taskNode.m_Request.SignalDeal.PositionID);
								//!@ 跟单关系加入到缓存队列
								Follow::SharedGet()->m_PositionRelationQ.pushpush(
									taskNode.m_Request.SignalDeal.PositionID,
									[&](auto& deals)
									{
										deals.push(taskNode.m_Response.SignalDeal.PositionID, taskNode.m_Response.SignalDeal.Deal);
									});
								return MT_RET_OK;
							});
					}break;
					case IMTDeal::EnDealEntry::ENTRY_OUT:
					{
						m_FollowPathOutQ.push(taskNode.m_Response.SignalDeal.PositionID);
					}break;
					}///switch

					TaskCounterDecrement();
				});
		}break;
		case EnProcessTaskType::EN_PROCESS_TASK_TYPE_FOLLOW_POSITION_MODIFY:
		{
			m_ProcessFinishQ.search(
				signal.SignalPosition.Position,
				[&](const auto&, auto& taskNode)
				{
					taskNode.TimeEnd = sk::Helper::TickCountGet<std::chrono::milliseconds>();
					taskNode.m_Response = signal;
					TaskCounterDecrement();
				});
		}break;
		case EnProcessTaskType::EN_PROCESS_TASK_TYPE_FOLLOW_PATHC_OPEN:
		{
			m_ProcessFinishQ.search(
				signal.SignalRequest.ID,
				[&](const auto&, auto& taskNode)
				{
					taskNode.TimeEnd = sk::Helper::TickCountGet<std::chrono::milliseconds>();
					taskNode.m_Response = signal;
					//!@ 记住当前持仓到前一笔缓冲 对冲时使用
					FollowGet()->PositionApiDataAt(
						signal.SignalPosition.Position,
						[&](IMTPosition* positionObj)->MTAPIRES
						{
							Follow::ApiDataSet(positionObj, shared::EnMTApiParamType::EN_MTAPI_PARAMTYPE_DEAL, taskNode.m_Request.SignalDeal.Deal);
							Follow::ApiDataSet(positionObj, shared::EnMTApiParamType::EN_MTAPI_PARAMTYPE_LOGIN, taskNode.m_Request.SignalDeal.Login);
							Follow::ApiDataSet(positionObj, shared::EnMTApiParamType::EN_MTAPI_PARAMTYPE_ORDER, taskNode.m_Request.SignalDeal.Order);
							Follow::ApiDataSet(positionObj, shared::EnMTApiParamType::EN_MTAPI_PARAMTYPE_POSITION, taskNode.m_Request.SignalDeal.PositionID);
							Follow::ApiDataSet(positionObj, shared::EnMTApiParamType::EN_MTAPI_PARAMTYPE_POSITION_PREV, signal.SignalPosition.Position);
							return MT_RET_OK;
						});
					//!@ 跟单关系落实到磁盘
					FollowGet()->DealApiDataAt(
						signal.SignalDeal.Deal,
						[&](IMTDeal* dealObj)->MTAPIRES
						{
							Follow::ApiDataSet(dealObj, shared::EnMTApiParamType::EN_MTAPI_PARAMTYPE_DEAL, taskNode.m_Request.SignalDeal.Deal);
							Follow::ApiDataSet(dealObj, shared::EnMTApiParamType::EN_MTAPI_PARAMTYPE_LOGIN, taskNode.m_Request.SignalDeal.Login);
							Follow::ApiDataSet(dealObj, shared::EnMTApiParamType::EN_MTAPI_PARAMTYPE_ORDER, taskNode.m_Request.SignalDeal.Order);
							Follow::ApiDataSet(dealObj, shared::EnMTApiParamType::EN_MTAPI_PARAMTYPE_POSITION, taskNode.m_Request.SignalDeal.PositionID);
							//!@ 跟单关系加入到缓存队列
							Follow::SharedGet()->m_PositionRelationQ.pushpush(
								taskNode.m_Request.SignalDeal.PositionID,
								[&](auto& deals)
								{
									deals.push(taskNode.m_Response.SignalDeal.PositionID, taskNode.m_Response.SignalDeal.Deal);
								});
							return MT_RET_OK;
						});
					TaskCounterDecrement();
				});
		}break;
		case EnProcessTaskType::EN_PROCESS_TASK_TYPE_FOLLOW_PATCH_CLOSE:
		{
			m_ProcessFinishQ.search(
				signal.SignalRequest.ID,
				[&](const auto&, auto& taskNode)
				{
					taskNode.TimeEnd = sk::Helper::TickCountGet<std::chrono::milliseconds>();
					taskNode.m_Response = signal;
					TaskCounterDecrement();
				});
		}break;
		default:
		{

		}break;
		}///switch





		for (const auto& reqField : reqFields)
		{
			TaskNode taskNode;
			taskNode.TimeBegin = sk::Helper::TickCountGet<std::chrono::milliseconds>();
			taskNode.m_Request = signal;


			switch (ProcessTaskType())
			{
			case EnProcessTaskType::EN_PROCESS_TASK_TYPE_FOLLOW:
			{
				m_ProcessFinishQ.push(reqField.InitRequest.ID, taskNode);
			}break;
			case EnProcessTaskType::EN_PROCESS_TASK_TYPE_FOLLOW_PATCH_CLOSE:
			{
				taskNode.m_Request = reqField;
				taskNode.m_Request.SignalRequest.ID = reqField.InitRequest.ID;
				m_ProcessFinishQ.push(reqField.InitRequest.ID, taskNode);
			}break;
			case EnProcessTaskType::EN_PROCESS_TASK_TYPE_FOLLOW_PATHC_OPEN:
			{
				taskNode.m_Request = reqField;
				taskNode.m_Request.SignalRequest.ID = reqField.InitRequest.ID;
				m_ProcessFinishQ.push(reqField.InitRequest.ID, taskNode);
			}break;
			case EnProcessTaskType::EN_PROCESS_TASK_TYPE_FOLLOW_POSITION_MODIFY:
			{
				taskNode.m_Request = reqField;
				m_ProcessFinishQ.push(reqField.InitRequest.Position, taskNode);
			}break;
			}///switch

			TaskCounterIncrement();
		}
		if (Finish())
		{
			m_TaskTimeEnd = sk::Helper::TickCountGet<std::chrono::milliseconds>();
		}
		return Finish();
	}

}///namespace follow