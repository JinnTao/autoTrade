#pragma once
#include "cstrategy.h"

class cStrategyKingKeltner :
    public cStrategy
{
private:
    string collection_name_;
    double intra_high_;
    double intra_low_;

public:
    cStrategyKingKeltner(void);
    ~cStrategyKingKeltner(void);
    virtual void onInit();
    virtual void onOrder(cOrderPtr);
    virtual void onTrade(CThostFtdcTradeField);
    virtual void onLoop(contextPtr);
    void sendOcoOrder(std::string inst,double upPrice, double downPrice, int fixedSize);
};

