#include <io.h>
#include <direct.h>

#include "cTradingPlatform.h"
#include "IniFile.h"

using std::string;

cTradingPlatform::cTradingPlatform() {}
cTradingPlatform::~cTradingPlatform() {
    ILOG("Clear trading platform.");
    trade_day_list_.clear();
    strategy_list_.clear();
    subscribe_inst_v_.reset();

    for (auto s : strategy_list_) {
        if (s->GetStrategyStatus()) {
            s->stop();
        }
    }
    if (inter_thread_.joinable()) {
        inter_thread_.join();
    }

}

int32 cTradingPlatform::AutoTrading() {
    std::this_thread::sleep_for(std::chrono::microseconds(1000));
    string str;
    char   dire[50], offset[50], inst[50], price[50], order[50];
    int    vol, mark = 0, orderNo;
    char   tag[10];
    ILOG("--------------------Human-computer interaction function Start--------------------------------");
    ILOG(
        "OrderList: help | show | order| trade | stop | run |close |buy/sell open/close inst vol price| cancel seqNo.");

    while (global::is_running) {
        memset(price, 0, 50);
        memset(order, 0, 50);
        memset(tag, 0, 10);
        vol     = 0;
        orderNo = 0;
        getline(std::cin, str);

        if (str == "show") {
            ctp_td_spi_->showPositionDetail();
        }

        else if (str == "close") {
            cerr << "close Position:" << endl;
            // g_pUserSpi_tradeAll->ForceClose();
        } else if (str == "run") {

            for (auto iter : strategy_list_) {
                iter->start();
            }
        } else if (str == "stop") {
            for (auto iter : strategy_list_) {
                iter->stop();
            }
        } else if (str == "order") {
            this->order_collection_->PrintPendingOrders();
        } else if (str == "trade") {
            this->ctp_td_spi_->ReqQryTrade();
        } else if (str == "help") {
            cerr << "OrderList: show | order| trade | stop | run |close |buy/sell open/close inst vol price -> ";
        } else if (str == "account") {
            this->ctp_td_spi_->ReqQryTradingAccount();
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

void cTradingPlatform::cancleOrder(string order, int seqNo) {
    if (order == "cancel") {

        shared_ptr<cOrder> pOrder = NULL;
        if (seqNo != -1) {
            if (!this->order_collection_->getOrderByNo(seqNo, pOrder)) {
                cerr << "  Order Seq No Exist." << endl;
                return;
            }
            this->ctp_td_spi_->ReqOrderAction(pOrder);
        } else {
            this->ctp_td_spi_->cancleAllPendingOrder();
        }
    }
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
    this->ctp_td_spi_->insertOrder(inst, eDire, eFlag, vol, orderPrice, tag);
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
        return 0;
    } catch (std::exception& e) {
        WLOG("Error:{}", e.what());
        return -1;
    }
}

int32 cTradingPlatform::createPath() {
    try {
        auto createFile = [this](std::string path) -> bool {
            if (_access(path.c_str(), 0) == -1) {
#ifdef WIN32
                int flag = _mkdir(path.c_str());
#endif  // WIN32
#ifdef linux
                int flag = _mkdir(path.c_str(), 0777);  // 0777: the biggest access
#endif                                                  // linux
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
            WLOG("md flow path create failed.");
            return -1;
        }
        if (!createFile(ctpConfig_.td_flow_path_)) {
            WLOG("td flow path create failed.");
            return -2;
        }
        // if (!createFile("logs")) {
        //    WLOG("logs path create failed.");
        //    return -3;
        //}
        return 0;

    } catch (std::exception& e) {
        WLOG("Error:{}.", e.what());
        return -1;
    }
}

int32 cTradingPlatform::init() {
    int32 init_result = 0;
    try {
        // md init
        if (ctp_md_spi_) {
            ctp_md_spi_.reset();
        }
        ctp_md_spi_ = std::make_shared<cMdSpi>();
        init_result = ctp_md_spi_->init(ctpConfig_);
        if (init_result != 0) {
            WLOG("Md init failed! Result:{}", init_result);
            return -1;
        }
        // td init
        if (ctp_td_spi_) {
            ctp_td_spi_.reset();
        }
        ctp_td_spi_ = std::make_shared<cTraderSpi>();
        init_result = ctp_td_spi_->init(ctpConfig_);
        if (init_result != 0) {
            WLOG("Td init failed! Result:{}", init_result);
            return -2;
        }

        // init Position、Orders、Trades、MarketDataEngine
        {
            if (marketdate_collection_) {
                marketdate_collection_.reset();
            }
            marketdate_collection_ = std::make_shared<cMarketDataCollection>();
            ILOG("Market data collection create success!");

            if (position_collection_) {
                position_collection_.reset();
            }
            position_collection_ = std::make_shared<cPositionCollection>();
            ILOG("Position collection create success!");
            if (order_collection_) {
                order_collection_.reset();
            }
            order_collection_ = std::make_shared<cOrderCollection>();
            ILOG("Order collection create success!");
            if (trade_collection_) {
                trade_collection_.reset();
            }
            trade_collection_ = std::make_shared<cTradeCollection>();
            ILOG("Trade collection create success!");
            if (inst_field_map_) {
                inst_field_map_.reset();
            }
            inst_field_map_ = std::make_shared<std::map<std::string, std::shared_ptr<CThostFtdcInstrumentField>>>();
            ILOG("Inst field list create success!");
            if (inst_commission_rate_field_map_) {
                inst_commission_rate_field_map_.reset();
            }
            inst_commission_rate_field_map_ =
                std::make_shared<std::map<std::string, std::shared_ptr<CThostFtdcInstrumentCommissionRateField>>>();
            ILOG("Inst commission rate field map create success!");

            if (subscribe_inst_v_) {
                subscribe_inst_v_.reset();
            }
            subscribe_inst_v_ = std::make_shared<std::vector<std::string>>();
            ILOG("Subscribe inst vector success!");
        }
        // init connection
        {
            position_collection_->registerInstFiledMap(inst_field_map_);

            ctp_md_spi_->RegisterMarketDataCollection(marketdate_collection_.get());
            ctp_md_spi_->RegisterPositionCollection(position_collection_);

            ctp_td_spi_->RegisterMarketDataCollection(marketdate_collection_);
            ctp_td_spi_->RegisterPositionCollection(position_collection_);
            ctp_td_spi_->RegisterOrderCollection(order_collection_);
            ctp_td_spi_->RegisterTradeCollection(trade_collection_);
            ctp_td_spi_->RegisterInstMessageMap(inst_field_map_.get());
            ctp_td_spi_->RegisterInstCommissionMap(inst_commission_rate_field_map_.get());
            ctp_td_spi_->RegisterSubscribeInstList(subscribe_inst_v_);
            ctp_td_spi_->RegisterCtpMdSpi(ctp_md_spi_.get());

            marketdate_collection_->registerMongoSetting(&mongoConfig_);
            // fresh strategy
            for (auto pStrategy : strategy_list_) {
                pStrategy->RegisterMarketDataCollection(marketdate_collection_);
                pStrategy->RegisterTradeSpi(ctp_td_spi_);
                pStrategy->RegisterMdSpi(ctp_md_spi_);
                pStrategy->RegisterPositionCollectionPtr(position_collection_);
                pStrategy->RegisterOrderCollectionPtr(order_collection_);
                pStrategy->RegisterTradeCollectionPtr(trade_collection_);


                this->readDay(string(strategyConfig_.tradeDayDir), trade_day_list_);
                marketdate_collection_->setTradeDayList(&trade_day_list_);
            }
        }
        init_result = ctp_td_spi_->start();
        if (init_result != 0) {
            WLOG("Td start failed! Result:{}", init_result);
            return -3;
        }

        init_result = ctp_md_spi_->start();
        ctp_md_spi_->SubscribeMarketData(this->subscribe_inst_v_);
        ctp_md_spi_->SubscribeMarketData(string("ni1810"));
        if (init_result != 0) {
            WLOG("Md start failed! Result:{}", init_result);
            return -4;
        }

    } catch (std::exception& e) {
        WLOG("Init failed!Msg:{}", e.what());
        return -5;
    }
    return init_result;
}

int32 cTradingPlatform::start() {
    int start_result = 0;
    try {
        // strategy create & init & start
        {
            std::vector<std::string> instList     = this->splitToStr(std::string(strategyConfig_.inst), ",");
            std::vector<int32>       lotsList     = this->splitToInt(std::string(strategyConfig_.lots), ",");
            std::vector<int32>       timeModeList = this->splitToInt(std::string(strategyConfig_.timeMode), ",");
            std::vector<std::string> collectionList =
                this->splitToStr(std::string(strategyConfig_.collectionList), ",");

            if (instList.size() != lotsList.size() || timeModeList.size() != lotsList.size() ||
                collectionList.size() != lotsList.size()) {
                ILOG(" InitStrategy  inst lot timeMode Error,InitListSize:{},timeModeListSize:{},collectionListSize:{}",
                     lotsList.size(),
                     timeModeList.size(),
                     collectionList.size());
                return -3;
            }
            for (int i = 0; i < instList.size(); i++) {
                std::shared_ptr<cStrategyKingKeltner> pStrategy = std::make_shared<cStrategyKingKeltner>();

                pStrategy->RegisterMarketDataCollection(marketdate_collection_);
                pStrategy->RegisterTradeSpi(ctp_td_spi_);
                pStrategy->RegisterMdSpi(ctp_md_spi_);
                pStrategy->RegisterPositionCollectionPtr(position_collection_);
                pStrategy->RegisterOrderCollectionPtr(order_collection_);
                pStrategy->RegisterTradeCollectionPtr(trade_collection_);

                // why do this? I donot konw,maybe just filter carlenday
                this->readDay(string(strategyConfig_.tradeDayDir), trade_day_list_);
                marketdate_collection_->setTradeDayList(&trade_day_list_);

                ctp_td_spi_->RegisterStrategy(pStrategy.get());  // onTrade onOrder

                subscribe_inst_v_->push_back(instList[i]);
                strategy_list_.push_back(pStrategy);
                ILOG("Init strategy inst:{},Lost:{},timeMode:{},Collection:{}",
                     instList[i],
                     lotsList[i],
                     timeModeList[i],
                     collectionList[i]);
            }
        }

        // TradingPlatform start
        {
            // inter thread start
            inter_thread_ = std::thread(&cTradingPlatform::AutoTrading, this);
        }

    } catch (std::exception& e) {
        WLOG("Trader Start error:{}.", e.what());
        return -2;
    }
    return 0;
}

int32 cTradingPlatform::reConnect() {
    try {
        ILOG("try reConnect!");
        auto result = stop();
        result      = init();
        if (result != 0) {
            WLOG("Trader init failed! Result:{}.", result);
            auto result = stop();
            return -3;
        }
        ILOG("Trader init success!");
        // result = start();
        // if (result != 0) {
        //    LOG(ERROR) << "Trader start failed! Result: " << result;
        //    return -4;
        //}
        return 0;
    } catch (std::exception& e) {
        WLOG("TradingPlatform reconnect failed!Msg:{}.", e.what());
        return -1;
    } catch (...) {
        WLOG("TradingPlatform reconnect failed!");
        return -2;
    }
}

int32 cTradingPlatform::stop() {
    // stop
    try {
        // should stop md than stop td?
        if (ctp_md_spi_) {
            this->ctp_md_spi_->stop();
            ctp_md_spi_.reset();
        }
        // clean
        if (ctp_td_spi_) {
            this->ctp_td_spi_->stop();
            ctp_td_spi_.reset();
        }

    } catch (std::exception& e) {
        WLOG("Stop Failed!Msg:{}.", e.what());
    } catch (...) {
        WLOG("stop failed:noknow error.");
    }
    return 0;
}
//字符串分割函数
std::vector<std::string> cTradingPlatform::splitToStr(std::string str, std::string pattern) {
    std::string::size_type   pos;
    std::vector<std::string> result;
    str += pattern;  //扩展字符串以方便操作
    size_t size = str.size();

    for (size_t i = 0; i < size; i++) {
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
    size_t size = str.size();

    for (size_t i = 0; i < size; i++) {
        pos = str.find(pattern, i);
        if (pos < size) {
            std::string s = str.substr(i, pos - i);
            result.push_back(std::atoi(s.c_str()));
            i = pos + pattern.size() - 1;
        }
    }
    return result;
}

void cTradingPlatform::readDay(string fileName, map<string, int>& workDay) {
    ifstream file1(fileName, ios::in);  //以只读方式读入,读取原始数据
    char     dataline[512];             //行数变量
    string   date;
    int      i = 1;
    if (!file1) {
        cout << "Not exist trade day file!" << endl;
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