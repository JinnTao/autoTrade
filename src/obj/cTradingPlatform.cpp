//
#include <cSystem.h>
#include <cTradingPlatform.h>
#include <cTraderSpi.h>
#include <cTick.h>

#include <iostream>
#include <string>
#include "easylogging\easylogging++.h"
#include <io.h>
#include "IniFile.h"
using std::string;

#ifndef _DEBUG
#define _DEBUG 0
#endif
bool CompareStringArray(const cArray<cString>& strArray1, const cArray<cString>& strArray2) {
    if (&strArray1 == &strArray2)
        return true;
    if (strArray1.getSize() != strArray2.getSize())
        return false;

    for (int i = 0; i < strArray1.getSize(); ++i) {
        cString str1 = strArray1[i];
        int     j;
        for (j = 0; j < strArray2.getSize(); ++j) {

            if (Compare(str1, strArray2[j]))
                break;
        }
        if (j >= strArray2.getSize())
            return false;
    }
    return true;
}

// cTradingPlatform::cTradingPlatform()
//: m_pTraderSpi( NULL )
//, m_nRequestID( 0 )
//, m_runAutoTrade( true )
//{
//    if( m_pMarketDataEngine.get() )
//        m_pMarketDataEngine.reset();
//
//    if( m_pStrategy.get() )
//        m_pStrategy.reset();
//
//    m_pPositions = make_shared< cPositionCollection >();
//    m_pOrders = make_shared< cOrderCollection >();
//    m_pTrades = make_shared< cTradeCollection >();
//    m_pSubscribeInst = make_shared<vector<string>>();
//    m_pInstMessageMap = new map<string, CThostFtdcInstrumentField*>();
//    m_pInstCommissionRate = new map<string,shared_ptr< CThostFtdcInstrumentCommissionRateField>>();
//
////    m_pInstMessageMap = make_shared<map<string,CThostFtdcInstrumentField*>>();
//}

// cTradingPlatform::~cTradingPlatform()
//{
//    ClearPlatform();
//}
cTradingPlatform::cTradingPlatform() {}
cTradingPlatform::~cTradingPlatform() {
    LOG(INFO) << "Clear trading platform.";
    // this->ctp_td_spi_.clear();
    // this->ctp_md_spi_.clear();
}

void cTradingPlatform::RegisterTraderSpi(cTraderSpi* pTraderSpi) {
    if (m_pTraderSpi == pTraderSpi)
        return;

    m_pTraderSpi = pTraderSpi;

    // m_pTraderSpi->RegisterPositionCollection(m_pPositions);
    // m_pTraderSpi->RegisterOrderCollection(m_pOrders);
    // m_pTraderSpi->RegisterTradeCollection(m_pTrades);
    // m_pTraderSpi->RegisterInstMessageMap(m_pInstMessageMap);
    // m_pTraderSpi->RegisterInstCommissionMap(m_pInstCommissionRate);
    // m_pTraderSpi->RegisterSubscribeInstList(m_pSubscribeInst);
    // cArray< cString > instrumentIDs;
    // pTraderSpi->GetInstrumentIDs( instrumentIDs );

    // if( !m_instrumentIDs.getSize() )
    //{
    //    for( int i = 0; i < instrumentIDs.getSize(); ++i )
    //        m_instrumentIDs.push_back( instrumentIDs[i] );
    //}
    // else
    //{
    //    if( !CompareStringArray( m_instrumentIDs, instrumentIDs ) )
    //        yr_error( "cTraderSpi and cTradingPlatform has different instruments" );
    //}

    // for( int i = 0; i < m_instrumentIDs.getSize(); ++i )
    //{
    //    map< cString, double >::iterator it = m_closedPnL.find( m_instrumentIDs[i] );
    //    if( it == m_closedPnL.end() )
    //    {
    //        m_closedPnL.insert( map< cString, double >::value_type( m_instrumentIDs[i], 0.0 ) );
    //    }
    //}
}

void cTradingPlatform::RegisterMdSpi(cMdSpi* p) {
    if (m_pMdSpi == p)
        return;

    m_pMdSpi = p;
}

