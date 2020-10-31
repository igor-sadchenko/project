#include "stdafx.h"
#include "Manager.h"

namespace manager {

	MTAPIRES Manager::PositionCloseAt(const UINT64& login)
	{
		MTAPIRES result = MT_RET_ERROR;

		if (Global::CoreGet()->StatusManager() < sk::network::EnNetStatus::EN_NETSTATUS_SYNCHRONIZED)
			return result;

		IMTPositionArray* positions = Api()->PositionCreateArray();
		if (!positions)
			return result;

		result = Api()->PositionGet(login, positions);
		if (result != MT_RET_OK)
			return result;

		sk::container::map<UINT64, shared::PositionField> mapPositions;
		for (UINT i = 0; i < positions->Total(); ++i)
		{
			auto position = positions->Next(i);
			if (position)
			{
				shared::RequestField reqField;
				reqField.Login = position->Login();
				reqField.Position = position->Position();
				reqField.Volume = position->Volume();
				reqField.PriceOrder = 0;
				reqField.TimeExpiration = 0;
				sk::SafeCopyW(reqField.Symbol, position->Symbol(), _countof(reqField.Symbol));
				reqField.Action = IMTRequest::EnTradeActions::TA_DEALER_POS_EXECUTE;
				if (position->Action() == (UINT)IMTPosition::EnPositionAction::POSITION_BUY)
				{
					reqField.Type = UINT(IMTOrder::EnOrderType::OP_SELL);
				}
				else
				{
					reqField.Type = UINT(IMTOrder::EnOrderType::OP_BUY);
				}
				Global::SharedGet()->m_RequestQ.push(reqField);
			}
		}
		positions->Release();
		Global::Log(sk::EnLogType::EN_LOG_TYPE_WARN)->warn("{}", "平仓命令发送完成.");
		return result;
	}
	MTAPIRES Manager::SymbolsGet(sk::container::map<sk::stringw, shared::ConSymbolField>& symbolFields)
	{
		MTAPIRES result = MT_RET_ERROR;

		if (Global::CoreGet()->StatusManager() < sk::network::EnNetStatus::EN_NETSTATUS_SYNCHRONIZED)
			return result;

		auto total = Api()->SymbolTotal();
		for (UINT i = 0; i < total; ++i)
		{
			auto symbol = Api()->SymbolCreate();
			result = Api()->SymbolNext(i, symbol);
			if (result != MT_RET_OK)
			{
				symbol->Release();
				break;
			}
			shared::ConSymbolField symbolField;
			shared::Business::MakeConSymbolField(symbol, symbolField);
			symbolFields.push(symbolField.Symbol, symbolField);
			symbol->Release();
		}
		return result;
	}
	MTAPIRES Manager::GroupsGet(sk::container::map<sk::stringw, shared::ConGroupField>& groupFields)
	{
		MTAPIRES result = MT_RET_ERROR;

		if (Global::CoreGet()->StatusManager() < sk::network::EnNetStatus::EN_NETSTATUS_SYNCHRONIZED)
			return result;

		auto groups = Api()->GroupTotal();
		if (groups > 0)
		{
			for (decltype(groups) i = 0; i < groups; ++i)
			{
				auto group = Api()->GroupCreate();
				result = Api()->GroupNext(i, group);
				if (result != MT_RET_OK)
				{
					group->Release();
					break;
				}
				shared::ConGroupField groupField;
				shared::Business::MakeConGroupField(group, groupField);
				groupFields.push(groupField.Group, groupField);
				group->Release();
			}
		}
		return result;
	}

	MTAPIRES Manager::UsersGet(sk::container::map<UINT64, shared::UserField>& userFields)
	{
		MTAPIRES result = MT_RET_ERROR;

		if (Global::CoreGet()->StatusManager() < sk::network::EnNetStatus::EN_NETSTATUS_SYNCHRONIZED)
			return result;

		sk::container::map<sk::stringw, shared::ConGroupField> groups;
		GroupsGet(groups);
		if (groups.empty())
		{
			return result;
		}
		groups.iterate(
			[&](const sk::stringw& groupName, shared::ConGroupField& groupField, const bool&, bool& itberak)
			{
				auto group = Api()->GroupCreate();
				result = Api()->GroupGet(groupName.c_str(), group);
				if (MT_RET_OK == result)
				{
					UINT64* users = nullptr;
					UINT total = 0;
					result = Api()->UserLogins(group->Group(), users, total);
					if (result == MT_RET_OK)
					{
						for (UINT i = 0; i < total; ++i)
						{
							auto user = Api()->UserCreate();
							result = Api()->UserGet(users[i], user);
							if (MT_RET_OK != result)
							{
								user->Release();
								break;
							}
							shared::UserField userField;
							shared::Business::MakeUserField(user, userField);
							userFields.push(userField.Login, userField);
							user->Release();
						}
					}
				}
				group->Release();
			});
		return result;
	}

