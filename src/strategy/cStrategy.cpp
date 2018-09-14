#include "cStrategy.h"
#include <future>
#include <functional>
#include "IniFile.h"
#include <regex>

cStrategy::cStrategy() {
    time_span_ms_ = 1000;
    name_         = "undefine_name";
    context_ptr_  = std::make_shared<std::map<std::string, ArrayManager>>();

}

cStrategy::cStrategy(const string& strategyID) {
    time_span_ms_ = 1000;
    name_         = strategyID;
    context_ptr_  = std::make_shared<std::map<std::string, ArrayManager>>();
}

cStrategy::~cStrategy() {}

void cStrategy::start() {
    if (!is_running_) {
        is_running_   = true;
        inner_thread_ = std::thread(&cStrategy::autoTrader, this);
        ILOG("Strategy name:{} start up.", name_);
    } else {
        ILOG("Strategy status: {}.", is_running_);
    }
}

void cStrategy::stop() {
    is_running_ = false;
    if (inner_thread_.joinable()) {
        inner_thread_.join();
    }
    ILOG("Strategy name:{} stop.", name_);
}

void cStrategy::autoTrader() {

    onInit();

    // do loop
    while (is_running_) {

        if (update_context()) {
            this->onLoop(context_ptr_);
        }
        // working stop order
        this->processStopOrder();
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(std::chrono::milliseconds(time_span_ms_));
    }
    onStop();
    return;
}

void cStrategy::processStopOrder() {
    double lastPrice     = 0;
    bool   longTriggered = false, shortTriggered = false;
    for (int i = 0; i < stop_order_list_.size(); i++) {
        if (stop_order_list_[i].status) {
            lastPrice = this->marketdata_collection_->GetMarketDataHandle(stop_order_list_[i].instrument)
                            ->getLastMarketData()
                            .LastPrice;
            longTriggered =
                stop_order_list_[i].direction == traderTag::DIRECTION::buy && lastPrice >= stop_order_list_[i].price;
            shortTriggered =
                stop_order_list_[i].direction == traderTag::DIRECTION::sell && lastPrice <= stop_order_list_[i].price;

            if (longTriggered || shortTriggered) {
                if (mode_ == STRATEGY_MODE::REAL) {
                    this->trader_spi_->insertOrder(stop_order_list_[i].instrument,
                                                   stop_order_list_[i].direction,
                                                   stop_order_list_[i].offset,
                                                   stop_order_list_[i].volume,
                                                   0);
                }
                // 默认认为insertOrder一定成交 删除订单
                stop_order_list_[i].status = false;
                stop_order_list_.erase(stop_order_list_.begin() + i);
                i--;  // becasuse after erase,already point to next element;

            }
        }
    }
}
void cStrategy::RegisterMarketDataCollection(cMarketDataCollectionPtr p) {
    marketdata_collection_ = p;
}
void cStrategy::RegisterTradeSpi(std::shared_ptr<cTraderSpi> p) {
    trader_spi_ = p;
}
void cStrategy::RegisterMdSpi(std::shared_ptr<cMdSpi> p) {
    md_spi_ = p;
}
void cStrategy::RegisterPositionCollectionPtr(cPositionCollectionPtr p) {
    position_collection_ = p;
};
void cStrategy::RegisterOrderCollectionPtr(cOrderCollectionPtr p) {
    order_collection_ = p;
}
void cStrategy::RegisterTradeCollectionPtr(cTradeCollectionPtr p) {
    trade_collection_ = p;
}

bool cStrategy::GetStrategyStatus() {
    return is_running_;
}