void cTradingPlatform::RegisterMarketDataEngine(cMarketDataCollectionPtr pMarketDataEngine) {
    if (m_pMarketDataEngine.get())
        m_pMarketDataEngine.reset();

    m_pMarketDataEngine = pMarketDataEngine;
    /*
        cArray< cString > instrumentIDs;
        pMarketDataEngine->GetInstrumentIDs( instrumentIDs );

        if( !m_instrumentIDs.getSize() )
        {
            
            for( int i = 0; i < instrumentIDs.getSize(); ++i )
                m_instrumentIDs.push_back( instrumentIDs[i] );
        }
        else
        {
            if( !CompareStringArray( m_instrumentIDs, instrumentIDs ) )
                yr_error( "cMarketDataCollection and cTradingPlatform has different instruments" );
        }

        for( int i = 0; i < m_instrumentIDs.getSize(); ++i )
        {
            map< cString, double >::iterator it = m_closedPnL.find( m_instrumentIDs[i] );
            if( it == m_closedPnL.end() )
            {
                m_closedPnL.insert( map< cString, double >::value_type( m_instrumentIDs[i], 0.0 ) );
            }
        }
    */
}

void cTradingPlatform::RegisterStrategy(cStrategyPtr pStrategy) {}

void cTradingPlatform::RegisterParameters(ctpConfig* p, mongoConfig* pM) {
    // this->m_pAutoSetting = p;
    // this->m_pMongoSetting = pM;
    this->m_pMarketDataEngine->registerMongoSetting(pM);
}
void cTradingPlatform::SendNewOrder(cOrder* pOrder) {}

void cTradingPlatform::PrintClosePnL(int tradeID) const {}

const sInstrumentInfo* cTradingPlatform::GetInstrumentInfo(const cString& instrumentID) const {
    if (m_pTraderSpi)
        return m_pTraderSpi->GetInstrumentInfo(instrumentID);
    else {
        return NULL;
        // demo paper trading
        //@todo
    }
}

const sTradingAccountInfo* cTradingPlatform::GetTradingAccountInfo() const {
    if (m_pTraderSpi)
        return m_pTraderSpi->GetTradingAccountInfo();
    else {
        return NULL;
        // demo paper trading
        //@todo
    }
}

void cTradingPlatform::CancelPendingOrders() {}

void cTradingPlatform::CancelPendingOrders(const cString& instrumentID) {}

void cTradingPlatform::CancelPendingOrder(int orderID) {
    // cOrder* pOrder = m_pOrders->GetOrderHandle( orderID );
    // if( m_pTraderSpi )
    //{
    //    if( pOrder && pOrder->IsPendingOrder() )
    //        m_pTraderSpi->ReqOrderAction( pOrder );
    //}
    // else
    //{
    //    //
    //    /*
    //        here we do the demo trade and no TraderSpi is required
    //        and thus we simply change m_orders
    //    */
    //    m_pOrders->Remove( orderID );
    //}
}
//字符串分割函数

std::vector<std::string> cTradingPlatform::splitToStr(std::string str, std::string pattern) {
    std::string::size_type   pos;
    std::vector<std::string> result;
    str += pattern;  //扩展字符串以方便操作
    size_t size = str.size();

    for (int i = 0; i < size; i++) {
        pos = str.find(pattern, i);
        if (pos < size) {
            std::string s = str.substr(i, pos - i);
            result.push_back(s);
            i = pos + pattern.size() - 1;
        }
    }
    return result;
}

