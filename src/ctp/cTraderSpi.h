#ifndef __CTRADERSPI_H__
#define __CTRADERSPI_H__

#include <map>
#include <yr_structs.h>
#include <cPositionCollection.h>
#include <cTradeCollection.h>
#include <cOrderCollection.h>
#include <cMdSpi.h>
#include <regex> // 正则
#include <cMarketDataCollection.h>
#include "cStrategy.h"

using namespace std;
extern int iRequestID;
class cString;
template< class T > class cArray;

class cStrategy;

class cTraderSpi : public CThostFtdcTraderSpi
{
public:
    cTraderSpi( CThostFtdcTraderApi* pUserTraderApi, cMdSpi* pUserMdSpi,CThostFtdcMdApi* pUserMdApi,TThostFtdcBrokerIDType brokerID, TThostFtdcInvestorIDType investorID, TThostFtdcPasswordType password, bool genLog = false );
    
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
    //virtual void OnRspQryInvestorPosition( CThostFtdcInvestorPositionField* pInvestorPosition, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast );
    
    // After receiving the investor position detail request, the CTP server will send the following response to notify the client whether the request success or not
    virtual void OnRspQryInvestorPositionDetail( CThostFtdcInvestorPositionDetailField* pInvestorPositionDetail, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast );

    // order insertion response
    virtual void OnRspOrderInsert( CThostFtdcInputOrderField* pInputOrder, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast );

    // order action response
    virtual void OnRspOrderAction( CThostFtdcInputOrderActionField* pInputOrderAction, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ); 

    // order query response
    virtual void OnRspQryOrder( CThostFtdcOrderField* pOrder, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast );
    
    // the error notification caused by client request
    virtual void OnRspError( CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast );

    virtual void OnHeartBeatWarning( int nTimeLapse );