void cStrategy::onLoop(contextPtr) {
    ILOG("Strategy name:{} onLoop.", name_);
}
void cStrategy::makeStopOrder(std::string           inst,
                              double                price,
                              double                vol,
                              traderTag::DIRECTION  dire,
                              traderTag::OFFSETFLAG offset) {
    cStopOrder order;
    order.instrument   = inst;
    order.direction    = dire;
    order.offset       = offset;
    order.price        = price;
    order.volume       = vol;
    order.strategyName = name_;
    order.orderType    = "";
    order.status       = true;
    order.slipTickNum  = 1;
    order.orderTime    = std::chrono::system_clock::now();
    order.order_id_    = sto_order_id_seq_;
    // add order to working list
    stop_order_list_.push_back(order);
    sto_order_id_seq_++;
}
void cStrategy::buyOpen(std::string inst, double price, double volume, bool stop) {
    if (stop == true) {
        makeStopOrder(inst, price, volume, traderTag::DIRECTION::buy, traderTag::OFFSETFLAG::open);
    } else {
        if (mode_ = STRATEGY_MODE::REAL) {
            this->trader_spi_->insertOrder(inst, traderTag::buy, traderTag::open, volume, price);
        }
    }
}
void cStrategy::buyClose(std::string inst, double price, double volume, bool stop) {
    if (stop == true) {
        makeStopOrder(inst, price, volume, traderTag::DIRECTION::buy, traderTag::OFFSETFLAG::close);
    } else {
        if (mode_ = STRATEGY_MODE::REAL) {
            this->trader_spi_->insertOrder(inst, traderTag::buy, traderTag::close, volume, price);
        }
    }
}
void cStrategy::sellOpen(std::string inst, double price, double volume, bool stop) {
    if (stop == true) {
        makeStopOrder(inst, price, volume, traderTag::DIRECTION::sell, traderTag::OFFSETFLAG::open);
    } else {
        if (mode_ = STRATEGY_MODE::REAL) {
            this->trader_spi_->insertOrder(inst, traderTag::sell, traderTag::open, volume, price);
        }
    }
}
void cStrategy::sellClose(std::string inst, double price, double volume, bool stop) {
    if (stop == true) {
        makeStopOrder(inst, price, volume, traderTag::DIRECTION::sell, traderTag::OFFSETFLAG::close);
    } else {
        if (mode_ = STRATEGY_MODE::REAL) {
            this->trader_spi_->insertOrder(inst, traderTag::sell, traderTag::close, volume, price);
        }
    }
}

void cStrategy::onInit() {}
void cStrategy::onStop() {}
void cStrategy::onTick(CThostFtdcDepthMarketDataField) {}
void cStrategy::onBar(barData) {}
void cStrategy::onOrder(cOrderPtr) {}
void cStrategy::onTrade(CThostFtdcTradeField trade) {
}
void cStrategy::cancelOrder(int order_id) {
    trader_spi_->cancelOrderById(order_id);
}
void cStrategy::cancelAllOrder() {

    stop_order_list_.clear();

    trader_spi_->cancleMyPendingOrder();
}

void cStrategy::subscribe(std::vector<std::string> commodity_list,
                         long long                frequency,
                         int                      data_length,
                         STRATEGY_MODE            trade_mode) {
    // 防止多策略冲突
    std::lock_guard<std::mutex> lock(global::init_mutex);

    mode_            = trade_mode;
    trade_inst_list_ = commodity_list;
    frequency_       = frequency;
    data_length_     = data_length;
    the_previous_    = std::chrono::time_point_cast<std::chrono::minutes>(std::chrono::system_clock::now());



    if (trade_inst_list_.size() != 0) {
        this->md_spi_->SubscribeMarketData(std::make_shared<vector<string>>(trade_inst_list_));
    } else {
        ILOG("Please put trade instrument to trade_inst_list_");
        return;
    }
    for (auto inst = trade_inst_list_.begin(); inst != trade_inst_list_.end(); inst++) {
        ArrayManager         am(data_length_);
        std::vector<barData> bar_data_vec;
        marketdata_collection_->loadHistoryFromMongo(*inst, data_length_, bar_data_vec);
        for (auto iter : bar_data_vec) {
            am.update(iter);
        }
        context_ptr_->insert(std::make_pair(*inst, am));
    }
}

bool cStrategy::update_context() {
    if (context_ptr_->size() == 0) {
        ILOG("Please SubScribe Inst && Initail inst trade list");
    }
    
    if (mode_ == STRATEGY_MODE::REAL) {
        auto local_now     = std::chrono::system_clock::now();
        auto duration_time = std::chrono::duration_cast<std::chrono::seconds>(local_now - the_previous_);
        bool mark          = false;
        if (duration_time.count() % frequency_ == 0) {
            for (auto& context : *context_ptr_) {
                std::string inst_name = context.first;
                if (marketdata_collection_->GetMarketDataHandle(inst_name) && isTradeTime(inst_name)) {
                    mark = true;
                    the_previous_ = local_now;
                    context.second.setTradable(true);
                    CThostFtdcDepthMarketDataField lastData =
                        marketdata_collection_->GetMarketDataHandle(inst_name)->getLastMarketData();
                    barData bar_data;
                    bar_data.high              = lastData.LastPrice;
                    bar_data.low               = lastData.LastPrice;
                    bar_data.close             = lastData.LastPrice;
                    bar_data.open              = lastData.LastPrice;
                    bar_data.volume            = lastData.Volume;
                    bar_data.collection_symbol = inst_name;
                    bar_data.exchange          = lastData.ExchangeID;
                    bar_data.openInterest      = lastData.OpenInterest;
                    context.second.update(bar_data);
                    
                } else {
                    context.second.setTradable(false);
                }
            }
        } else {
            for (auto& context : *context_ptr_) {
                std::string inst_name = context.first;
                if (marketdata_collection_->GetMarketDataHandle(inst_name) && isTradeTime(inst_name)) {
                    context.second.setTradable(true);
                    CThostFtdcDepthMarketDataField lastData =
                        marketdata_collection_->GetMarketDataHandle(inst_name)->getLastMarketData();
                    barData previous_bar = context.second.lastBarData();
                    barData bar_data;
                    bar_data.high         = max(lastData.LastPrice, previous_bar.high);
                    bar_data.low          = min(lastData.LastPrice, previous_bar.low);
                    bar_data.open         = previous_bar.open;
                    bar_data.volume       = lastData.Volume - previous_bar.volume;
                    bar_data.close        = lastData.LastPrice;
                    bar_data.exchange     = previous_bar.exchange;
                    bar_data.openInterest = lastData.OpenInterest;
                    bar_data.date_time    = std::chrono::system_clock::now();
                    context.second.fresh(bar_data);
                } else {
                    context.second.setTradable(false);
                }
            }
        }
        return mark;
    } else {
        return false;
    }
}

