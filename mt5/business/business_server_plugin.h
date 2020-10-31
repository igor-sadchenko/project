#if !defined(AFX_SKSTU_BUSINESS_SERVER_PLUGIN_H__F7E4D_837A2AA3_CF956D9AB8323AA_D290__HEAD__)
#define AFX_SKSTU_BUSINESS_SERVER_PLUGIN_H__F7E4D_837A2AA3_CF956D9AB8323AA_D290__HEAD__

#include "mt5api/Include/MT5APIServer.h"
#include "mt5api/Include/MT5APITools.h"
#include "interface.h"
#include "business.hpp"

namespace shared {
	namespace serverplugin {

#pragma pack(push,1)

		typedef unsigned long long MsgID;

		enum struct EnMsgTypeSub : MsgID {
			EN_MSG_TYPE_SUB_ADD = 0x1,
			EN_MSG_TYPE_SUB_UDPATE = 0x2,
			EN_MSG_TYPE_SUB_PERFORM = 0x3,
			EN_MSG_TYPE_SUB_DELETE = 0x4,
			EN_MSG_TYPE_SUB_REFUSE = 0x5,
			EN_MSG_TYPE_SUB_PROCESS = 0x6,
		};
		enum struct EnMsgTypeMain : MsgID {
			EN_MSG_TYPE_MAIN_ONDEAL = 0x10000000,
			EN_MSG_TYPE_MAIN_ONORDER = 0x20000000,
			EN_MSG_TYPE_MAIN_ONPOSITION = 0x30000000,
			EN_MSG_TYPE_MAIN_ONTRADEREQUEST = 0x40000000,
			EN_MSG_TYPE_MAIN_ONUSER = 0x50000000,
			EN_MSG_TYPE_MAIN_ONCONGROUP = 0x60000000,
			EN_MSG_TYPE_MAIN_ONCONSYMBOL = 0x70000000,
		};

		typedef struct tagMsgType {
			MsgID ID;

			static MsgID TypeMainMsg(const MsgID& id) { return id & 0xFFFFFFFFF0000000; }
			static MsgID TypeSubMsg(const MsgID& id) { return id & 0x000000000000FFFF; }
			static MsgID TypeMainMsg(const tagMsgType& id) { return id.ID & 0xFFFFFFFFF0000000; }
			static MsgID TypeSubMsg(const tagMsgType& id) { return id.ID & 0x000000000000FFFF; }
			tagMsgType() { memset(this, 0x00, sizeof(*this)); }
			tagMsgType(const MsgID& id) { ID = id; }
			void operator=(const MsgID& obj) { ID = obj; }
			void operator=(const tagMsgType& obj) { ID = obj.ID; }
			bool operator==(const tagMsgType& obj) const { return ID == obj.ID; }
			bool operator<(const tagMsgType& obj) const { return ID < obj.ID; }
			bool operator==(const MsgID& obj) const { return ID == obj; }
			bool operator<(const MsgID& obj) const { return ID < obj; }
		}MsgType;

		__inline static MsgType operator|(const MsgType& mainMsg, const MsgType& subMsg) {
			return mainMsg.ID | subMsg.ID;
		}

		typedef struct tagOnMTBase {
			MsgType MsgType;
		}OnMTBase;

		typedef struct tagOnTradeRequestField final : public OnMTBase {
			RequestField Request;
			ConfirmField Confirm;
			ConGroupField Group;
			ConSymbolField Symbol;
			PositionField Position;
			OrderField Order;
			DealField Deal;

			tagOnTradeRequestField(const shared::serverplugin::EnMsgTypeSub& subID) {
				memset(this, 0x00, sizeof(this));
				MsgType = MsgID(EnMsgTypeMain::EN_MSG_TYPE_MAIN_ONTRADEREQUEST) | MsgID(subID);
			}
		}OnTradeRequestField;

		typedef struct tagOnDealField final : public OnMTBase {
			DealField Deal;
			AccountField Account;
			PositionField Position;
			tagOnDealField(const shared::serverplugin::EnMsgTypeSub& subID) {
				memset(this, 0x00, sizeof(this));
				MsgType = MsgID(EnMsgTypeMain::EN_MSG_TYPE_MAIN_ONDEAL) | MsgID(subID);
			}
		}OnDealField;

		typedef struct tagOnOrderField final : public OnMTBase {
			OrderField Order;
			tagOnOrderField(const shared::serverplugin::EnMsgTypeSub& subID) {
				memset(this, 0x00, sizeof(this));
				MsgType = MsgID(EnMsgTypeMain::EN_MSG_TYPE_MAIN_ONORDER) | MsgID(subID);
			}
		}OnOrderField;

		typedef struct tagOnPositionField final : public OnMTBase {
			PositionField Position;
			tagOnPositionField(const shared::serverplugin::EnMsgTypeSub& subID) {
				memset(this, 0x00, sizeof(this));
				MsgType = MsgID(EnMsgTypeMain::EN_MSG_TYPE_MAIN_ONPOSITION) | MsgID(subID);
			}
		}OnPositionField;

