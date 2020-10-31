#if !defined(AFX_SKSTU_LP_H__92E91_13E60AF4_C66C8790E43C05C_46E9__HEAD__)
#define AFX_SKSTU_LP_H__92E91_13E60AF4_C66C8790E43C05C_46E9__HEAD__

namespace shared {
	//!@ 新生联创（上海）标准LP定义
	class ILiquidityProvider :
		public libfix::IExchangeSpi,
		public sk::network::INetworkSpi {
	protected:
		std::mutex m_mutex;
		sk::container::map<shared::SystemID, shared::LPProduct> m_ProductQ;
	protected:
		virtual void FixHeartbeat(const libfix::_0__*) const {}
		virtual void FixReject(const libfix::_3__*) = 0;
		virtual void ProcessT() = 0;
		virtual void OnNetLoginResult(const sk::network::Header&, const sk::packet&) = 0;
		virtual void OnNetShutdown(const sk::network::Header&, const sk::packet&) = 0;
		virtual void OnNetShowWindow(const sk::network::Header&, const sk::packet&) = 0;
		virtual void OnNetSystemSetup(const sk::network::Header&, const sk::packet&) = 0;
		virtual void OnNetSystemTime(const sk::network::Header&, const sk::packet&) = 0;
		virtual void OnNetProductsRequest(const sk::network::Header&, const sk::packet&) = 0;
		virtual void OnNetSystemProducts(const sk::network::Header&, const sk::packet&) = 0;
	protected:

		virtual int FixSequenceReset(const libfix::_4__& req, UINT& SeqNum) const
		{
			if (!FixApi() || FixStatus() < libfix::EnFixStatus::EN_FIXSTATUS_CONNECTED)
			{
				return -1;
			}
			return FixApi()->FixRequest(libfix::EnFixMessageType::EN_FIX_MESSAGE_TYPE_4, TypeToPtr(&req), SeqNum);
		}

		virtual int FixTestRequest() const
		{
			if (!FixApi() || FixStatus() < libfix::EnFixStatus::EN_FIXSTATUS_CONNECTED)
			{
				return -1;
			}
			libfix::_1__ req;
			req._112 = FixApi()->MakeReqID();
			UINT SeqNum = 0;
			return FixApi()->FixRequest(libfix::EnFixMessageType::EN_FIX_MESSAGE_TYPE_1, TypeToPtr(&req), SeqNum);
		}

		virtual int FixResendRequest(const libfix::_2__& req, UINT SeqNum) const
		{
			if (!FixApi() || FixStatus() < libfix::EnFixStatus::EN_FIXSTATUS_CONNECTED)
			{
				return -1;
			}
			return FixApi()->FixRequest(libfix::EnFixMessageType::EN_FIX_MESSAGE_TYPE_2, TypeToPtr(&req), SeqNum);
		}

		virtual int NetSystemTimeRequest() const
		{
			if (!NetApi<sk::network::INetworkApi>() || \
				NetApi<sk::network::INetworkApi>()->NetStatus() < sk::network::EnNetStatus::EN_NETSTATUS_CONNECTED)
				return -1;

			sk::packet enpak;
			sk::network::Header head;
			head.Cmd = (sk::network::EnNetCmd::EN_NETCMD_300000010);
			NetApi<sk::network::INetworkApi>()->Packet("", head, enpak);
			NetApi<sk::network::INetworkApi>()->Write(enpak);
			return 0;
		}

		virtual int NetSystemSetupRequest() const
		{
			if (!NetApi<sk::network::INetworkApi>() || \
				NetApi<sk::network::INetworkApi>()->NetStatus() < sk::network::EnNetStatus::EN_NETSTATUS_CONNECTED)
				return -1;
			sk::packet enpak;
			sk::network::Header head;
			head.Cmd = (sk::network::EnNetCmd::EN_NETCMD_300000020);
			NetApi<sk::network::INetworkApi>()->Packet("", head, enpak);
			NetApi<sk::network::INetworkApi>()->Write(enpak);
			return 0;
		}

		virtual int NetLoginRequest(const sk::packet& loginFieldBuf) const
		{
			if (!NetApi<sk::network::INetworkApi>() || \
				NetApi<sk::network::INetworkApi>()->NetStatus() < sk::network::EnNetStatus::EN_NETSTATUS_CONNECTED)
				return -1;
			sk::packet enpak;
			sk::network::Header head;
			head.Cmd = (sk::network::EnNetCmd::EN_NETCMD_100000070);
			NetApi<sk::network::INetworkApi>()->Packet(loginFieldBuf, head, enpak);
			NetApi<sk::network::INetworkApi>()->Write(enpak);
			return 0;
		}

		virtual int NetProductsSend(const sk::packet& productsBuf) const
		{
			if (!NetApi<sk::network::INetworkApi>() || \
				NetApi<sk::network::INetworkApi>()->NetStatus() < sk::network::EnNetStatus::EN_NETSTATUS_CONNECTED)
				return -1;
			sk::packet enpak;
			sk::network::Header head;
			head.Cmd = (sk::network::EnNetCmd::EN_NETCMD_300000110);
			productsBuf.empty() ? \
				head.Retcode = (decltype(head.Retcode))SKAPIRES::EXCHANGE_RET_ERROR_DATA_EMPTY : \
				head.Retcode = (decltype(head.Retcode))SKAPIRES::EXCHANGE_RET_SUCCESS;
			NetApi<sk::network::INetworkApi>()->Packet(productsBuf, head, enpak);
			NetApi<sk::network::INetworkApi>()->Write(enpak);
			return 0;
		}

		int NetSystemProductsRequest() const
		{
			if (!NetApi<sk::network::INetworkApi>() || \
				NetApi<sk::network::INetworkApi>()->NetStatus() < sk::network::EnNetStatus::EN_NETSTATUS_CONNECTED)
				return -1;
			sk::packet enpak;
			sk::network::Header head;
			head.Cmd = (sk::network::EnNetCmd::EN_NETCMD_300000120);
			NetApi<sk::network::INetworkApi>()->Packet("", head, enpak);
			NetApi<sk::network::INetworkApi>()->Write(enpak);
			return 0;
		}
	};