bool cStrategy::isTradeTime(std::string inst) {
    auto       local_now    = std::chrono::system_clock::now();
    time_t     local_now_tm = std::chrono::system_clock::to_time_t(local_now);
    struct tm* timeInfo     = localtime(&local_now_tm);

    int nowTime   = timeInfo->tm_hour * 100 + timeInfo->tm_min;
    std::string time_mode;
    std::smatch m;
    bool        found = std::regex_search(inst, m, std::regex("^[a-zA-Z]{1,4}"));
    if (found){
        time_mode = instrument_info_[m.str()];
    }else{
        ELOG("inst {} inValid.", inst);
        return false;
    }
    if (time_mode == "mode1") {
        return mode1(nowTime);
    }
    if (time_mode == "mode2") {
        return mode2(nowTime);
    }
    if (time_mode == "mode3") {
        return mode3(nowTime);
    }
    if (time_mode == "mode4") {
        return mode4(nowTime);
    }
    if (time_mode == "mode5") {
        return mode5(nowTime);
    }
    if (time_mode == "mode6") {
        return mode5(nowTime);
    }
    if (time_mode == "mode7") {
        return mode5(nowTime);
    }
    return false;
}
bool cStrategy::mode1(int nowTime) {

    int  s0900 = 900, s1015 = 1015, s1030 = 1030, s1130 = 1130, s1330 = 1330, s1500 = 1500;
    bool newState;
    if ((nowTime >= s0900 && nowTime < s1015) || (nowTime >= s1030 && nowTime < s1130) ||
        (nowTime >= s1330 && nowTime < s1500)) {

        newState = true;
    } else {

        newState = false;
    }
    return newState;
    return true;
}
bool cStrategy::mode2(int nowTime) {
    int  s0900 = 900, s1015 = 1015, s1030 = 1030, s1130 = 1130, s1330 = 1330, s1500 = 1500, s2100 = 2100, s2300 = 2300;
    bool newState;
    if ((nowTime >= s0900 && nowTime < s1015) || (nowTime >= s1030 && nowTime < s1130) ||
        (nowTime >= s1330 && nowTime < s1500) || (nowTime >= s2100 && nowTime < s2300)) {

        newState = true;
    } else {

        newState = false;
    }
    return newState;
}
bool cStrategy::mode3(int nowTime) {
    int  s0900 = 900, s1015 = 1015, s1030 = 1030, s1130 = 1130, s1330 = 1330, s1500 = 1500, s2100 = 2100, s2330 = 2330;
    bool newState;
    if ((nowTime >= s0900 && nowTime < s1015) || (nowTime >= s1030 && nowTime < s1130) ||
        (nowTime >= s1330 && nowTime < s1500) || (nowTime >= s2100 && nowTime < s2330)) {

        newState = true;
    } else {

        newState = false;
    }
    return newState;
}
bool cStrategy::mode4(int nowTime) {
    int s0900 = 900, s1015 = 1015, s1030 = 1030, s1130 = 1130, s1330 = 1330, s1500 = 1500, s2100 = 2100, s2359 = 2359,
        s0000 = 0, s0100 = 100;
    bool newState;
    if ((nowTime >= s0900 && nowTime < s1015) || (nowTime >= s1030 && nowTime < s1130) ||
        (nowTime >= s1330 && nowTime < s1500) || (nowTime >= s2100 && nowTime < s2359) ||
        (nowTime >= s0000 && nowTime < s0100)) {

        newState = true;
    } else {

        newState = false;
    }
    return newState;
}
bool cStrategy::mode5(int nowTime) {
    int s0900 = 900, s1015 = 1015, s1030 = 1030, s1130 = 1130, s1330 = 1330, s1500 = 1500, s2100 = 2100, s2359 = 2359,
        s0000 = 0, s0230 = 230;
    bool newState;
    if ((nowTime >= s0900 && nowTime < s1015) || (nowTime >= s1030 && nowTime < s1130) ||
        (nowTime >= s1330 && nowTime < s1500) || (nowTime >= s2100 && nowTime < s2359) ||
        (nowTime >= s0000 && nowTime < s0230)) {

        newState = true;
    } else {

        newState = false;
    }
    return newState;
}
bool cStrategy::mode6(int nowTime) {
    int  s0900 = 900,s1130 = 1130, s1300 = 1300, s1500 = 1500;
    bool newState;
    if ((nowTime >= s0900 && nowTime < s1130) || (nowTime >= s1300 && nowTime < s1500)) {

        newState = true;
    } else {

        newState = false;
    }
    return newState;
}
bool cStrategy::mode7(int nowTime) {
    int  s0915 = 915, s1130 = 1130, s1300 = 1300, s1515 = 1515;
    bool newState;
    if ((nowTime >= s0915 && nowTime < s1130) || (nowTime >= s1300 && nowTime < s1515)) {
        newState = true;
    } else {

        newState = false;
    }
    return newState;
}

