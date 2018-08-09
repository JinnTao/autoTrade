#ifndef __CTRADERSPI_H__
#define __CTRADERSPI_H__

#include <map>
#include <regex>  // 正则

#include "cPositionCollection.h"
#include "cTradeCollection.h"
#include "cOrderCollection.h"
#include "cMarketDataCollection.h"
#include "cMdSpi.h"
#include "cStrategy.h"

class cStrategy;

using namespace std;

class cTraderSpi : public CThostFtdcTraderSpi
{
public:
    cTraderSpi();

    ~cTraderSpi();

    // After making a succeed connection with the CTP server, the client should send the login request to the CTP server.
    virtual void OnFrontConnected();

    // When the connection between client and the CTP server disconnected, the following function will be called
    virtual void OnFrontDisconnected(int nReason);

    // After receiving the login request from the client, the CTP server will send the following response to notify the client whether the login success or not
    virtual void OnRspUserLogin( CThostFtdcRspUserLoginField* pRspUserLogin, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast );

    // investor settlement information confirmation response
    virtual void OnRspSettlementInfoConfirm( CThostFtdcSettlementInfoConfirmField* pSettlementInfoConfirm, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast );

    // query instrument response
    virtual void OnRspQryInstrument( CThostFtdcInstrumentField* pInstrument, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast );

    // query trading account response
    virtual void OnRspQryTradingAccount( CThostFtdcTradingAccountField* pTradingAccount, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast );

    // After receiving the investor position request, the CTP server will send the following response to notify the client whether the request success or not
    virtual void OnRspQryInvestorPosition( CThostFtdcInvestorPositionField* pInvestorPosition, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast );
    
    // After receiving the investor position detail request, the CTP server will send the following response to notify the client whether the request success or not
    virtual void OnRspQryInvestorPositionDetail( CThostFtdcInvestorPositionDetailField* pInvestorPositionDetail, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast );

    // order insertion response
    virtual void OnRspOrderInsert( CThostFtdcInputOrderField* pInputOrder, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast );

	///报单录入错误回报
    virtual void OnErrRtnOrderInsert(CThostFtdcInputOrderField* pInputOrder, CThostFtdcRspInfoField* pRspInfo);

    ///报单操作错误回报
    virtual void OnErrRtnOrderAction(CThostFtdcOrderActionField* pOrderAction, CThostFtdcRspInfoField* pRspInfo);

    // order action response
    virtual void OnRspOrderAction( CThostFtdcInputOrderActionField* pInputOrderAction, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ); 

    // order query response
    virtual void OnRspQryOrder( CThostFtdcOrderField* pOrder, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast );
    
    // the error notification caused by client request
    virtual void OnRspError( CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast );

    virtual void OnHeartBeatWarning( int nTimeLapse );

