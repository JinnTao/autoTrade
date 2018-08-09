#include <cTraderSpi.h>
#include <cTrade.h>
#include <memory>
#include <future>
#include <chrono>
#include <thread>
#include "global.h"
#include "logger.h"
#define ROHON 1
//#undef  ROHON
#define _CTP 1

using namespace std::chrono_literals;

bool IsFlowControl(int iResult) {
    return ((iResult == -2) || (iResult == -3));
}
cTraderSpi::cTraderSpi() {
}
cTraderSpi::~cTraderSpi() {
    delete m_accountInfo;
}

// After making a succeed connection with the CTP server, the client should send the login request to the CTP server.
void cTraderSpi::OnFrontConnected() {
    
    ILOG("cTraderSpi::OnFrontConnected.");
    if (on_connected_fun_) {
        std::invoke(on_connected_fun_);
    }
}

// When the connection between client and the CTP server disconnected, the following function will be called
void cTraderSpi::OnFrontDisconnected(int nReason) {
    // in this case, API will reconnect, the client application can ignore this
    ILOG("cTraderSpi::OnFrontDisconnected,Reason:{}.", nReason);
    if (on_disconnected_fun_) {
        std::invoke(on_disconnected_fun_, nReason);
    }
}

// After receiving the login request from the client, the CTP server will send the following response to notify the
// client whether the login success or not
void cTraderSpi::OnRspUserLogin(CThostFtdcRspUserLoginField* pRspUserLogin,
                                CThostFtdcRspInfoField*      pRspInfo,
                                int                          nRequestID,
                                bool                         bIsLast) {
    // std::lock_guard<std::mutex> guard(mut_);
    if (!IsErrorRspInfo(pRspInfo) && pRspUserLogin) {
        m_FRONT_ID        = pRspUserLogin->FrontID;
        m_SESSION_ID      = pRspUserLogin->SessionID;
        int iNextOrderRef = atoi(pRspUserLogin->MaxOrderRef);
        m_tradeDay        = string(pRspUserLogin->TradingDay);
        m_actionDay       = "";  // cSystem::GetCurrentDayBuffer();

        iNextOrderRef++;
        sprintf(m_ORDER_REF, "%d", iNextOrderRef);

        ILOG("cTraderSpi::OnRspUserLogin,TradeDate:{},SessionID:{},FrontID:{},MaxOrderRef:{}.",
             pRspUserLogin->TradingDay,
             pRspUserLogin->SessionID,
             pRspUserLogin->FrontID,
             pRspUserLogin->MaxOrderRef);
    }
    if (bIsLast) {
        if (on_login_fun_) {
            std::invoke(on_login_fun_, pRspUserLogin, pRspInfo);
        }
    }
}

void cTraderSpi::ReqSettlementInfoConfirm() {
    std::lock_guard<std::mutex>          guard(mut_);
    CThostFtdcSettlementInfoConfirmField req;
    memset(&req, 0, sizeof(req));
    strcpy_s(req.BrokerID, sizeof(TThostFtdcBrokerIDType), ctp_config_.brokerId);
    strcpy_s(req.InvestorID, sizeof(TThostFtdcUserIDType), ctp_config_.userId);
    int iResult = ctpTdApi_->ReqSettlementInfoConfirm(&req, ++request_id_);
    ILOG("ReqSettlementInforConrim,Result:{}.", iResult);
}

void cTraderSpi::OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField* pSettlementInfoConfirm,
                                            CThostFtdcRspInfoField*               pRspInfo,
                                            int                                   nRequestID,
                                            bool                                  bIsLast) {

    if (!IsErrorRspInfo(pRspInfo) && pSettlementInfoConfirm) {
        
        ILOG("OnRspSettlementInfoConfirm: Success!");
    }
    if (bIsLast) {
        ReqQryInstrument_all();
    }
}

void cTraderSpi::ReqQryInstrument_all() {
    std::lock_guard<std::mutex>  guard(mut_);
    CThostFtdcQryInstrumentField req;
    memset(&req, 0, sizeof(req));
    while (true) {
        int iResult = ctpTdApi_->ReqQryInstrument(&req, ++request_id_);
        ILOG("ReqQryInstrument, result:{}",iResult);
        if (IsFlowControl(iResult)) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        } else {
            break;
        }
    }
}

void cTraderSpi::OnRspQryInstrument(CThostFtdcInstrumentField* pInstrument,
                                    CThostFtdcRspInfoField*    pRspInfo,
                                    int                        nRequestID,
                                    bool                       bIsLast) {

    if (!IsErrorRspInfo(pRspInfo) && pInstrument) {
        if (m_first_inquiry_Instrument == true) {
            // save all instrument to Inst message map
            auto instField = make_shared<CThostFtdcInstrumentField>(*pInstrument);
            // maybe save all Insturment msg is better?but save to csv or save to database?
            m_InstMeassageMap->insert(
                pair<string, std::shared_ptr<CThostFtdcInstrumentField>>(instField->InstrumentID, instField));
        }
    }
    if (bIsLast) {
        m_first_inquiry_Instrument = false;
        ILOG("OnRspQryInstrument,bIsLast:{}.", bIsLast);
        ReqQryOrder();

    }
}
void cTraderSpi::ReqQryOrder() {
    std::lock_guard<std::mutex> guard(mut_);
    CThostFtdcQryOrderField     req;
    memset(&req, 0, sizeof(req));
    strcpy_s(req.InvestorID, sizeof(TThostFtdcInvestorIDType), ctp_config_.userId);

    while (true) {
        int iResult = ctpTdApi_->ReqQryOrder(&req, ++request_id_);
        ILOG("ReqQryOrder result:{}.", iResult);
        if (IsFlowControl(iResult)) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        } else {
            break;
        }
    }
}