std::vector<int32> cTradingPlatform::splitToInt(std::string str, std::string pattern) {
    std::string::size_type pos;
    std::vector<int32>     result;
    str += pattern;  //扩展字符串以方便操作
    uint32 size = str.size();

    for (int i = 0; i < size; i++) {
        pos = str.find(pattern, i);
        if (pos < size) {
            std::string s = str.substr(i, pos - i);
            result.push_back(std::atoi(s.c_str()));
            i = pos + pattern.size() - 1;
        }
    }
    return result;
}
void cTradingPlatform::initStrategy(strategyConfig& para) {

    std::vector<std::string> instList       = this->splitToStr(std::string(para.inst), ",");
    std::vector<int32>       lotsList       = this->splitToInt(std::string(para.lots), ",");
    std::vector<int32>       timeModeList   = this->splitToInt(std::string(para.timeMode), ",");
    std::vector<std::string> collectionList = this->splitToStr(std::string(para.collectionList), ",");

    if (instList.size() != lotsList.size() || timeModeList.size() != lotsList.size() ||
        collectionList.size() != lotsList.size()) {
        LOG(ERROR) << " initStrategy  inst lot timeMode Error, init Strategy Failed";
        return;
    }
    for (int i = 0; i < instList.size(); i++) {
        std::shared_ptr<cStrategyKingKeltner> pStrategy = std::make_shared<cStrategyKingKeltner>();

        pStrategy->RegisterMarketDataCollection(this->m_pMarketDataEngine);
        // pStrategy->RegisterTradeSpi(ctp_td_spi_);
        // pStrategy->RegisterMdSpi(ctp_md_spi_);
        pStrategy->RegisterPositionCollectionPtr(this->m_pPositions);
        pStrategy->RegisterOrderCollectionPtr(this->m_pOrders);
        pStrategy->RegisterTradeCollectionPtr(this->m_pTrades);
        pStrategy->RegisterTxtDir(string(para.tradeDayDir), string(para.dataBaseDir));
        pStrategy->RegisterAutoSetting(&para);

        this->readDay(string(para.tradeDayDir), this->m_tradeDayList);
        this->m_pMarketDataEngine->setTradeDayList(&this->m_tradeDayList);

        pStrategy->setInst(instList[i]);
        pStrategy->setlots(lotsList[i]);
        pStrategy->setTimeMode(timeModeList[i]);
        pStrategy->setInitDate(para.startDate, para.endDate);
        pStrategy->setCollectionName(collectionList[i]);

        this->m_pTraderSpi->RegisterStrategy(pStrategy.get());
        this->m_StrategyKKList.push_back(pStrategy);
    }
}

void cTradingPlatform::readDay(string fileName, map<string, int>& workDay) {
    ifstream file1(fileName, ios::in);  //以只读方式读入,读取原始数据
    char     dataline[512];             //行数变量
    string   date;
    int      i = 1;
    if (!file1) {
        cout << "no such file!" << endl;
        // abort();
        return;
    }
    while (file1.getline(dataline, 1024, '\n'))  // while开始，读取一行1024够大？
    {
        // sscanf_s(dataline,"%s",date);
        date = dataline;
        workDay.insert(pair<string, int>(date, i));
        i++;
        // cout << date << endl;
    }
}

DWORD cTradingPlatform::AutoTrading() {
    std::this_thread::sleep_for(std::chrono::microseconds(1000));
    string str;
    char   dire[50], offset[50], inst[50], price[50], order[50];
    int    vol, mark = 0, orderNo;
    char   tag[10];
    LOG(INFO) << "--------------------Human-computer interaction function Start--------------------------------";
    LOG(INFO) << "OrderList: help | show | order| trade | stop | run |close |buy/sell open/close inst vol price| "
                 "cancel seqNo.";
    // initial subcribe instrument
    // this->ctp_md_spi_.SubscribeMarketData(this->m_pSubscribeInst);

    while (true) {
        // std::cin>>str;
        memset(price, 0, 50);
        memset(order, 0, 50);
        memset(tag, 0, 10);
        vol     = 0;
        orderNo = 0;
        getline(std::cin, str);
        if (str == "show") {
            ctp_td_spi_.showPositionDetail();
        }

        else if (str == "close") {
            cerr << "close Position:" << endl;
            // g_pUserSpi_tradeAll->ForceClose();
        } else if (str == "run") {
            //            m_strategy.start();
            for (auto iter : strategy_list_) {
                iter->start();
            }
        } else if (str == "stop") {
            //    m_strategy.stop();
            for (auto iter : strategy_list_) {
                iter->stop();
            }

        } else if (str == "order") {
            this->order_collection_->PrintPendingOrders();
        } else if (str == "trade") {
            // 首先查询手续费 再查询成交
            this->ctp_td_spi_.ReqQryInstrumentCommissionRate(true);
        } else if (str == "help") {
            cerr << "OrderList: show | order| trade | stop | run |close |buy/sell open/close inst vol price -> ";
        } else if (str == "account") {
            this->ctp_td_spi_.ReqQryTradingAccount();
        } else if (str == "SP") {
            cerr << "Sp Order command: " << endl;
            getline(std::cin, str);
            // insert order
            char SpPre[50];
            sscanf(str.c_str(), "%s %s %s %s %d %s %c", dire, offset, SpPre, inst, &vol, price, &tag);
            string allInst = string(SpPre) + string(" ") + string(inst);
            double dPrice  = atof(price);
            if (vol != 0) {
                this->insertOrder(string(allInst), string(dire), string(offset), vol, dPrice, tag);
            }
            // cancle order
            sscanf(str.c_str(), "%s %d", order, &orderNo);
            if (orderNo != 0) {
                this->cancleOrder(order, orderNo);
            }
        } else if (str.length() > 7) {
            // insert order
            sscanf(str.c_str(), "%s %s %s %d %s %c", dire, offset, inst, &vol, price, &tag);
            double dPrice = atof(price);
            if (vol != 0) {
                this->insertOrder(string(inst), string(dire), string(offset), vol, dPrice, tag);
            }
            // cancle order
            sscanf(str.c_str(), "%s %d", order, &orderNo);
            if (orderNo != 0) {
                this->cancleOrder(order, orderNo);
            }
        }
    }
    return 0;
}