		typedef struct tagOnUserField final : public OnMTBase {
			UserField User;
			tagOnUserField(const shared::serverplugin::EnMsgTypeSub& subID) {
				memset(this, 0x00, sizeof(this));
				MsgType = MsgID(EnMsgTypeMain::EN_MSG_TYPE_MAIN_ONUSER) | MsgID(subID);
			}
		}OnUserField;

		typedef struct tagOnGroupField final : public OnMTBase {
			ConGroupField Group;
			tagOnGroupField(const shared::serverplugin::EnMsgTypeSub& subID) {
				memset(this, 0x00, sizeof(this));
				MsgType = MsgID(EnMsgTypeMain::EN_MSG_TYPE_MAIN_ONCONGROUP) | MsgID(subID);
			}
		}OnGroupField;

		typedef struct tagOnSymbolField final : public OnMTBase {
			ConSymbolField Symbol;
			tagOnSymbolField(const shared::serverplugin::EnMsgTypeSub& subID) {
				memset(this, 0x00, sizeof(this));
				MsgType = MsgID(EnMsgTypeMain::EN_MSG_TYPE_MAIN_ONCONSYMBOL) | MsgID(subID);
			}
		}OnSymbolField;



		typedef struct tagOnTradeSignalKey final
		{
			UINT64 MTReqID;
			UINT64 MTOrderType;

			tagOnTradeSignalKey() { memset(this, 0x00, sizeof(*this)); }
			tagOnTradeSignalKey(const UINT64& id, const UINT64& type) { MTReqID = id; MTOrderType = type; }
			void operator=(const tagOnTradeSignalKey& obj) { memcpy(this, &obj, sizeof(*this)); }
			bool operator<(const tagOnTradeSignalKey& obj) const
			{
				if (MTReqID != obj.MTReqID)
					return MTReqID < obj.MTReqID;
				return MTOrderType < obj.MTOrderType;
			}
		}OnTradeSignalKey;

		//!@ MT Server API 交易跟单信号
		typedef struct tagOnTradeSignalField final
		{
			enum struct EnPositionUpdateFlag : UINT64
			{
				EN_POSITION_UPDATE_DEFAULT = 0,

				EN_POSITION_UPDATE_PRICE_SL = 1,
				EN_POSITION_UPDATE_PRICE_TP = EN_POSITION_UPDATE_PRICE_SL<<1,

				EN_POSITION_UPDATE_PRICE_FIRST = EN_POSITION_UPDATE_DEFAULT,
				EN_POSITION_UPDATE_PRICE_LAST = EN_POSITION_UPDATE_PRICE_TP,
				EN_POSITION_UPDATE_PRICE_ALL = EN_POSITION_UPDATE_PRICE_SL | EN_POSITION_UPDATE_PRICE_TP,
			};
		
			UINT SignalType;//! RouteObj::EnProcessTaskType
			DealField SignalDeal;
			PositionField SignalPosition;
			RequestField SignalRequest;
			RequestField InitRequest;
			//! 如果是补仓/强跟 
			FollowField Followcon;//! 补仓/强跟 Followcon/patchLogin
			UINT64 PositionUpdateFlag;

			tagOnTradeSignalField() { memset(this, 0x00, sizeof(*this)); }
			void operator=(const tagOnTradeSignalField& obj) { memcpy(this, &obj, sizeof(obj)); }

			SignalField toSignalField(time_t toMTServerTimeDiff = 0) const
			{
				SignalField out;
				out.Action = SignalDeal.Action;
				out.PositionAction = SignalPosition.Action;
				out.Deal = SignalDeal.Deal;
				out.Digits = SignalDeal.Digits;
				out.Entry = SignalDeal.Entry;
				out.FollowPositionTotal = 0;
				out.Login = SignalDeal.Login;
				out.Time = SignalDeal.Time;
				out.Order = SignalDeal.Order;
				out.Position = SignalDeal.PositionID;
				out.Price = SignalDeal.Price;
				out.PriceSL = SignalDeal.PriceSL;
				out.PriceTP = SignalDeal.PriceTP;
				out.TimeLocal = SignalDeal.Time - toMTServerTimeDiff;
				sk::SafeCopyW(out.Symbol, SignalDeal.Symbol, _countof(out.Symbol));
				out.Volume = SignalDeal.Volume;
				out.VolumeClosed = SignalDeal.VolumeClosed;
				out.VolumeTotal = SignalPosition.Volume;
				if (SignalRequest.PositionExternalID[0])
				{
					sk::SafeCopyW(out.ExternalID, SignalRequest.PositionExternalID, _countof(out.ExternalID));
				}
				return out;
			}
		}OnTradeSignalField;

#pragma pack(pop)
	}///serverplugin
}///namespace business





/// /*新生联创（上海）**/
/// /*2019年10月27日 10:29:00|192**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///AFX_SKSTU_BUSINESS_SERVER_PLUGIN_H__F7E4D_837A2AA3_CF956D9AB8323AA_D290__TAIL__