	class ILiquidityProviderMarketData :
		virtual public ILiquidityProvider
	{
	protected:
		virtual int FixMarketDataRequest() = 0;
		virtual void FixMarketDataRequestReject(const libfix::_Y__*) = 0;
		virtual void FixMarketDataSnapshotFullRefresh(const libfix::_W__*) = 0;
	protected:
		virtual void OnNetMDCacheReqeust(const sk::network::Header&, const sk::packet&) = 0;
		virtual void OnNetMarketDataSubscribe(const sk::network::Header&, const sk::packet& pak)
		{
			m_MDSubscribeType = (shared::marketdata::MDRequest*)pak.data();
		}
		virtual int NetMarketDataSend(const sk::packet& quotes) const
		{
			if (!NetApi<sk::network::INetworkApi>() || \
				NetApi<sk::network::INetworkApi>()->NetStatus() < sk::network::EnNetStatus::EN_NETSTATUS_CONNECTED)
				return -1;
			if (m_MDSubscribeType.MDSubscribeFlag == shared::marketdata::MDRequest::EnMDSubscribeFlag::EN_MD_SUBSCRIBE_FLAG_SUBSCRIBE)
			{
				sk::packet enpak;
				sk::network::Header head;
				head.Cmd = (sk::network::EnNetCmd::EN_NETCMD_300000310);
				NetApi<sk::network::INetworkApi>()->Packet(quotes, head, enpak);
				NetApi<sk::network::INetworkApi>()->Write(enpak);
				return 0;
			}
			return -1;
		}

		virtual int NetMarketDataCacheSend(const sk::packet& quotes)
		{
			if (!NetApi<sk::network::INetworkApi>() || \
				NetApi<sk::network::INetworkApi>()->NetStatus() < sk::network::EnNetStatus::EN_NETSTATUS_CONNECTED)
				return -1;
			sk::packet enpak;
			sk::network::Header head;
			head.Cmd = (sk::network::EnNetCmd::EN_NETCMD_300000320);
			NetApi<sk::network::INetworkApi>()->Packet(quotes, head, enpak);
			NetApi<sk::network::INetworkApi>()->Write(enpak);
			return 0;
		}
	private:
		shared::marketdata::MDRequest m_MDSubscribeType;
	};


