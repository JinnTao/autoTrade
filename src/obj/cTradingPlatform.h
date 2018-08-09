#ifndef __CTRADINGPLATFORM_H__
#define __CTRADINGPLATFORM_H__
#include <iostream>
#include <string>

#include "cOrderCollection.h"
#include "cTradeCollection.h"
#include "cPositionCollection.h"
#include "cMarketDataCollection.h"

#include "cStrategy.h"
#include "cStrategyTemplate.h"
#include "cStrategyBayes.h"
#include "cStrategyKingKeltner.h"

#include "common.h"
#include "global.h"
#include "logger.h"

class cTradingPlatform {
public:
    cTradingPlatform();
    ~cTradingPlatform();
    //
    /* realtime trading */
    int32 AutoTrading();

    void insertOrder(string inst, string dire, string flag, int vol, double orderPrice, string tag);
    void cancleOrder(string order, int seqNo);
    void readDay(string fileName, map<string, int>& workDay);

    std::vector<std::string> cTradingPlatform::splitToStr(std::string str, std::string pattern);
    std::vector<int32>       cTradingPlatform::splitToInt(std::string str, std::string pattern);

    int32 loadConfig(const string& config_file);
    int32 createPath();
    int32 init();
    int32 start();
    int32 stop();
    int32 reConnect();

private:
    using cStrategyListPtr       = std::list<std::shared_ptr<cStrategyKingKeltner>>;
    using cInstrumentFieldMapPtr = shared_ptr<std::map<std::string, std::shared_ptr<CThostFtdcInstrumentField>>>;
    using cInstrumentCommissionRateFieldMapPtr =
        shared_ptr<std::map<std::string, std::shared_ptr<CThostFtdcInstrumentCommissionRateField>>>;
    using cTraderSpiPtr = std::shared_ptr<cTraderSpi>;
    using cMdSpiPtr     = std::shared_ptr<cMdSpi>;

    ctpConfig      ctpConfig_;
    strategyConfig strategyConfig_;
    mongoConfig    mongoConfig_;

    cTraderSpiPtr ctp_td_spi_;
    cMdSpiPtr     ctp_md_spi_;

    cMarketDataCollectionPtr             marketdate_collection_;
    cPositionCollectionPtr               position_collection_;
    cTradeCollectionPtr                  trade_collection_;
    cOrderCollectionPtr                  order_collection_;
    cStrategyListPtr                     strategy_list_;
    cInstrumentFieldMapPtr               inst_field_map_;
    cInstrumentCommissionRateFieldMapPtr inst_commission_rate_field_map_;
    map<string, int>                     trade_day_list_;
    shared_ptr<vector<string>>           subscribe_inst_v_;


    std::thread inter_thread_;  // for process()
};

#endif
