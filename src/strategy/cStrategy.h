#ifndef __CSTRATEGY_H__
#define __CSTRATEGY_H__

#include <map>
#include <thread>
#include <atomic>

#include "ta_libc.h"

#include "cMarketDataCollection.h"
#include "cTradeCollection.h"
#include "cPositionCollection.h"
#include "cOrderCollection.h"

#include "cTraderSpi.h"
#include "cMdSpi.h"

#include "logger.h"
#include "common.h"
#include "global.h"

#include "cObject.h"

class cTraderSpi;

using contextPtr = std::shared_ptr<std::map<std::string, ArrayManager>>;

class cStrategy {
private:
    // non-copyable
    DISALLOW_COPY_AND_ASSIGN(cStrategy);

public:
    cStrategy();
    cStrategy(const string&);
    virtual ~cStrategy();
    virtual void onInit();
    virtual void onStop();
    virtual void onTick(CThostFtdcDepthMarketDataField);
    virtual void onBar(barData);
    virtual void onOrder(cOrderPtr);
    virtual void onTrade(CThostFtdcTradeField);
    virtual void onLoop(contextPtr);

    void buyOpen(std::string inst, double price, double volume, bool stop = false);
    void buyClose(std::string inst, double price, double volume, bool stop = false);
    void sellOpen(std::string inst, double price, double volume, bool stop = false);
    void sellClose(std::string inst, double price, double volume, bool stop = false);

    void cancelOrder(int order_id);
    void cancelAllOrder();

    void start();
    void stop();
    void processStopOrder();

    void RegisterMarketDataCollection(cMarketDataCollectionPtr p);
    void RegisterTradeSpi(std::shared_ptr<cTraderSpi> p);
    void RegisterMdSpi(std::shared_ptr<cMdSpi> p);
    void RegisterPositionCollectionPtr(cPositionCollectionPtr p);
    void RegisterOrderCollectionPtr(cOrderCollectionPtr p);
    void RegisterTradeCollectionPtr(cTradeCollectionPtr p);
    void RegistreInstrumentInfo(std::string);

    bool GetStrategyStatus();
    void makeStopOrder(std::string inst, double price, double vol, traderTag::DIRECTION, traderTag::OFFSETFLAG);
    void subscribe(std::vector<std::string> commodity, long long frequncy, int dataCount, STRATEGY_MODE trade_mode);
    bool update_context();
    bool isTradeTime(std::string);
    std::string getName() { return name_; }
    sTradingAccountInfo getAccount();
    void                showStopOrders();

protected:
    bool mode1(int hourMinTime);
    bool mode2(int hourMinTime);
    bool mode3(int hourMinTime);
    bool mode4(int hourMinTime);
    bool mode5(int hourMinTime);
    bool mode6(int hourMinTime);
    bool mode7(int hourMinTime);
    // base collection
    cMarketDataCollectionPtr marketdata_collection_;

    cPositionCollectionPtr position_collection_;
    cOrderCollectionPtr    order_collection_;
    cTradeCollectionPtr    trade_collection_;

    // stop Order List
    std::vector<cStopOrder>               stop_order_list_;
    std::vector<std::string>              trade_inst_list_;
    std::map<std::string, std::string>    instrument_info_;
    int                                   frequency_;
    int                                   data_length_;
    std::string                                name_;
    int                                   sto_order_id_seq_ = 1;
    contextPtr                            context_ptr_;
    std::chrono::system_clock::time_point the_previous_;

private:
    void                   autoTrader();
    std::thread            inner_thread_;
    std::atomic<bool>      is_running_ = false;
    shared_ptr<cTraderSpi> trader_spi_;
    shared_ptr<cMdSpi>     md_spi_;

    int time_span_ms_;

    STRATEGY_MODE mode_;
};

typedef shared_ptr<cStrategy> cStrategyPtr;

#endif