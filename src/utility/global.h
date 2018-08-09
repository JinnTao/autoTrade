#pragma once
#include <mutex>
#include <atomic>
#include <csignal>

#include "ThostFtdcUserApiDataType.h"

namespace global {
    extern std::mutex init_mutex;
    extern std::mutex run_mutex;
    extern std::atomic<bool> need_reconnect;
    extern volatile std::sig_atomic_t is_running;
}


struct ctpConfig {
    TThostFtdcBrokerIDType   brokerId;
    TThostFtdcInvestorIDType userId;
    char                     passwd[252];

    char mdAddress[50];
    char tdAddress[50];
    char md_flow_path_[50];
    char td_flow_path_[50];
    ctpConfig() { memset(this, 0, sizeof(ctpConfig)); }
    void reset() { memset(this, 0, sizeof(ctpConfig)); }
};

struct mongoConfig {
    char address[50];  // Host address
    char database[50];
    int  mongoPort;     //
    bool mongoLogging;  //

    mongoConfig() { memset(this, 0, sizeof(mongoConfig)); }
    void reset() { memset(this, 0, sizeof(mongoConfig)); }
};

struct strategyConfig {
    char tradeDayDir[125];
    char dataBaseDir[125];

    char startDate[25];
    char endDate[25];

    // dataSource
    char startDateTime[150];
    char endDateTime[150];
    char collectionName[20];

    // kingKeltNer
    //;计算通道中值的窗口数
    double kkLength;  // = 11
    //;计算通道宽度的偏差
    double kkDev;  // = 1.6
    //;移动止损
    double trailingPrcnt;  // = 0.8
    //;每次交易的数量
    double fixedSize;  // = 1
    //;初始化数据所用的天数
    double initDays;  // = 10

    // strategy base Information
    char inst[300];            // strategy Trading Inst
    char lots[300];            // strategy Trading Inst lost
    char timeMode[300];        // strategy Time filter mode
    char collectionList[500];  // strategy collection name

    strategyConfig() { memset(this, 0, sizeof(strategyConfig)); }
    void reset() { memset(this, 0, sizeof(strategyConfig)); }
};

struct sInstrumentInfo {
    char   InstrumentID[32];
    char   ExchangeID[32];
    char   ProductID[32];
    int    MaxMarketOrderVolume;
    int    MinMarketOrderVolume;
    int    MaxLimitOrderVolume;
    int    MinLimitOrderVolume;
    int    VolumeMultiple;
    double PriceTick;
    int    IsTrading;
    double LongMarginRatio;
    double ShortMarginRatio;
};

struct sTradingAccountInfo {
    char   BrokerID[32];
    char   AccountID[32];
    double PreBalance;
    double Balance;
    double Available;
    double WithdrawQuota;
    double CurrMargin;
    double CloseProfit;
    double PositionProfit;
    double Commission;
    double FrozenMargin;
};

// Direction Offset
namespace traderTag {
enum DIRECTION { buy, sell };
enum OFFSETFLAG { open, close };
}  // namespace traderTag