DWORD cTradingPlatform::ProcessOrderTest() {
    return 0;
}

DWORD cTradingPlatform::IOProcess() {
    //    return m_pMarketDataEngine->IOProcess();
    return 0;
}

DWORD cTradingPlatform::SimulationIOProcess() {
    // if( m_pMarketDataEngine.get() )
    //    return m_pMarketDataEngine->SimulationIOProcess();
    // else
    //    return 0;
    return 0;
}

void cTradingPlatform::PrintPendingOrders() const {
    m_pOrders->PrintPendingOrders();
}

void cTradingPlatform::PrintCancelledOrders() const {
    m_pOrders->PrintCancelledOrders();
}

void cTradingPlatform::PrintAllOrders() const {
    m_pOrders->PrintAllOrders();
}

void cTradingPlatform::PrintPositionSummary() const {
    //    m_pPositions->SummaryByInstrument();
}

bool cTradingPlatform::SimulationUpdate(const cTick& tick) {
    // if( m_pTraderSpi )
    //    return false;

    // m_pMarketDataEngine->SimulationUpdate( tick );
    // bool flag = m_pStrategy->SimTimeIndicatorSignalUpdate( tick );
    // return flag;
    return true;
}

void cTradingPlatform::Sleep() {
    int  hh   = cSystem::HourNow();
    int  mm   = cSystem::MinuteNow();
    bool flag = false;
    if (hh > 2 && hh < 8)
        flag = true;
    else if (hh == 8 && mm < 30)
        flag = true;

    if (flag) {
        //
        if (m_pTrades->Count())
            m_pTrades->Clear();
        //
        if (m_pOrders->Count())
            m_pOrders->Clear();
    }
}

void cTradingPlatform::WakeUp() {}

void cTradingPlatform::ClearPlatform() {
    //    /*m_instrumentIDs.clear();*/
    m_runAutoTrade = false;
    m_nRequestID   = 0;
    m_pTraderSpi   = NULL;

    /*if( m_pMarketDataEngine.get() )
        m_pMarketDataEngine.reset();

    if( m_pStrategy.get() )
        m_pStrategy.reset();*/

    if (m_pPositions.get()) {
        //        m_pPositions->Clear();
        m_pPositions.reset();
    }

    if (m_pOrders.get()) {
        m_pOrders->Clear();
        m_pOrders.reset();
    }

    if (m_pTrades.get()) {
        m_pTrades->Clear();
        m_pTrades.reset();
    }
    //
    delete this->m_pInstCommissionRate;
    delete this->m_pInstMessageMap;
}
void cTradingPlatform::cancleOrder(string order, int seqNo) {
    if (order == "cancle") {

        shared_ptr<cOrder> pOrder = NULL;
        if (seqNo != -1) {
            if (!this->order_collection_->getOrderByNo(seqNo, pOrder)) {
                cerr << "  Order Seq No Exist." << endl;
                return;
            }
            this->ctp_td_spi_.ReqOrderAction(pOrder);
        } else {
            this->ctp_td_spi_.cancleAllPendingOrder();
        }
    }
}
void cTradingPlatform::cancleAllOrder(string order, string tag) {
    // if(order == "cancle" && tag == "all"){
    //    vector<cOrderPtr> vOrder = this->m_pOrders->GetAllOrder();
    //    for(auto it = vOrder.begin();it!=vOrder.end();it++){

    //        this->m_pTraderSpi->ReqOrderAction(it->get()->GetOrderID());
    //    }
    //}
}

