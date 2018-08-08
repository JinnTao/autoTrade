#include "cStrategyKingKeltner.h"
#include "easylogging\easylogging++.h"
#include "global.h"
cStrategyKingKeltner::cStrategyKingKeltner(void)
{
    m_candleMinute = -1;
    m_lastOpen = -1;
    m_lastHigh = -1;
    m_lastLow = -1;
    m_lastClose = -1;
    m_lastVolume = -1;
    m_strategyName = "KingKeltner";
    cStrategy::cStrategy();
    m_netPos = 0;
    m_oldState = false;
}


cStrategyKingKeltner::~cStrategyKingKeltner(void)
{

}

void cStrategyKingKeltner::init(){
    std::lock_guard<std::mutex> lock(global::init_mutex);
    if (m_close.size() == 0) {
        // Start Time 
        //this->m_marketData->loadSeriesHistory(m_oneMinuteDataDir, m_startDate, m_endDate, m_open, m_high, m_low, m_close, m_volume);
        this->m_marketData->loadHistoryFromMongo(m_collectionName,m_pAutoSetting->startDateTime,m_pAutoSetting->endDateTime, m_open, m_high, m_low, m_close, m_volume);
    }
    this->m_pMdSpi->SubscribeMarketData(m_inst);// trade 1801

}

void cStrategyKingKeltner::unInit(){



}

void cStrategyKingKeltner::run(){
    std::lock_guard<std::mutex> lock(global::run_mutex);
    if (this->m_marketData->GetMarketDataHandle(m_inst) && isTradeTime()) {
        CThostFtdcDepthMarketDataField lastData = this->m_marketData->GetMarketDataHandle(m_inst)->getLastMarketData();
        int                            tickMinute = 1;  // cDateTime(cSystem::GetCurrentTimeBuffer().c_str()).Minute();
        // new Candle
        if (tickMinute != m_candleMinute) {
            if (m_candleMinute != -1) {
                //add
                m_open.push_back(m_lastOpen);
                m_high.push_back(m_lastHigh);
                m_low.push_back(m_lastLow);
                m_close.push_back(m_lastClose);
                m_volume.push_back(m_lastVolume);
                //erase
                m_open.erase(m_open.begin());
                m_high.erase(m_high.begin());
                m_low.erase(m_low.begin());
                m_close.erase(m_close.begin());
                m_volume.erase(m_volume.begin());
            }
            m_lastOpen = *(m_open.end()-1);
            m_lastHigh = *(m_high.end()-1);
            m_lastLow = *(m_low.end()-1);
            m_lastClose = *(m_close.end()-1);
            m_lastVolume = *(m_volume.end()-1);
            on1MBar();
            // latest bar data
            m_lastOpen = lastData.LastPrice;
            m_lastHigh = lastData.LastPrice;
            m_lastLow = lastData.LastPrice;
            m_lastClose = lastData.LastPrice;
            m_lastVolume = lastData.Volume;
            m_candleMinute = tickMinute;// update current candle Minute

        }
        else {
            // update bar data
            m_lastHigh = 
                
                
                (m_lastHigh, lastData.LastPrice);

            m_lastLow = min(m_lastLow, lastData.LastPrice);

            m_lastClose = lastData.LastPrice;

            m_lastVolume += lastData.Volume;

        }
        // 处理停止单
        this->processStopOrder(m_inst, m_lastClose);
    }else{
        auto tick = this->m_marketData->GetMarketDataHandle(m_inst)->getLastMarketData();
        LOG(INFO) << m_inst << " cStrategyKingKeltner "
                  << " wait: " << m_lots << " last Price" << tick.LastPrice << " updateTime: " << tick.LastPrice;

    }
}

void cStrategyKingKeltner::sendOcoOrder(double upPrice, double downPrice, int fixedSize) {
    
    //
    //    发送OCO委托

    //    OCO(One Cancel Other)委托：
    //    1. 主要用于实现区间突破入场
    //    2. 包含两个方向相反的停止单
    //    3. 一个方向的停止单成交后会立即撤消另一个方向的
    //
    this->sendStopOrder(
        m_inst, traderTag::DIRECTION::buy, traderTag::OFFSETFLAG::open, upPrice, fixedSize, this->m_strategyName);
    this->sendStopOrder(
        m_inst, traderTag::DIRECTION::sell, traderTag::OFFSETFLAG::open, downPrice, fixedSize, this->m_strategyName);

}