	class ILiquidityProviderTradeOrder :
		virtual public ILiquidityProvider
	{
	protected:
		std::atomic_bool m_TradeRecordRequestFlag = false;
		sk::container::queue<shared::tradeorder::TradeRecordRequestResult> m_TradeRecordCacheQ;
	protected:
		virtual SKAPIRES FixOrderStatusRequest(const shared::SystemOrder&) = 0;
		virtual SKAPIRES FixNewOrderSingle(shared::SystemOrder&, UINT&) = 0;
		virtual void FixExecutionReport(const libfix::_8__*) = 0;
		virtual void OnNetTradeOrderSignal(const sk::network::Header&, const sk::packet&) = 0;
		virtual void OnNetTradeOrderTimeoutSignal(const sk::network::Header&, const sk::packet&) = 0;
		virtual void OnNetPositionRequest(const sk::network::Header&, const sk::packet&) = 0;
		virtual void OnNetOrderStatusRequest(const sk::network::Header&, const sk::packet&) = 0;
	protected:
		virtual int NetTradeOrderRequestRes(const shared::SystemOrder& res)
		{
			if (!NetApi<sk::network::INetworkApi>() || \
				NetApi<sk::network::INetworkApi>()->NetStatus() < sk::network::EnNetStatus::EN_NETSTATUS_CONNECTED)
				return -1;
			sk::packet enpak;
			sk::network::Header head;
			head.Cmd = (sk::network::EnNetCmd::EN_NETCMD_300000410);
			NetApi<sk::network::INetworkApi>()->Packet(sk::packet((char*)&res, shared::LENSYSTEMORDER), head, enpak);
			NetApi<sk::network::INetworkApi>()->Write(enpak);
			return 0;
		}

		virtual int NetTradeTimeoutOrderRequestRes(const shared::SystemOrder& res)
		{
			if (!NetApi<sk::network::INetworkApi>() || \
				NetApi<sk::network::INetworkApi>()->NetStatus() < sk::network::EnNetStatus::EN_NETSTATUS_CONNECTED)
				return -1;
			sk::packet enpak;
			sk::network::Header head;
			head.Cmd = (sk::network::EnNetCmd::EN_NETCMD_300000415);
			NetApi<sk::network::INetworkApi>()->Packet(sk::packet((char*)&res, shared::LENSYSTEMORDER), head, enpak);
			NetApi<sk::network::INetworkApi>()->Write(enpak);
			return 0;
		}

		virtual int NetPositionSend(const sk::network::HEADER::EnDataProcessFlag& flag,const sk::packet& positions)
		{
			if (!NetApi<sk::network::INetworkApi>() || \
				NetApi<sk::network::INetworkApi>()->NetStatus() < sk::network::EnNetStatus::EN_NETSTATUS_CONNECTED)
				return -1;
			sk::packet enpak;
			sk::network::Header head;
			head.DataProcessFlag = flag;
			head.Cmd = (sk::network::EnNetCmd::EN_NETCMD_300000420);
			NetApi<sk::network::INetworkApi>()->Packet(positions, head, enpak);
			NetApi<sk::network::INetworkApi>()->Write(enpak);
			return 0;
		}

		virtual void MadeExchangePosition(__in SystemID& exchange_product_id, __in const libfix::_8__& in, __out tradeorder::ExchangePosition& out)
		{
			out.ID = exchange_product_id;
			if (!in._48.empty()) { sk::SafeCopyA(out.SecurityID, in._48.c_str(), _countof(out.SecurityID)); }
			if (!in._22.empty()) { sk::SafeCopyA(out.SecurityIDSource, in._22.c_str(), _countof(out.SecurityIDSource)); }
			if (!in._527.empty()) { sk::SafeCopyA(out.SecondaryExecID, in._527.c_str(), _countof(out.SecondaryExecID)); }
			if (!in._790.empty()) { sk::SafeCopyA(out.OrdStatusReqID, in._790.c_str(), _countof(out.OrdStatusReqID)); }
			if (!in._60.empty()) { sk::SafeCopyA(out.TransactTime, in._48.c_str(), _countof(out.TransactTime)); }
			if (!in._17.empty()) { sk::SafeCopyA(out.ExecID, in._17.c_str(), _countof(out.ExecID)); }
			if (!in._37.empty()) { sk::SafeCopyA(out.OrderID, in._37.c_str(), _countof(out.OrderID)); }
			if (!in._11.empty()) { sk::SafeCopyA(out.CIOrdID, in._11.c_str(), _countof(out.CIOrdID)); }
			if (!in._584.empty()) { sk::SafeCopyA(out.MassStatusReqId, in._584.c_str(), _countof(out.MassStatusReqId)); }
			if (!in._66.empty()) { sk::SafeCopyA(out.ListId, in._66.c_str(), _countof(out.ListId)); }
			if (!in._1.empty()) { sk::SafeCopyA(out.Account, in._1.c_str(), _countof(out.Account)); }
			if (!in._55.empty()) { sk::SafeCopyA(out.Symbol, in._55.c_str(), _countof(out.Symbol)); }
			if (!in._64.empty()) { sk::SafeCopyA(out.SettlDate, in._64.c_str(), _countof(out.SettlDate)); }
			if (!in._15.empty()) { sk::SafeCopyA(out.Currency, in._15.c_str(), _countof(out.Currency)); }
			if (!in._432.empty()) { sk::SafeCopyA(out.ExpireDate, in._432.c_str(), _countof(out.ExpireDate)); }
			if (!in._479.empty()) { sk::SafeCopyA(out.CommCurrency, in._479.c_str(), _countof(out.CommCurrency)); }
			if (!in._150.empty()) { out.ExecType = in._150[0]; }
			if (!in._39.empty()) { out.OrdStatus = in._39[0]; }
			if (!in._54.empty()) { out.Side = in._54[0]; }
			if (!in._13.empty()) { out.CommType = in._13[0]; }
			if (!in._21.empty()) { out.HandInst = in._21[0]; }
			if (!in._40.empty()) { out.OrdType = in._40[0]; }
			if (!in._59.empty()) { out.TimeInForce = in._59[0]; }
			if (!in._103.empty()) { out.OrdRejReason = strtol(in._103.c_str(), nullptr, 10); }
			if (!in._5003.empty()) { out.Track = strtol(in._5003.c_str(), nullptr, 10); }
			if (!in._38.empty()) { out.OrderQty = strtod(in._38.c_str(), nullptr); }
			if (!in._44.empty()) { out.Price = strtod(in._44.c_str(), nullptr); }
			if (!in._99.empty()) { out.StopPx = strtod(in._99.c_str(), nullptr); }
			if (!in._32.empty()) { out.LastQty = strtod(in._32.c_str(), nullptr); }
			if (!in._31.empty()) { out.LastPx = strtod(in._31.c_str(), nullptr); }
			if (!in._151.empty()) { out.LeavesQty = strtod(in._151.c_str(), nullptr); }
			if (!in._14.empty()) { out.CumQty = strtod(in._14.c_str(), nullptr); }
			if (!in._6.empty()) { out.AvgPx = strtod(in._6.c_str(), nullptr); }
			if (!in._12.empty()) { out.Commission = strtod(in._12.c_str(), nullptr); }
			if (!in._5001.empty()) { out.MarkUp = strtod(in._5001.c_str(), nullptr); }
		}
	};
}///namespace shared




/// /*新生联创（上海）**/
/// /*2019年12月14日 9:24:04|628**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///AFX_SKSTU_LP_H__92E91_13E60AF4_C66C8790E43C05C_46E9__TAIL__