void cTradingPlatform::insertOrder(string inst, string dire, string flag, int vol, double orderPrice, string tag) {
    // get parameters type
    traderTag::DIRECTION  eDire;
    traderTag::OFFSETFLAG eFlag;
    if (dire == "buy") {
        eDire = traderTag::DIRECTION::buy;
    } else if (dire == "sell") {
        eDire = traderTag::DIRECTION::sell;
    } else {
        cerr << "input parameter Error" << endl;
        return;
    }

    if (flag == "open") {
        eFlag = traderTag::OFFSETFLAG::open;
    } else if (flag == "close") {
        eFlag = traderTag::OFFSETFLAG::close;
        ;
    } else {
        cerr << "input parameter Error" << endl;
        return;
    }

    // go into order
    this->ctp_td_spi_.insertOrder(inst, eDire, eFlag, vol, orderPrice, tag);
    _sleep(500);  // wait 500ms for pTrader response.
}

int32 cTradingPlatform::loadConfig(const string& config_file) {
    try {
        IniFile ini("setting.ini");

        ini.ReadString("account", "brokerID", "1").copy(ctpConfig_.brokerId, sizeof ctpConfig_.brokerId);
        ini.ReadString("account", "userID", "1").copy(ctpConfig_.userId, sizeof ctpConfig_.userId);
        ini.ReadString("account", "password", "1").copy(ctpConfig_.passwd, sizeof ctpConfig_.passwd);
        ini.ReadString("account", "tdAddress", "1").copy(ctpConfig_.tdAddress, sizeof ctpConfig_.tdAddress);
        ini.ReadString("account", "mdAddress", "1").copy(ctpConfig_.mdAddress, sizeof ctpConfig_.mdAddress);
        ini.ReadString("account", "mdFlowPath", "1").copy(ctpConfig_.md_flow_path_, sizeof ctpConfig_.md_flow_path_);
        ini.ReadString("account", "tdFlowPath", "1").copy(ctpConfig_.td_flow_path_, sizeof ctpConfig_.td_flow_path_);

        ini.ReadString("dbMongo", "address", "1").copy(mongoConfig_.address, sizeof mongoConfig_.address);
        ini.ReadString("dbMongo", "dataBase", "1").copy(mongoConfig_.database, sizeof mongoConfig_.database);
        mongoConfig_.mongoPort    = ini.ReadInt("dbMongo", "mongoPort", 1);
        mongoConfig_.mongoLogging = ini.ReadInt("dbMongo", "mongoLogging", 1);

        ini.ReadString("dataBase", "dataBaseDir", "1")
            .copy(strategyConfig_.dataBaseDir, sizeof strategyConfig_.dataBaseDir);
        ini.ReadString("dataBase", "tradeDayDir", "1")
            .copy(strategyConfig_.tradeDayDir, sizeof strategyConfig_.tradeDayDir);
        ini.ReadString("dataBase", "startDate", "1").copy(strategyConfig_.startDate, sizeof strategyConfig_.startDate);
        ini.ReadString("dataBase", "endDate", "1").copy(strategyConfig_.endDate, sizeof strategyConfig_.endDate);

        ini.ReadString("strategy", "inst", "1").copy(strategyConfig_.inst, sizeof strategyConfig_.inst);
        ini.ReadString("strategy", "lots", "1").copy(strategyConfig_.lots, sizeof strategyConfig_.lots);
        ini.ReadString("strategy", "timeModes", "1").copy(strategyConfig_.timeMode, sizeof strategyConfig_.timeMode);
        ini.ReadString("strategy", "collectionList", "1")
            .copy(strategyConfig_.collectionList, sizeof strategyConfig_.collectionList);
        // KingKeltner
        ini.ReadString("KingKeltner", "collectionName", "1")
            .copy(strategyConfig_.collectionName, sizeof strategyConfig_.collectionName);
        ini.ReadString("KingKeltner", "startDateTime", "1")
            .copy(strategyConfig_.startDateTime, sizeof strategyConfig_.startDateTime);
        ini.ReadString("KingKeltner", "endDateTime", "1")
            .copy(strategyConfig_.endDateTime, sizeof strategyConfig_.endDateTime);

        strategyConfig_.kkLength      = ini.ReadDouble("KingKeltner", "kkLength", 1);
        strategyConfig_.kkDev         = ini.ReadDouble("KingKeltner", "kkDev", 1);
        strategyConfig_.trailingPrcnt = ini.ReadDouble("KingKeltner", "trailingPrcnt", 1);
        strategyConfig_.fixedSize     = ini.ReadDouble("KingKeltner", "fixedSize", 1);
        strategyConfig_.initDays      = ini.ReadDouble("KingKeltner", "initDays", 1);

        // strcpy_s(ctpConfig_.brokerId, sizeof(ctpConfig_.brokerId), ini.ReadString("account", "brokerID",
        // "1").c_str());  strcpy_s(ctpConfig_.tdAddress, sizeof(ctpConfig_.tdAddress), ini.ReadString("account",
        // "tdAddress", "1").c_str());  strcpy_s(ctpConfig_.passwd,sizeof(ctpConfig_.passwd), ini.ReadString("account",
        // "password", "1").c_str());  strcpy_s(ctpConfig_.mdAddress, sizeof(ctpConfig_.mdAddress),
        // ini.ReadString("account", "mdAddress", "1").c_str());  strcpy_s(ctpConfig_.userId, sizeof(ctpConfig_.userId),
        // ini.ReadString("account", "userID", "1").c_str());
        //// mongoDB
        // strcpy_s(mongoConfig_.address, sizeof(mongoConfig_.address), ini.ReadString("dbMongo", "address",
        // "1").c_str());  strcpy_s(mongoConfig_.database, sizeof(mongoConfig_.database), ini.ReadString("dbMongo",
        // "dataBase", "1").c_str());  mongoConfig_.mongoPort = ini.ReadInt("dbMongo", "mongoPort", 1);
        // mongoConfig_.mongoLogging = ini.ReadInt("dbMongo", "mongoLogging", 1);
        //// txt database dir
        // strcpy_s(strategyConfig_.dataBaseDir, sizeof(strategyConfig_.dataBaseDir), ini.ReadString("dataBase",
        // "dataBaseDir", "1").c_str());  strcpy_s(strategyConfig_.tradeDayDir, sizeof(strategyConfig_.tradeDayDir),
        // ini.ReadString("dataBase", "tradeDayDir", "1").c_str());  strcpy_s(strategyConfig_.startDate,
        // sizeof(strategyConfig_.startDate), ini.ReadString("dataBase", "startDate", "1").c_str());
        // strcpy_s(strategyConfig_.endDate, sizeof(strategyConfig_.endDate), ini.ReadString("dataBase", "endDate",
        // "1").c_str());
        //// strategy
        // strcpy_s(strategyConfig_.inst, sizeof(strategyConfig_.inst), ini.ReadString("strategy", "inst",
        // "1").c_str());  strcpy_s(strategyConfig_.lots, sizeof(strategyConfig_.lots), ini.ReadString("strategy",
        // "lots", "1").c_str());  strcpy_s(strategyConfig_.timeMode, sizeof(strategyConfig_.timeMode),
        // ini.ReadString("strategy", "timeModes", "1").c_str());  strcpy_s(strategyConfig_.collectionList,
        // sizeof(strategyConfig_.collectionList), ini.ReadString("strategy", "collectionList", "1").c_str());
        //// datasource
        // strcpy_s(strategyConfig_.collectionName, sizeof(strategyConfig_.collectionName),
        // ini.ReadString("KingKeltner", "collectionName", "").c_str());  strcpy_s(strategyConfig_.startDateTime,
        // sizeof(strategyConfig_.startDateTime), ini.ReadString("KingKeltner", "startDateTime", "").c_str());
        // strcpy_s(strategyConfig_.endDateTime, sizeof(strategyConfig_.endDateTime), ini.ReadString("KingKeltner",
        // "endDateTime", "").c_str());
        //// KingKeltner
        // strategyConfig_.kkLength = ini.ReadDouble("KingKeltner", "kkLength", 1);
        // strategyConfig_.kkDev = ini.ReadDouble("KingKeltner", "kkDev", 1);
        // strategyConfig_.trailingPrcnt = ini.ReadDouble("KingKeltner", "trailingPrcnt", 1);
        // strategyConfig_.fixedSize = ini.ReadDouble("KingKeltner", "fixedSize", 1);
        // strategyConfig_.initDays = ini.ReadDouble("KingKeltner", "initDays", 1);
        return 0;
    } catch (std::exception& e) {
        LOG(ERROR) << "Errpr:" << e.what();
        return -1;
    }
}