void cTraderSpi::OnRspQryOrder(CThostFtdcOrderField*   pOrder,
                               CThostFtdcRspInfoField* pRspInfo,
                               int                     nRequestID,
                               bool                    bIsLast) {
    if (!IsErrorRspInfo(pRspInfo) && pOrder) {
        if (m_first_inquiry_order == true) {
            this->m_orderCollection->Add(pOrder);
        }
    } 
    if (bIsLast) {
        m_first_inquiry_order = false;
        ILOG("OnRspQryOrder,bIslast:{}.", bIsLast);
        this->m_orderCollection->PrintAllOrders();  // should save ?
        ReqQryInvestorPositionDetail();
    }
}

// request Query Investor posistion Detail
void cTraderSpi::ReqQryInvestorPositionDetail() {
    std::lock_guard<std::mutex>              guard(mut_);
    CThostFtdcQryInvestorPositionDetailField req;
    memset(&req, 0, sizeof(req));
    strcpy_s(req.InvestorID, sizeof TThostFtdcInvestorIDType, ctp_config_.userId);  // investor Id

    while (true) {
        int iResult = ctpTdApi_->ReqQryInvestorPositionDetail(&req, ++request_id_);
        ILOG("ReqQryInvestorPositionDetail,result:{}.", iResult);
        if (IsFlowControl(iResult)) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        } else {
            break;
        }
    }
}

/// Request Query Investor position Detail,First Query & followed Query
void cTraderSpi::OnRspQryInvestorPositionDetail(CThostFtdcInvestorPositionDetailField* pInvestorPositionDetail,
                                                CThostFtdcRspInfoField*                pRspInfo,
                                                int                                    nRequestID,
                                                bool                                   bIsLast) {
    using namespace std::chrono_literals;
    if (!IsErrorRspInfo(pRspInfo) && pInvestorPositionDetail) {
        // all Instrument
        if (m_firs_inquiry_Detail == true) {
            ILOG(
                "Inst:{},Dire:{},OpenPrice{},Vol:{},OpenDate:{},TradingDay:{},ExchangeId:{},Margin:{}."
                "MarginRateByMoney:{}.",
                pInvestorPositionDetail->InstrumentID,
                (pInvestorPositionDetail->Direction == '0' ? "long" : "short"),
                pInvestorPositionDetail->OpenPrice,
                pInvestorPositionDetail->Volume,
                pInvestorPositionDetail->OpenDate,
                pInvestorPositionDetail->TradingDay,
                pInvestorPositionDetail->ExchangeID,
                pInvestorPositionDetail->Margin,
                pInvestorPositionDetail->MarginRateByMoney);
        }
    } 
    //输出所有合约的持仓明细，要在这边进行下一步的查询ReqQryTradingAccount();
    if (bIsLast) {
        m_firs_inquiry_Detail = false;
        ILOG("OnRspQryInvestorPositionDetail,bIsLast:{}.", bIsLast);
        ReqQryTradingAccount();
    }
}

void cTraderSpi::ReqQryTradingAccount() {
    std::lock_guard<std::mutex>      guard(mut_);
    CThostFtdcQryTradingAccountField req;
    memset(&req, 0, sizeof(req));
    strcpy_s(req.BrokerID, sizeof TThostFtdcBrokerIDType, ctp_config_.brokerId);
    strcpy_s(req.InvestorID, sizeof TThostFtdcUserIDType, ctp_config_.userId);
    while (true) {
        int iResult = ctpTdApi_->ReqQryTradingAccount(&req, ++request_id_);
        ILOG("ReqQryTradingAccount, result:{}.", iResult);
        if (IsFlowControl(iResult)) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        } else {
            break;
        }
    } 

}

void cTraderSpi::OnRspQryTradingAccount(CThostFtdcTradingAccountField* pTradingAccount,
                                        CThostFtdcRspInfoField*        pRspInfo,
                                        int                            nRequestID,
                                        bool                           bIsLast) {
    m_accountInfo = new sTradingAccountInfo;
    memset(m_accountInfo, 0, sizeof(sTradingAccountInfo));
    if (!IsErrorRspInfo(pRspInfo) && pTradingAccount) {
        strcpy_s(m_accountInfo->BrokerID, sizeof sTradingAccountInfo::BrokerID, pTradingAccount->BrokerID);
        strcpy_s(m_accountInfo->AccountID, sizeof sTradingAccountInfo::AccountID, pTradingAccount->AccountID);
        m_accountInfo->PreBalance     = pTradingAccount->PreBalance;
        m_accountInfo->Balance        = pTradingAccount->Balance;
        m_accountInfo->Available      = pTradingAccount->Available;
        m_accountInfo->WithdrawQuota  = pTradingAccount->WithdrawQuota;
        m_accountInfo->CurrMargin     = pTradingAccount->CurrMargin;
        m_accountInfo->CloseProfit    = pTradingAccount->CloseProfit;
        m_accountInfo->PositionProfit = pTradingAccount->PositionProfit;
        m_accountInfo->Commission     = pTradingAccount->Commission;
        m_accountInfo->FrozenMargin   = pTradingAccount->FrozenMargin;
    }
    if (bIsLast){

        printf("Account Summary:\n");
        printf("   AccountID:%s\n", m_accountInfo->AccountID);
        printf("   PreBalance:%.2f\n", m_accountInfo->PreBalance);
        printf("   Balance:%.2f\n", m_accountInfo->Balance);
        printf("   WithdrawQuota:%f\n", m_accountInfo->WithdrawQuota);
        printf("   totalPnl:%.2f\n", m_accountInfo->CloseProfit + m_accountInfo->PositionProfit);
        printf("   CloseProfit:%.2f\n", m_accountInfo->CloseProfit);
        printf("   PositionProfit:%.2f\n", m_accountInfo->PositionProfit);
        printf("   Commission:%.2f\n", m_accountInfo->Commission);
        printf("   Available:%.2f\n", m_accountInfo->Available);
        printf("   CurrMargin:%.2f\n", m_accountInfo->CurrMargin);

        if (m_firs_inquiry_TradingAccount){
            ILOG("OnRspQryTradingAccount,isLast:{}", bIsLast);
            ReqQryInvestorPosition_all();
        }
        m_firs_inquiry_TradingAccount = false;
    }
}
///
void cTraderSpi::ReqQryInvestorPosition_all() {
    std::lock_guard<std::mutex>        guard(mut_);
    CThostFtdcQryInvestorPositionField req;
    memset(&req, 0, sizeof(req));
    while (true) {
        int iResult = ctpTdApi_->ReqQryInvestorPosition(&req, ++request_id_);
        
        ILOG("ReqQryInvestorPostiion,result:{}", iResult);
        if (IsFlowControl(iResult)) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        } else {
            break;
        }
    }
}