    ///请求查询成交响应
    virtual void OnRspQryTrade(CThostFtdcTradeField *pTrade, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

    // order insertion return
    virtual void OnRtnOrder( CThostFtdcOrderField* pOrder );

    virtual void OnRtnTrade( CThostFtdcTradeField* pTrade);

    //请求查询合约手续费率响应
    virtual void OnRspQryInstrumentCommissionRate(CThostFtdcInstrumentCommissionRateField *pInstrumentCommissionRate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) ;
   
    void ReqSettlementInfoConfirm();

    bool IsErrorRspInfo(CThostFtdcRspInfoField* pRspInfo);

    bool IsMyOrder(CThostFtdcOrderField* pOrder);

    void RegisterPositionCollection( cPositionCollectionPtr p );

    void RegisterOrderCollection( cOrderCollectionPtr p );

    void RegisterTradeCollection( cTradeCollectionPtr p );

    void RegisterSubscribeInstList(shared_ptr<vector<string>> p);

    void RegisterInstMessageMap( map<string, std::shared_ptr<CThostFtdcInstrumentField>>* p );

    void RegisterInstCommissionMap(map<string, std::shared_ptr<CThostFtdcInstrumentCommissionRateField>> *p);

    void ReqQryInstrument_all();

    void ReqQryTradingAccount();

    void ReqQryInvestorPosition_all();

    //void ReqQryInvestorPosition();
    void ReqQryInvestorPositionDetail();

    void ReqQryOrder();

    void ReqQryTrade();

    void ReqOrderInsert( cOrder* pOrder );

    void ReqOrderAction( shared_ptr<cOrder> pOrder );

    void ReqQryInstrumentCommissionRate(bool qryTrade = false);


    const sTradingAccountInfo* GetTradingAccountInfo() const { return m_accountInfo; }


    void showPositionDetail();

    void ReqOrderInsert(TThostFtdcInstrumentIDType instId,TThostFtdcDirectionType dir, TThostFtdcCombOffsetFlagType kpp,TThostFtdcPriceType price,   TThostFtdcVolumeType vol);

    void insertOrder(string             inst,
                     traderTag::DIRECTION dire,
                     traderTag::OFFSETFLAG  flag,
                     int                vol,
                     double             orderPrice,
                     string             tag = "");

    void StraitClose(TThostFtdcInstrumentIDType instId,TThostFtdcDirectionType dir,TThostFtdcPriceType price,TThostFtdcVolumeType vol,string tag = "");

    char MapDirection(char src, bool toOrig);

    char MapOffset(char src, bool toOrig);

    bool subscribeInst(TThostFtdcInstrumentIDType instrumentName,bool tag);

    bool isValidInsturment(string inst,string& instName);

    void cancleAllPendingOrder();

    void cancleMyPendingOrder();

    void RegisterMarketDataCollection(cMarketDataCollectionPtr p) { this->m_pMarketDataEngine = p; }

    void RegisterCtpMdSpi(cMdSpi*p) { ctp_md_spi_ = p; }

    void RegisterStrategy(cStrategy *p) { m_strategyList.push_back(p); };
    
    int32 init(const ctpConfig& ctp_config);

    int32 stop();

    int32 reConnect(const ctpConfig& ctp_config);

    int32 start();

    void clearCallBack();

    void clear();

private:
 

    TThostFtdcOrderRefType    m_ORDER_REF;
    TThostFtdcFrontIDType    m_FRONT_ID;
    TThostFtdcSessionIDType    m_SESSION_ID;
    
    sTradingAccountInfo* m_accountInfo;
   //
    /* postions */
    cPositionCollectionPtr m_positionCollection;
    // 
    /* orders */
    cOrderCollectionPtr m_orderCollection;

    vector<int> m_allOrderRef;                                            // list of all orderRef
    //
    /* trades */
    cTradeCollectionPtr m_tradeCollection;

    //subscribe inst
    shared_ptr<vector<string>> m_pSubscribeInst;
    // marketData
    cMarketDataCollectionPtr m_pMarketDataEngine;
    //cMarketDataHelloWorld    ets;
    // Instrument detail Message Map    
    map<string, std::shared_ptr<CThostFtdcInstrumentField>>* m_InstMeassageMap = nullptr;

    // strategy List
    std::list<cStrategy*> m_strategyList;

    fstream m_output;
    //
    map<string,std::shared_ptr<CThostFtdcInstrumentCommissionRateField>>*m_pInstCommissionMap;

    //=======================20170828==================
    bool m_first_inquiry_order = true;//是否首次查询报单
    bool m_first_inquiry_trade = true;//是否首次查询成交
    bool m_firs_inquiry_Detail = true;//是否首次查询持仓明细
    bool m_firs_inquiry_TradingAccount = true;//是否首次查询资金账号
    bool m_firs_inquiry_Position = true;//是否首次查询投资者持仓
    bool m_first_inquiry_Instrument = true;//是否首次查询合约
    bool m_first_inquiry_commissionRate = true;//是否首次查询手续费
    

    string m_tradeDay;
    string m_actionDay;
    ///
    map<string,std::shared_ptr<CThostFtdcInstrumentField>>::iterator m_itMap;// 用于查询合约

    using CtpTdApiPtr = std::unique_ptr<CThostFtdcTraderApi, std::function<void(CThostFtdcTraderApi*)>>;
    CtpTdApiPtr ctpTdApi_;
    int32       request_id_ = 0;
    std::function<void()> on_connected_fun_;
    std::function<void(CThostFtdcRspUserLoginField*,CThostFtdcRspInfoField*)> on_login_fun_;
    std::function<void(int32)>                                                on_disconnected_fun_;
    std::function<void()>                                                     on_started_fun_;
    std::mutex                                                                mut_;
    ctpConfig                                                                 ctp_config_;
    cMdSpi*                                                                   ctp_md_spi_;

};

#endif