    virtual void OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);


    ///请求查询成交响应
    virtual void OnRspQryTrade(CThostFtdcTradeField *pTrade, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

    // order insertion return
    virtual void OnRtnOrder( CThostFtdcOrderField* pOrder );

    virtual void OnRtnTrade( CThostFtdcTradeField* pTrade);

    //请求查询合约手续费率响应
    virtual void OnRspQryInstrumentCommissionRate(CThostFtdcInstrumentCommissionRateField *pInstrumentCommissionRate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) ;


    void RegisterPositionCollection( cPositionCollectionPtr p );
    void RegisterOrderCollection( cOrderCollectionPtr p );
    void RegisterTradeCollection( cTradeCollectionPtr p );
    void RegisterSubscribeInstList(shared_ptr<vector<string>> p);
    void RegisterInstMessageMap( map<string, CThostFtdcInstrumentField*>* p );
    void RegisterInstCommissionMap(map<string,shared_ptr< CThostFtdcInstrumentCommissionRateField>> *p);
    void ReqQryInstrument();

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

    void Close();

    void Init();

    void GetInstrumentIDs( cArray< cString >& ) const;

    const sInstrumentInfo* GetInstrumentInfo( const cString& ) const;

    const sTradingAccountInfo* GetTradingAccountInfo() const { return m_accountInfo; }

    void saveInstrumentField(CThostFtdcInstrumentField* instField);

    void showPositionDetail();

    void ReqOrderInsert(TThostFtdcInstrumentIDType instId,TThostFtdcDirectionType dir, TThostFtdcCombOffsetFlagType kpp,TThostFtdcPriceType price,   TThostFtdcVolumeType vol);

    void insertOrder(string inst, DIRECTION dire, OFFSETFLAG flag, int vol, double orderPrice, string tag = "");

    void StraitClose(TThostFtdcInstrumentIDType instId,TThostFtdcDirectionType dir,TThostFtdcPriceType price,TThostFtdcVolumeType vol,string tag = "");

    char MapDirection(char src, bool toOrig);

    char MapOffset(char src, bool toOrig);

    bool subscribeInst(TThostFtdcInstrumentIDType instrumentName,bool tag);

    bool isValidInsturment(string inst,string& instName);


    void cancleAllPendingOrder();

    void cancleMyPendingOrder();

    void RegisterMarketDataEngine(cMarketDataCollectionPtr p){ this->m_pMarketDataEngine = p;}

    void RegisterStrategy(cStrategy *p) { m_strategyList.push_back(p); };
private:
    CThostFtdcTraderApi* m_pUserTraderApi;
    cArray< cString > m_instrumentIDs;

    TThostFtdcOrderRefType    m_ORDER_REF;
    TThostFtdcFrontIDType    m_FRONT_ID;
    TThostFtdcSessionIDType    m_SESSION_ID;
    
    sTradingAccountInfo* m_accountInfo;
    map< cString, sInstrumentInfo* > m_instrumentInfo;        // useful trading information for traded instruments
    //
    /* postions */
    /*cPositionCollection* m_positionCollection;*/
    cPositionCollectionPtr m_positionCollection;
    // 
    /* orders */
    /*cOrderCollection* m_orderCollection;*/
    cOrderCollectionPtr m_orderCollection;

    vector<int> m_allOrderRef;                                            // list of all orderRef
    //
    /* trades */
    /*cTradeCollection* m_tradeCollection;*/
    cTradeCollectionPtr m_tradeCollection;

    //subscribe inst
    shared_ptr<vector<string>> m_pSubscribeInst;

    // Instrument detail Message Map    
    map<string, CThostFtdcInstrumentField*>* m_InstMeassageMap;

    // strategy List
    std::list<cStrategy*> m_strategyList;

    //
    map<string,shared_ptr<CThostFtdcInstrumentCommissionRateField>>*m_pInstCommissionMap;

    void ReqUserLogin();
    void ReqSettlementInfoConfirm();
    bool IsErrorRspInfo( CThostFtdcRspInfoField* pRspInfo );
    bool IsMyOrder( CThostFtdcOrderField* pOrder );

    TThostFtdcBrokerIDType    m_brokerID;
    TThostFtdcInvestorIDType m_investorID;
    char m_password[252];

    bool m_genLog;
    cString m_logFile;

    //=======================20170828==================
    bool m_first_inquiry_order;//是否首次查询报单
    bool m_first_inquiry_trade;//是否首次查询成交
    bool m_firs_inquiry_Detail;//是否首次查询持仓明细
    bool m_firs_inquiry_TradingAccount;//是否首次查询资金账号
    bool m_firs_inquiry_Position;//是否首次查询投资者持仓
    bool m_first_inquiry_Instrument;//是否首次查询合约
    bool m_first_inquiry_commissionRate;//是否首次查询手续费
    
    vector<CThostFtdcOrderField*> m_orderList;//委托记录，全部合约
    vector<CThostFtdcOrderField*> m_pendOrderList;//挂单记录，全部合约
    vector<CThostFtdcTradeField*> m_tradeList;//成交记录，全部合约

    vector<CThostFtdcTradeField*> m_tradeListNotClosedAccount;//未平仓记录

    map<string,cPositionDetailPtr> m_position_message_map;//持仓记录 

    double m_closeProfit;//平仓盈亏，所有合约一起算后的值，另外在m_trade_message_map有单独计算每个合约的平仓盈亏值
    
    double m_OpenProfit;//浮动盈亏，所有合约一起算后的值，另外在m_trade_message_map有单独计算每个合约的浮动盈亏值

    //map<string, CThostFtdcInstrumentField*> m_instMessage_map;//保存合约信息的map
    
    cMdSpi* m_pMdSpi;//行情API指针，构造函数里赋值

    CThostFtdcMdApi* m_pMDUserApi_td;
    double m_accountMargin;

    fstream m_output;
    string m_tradeDay;
    string m_actionDay;
    bool m_qryStatus;



    ///
    map<string,CThostFtdcInstrumentField*>::iterator m_itMap;// 用于查询合约
    /// marketData
    cMarketDataCollectionPtr m_pMarketDataEngine;
};

typedef int (*ccbf_secureApi_LoginTrader)(CThostFtdcTraderApi* ctp_futures_pTraderApi, TThostFtdcBrokerIDType brokeId, TThostFtdcUserIDType userId, char* pChar_passwd, int& ctp_futures_requestId);

#endif