//持仓查询回调函数,已经平仓的单子，持仓量为0了还会返回
void cTraderSpi::OnRspQryInvestorPosition(CThostFtdcInvestorPositionField* pInvestorPosition,
                                          CThostFtdcRspInfoField*          pRspInfo,
                                          int                              nRequestID,
                                          bool                             bIsLast) {
    if (!IsErrorRspInfo(pRspInfo) && pInvestorPosition) {
        if (m_firs_inquiry_Position == true) {
            // update position
            this->m_positionCollection->update(pInvestorPosition);
            // subscribe Instrument
            this->subscribeInst(pInvestorPosition->InstrumentID, false);
        }
    } 
    if (bIsLast) {
        m_firs_inquiry_Position = false;
        this->m_positionCollection->PrintDetail();
        ILOG("OnRspQryInvestorPosition isLast:{}.",bIsLast);
        //融航需要指定合约
#ifdef ROHON
        m_itMap = m_InstMeassageMap->begin();

        // First Start up
        m_output.open("output/" + string(ctp_config_.userId) + "_" + m_tradeDay + "_commission.txt",
                      ios::_Nocreate | ios::in);
        if (m_output) {
            while (!m_output.eof()) {
                // get Instrument List
                shared_ptr<CThostFtdcInstrumentCommissionRateField> instField =
                    make_shared<CThostFtdcInstrumentCommissionRateField>();
                m_output >> instField->InstrumentID >> instField->CloseRatioByMoney >> instField->CloseRatioByVolume >>
                    instField->CloseTodayRatioByMoney >> instField->CloseTodayRatioByVolume >>
                    instField->OpenRatioByMoney >> instField->OpenRatioByVolume;
                m_pInstCommissionMap->insert(pair<string, std::shared_ptr<CThostFtdcInstrumentCommissionRateField>>(
                    instField->InstrumentID, instField));
            }

            if (m_pInstCommissionMap->size() > 10) {
                ILOG("Exist commission file,go to qry trade.");
                ReqQryTrade();
                return;
            }
        }
        m_output.open("output/" + string(ctp_config_.userId) + "_" + m_tradeDay + "_commission.txt",
                      ios::app | ios::out);

        ReqQryInstrumentCommissionRate();
#else
        ReqQryInstrumentCommissionRate();
#endif  //  ROHON
    }
}
// 这种循环查询 手续费的方式：因为接口一次只能查询一个，为了方便，查询出来的结果
// 保存到txt中，第二次启动时直接从文件读取，速度就快很多，但是第一次查询需要花费10Min
void cTraderSpi::ReqQryInstrumentCommissionRate(bool qryTrade) {
    std::lock_guard<std::mutex> guard(mut_);
#ifdef ROHON
    if (qryTrade == true) {
        //guard.~lock_guard();
        ReqQryTrade();
        return;
    }
    CThostFtdcQryInstrumentCommissionRateField req;
    memset(&req, 0, sizeof(req));
    strcpy_s(req.InvestorID, sizeof TThostFtdcInvestorIDType, ctp_config_.userId);
    strcpy_s(req.BrokerID, sizeof TThostFtdcBrokerIDType, ctp_config_.brokerId);   

    // 为了过滤组合合约 比如 XX-SR801&SR803 这个没必要查询手续费了。 交割日为当日的合约 查询会error，忽略
    string instName;
    while (m_itMap != this->m_InstMeassageMap->end() &&
           (string(m_itMap->second->ExpireDate) == m_actionDay ||
            (!isValidInsturment(string(m_itMap->second->InstrumentID), instName)))) {
        ILOG("Ignore:{}.", m_itMap->second->InstrumentID);
        m_itMap++;
    }
    // over QryTrade；
    if (m_itMap == this->m_InstMeassageMap->end()) {
        //LOG(INFO) << "Finish qry instrument ";
        ILOG("Finish qry instrument.");
        ReqQryTrade();

        return;
    }
    strcpy_s(req.InstrumentID, sizeof TThostFtdcInstrumentIDType, m_itMap->second->InstrumentID);
    while (true) {
        int iResult = ctpTdApi_->ReqQryInstrumentCommissionRate(&req, ++request_id_);
        ILOG("ReqQryInstrumentCommissionRate Inst:{},Result:{}.", req.InstrumentID, iResult);
        if (IsFlowControl(iResult)) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        } else {
            break;
        }
    }
#else
    CThostFtdcQryInstrumentCommissionRateField req;
    memset(&req, 0, sizeof(req));
    strcpy_s(req.InvestorID, sizeof TThostFtdcInvestorIDType, ctp_config_.userId);  // investor Id
    strcpy_s(req.BrokerID, sizeof TThostFtdcBrokerIDType, ctp_config_.brokerId);    // broker Id
    m_pInstCommissionMap->clear();
    int iResult = m_pUserTraderApi->ReqQryInstrumentCommissionRate(&req, ++request_id_);

    if (m_first_inquiry_commissionRate) {
        m_first_inquiry_commissionRate = false;
        LOG(INFO) << " Qry:" << req.InstrumentID << ((iResult == 0) ? "Success" : "Fail");
    }
#endif  // ROHON
}