int32 cTradingPlatform::createPath() {
    try {
        auto createFile = [this](std::string path) -> bool {
            if (access(path.c_str(), 0) == -1) {
#ifdef WIN32
                int flag = mkdir(path.c_str());
#endif  // WIN32
#ifdef linux
                int flag = mkdir(path.c_str(), 0777);  // 0777: the biggest access
#endif                                                 // linux
                if (flag == 0) {
                    return true;
                } else {
                    return false;
                }
            } else {
                return true;
            }
        };
        if (!createFile(ctpConfig_.md_flow_path_)) {
            LOG(ERROR) << "md flow path create failed";
            return -1;
        }
        if (!createFile(ctpConfig_.td_flow_path_)) {
            LOG(ERROR) << "td flow path create failed";
            return -2;
        }
        return 0;

    } catch (std::exception& e) {
        LOG(ERROR) << "Error:" << e.what();
        return -1;
    }
}

int32 cTradingPlatform::init() {
    int32 init_result = 0;
    try {
        // mongo store
        // init_result = mongo_store_.init(mongoConfig_);
        // if (init_result != 0){
        //    LOG(ERROR) << "MongoDb init failed! Result:" << init_result;
        //}

        // md init
        // if (ctp_md_spi_) {
        //    ctp_md_spi_.reset();
        //}
        // ctp_md_spi_ = std::make_shared<cMdSpi>();
        init_result = ctp_md_spi_.init(ctpConfig_);
        if (init_result != 0) {
            LOG(ERROR) << "Md init failed! Result:" << init_result;
            return -1;
        }

        // td init
        // if (ctp_td_spi_) {
        //    ctp_td_spi_.reset();
        //}
        // ctp_td_spi_ = std::make_shared<cTraderSpi>();
        init_result = ctp_td_spi_.init(ctpConfig_);
        if (init_result != 0) {
            LOG(ERROR) << "Td init failed! Result:" << init_result;
            return -2;
        }

        // init Position、Orders、Trades、MarketDataEngine
        {
            if (marketdate_collection_) {
                marketdate_collection_.reset();
            }
            marketdate_collection_ = std::make_shared<cMarketDataCollection>();
            LOG(INFO) << "Market data collection create success!";

            if (position_collection_) {
                position_collection_.reset();
            }
            position_collection_ = std::make_shared<cPositionCollection>();
            LOG(INFO) << "Position collection create success!";
            if (order_collection_) {
                order_collection_.reset();
            }
            order_collection_ = std::make_shared<cOrderCollection>();
            LOG(INFO) << "order collection create success!";
            if (trade_collection_) {
                trade_collection_.reset();
            }
            trade_collection_ = std::make_shared<cTradeCollection>();
            LOG(INFO) << "Trade collection create success!";
            if (inst_field_map_) {
                inst_field_map_.reset();
            }
            inst_field_map_ = std::make_shared<std::map<std::string, std::shared_ptr<CThostFtdcInstrumentField>>>();
            LOG(INFO) << "Inst field list create success!";
            if (inst_commission_rate_field_map_) {
                inst_commission_rate_field_map_.reset();
            }
            inst_commission_rate_field_map_ =
                std::make_shared<std::map<std::string, std::shared_ptr<CThostFtdcInstrumentCommissionRateField>>>();
            LOG(INFO) << "Inst commission rate field map create success!";

            if (subscribe_inst_v_) {
                subscribe_inst_v_.reset();
            }
            subscribe_inst_v_ = std::make_shared<std::vector<std::string>>();
            LOG(INFO) << "Subscribe inst vector success!";

            // if (strategy_list_) {
            //    strategy_list_.reset();
            //}
            // strategy_list_ = std::make_shared<std::list<std::shared_ptr<cStrategyKingKeltner>>>();
            // LOG(INFO) << "Strategy list create success!";
        }
        // init connection
        {
            ctp_md_spi_.RegisterMarketDataCollection(marketdate_collection_.get());
            ctp_td_spi_.RegisterMarketDataCollection(marketdate_collection_);

            ctp_td_spi_.RegisterPositionCollection(position_collection_);
            ctp_td_spi_.RegisterOrderCollection(order_collection_);
            ctp_td_spi_.RegisterTradeCollection(trade_collection_);
            ctp_td_spi_.RegisterInstMessageMap(inst_field_map_.get());
            ctp_td_spi_.RegisterInstCommissionMap(inst_commission_rate_field_map_.get());
            ctp_td_spi_.RegisterSubscribeInstList(subscribe_inst_v_);
            ctp_td_spi_.RegisterCtpMdSpi(&ctp_md_spi_);

            std::vector<std::string> instList     = this->splitToStr(std::string(strategyConfig_.inst), ",");
            std::vector<int32>       lotsList     = this->splitToInt(std::string(strategyConfig_.lots), ",");
            std::vector<int32>       timeModeList = this->splitToInt(std::string(strategyConfig_.timeMode), ",");
            std::vector<std::string> collectionList =
                this->splitToStr(std::string(strategyConfig_.collectionList), ",");

            if (instList.size() != lotsList.size() || timeModeList.size() != lotsList.size() ||
                collectionList.size() != lotsList.size()) {
                LOG(INFO) << " InitStrategy  inst lot timeMode Error:size not match, init Strategy Failed";
                return -3;
            }
            for (int i = 0; i < instList.size(); i++) {
                std::shared_ptr<cStrategyKingKeltner> pStrategy = std::make_shared<cStrategyKingKeltner>();

                pStrategy->RegisterMarketDataCollection(marketdate_collection_);
                //pStrategy->RegisterTradeSpi(ctp_td_spi_);
                //pStrategy->RegisterMdSpi(ctp_md_spi_);
                pStrategy->RegisterPositionCollectionPtr(position_collection_);
                pStrategy->RegisterOrderCollectionPtr(order_collection_);
                pStrategy->RegisterTradeCollectionPtr(trade_collection_);
                pStrategy->RegisterTxtDir(string(strategyConfig_.tradeDayDir), string(strategyConfig_.dataBaseDir));
                pStrategy->RegisterAutoSetting(&strategyConfig_);

                this->readDay(string(strategyConfig_.tradeDayDir), trade_day_list_);
                marketdate_collection_->setTradeDayList(&trade_day_list_);

                pStrategy->setInst(instList[i]);
                pStrategy->setlots(lotsList[i]);
                pStrategy->setTimeMode(timeModeList[i]);
                pStrategy->setInitDate(strategyConfig_.startDate, strategyConfig_.endDate);
                pStrategy->setCollectionName(collectionList[i]);

                // ctp_td_spi_->RegisterStrategy(pStrategy.get());
                strategy_list_.push_back(pStrategy);
                LOG(INFO) << "Init strategy inst:" << instList[i] << " lots:" << lotsList[i]
                          << " timeMode: " << timeModeList[i] << " collectionList: " << collectionList[i];
            }
        }

    } catch (std::exception& e) {
        LOG(ERROR) << "Init failed! " << e.what();
        return -3;
    }
    return init_result;
}