void cStrategyKingKeltner::on1MBar(){
    // =================================================================  指标计算 =================================================
    double up, down;
    if (!keltner(int(this->m_pAutoSetting->kkLength), this->m_pAutoSetting->kkDev, up, down)) {
        return;
    }

    
    //=============================================================取消前面所有未成交单 ==============================================
    this->m_pTradeSpi->cancleMyPendingOrder();
    this->m_workingStopOrderList.clear();
    // ===========================================================下单逻辑============================================================

    int longPos = this->m_pPositionC.get()->getPosition(m_inst,DIRE::AUTO_LONG);
    int shortPos = this->m_pPositionC.get()->getPosition(m_inst,DIRE::AUTO_SHORT);

    this->m_netPos = longPos - shortPos;

    if (m_netPos == 0) {
        this->m_intraTradeHigh = m_lastHigh;
        this->m_intraTradeLow = m_lastLow;
        // according strategy config set trading lots
        this->sendOcoOrder(up, down, int(m_lots));

    }
    else if (m_netPos > 0) {
        m_intraTradeHigh = max(m_lastHigh, m_intraTradeHigh);
        m_intraTradeLow = m_lastLow;
        this->sendStopOrder(m_inst,
                            traderTag::DIRECTION::sell,
                            traderTag::OFFSETFLAG::close,
                            m_intraTradeHigh * (1 - m_pAutoSetting->trailingPrcnt / 100.0),
                            UINT(std::abs(m_netPos)),
                            this->m_strategyName);
        
    }
    else if (m_netPos < 0) {
        m_intraTradeHigh = m_lastHigh;
        m_intraTradeLow = min(m_lastLow, m_intraTradeLow);
        this->sendStopOrder(m_inst,
                            traderTag::DIRECTION::buy,
                            traderTag::OFFSETFLAG::close,
                            m_intraTradeLow * (1 + m_pAutoSetting->trailingPrcnt / 100.0),
                            UINT(std::abs(m_netPos)),
                            this->m_strategyName);
    }

    // ==============================================================日志输出========================================================
    //double rsiValue = outReal[0];
    LOG(INFO)  << " netPos " << m_netPos << " up: " << up << " down: " << down << " lastPrice " << m_lastHigh << endl;
    printStatus();

}



bool cStrategyKingKeltner::keltner( int kkLength, double kkDev, double& kkUp,double &kkDown) {
    try {
        double mid = 0, atr = 0;
        int outBegIdx_SMA[100] = {};
        int outNBElement_SMA[100] = {};
        double outReal_SMA[100] = {};

        int outBegIdx_ATR[100] = {};
        int outNBElement_ATR[100] = {};
        double outReal_ATR[100] = {};
        // 输出的值 在out_real中的最后一个数据中，前提要求输入数据从old到new
        TA_SMA(m_close.size() - kkLength, m_close.size(), &m_close[0], kkLength, outBegIdx_SMA, outNBElement_SMA, outReal_SMA);

        TA_ATR(m_close.size() - kkLength, m_close.size(), &m_high[0], &m_low[0], &m_close[0], kkLength, outBegIdx_ATR, outNBElement_ATR, outReal_ATR);

        kkUp = outReal_SMA[kkLength - 1] + outReal_ATR[kkLength - 1] * kkDev;

        kkDown = outReal_SMA[kkLength - 1] - outReal_ATR[kkLength - 1] * kkDev;
        return true;
    }
    catch (...) {
        
        return false;
    
    }

}

void cStrategyKingKeltner::onTrade(CThostFtdcTradeField pTrade) {
    
    if (strcmp(pTrade.InstrumentID, m_inst.c_str()) != 0) {
        //LOG(INFO) << "Not " << m_inst << " on trade";
    }
    else{
        LOG(INFO) << this->m_strategyName << " onTrade " << endl;
        if (m_netPos != 0) {
            if (m_netPos > 0) {
                for (auto i = m_workingStopOrderList.begin();i != m_workingStopOrderList.end();i++) {

                    if (i->instrument == m_inst && i->direction == traderTag::DIRECTION::sell) {
                        i->status = false;
                        LOG(INFO) << " cancle sell "
                                  << ((i->offset == traderTag::OFFSETFLAG::close) ? " close  " : " open ")
                                  << " stop order" << endl;
                    }
                }
            }
            if (m_netPos < 0) {
                for (auto i = m_workingStopOrderList.begin();i != m_workingStopOrderList.end();i++) {
                    if (i->instrument == m_inst && i->direction == traderTag::DIRECTION::buy) {
                        i->status = false;
                        LOG(INFO) << " cancle buy "
                                  << ((i->offset == traderTag::OFFSETFLAG::close) ? " close  " : " open ")
                                  << " stop order" << endl;
                    }
                }
            }
        }
    }

}

void cStrategyKingKeltner::printStatus() {
    for each (auto var in m_workingStopOrderList)
    {
        if(var.status){
            time_t orderTimeT = std::chrono::system_clock::to_time_t(var.orderTime);

            struct tm* ptm = localtime(&orderTimeT);
            char date[60] = { 0 };
            sprintf(date, "%d-%02d-%02d %02d:%02d:%02d",
                (int)ptm->tm_year + 1900, (int)ptm->tm_mon + 1, (int)ptm->tm_mday,
                (int)ptm->tm_hour, (int)ptm->tm_min, (int)ptm->tm_sec);
            string orderDateTime = string(date);
            LOG(INFO) << orderDateTime << " " << var.instrument << " stop order "
                      << ((var.direction == traderTag::DIRECTION::buy) ? "buy" : "sell") << " "
                      << ((var.offset == traderTag::OFFSETFLAG::close) ? "close " : "open ") << var.price << " "
                      << var.volume
                      << " " << var.slipTickNum << " " << var.strategyName << endl;

        }

    }
}

void cStrategyKingKeltner::setInst(string inst) {
    this->m_inst = inst;
}