void cStrategy::showStopOrders() {
    for each (auto var in stop_order_list_)
    {
        if(var.status){
            time_t orderTimeT = std::chrono::system_clock::to_time_t(var.orderTime);

            struct tm* ptm = localtime(&orderTimeT);
            char date[60] = { 0 };
            sprintf(date, "%d-%02d-%02d %02d:%02d:%02d",
                (int)ptm->tm_year + 1900, (int)ptm->tm_mon + 1, (int)ptm->tm_mday,
                (int)ptm->tm_hour, (int)ptm->tm_min, (int)ptm->tm_sec);
            string orderDateTime = string(date);
            ILOG("{} {} stop order {} {} {} {} {} {}.",
                 orderDateTime,
                 var.instrument,
                 ((var.direction == traderTag::DIRECTION::buy) ? "buy" : "sell"),
                 ((var.offset == traderTag::OFFSETFLAG::close) ? "close " : "open "),
                 var.price,
                 var.volume,
                 var.slipTickNum,
                 var.strategyName);

        }
    }
}

sTradingAccountInfo cStrategy::getAccount(){
    std::promise<bool>  obtain_account;
    std::future<bool>   is_obtain_account = obtain_account.get_future();

    sTradingAccountInfo the_last_account;
    memcpy(&the_last_account, trader_spi_->GetTradingAccountInfo(), sizeof sTradingAccountInfo);
    auto                obtain_account_info_func = [&the_last_account, &obtain_account](sTradingAccountInfo account) { 
        memcpy(&the_last_account, &account, sizeof(sTradingAccountInfo));
        obtain_account.set_value(true);
    };
    trader_spi_->ReqQryTradingAccount(false, obtain_account_info_func);
    if (is_obtain_account.valid()){
        auto wait_result = is_obtain_account.wait_for(std::chrono::seconds(1));
        if (wait_result != std::future_status::ready || is_obtain_account.get()!=true){
            WLOG("obtain account faild");
            return the_last_account;
        }
    }
    return the_last_account;
      
}

void cStrategy::RegistreInstrumentInfo(std::string file_dir) {
    try{
        // clean
        instrument_info_.clear();

        string  mode[] = {"mode1", "mode2", "mode3", "mode4", "mode5", "mode6", "mode7"};
        IniFile ini(file_dir);
        for (auto val : mode){
            std::string inst_list = ini.ReadString("InstrumentInfo", val, "1");
            // const char* convert to char*
            char* strc = new char[strlen(inst_list.c_str()) + 1];
            char* buff = new char[strlen(inst_list.c_str()) + 1];
            strcpy(strc, inst_list.c_str());
            std::string pattern = ",";
            char*       tmpStr  = strtok_s(strc, pattern.c_str(), &buff);
            while (tmpStr != NULL) {
                instrument_info_.insert(std::make_pair(std::string(tmpStr), val));
                tmpStr = strtok_s(buff, pattern.c_str(), &buff);
            }

            delete[] strc;
        }
        return;
    
    }catch(std::exception e){
        ELOG("RegistreInstrumentInfo error:{}", e.what());
    }

}