void cTraderSpi::OnRspQryInstrumentCommissionRate(CThostFtdcInstrumentCommissionRateField* pInstrumentCommissionRate,
                                                  CThostFtdcRspInfoField*                  pRspInfo,
                                                  int                                      nRequestID,
                                                  bool                                     bIsLast) {

#ifdef ROHON
    if (!IsErrorRspInfo(pRspInfo) && pInstrumentCommissionRate) {
        // save all instrument message map
        shared_ptr<CThostFtdcInstrumentCommissionRateField> instField =
            std::make_shared<CThostFtdcInstrumentCommissionRateField>(*pInstrumentCommissionRate);
        m_pInstCommissionMap->insert(pair<string, std::shared_ptr<CThostFtdcInstrumentCommissionRateField>>(
            pInstrumentCommissionRate->InstrumentID, instField));
        m_output << m_itMap->second->InstrumentID << " " << instField->CloseRatioByMoney << " "
                 << instField->CloseRatioByVolume << " " << instField->CloseTodayRatioByMoney << " "
                 << instField->CloseTodayRatioByVolume << " " << instField->OpenRatioByMoney << " "
                 << instField->OpenRatioByVolume << endl;
    } 
    if (bIsLast) {
        m_itMap++;
        if (m_itMap != this->m_InstMeassageMap->end()) {
            ReqQryInstrumentCommissionRate();
        } else {
            
            ILOG("Finish qry commissionRate.");
            ReqQryTrade();
        }
    }
#else
    if (!IsErrorRspInfo(pRspInfo) && pInstrumentCommissionRate) {
        // save all instrument message map
        shared_ptr<CThostFtdcInstrumentCommissionRateField> instField =
            make_shared<CThostFtdcInstrumentCommissionRateField>(*pInstrumentCommissionRate);
        m_pInstCommissionMap->insert(pair<string, shared_ptr<CThostFtdcInstrumentCommissionRateField>>(
            pInstrumentCommissionRate->InstrumentID, *instField));
        if (bIsLast) {
            ReqQryTrade();
        }
    } else {
        LOG(INFO) << "OnRspQryInstrumentCommissionRate,noexists.";
        ReqQryTrade();
    }
#endif  //  ROHON
}

void cTraderSpi::ReqQryTrade() {
    std::lock_guard<std::mutex> guard(mut_);
    if (m_output.is_open()) {
        m_output.close();
    }
    CThostFtdcQryTradeField req;
    memset(&req, 0, sizeof(req));
    strcpy_s(req.InvestorID, sizeof TThostFtdcInvestorIDType, ctp_config_.userId);  //
    while (true) {
        int  iResult = ctpTdApi_->ReqQryTrade(&req, ++request_id_);
        ILOG("ReqQryTrade, result:{}.", iResult);
        if (IsFlowControl(iResult)) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }else{
            break;
        }
    }
}

void cTraderSpi::OnRspQryTrade(CThostFtdcTradeField*   pTrade,
                               CThostFtdcRspInfoField* pRspInfo,
                               int                     nRequestID,
                               bool                    bIsLast) {

    if (!IsErrorRspInfo(pRspInfo) && pTrade && strlen(pTrade->InvestorID) != 0) {

         auto iter = (this->m_pInstCommissionMap->find(pTrade->InstrumentID) == this->m_pInstCommissionMap->end()?
             NULL : (this->m_pInstCommissionMap->at(pTrade->InstrumentID)));  
         if(iter == NULL){
            string instName;
            this->isValidInsturment(pTrade->InstrumentID,instName);
            iter = (this->m_pInstCommissionMap->find(instName) == this->m_pInstCommissionMap->end()? NULL :
            this->m_pInstCommissionMap->at(instName));
        }
         this->m_tradeCollection->Add(pTrade,&(*iter),&*(this->m_InstMeassageMap->at(pTrade->InstrumentID)));

    } 
    if (bIsLast) {
        mut_.try_lock();
        ILOG("OnRspQryTrade,bIsLast:{}.", bIsLast);
        this->m_tradeCollection->PrintAll();
        if (m_first_inquiry_trade) {
            m_first_inquiry_trade = false;
            ILOG("Td start success!");
            if (on_started_fun_) {
                std::invoke(on_started_fun_);
            }
        }
        mut_.unlock();
    }
}


void cTraderSpi::showPositionDetail() {
    this->m_positionCollection->PrintDetail();
}


void cTraderSpi::ReqOrderInsert(TThostFtdcInstrumentIDType   instId,
                                TThostFtdcDirectionType      dir,
                                TThostFtdcCombOffsetFlagType kpp,
                                TThostFtdcPriceType          price,
                                TThostFtdcVolumeType         vol) {
    CThostFtdcInputOrderField req;
    memset(&req, 0, sizeof(req));
    strcpy_s(req.BrokerID, sizeof TThostFtdcBrokerIDType, ctp_config_.brokerId);
    strcpy_s(req.InvestorID, sizeof TThostFtdcInvestorIDType, ctp_config_.userId);
    strcpy_s(req.InstrumentID, sizeof TThostFtdcInstrumentIDType, instId);
    strcpy_s(req.OrderRef, sizeof TThostFtdcOrderRefType, m_ORDER_REF);

    req.LimitPrice = price;  //价格
    if (0 == req.LimitPrice) {
        req.OrderPriceType = THOST_FTDC_OPT_AnyPrice;  //价格类型=市价
        req.TimeCondition  = THOST_FTDC_TC_IOC;        //有效期类型:立即完成，否则撤销
    } else {
        req.OrderPriceType = THOST_FTDC_OPT_LimitPrice;  //价格类型=限价
        req.TimeCondition  = THOST_FTDC_TC_GFD;          //有效期类型:当日有效
    }

    req.Direction         = this->MapDirection(dir, true);  //买卖方向
    req.CombOffsetFlag[0] = this->MapOffset(kpp[0], true);  //组合开平标志:开仓

    req.CombHedgeFlag[0]    = THOST_FTDC_HF_Speculation;  //组合投机套保标志
    req.VolumeTotalOriginal = vol;                        ///数量
    req.VolumeCondition     = THOST_FTDC_VC_AV;           //成交量类型:任何数量
    req.MinVolume           = 1;                          //最小成交量:1
    req.ContingentCondition = THOST_FTDC_CC_Immediately;  //触发条件:立即

    req.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;  //强平原因:非强平
    req.IsAutoSuspend    = 0;                             //自动挂起标志:否
    req.UserForceClose   = 0;                             //用户强评标志:否

    int iResult = ctpTdApi_->ReqOrderInsert(&req, ++request_id_);
    ILOG("ReqOrderInsert,result:{}", iResult);
    if (iResult == 0) {
        int orderRef = atoi(m_ORDER_REF);
        m_allOrderRef.push_back(orderRef);
        sprintf(m_ORDER_REF, "%d", ++orderRef);
    }
}