	MTAPIRES Manager::PositionsGet(sk::container::map<UINT64, shared::PositionField>& PositionFields)
	{
		MTAPIRES result = MT_RET_ERROR;

		if (Global::CoreGet()->StatusManager() < sk::network::EnNetStatus::EN_NETSTATUS_SYNCHRONIZED)
			return result;

		sk::container::map<UINT64, shared::UserField> users;
		UsersGet(users);
		users.iterate(
			[&](const UINT64& login, shared::UserField& userField, const bool&, bool& itbreak)
			{
				auto positions = Api()->PositionCreateArray();
				result = Api()->PositionGet(login, positions);
				if (result == MT_RET_OK)
				{
					for (UINT i = 0; i < positions->Total(); ++i)
					{
						auto position = positions->Next(i);
						shared::PositionField positionField;
						shared::Business::MakePositionField(position, positionField);
						PositionFields.push(positionField.Position, positionField);
					}
				}
				positions->Release();
			});
		return result;
	}

	MTAPIRES Manager::PositionNetVolumeGet(sk::container::map<shared::SystemID, sk::container::map<sk::stringw, INT64>>& ExchangePositionNetVolumes)
	{
		MTAPIRES result = MT_RET_ERROR;

		if (Global::CoreGet()->StatusManager() < sk::network::EnNetStatus::EN_NETSTATUS_SYNCHRONIZED)
			return result;

		sk::container::map<UINT64, shared::PositionField> positionFields;
		result = PositionsGet(positionFields);
		if (result == MT_RET_OK)
		{
			positionFields.iterate(
				[&](const auto& positionID, shared::PositionField& positionField)
				{
					shared::SystemID exchangeID = 0;
					time_t orderTimestamp;
					exchangeID = shared::SystemIDType::SystemOrderIDMadeUn(shared::SystemIDType::WStrToSystemOrderID(positionField.ExternalID), orderTimestamp);

					ExchangePositionNetVolumes.pushpush(
						exchangeID,
						[&](auto& obj)
						{
							obj.pushpush(
								positionField.Symbol,
								[&](INT64& VolumeOld)
								{
									if (positionField.Action == UINT(IMTPosition::EnPositionAction::POSITION_BUY))
									{
										VolumeOld += positionField.Volume;
									}
									else
									{
										VolumeOld -= positionField.Volume;
									}
								});
						});
				});
		}
#if 0
		positionNetVolumes.iterate_clear(
			[&](const auto&, auto& volume, bool&, bool& itclear)
			{
				if (volume == 0)
				{
					itclear = true;
				}
			});
#endif
		return result;
	}


	MTAPIRES Manager::CreateUsers()
	{
		MTAPIRES result = MT_RET_OK;

		if (Global::CoreGet()->StatusManager() < sk::network::EnNetStatus::EN_NETSTATUS_SYNCHRONIZED)
			return result;

		IMTUser* user = Api()->UserCreate();
		result = user->Clear();
		result = user->Name(L"API");
		result = user->Rights(IMTUser::USER_RIGHT_DEFAULT);
		result = user->Group(L"SKTEST");
		result = user->Leverage(100);
		result = Api()->UserAdd(user, L"www.skstu.com", L"www.skstu.com");
		result = user->Name(L"skstu");
		result = Api()->UserUpdate(user);
		result = Api()->UserPasswordCheck(IMTUser::USER_PASS_MAIN, user->Login(), L"www.skstu.com");
		result = Api()->UserPasswordCheck(IMTUser::USER_PASS_INVESTOR, user->Login(), L"www.skstu.com");
		result = Api()->UserCertConfirm(user->Login());
		UINT64 login = 0;
		result = Api()->DealerBalance(user->Login(), 10000000.00, IMTDeal::DEAL_BALANCE, L"Deposit", login);


		return result;
	}



}///namespace manager