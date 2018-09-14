#pragma once
#include <mutex>
#include <atomic>
#include <csignal>

#include "ThostFtdcUserApiDataType.h"

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

namespace global {
extern std::mutex                 init_mutex;
extern std::mutex                 run_mutex;
extern std::atomic<bool>          need_reconnect;
extern volatile std::sig_atomic_t is_running;
extern sTradingAccountInfo        account_info;
}  // namespace global