void cTraderSpi::ReqOrderInsert(cOrder* pOrder) {
    CThostFtdcInputOrderField req;

    memset(&req, 0, sizeof(req));

    strcpy(req.BrokerID, ctp_config_.brokerId);
    strcpy(req.InvestorID, ctp_config_.userId);
    strcpy(req.UserID, ctp_config_.userId);
    strcpy(req.InstrumentID, pOrder->GetInstrumentID().c_str());
    strcpy(req.OrderRef, m_ORDER_REF);

    req.Direction = pOrder->GetDirection();

    req.CombOffsetFlag[0] = pOrder->GetOffsetFlag();

    req.VolumeTotalOriginal = pOrder->GetVolumeOriginal();

    // order with market price
    req.OrderPriceType      = THOST_FTDC_OPT_LimitPrice;
    req.LimitPrice          = pOrder->GetPrice();
    req.TimeCondition       = THOST_FTDC_TC_GFD;
    req.ContingentCondition = THOST_FTDC_CC_Immediately;
    req.VolumeCondition     = THOST_FTDC_VC_AV;
    req.CombHedgeFlag[0]    = THOST_FTDC_HF_Speculation;
    req.MinVolume           = 1;
    req.ForceCloseReason    = THOST_FTDC_FCC_NotForceClose;
    req.IsAutoSuspend       = 0;
    req.UserForceClose      = 0;

    int iResult = ctpTdApi_->ReqOrderInsert(&req, ++request_id_);
    // cout << "--->>> ReqOrderInsert: " << iResult << ( ( iResult == 0 ) ? ", succeed" : ", fail") << endl;
    if (iResult == 0) {
        int orderRef = atoi(m_ORDER_REF);
        m_allOrderRef.push_back(orderRef);
        sprintf(m_ORDER_REF, "%d", ++orderRef);
        pOrder = NULL;
    }
}

// order insert response
void cTraderSpi::OnRspOrderInsert(CThostFtdcInputOrderField* pInputOrder,
                                  CThostFtdcRspInfoField*    pRspInfo,
                                  int                        nRequestID,
                                  bool                       bIsLast) {
    // output the order insertion result
    // it seems that this function is called only when error occurs during inserting orders
    if (!IsErrorRspInfo(pRspInfo) && pInputOrder) {
        //LOG(INFO) << "cTraderSpi::OnRspOrderInsert,Inst: " << pInputOrder->InstrumentID << ",requestId: " << pInputOrder->RequestID;
        ILOG("cTraderSpi::OnRspOrderInsert,Inst: {},requestId: {}.", pInputOrder->InstrumentID, pInputOrder->RequestID);
    }
    if (bIsLast){
        //LOG(INFO) << "cTraderSpi::OnRspOrderInsert,Inst: " << pInputOrder->InstrumentID << ",requestId: " << pInputOrder->RequestID;
        ILOG("cTraderSpi::OnRspOrderInsert,Inst: {},requestId:{}.", pInputOrder->InstrumentID, pInputOrder->RequestID);
    }
}
void cTraderSpi::OnErrRtnOrderInsert(CThostFtdcInputOrderField* pInputOrder, CThostFtdcRspInfoField* pRspInfo) {
    //LOG(INFO) << "OnErrRtnOrderInsert";
    WLOG("OnErrRtnOrderInsert");
}
///报单操作错误回报
void cTraderSpi::OnErrRtnOrderAction(CThostFtdcOrderActionField* pOrderAction, CThostFtdcRspInfoField* pRspInfo){
    //LOG(INFO) << "OnErrRtnOrderAction";
    WLOG("OnErrRtnOrderAction");
}
// order insertion return
void cTraderSpi::OnRtnOrder(CThostFtdcOrderField* pOrder) {

    if (pOrder && !strcmp(pOrder->InvestorID, this->ctp_config_.userId)) {
        m_orderCollection->Add( pOrder );
        if (!IsMyOrder(pOrder)) {
            ILOG("Other NO:{},Status:{},Msg:{}.", pOrder->OrderSysID, pOrder->OrderStatus, pOrder->StatusMsg);
        } else {
            ILOG("My NO:{},Status:{},Msg:{}.", pOrder->OrderSysID, pOrder->OrderStatus, pOrder->StatusMsg);
        }
    }

}

void cTraderSpi::OnRspOrderAction(CThostFtdcInputOrderActionField* pInputOrderAction,
                                  CThostFtdcRspInfoField*          pRspInfo,
                                  int                              nRequestID,
                                  bool                             bIsLast) {
    /* calling this function in case the order cancellation is failed */
    //LOG(INFO) << "OnRspOrderAction";
    ILOG("OnRspOrderAction.");
    if (!IsErrorRspInfo(pRspInfo)) {
    }
}

// transaction notification
// update m_positions
void cTraderSpi::OnRtnTrade(CThostFtdcTradeField* pTrade) {
    ///* update of m_tradeCollection */
     auto iter = (this->m_pInstCommissionMap->find(pTrade->InstrumentID) == this->m_pInstCommissionMap->end()? NULL :
     this->m_pInstCommissionMap->at(pTrade->InstrumentID));  m_tradeCollection->Add(
     pTrade,&*iter,&*(m_InstMeassageMap->at(pTrade->InstrumentID)));  int tradeID = atoi( pTrade->TradeID );
     m_tradeCollection->PrintTrade( tradeID );
    ///* update of m_positionDetail */
     m_positionCollection->update( pTrade );
    //subscirbe Instrument
     this->subscribeInst(pTrade->InstrumentID,true);
    //
     for each (auto var in m_strategyList)
    {
        var->onTrade(*pTrade);
    }
}

// the error notification caused by client request
void cTraderSpi::OnRspError(CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast) {
    WLOG("OnRspError,error code:{},Msg:{},RequestId:{},isLast:{}.",
         pRspInfo->ErrorID,
         pRspInfo->ErrorMsg,
         nRequestID,
         bIsLast);
}

