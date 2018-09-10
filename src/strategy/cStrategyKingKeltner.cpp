#include "cStrategyKingKeltner.h"

cStrategyKingKeltner::cStrategyKingKeltner(void) : cStrategy() {
    name_ = "KingKeltner";
}
cStrategyKingKeltner::~cStrategyKingKeltner(void){}

void cStrategyKingKeltner::onInit(){
    std::vector<std::string> inst_list;
    inst_list.push_back("rb1901");
    subcribe(inst_list, 60, 100, STRATEGY_MODE::REAL);
}
void cStrategyKingKeltner::onLoop(contextPtr context_ptr){
    // =================================================================  指标计算
    double up, down;
    if (!keltner(int(this->m_pAutoSetting->kkLength), this->m_pAutoSetting->kkDev, up, down)) {
        return;
    }
    //=============================================================取消前面所有未成交单
    this->m_pTradeSpi->cancleMyPendingOrder();
    this->m_workingStopOrderList.clear();
    // ===========================================================下单逻辑============================================================

    int longPos  = this->m_pPositionC.get()->getPosition(m_inst, DIRE::AUTO_LONG);
    int shortPos = this->m_pPositionC.get()->getPosition(m_inst, DIRE::AUTO_SHORT);

    this->m_netPos = longPos - shortPos;

    if (m_netPos == 0) {
        this->m_intraTradeHigh = m_lastHigh;
        this->m_intraTradeLow  = m_lastLow;
        // according strategy config set trading lots
        this->sendOcoOrder(up, down, int(m_lots));

    } else if (m_netPos > 0) {
        m_intraTradeHigh = max(m_lastHigh, m_intraTradeHigh);
        m_intraTradeLow  = m_lastLow;
        this->sendStopOrder(m_inst,
                            traderTag::DIRECTION::sell,
                            traderTag::OFFSETFLAG::close,
                            m_intraTradeHigh * (1 - m_pAutoSetting->trailingPrcnt / 100.0),
                            UINT(std::abs(m_netPos)),
                            this->m_strategyName);

    } else if (m_netPos < 0) {
        m_intraTradeHigh = m_lastHigh;
        m_intraTradeLow  = min(m_lastLow, m_intraTradeLow);
        this->sendStopOrder(m_inst,
                            traderTag::DIRECTION::buy,
                            traderTag::OFFSETFLAG::close,
                            m_intraTradeLow * (1 + m_pAutoSetting->trailingPrcnt / 100.0),
                            UINT(std::abs(m_netPos)),
                            this->m_strategyName);
    }
    // ==============================================================日志输出========================================================
    // double rsiValue = outReal[0];
    ILOG("NetPos:{},Up:{},Down:{},LastClosePrice:{}.", m_netPos, up, down, m_lastClose);
    printStatus();
}

void cStrategyKingKeltner::sendOcoOrder(std::string inst,double  upPrice, double downPrice, int fixedSize) {
    //
    //    发送OCO委托
    //    OCO(One Cancel Other)委托：
    //    1. 主要用于实现区间突破入场
    //    2. 包含两个方向相反的停止单
    //    3. 一个方向的停止单成交后会立即撤消另一个方向的
    this->buyOpen(inst, upPrice, fixedSize, true);
    this->sellOpen(inst, downPrice, fixedSize, true);
}

void cStrategyKingKeltner::printStatus() {
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