int32 cTradingPlatform::start() {
    int start_result = 0;
    try {

        start_result = ctp_md_spi_.start();
        if (start_result != 0) {
            LOG(ERROR) << "Md start failed! Result: " << start_result;
            return -1;
        }

        start_result = ctp_td_spi_.start();
        if (start_result != 0) {
            LOG(ERROR) << "Td start failed! Result: " << start_result;
            return -2;
        }

        // TradingPlatform start
        {
            // inter thread start
            inter_thread_ = std::thread(&cTradingPlatform::AutoTrading, this);

            // strategy list check status
            for (auto iter = strategy_list_.begin(); iter != strategy_list_.end(); iter++) {
                if (iter->get()->GetStrategyStatus()) {
                    iter->get()->start();
                }
            }
        }

    } catch (std::exception& e) {
        LOG(ERROR) << "Start error: " << e.what();
        return -2;
    }
    return 0;
}

int32 cTradingPlatform::reConnect() {
    try {
        LOG(INFO) << "try reConnect!";
        return 0;
    } catch (std::exception& e) {
        LOG(INFO) << "TradingPlatform reconnect failed :" << e.what();
        return -1;
    } catch (...) {
        LOG(INFO) << "TradingPlatform reconnect failed! ";
        return -2;
    }
}

int32 cTradingPlatform::stop() {
    this->ctp_td_spi_.clear();
    this->ctp_md_spi_.clear();
    return 0;
}