void cTraderSpi::OnHeartBeatWarning(int nTimeLapse) {

    ILOG("OnHeartBeatWarning,TimeLaps:{}.", nTimeLapse);
}

// ReqOrderAction is used for order cancellation
void cTraderSpi::ReqOrderAction(shared_ptr<cOrder> pOrder) {
    CThostFtdcInputOrderActionField req;
    memset(&req, 0, sizeof(req));

    strcpy(req.BrokerID, ctp_config_.brokerId);
    strcpy(req.InvestorID, ctp_config_.userId);
    // sprintf( req.OrderRef, "%d",pOrder->GetOrderRef() );
    strcpy(req.OrderSysID, pOrder->m_orderSysID);
    strcpy(req.ExchangeID, pOrder->ExchangeID);
    req.FrontID    = m_FRONT_ID;
    req.SessionID  = m_SESSION_ID;
    req.ActionFlag = THOST_FTDC_AF_Delete;

    int iResult = ctpTdApi_->ReqOrderAction(&req, ++request_id_);
    //LOG(INFO) << "ReqOrderAction,result: " << iResult;
    ILOG("ReqOrderAction,result: {}.",iResult);

}

bool cTraderSpi::IsErrorRspInfo(CThostFtdcRspInfoField* pRspInfo) {
    bool bResult = ((pRspInfo) && (pRspInfo->ErrorID != 0));
    if (bResult) {
        WLOG("ErrorID:{},ErrorMsg:{}.", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
    return bResult;
}

bool cTraderSpi::IsMyOrder(CThostFtdcOrderField* pOrder) {
    bool flag = false;

    int orderRef = atoi(pOrder->OrderRef);

    for (size_t i = 0; i < m_allOrderRef.size(); ++i) {
        if (orderRef == m_allOrderRef[i]) {
            flag = true;
            break;
        }
    }

    return (flag && (pOrder->FrontID == m_FRONT_ID) && (pOrder->SessionID == m_SESSION_ID));
}



void cTraderSpi::RegisterPositionCollection(cPositionCollectionPtr p) {
    if (m_positionCollection.get())
        m_positionCollection.reset();

    m_positionCollection = p;
}

void cTraderSpi::RegisterOrderCollection(cOrderCollectionPtr p) {
    if (m_orderCollection.get())
        m_orderCollection.reset();

    m_orderCollection = p;
}

void cTraderSpi::RegisterTradeCollection(cTradeCollectionPtr p) {
    if (m_tradeCollection.get())
        m_tradeCollection.reset();

    m_tradeCollection = p;
}
void cTraderSpi::RegisterSubscribeInstList(shared_ptr<vector<string>> p) {
    if (m_pSubscribeInst.get())
        m_pSubscribeInst.reset();

    m_pSubscribeInst = p;
}
void cTraderSpi::RegisterInstMessageMap(map<string, std::shared_ptr<CThostFtdcInstrumentField>>* p) {
    if (m_InstMeassageMap)
        m_InstMeassageMap = NULL;

    m_InstMeassageMap = p;
}
void cTraderSpi::RegisterInstCommissionMap(map<string, std::shared_ptr<CThostFtdcInstrumentCommissionRateField>>* p) {
    if (m_pInstCommissionMap)
        m_pInstCommissionMap = NULL;

    m_pInstCommissionMap = p;
}
void cTraderSpi::insertOrder(string                inst,
                             traderTag::DIRECTION  dire,
                             traderTag::OFFSETFLAG flag,
                             int                   vol,
                             double                orderPrice,
                             string                tag) {
    TThostFtdcInstrumentIDType   instId;
    TThostFtdcDirectionType      dir;    //方向,'0'买，'1'卖
    TThostFtdcCombOffsetFlagType kpp;    //开平，"0"开，"1"平,"3"平今
    TThostFtdcPriceType          price;  //价格，0是市价,上期所不支持
    strcpy(instId, inst.c_str());
    int priceTick = 2;  //  默认两个price Tick;
    // double miniChangeTick = m_instMessage_map[inst.c_str()]->PriceTick * 3; // 对手盘 最小变动价格 保证成交
    double BuyPrice  = orderPrice;  // +priceTick * this->m_InstMeassageMap->at(inst)->PriceTick;;
    double SellPrice = orderPrice;  //-priceTick * this->m_InstMeassageMap->at(inst)->PriceTick;;// 卖出价 买入价
    // make market price order
    if (orderPrice == 0) {
        cMarketData* p;
        double       lastprice = 0;
        p                      = this->m_pMarketDataEngine->GetMarketDataHandle(inst);
        if (p) {
            lastprice = p->getLastMarketData().LastPrice;
        } else {
            this->ctp_md_spi_->SubscribeMarketData(inst);
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            // return;
        }
        if (orderPrice == 0 && this->m_InstMeassageMap->size() != 0 &&
            m_InstMeassageMap->find(inst) != this->m_InstMeassageMap->end()) {
            switch (dire) {
                case traderTag::DIRECTION::buy:
                    BuyPrice = lastprice + (1 + priceTick) * this->m_InstMeassageMap->at(inst)->PriceTick;
                    break;
                case traderTag::DIRECTION::sell:
                    SellPrice = lastprice - (1 + priceTick) * this->m_InstMeassageMap->at(inst)->PriceTick;
                    break;
            }
        }
    }

    //开仓
    if (flag == traderTag::OFFSETFLAG::open) {
        //买入开仓
        if (dire == traderTag::DIRECTION::buy) {
            dir = '0';
            strcpy_s(kpp, "0");
            price = BuyPrice;
            this->ReqOrderInsert(instId, dir, kpp, price, vol);
        }
        // 卖出开仓
        if (dire == traderTag::DIRECTION::sell) {
            dir = '1';
            strcpy_s(kpp, "0");
            price = SellPrice;
            this->ReqOrderInsert(instId, dir, kpp, price, vol);
        }
    }
    if (flag == traderTag::OFFSETFLAG::close) {

        //买入平仓
        if (dire == traderTag::DIRECTION::buy) {
            dir   = '0';
            price = BuyPrice;
            this->StraitClose(instId, dir, price, vol, tag);
        }
        //卖出平仓
        if (dire == traderTag::DIRECTION::sell) {
            dir   = '1';
            price = SellPrice;
            this->StraitClose(instId, dir, price, vol, tag);
        }
    }
}
void cTraderSpi::StraitClose(TThostFtdcInstrumentIDType instId,
                             TThostFtdcDirectionType    dir,
                             TThostFtdcPriceType        price,
                             TThostFtdcVolumeType       vol,
                             string                     tag) {
    TThostFtdcCombOffsetFlagType kpp;  //开平，"0"开，"1"平昨 平,"3"平今
    string                       inst_str = string(instId);

    std::regex pattern("SP.*");
    bool       valid = regex_search(inst_str, pattern);
    if (valid && inst_str.size() > 9) {
        // LOG(INFO) << "Close Inst:" << instId << "dir" << dir << "vol" << vol << "price" << price << " tag" << tag;
        strcpy(kpp, "1");
        ReqOrderInsert(instId, dir, kpp, price, vol);
        return;
    }

    // close Long
    if (dir == '1') {
        // SPD 指令平仓需要注意

        if (this->m_positionCollection->getPosition(instId, DIRE::AUTO_LONG) < vol) {
            WLOG("Long:close vol more than hold vol.,position:{},vol:{}.",
                 this->m_positionCollection->getPosition(instId, DIRE::AUTO_LONG),
                 vol);
            return;
        }

        if (this->m_positionCollection->getPosition(instId, DIRE::AUTO_LONG) > 0) {
            if (strcmp(m_InstMeassageMap->at(instId)->ExchangeID, "SHFE") == 0) {
                if (strcmp(tag.c_str(), "") != 0) {
                    if (strcmp(tag.c_str(), "Y") == 0) {
                        strcpy(kpp, "1");
                    } else if (strcmp(tag.c_str(), "T") == 0) {
                        strcpy(kpp, "3");
                    } else {
                        WLOG("close error command.");
                        return;
                    }

                    ReqOrderInsert(instId, dir, kpp, price, vol);

                } else {
                    // close y than close t
                    int Yd_long = this->m_positionCollection->getYdPosition(instId, DIRE::AUTO_LONG);
                    int Td_long = this->m_positionCollection->getTdPosition(instId, DIRE::AUTO_LONG);
                    if (Yd_long > vol) {
                        strcpy(kpp, "1");  // close yP
                        ReqOrderInsert(instId, dir, kpp, price, vol);

                    } else {
                        if (Yd_long > 0) {
                            strcpy(kpp, "1");  // close yP
                            ReqOrderInsert(instId, dir, kpp, price, Yd_long);
                        }
                        strcpy(kpp, "3");  // close tP
                        ReqOrderInsert(instId, dir, kpp, price, vol - Yd_long);
                    }
                }

            } else {
                strcpy(kpp, "1");
                ReqOrderInsert(instId, dir, kpp, price, vol);
            }
        }
    }
    // close short
    else {
        if (this->m_positionCollection->getPosition(instId, DIRE::AUTO_SHORT) < vol) {
            WLOG("short:close vol more than hold vol,position:{},vol:{}.",
                 this->m_positionCollection->getPosition(instId, DIRE::AUTO_SHORT),
                 vol);
            return;
        }

        if (this->m_positionCollection->getPosition(instId, DIRE::AUTO_SHORT) > 0) {
            if (strcmp(m_InstMeassageMap->at(instId)->ExchangeID, "SHFE") == 0) {
                if (strcmp(tag.c_str(), "") != 0) {
                    if (strcmp(tag.c_str(), "Y") == 0) {
                        strcpy(kpp, "1");
                    } else if (strcmp(tag.c_str(), "T") == 0) {
                        strcpy(kpp, "3");
                    } else {
                        cerr << "close error command" << endl;
                        return;
                    }

                    ReqOrderInsert(instId, dir, kpp, price, vol);
                } else {
                    // close yP than tP
                    int Yd_short = this->m_positionCollection->getYdPosition(instId, DIRE::AUTO_SHORT);
                    int Td_short = this->m_positionCollection->getTdPosition(instId, DIRE::AUTO_SHORT);

                    if (Yd_short >= vol) {
                        strcpy(kpp, "1");  // yP
                        ReqOrderInsert(instId, dir, kpp, price, vol);

                    } else  // yd is not enough
                    {
                        if (Yd_short > 0) {
                            strcpy(kpp, "1");  // yP
                            ReqOrderInsert(instId, dir, kpp, price, Yd_short);
                        }
                        strcpy(kpp, "3");  // tP
                        ReqOrderInsert(instId, dir, kpp, price, vol - Yd_short);
                    }
                }

            } else {
                strcpy(kpp, "1");
                ReqOrderInsert(instId, dir, kpp, price, vol);
            }
        }
    }
}
char cTraderSpi::MapDirection(char src, bool toOrig = true) {
    if (toOrig) {
        if ('b' == src || 'B' == src) {
            src = '0';
        } else if ('s' == src || 'S' == src) {
            src = '1';
        }
    } else {
        if ('0' == src) {
            src = 'B';
        } else if ('1' == src) {
            src = 'S';
        }
    }
    return src;
}

char cTraderSpi::MapOffset(char src, bool toOrig = true) {
    if (toOrig) {
        if ('o' == src || 'O' == src) {
            src = '0';
        } else if ('c' == src || 'C' == src) {
            src = '1';
        } else if ('j' == src || 'J' == src) {
            src = '3';
        }
    } else {
        if ('0' == src) {
            src = 'O';
        } else if ('1' == src) {
            src = 'C';
        } else if ('3' == src) {
            src = 'J';
        }
    }
    return src;
}

bool cTraderSpi::subscribeInst(TThostFtdcInstrumentIDType instrumentName, bool subscribeTag) {
    bool find_instId_Trade = false;

    for (unsigned int i = 0; i < m_pSubscribeInst->size(); i++) {
        if (strcmp(m_pSubscribeInst->at(i).c_str(), instrumentName) == 0)  /// already subscribe Instrument
        {
            find_instId_Trade = true;
            break;
        }
    }

    if (!find_instId_Trade) {
        if (subscribeTag) {
            ctp_md_spi_->SubscribeMarketData(instrumentName);
        }
        m_pSubscribeInst->push_back(instrumentName);
    }
    return true;
}

bool cTraderSpi::isValidInsturment(string inst, string& instName) {

    std::regex pattern("[a-zA-Z]{1,2}[0-9]{3,4}");

    std::match_results<std::string::const_iterator> result;

    bool valid        = regex_match(inst, result, pattern);
    char instCName[3] = {};
    if (valid) {
        sscanf(inst.c_str(), "%[a-zA-Z]{1,2}", instCName);
    }
    instName = string(instCName);
    return valid;
}

void cTraderSpi::cancleAllPendingOrder() {
    vector<cOrderPtr> vOrder = this->m_orderCollection->GetAllOrder();
    for (auto it = vOrder.begin(); it != vOrder.end(); it++) {
        if (it->get()->IsPendingOrder()) {
            this->ReqOrderAction(*it);
        }
    }
}

void cTraderSpi::cancleMyPendingOrder() {
    vector<cOrderPtr> vOrder = this->m_orderCollection->GetAllOrder();
    for (auto it = vOrder.begin(); it != vOrder.end(); it++) {
        if (it->get()->IsPendingOrder() && IsMyOrder(it->get()->getOrderStruct())) {
            this->ReqOrderAction(*it);
        }
    }
}

int32 cTraderSpi::init(const ctpConfig& ctp_config) {
    using namespace std::chrono_literals;
    // 1.create td api instance
    {
        auto tdapi = CThostFtdcTraderApi::CreateFtdcTraderApi(ctp_config.td_flow_path_);
        if (tdapi == nullptr) {
            WLOG("CreateFtdcTraderApi instance failed");
            return -1;
        }
        // unique_ptr->ctp's document release just call Release api, release 2018/07/11 JinnTao
        ctpTdApi_ = {tdapi, [](CThostFtdcTraderApi* tdapi) {
                         if (tdapi != nullptr) {
                             tdapi->Release();
                         }
                         WLOG("Release tradeApi.");
                     }};
        ctpTdApi_->RegisterSpi(this);
        ILOG("Td create instance success!");
    }

    // 2.connect to td Front
    {
        this->clearCallBack();

        ctpTdApi_->RegisterFront(const_cast<char*>(ctp_config.tdAddress));
        std::promise<bool> connect_result;
        std::future<bool>  is_connected = connect_result.get_future();
        on_connected_fun_               = [&connect_result] { connect_result.set_value(true); };
        ctpTdApi_->Init();
        auto wait_result = is_connected.wait_for(15s);
        if (wait_result != std::future_status::ready || is_connected.get() != true) {
            return -2;
        }
        ILOG("Td connect front success!");
        ctpTdApi_->SubscribePrivateTopic(THOST_TERT_QUICK);  // Private QUICK recieve exchange send all msg after login
        ctpTdApi_->SubscribePublicTopic(THOST_TERT_QUICK);   // Public QUICK recieve exchange send all msg after login
    }

    // 3.login to Td.
    {
        this->clearCallBack();
        std::promise<bool> login_result;
        std::future<bool>  is_logined = login_result.get_future();
        on_login_fun_ = [&login_result](CThostFtdcRspUserLoginField* login, CThostFtdcRspInfoField* info) {
            if (info->ErrorID == 0) {
                login_result.set_value(true);
            } else {
                login_result.set_value(false);
            }
        };
        CThostFtdcReqUserLoginField req;

        memset(&req, 0, sizeof(req));
        strcpy_s(req.BrokerID, sizeof(TThostFtdcBrokerIDType), ctp_config.brokerId);
        strcpy_s(req.UserID, sizeof(TThostFtdcInvestorIDType), ctp_config.userId);
        strcpy_s(req.Password, sizeof TThostFtdcPasswordType, ctp_config.passwd);
        ctp_config_ = ctp_config;  // just no use deep copy
        // Try login
        auto req_login_result = ctpTdApi_->ReqUserLogin(&req, ++request_id_);
        if (req_login_result != 0) {
            WLOG("Td request login failed!");
            return -3;
        }
        auto wait_result = is_logined.wait_for(5s);
        if (wait_result != std::future_status::ready || is_logined.get() != true) {
            WLOG("Td request login TimeOut!");
            return -3;
        }
        ILOG("Td login success");
    }

    // 4.set callback
    {
        this->clearCallBack();
        global::need_reconnect.store(false,
                                     std::memory_order_release);  // current write/read cannot set this store back;
        on_disconnected_fun_ = [](int32 reason) {
            WLOG("Td disconnect,try reconnect! reason:{}", reason);
            global::need_reconnect.store(true, std::memory_order_release);
        };
    }
    return 0;
}
int32 cTraderSpi::stop() {
    if (ctpTdApi_) {
        ctpTdApi_.reset(nullptr);
    }
    return 0;
}
int32 cTraderSpi::reConnect(const ctpConfig& ctp_config) {
    return 0;
}
int32 cTraderSpi::start() {
    std::promise<bool> start_result;
    std::future<bool>  is_started = start_result.get_future();
    on_started_fun_               = [&start_result]() { start_result.set_value(true); };
    this->ReqSettlementInfoConfirm();
    auto wait_result = is_started.wait_for(20min);
    if (wait_result == std::future_status::timeout) {
        WLOG("Td start timeout.");
        return -1;
    } else if (wait_result != std::future_status::ready || is_started.get() != true) {
        WLOG("Td start failed,but not timeout with 20min");
        return -2;
    }

    return 0;
}
void cTraderSpi::clearCallBack() {
    std::lock_guard<std::mutex> guard(mut_);
    on_connected_fun_    = {};
    on_disconnected_fun_ = {};
    on_login_fun_        = {};
}

void cTraderSpi::clear() {

}