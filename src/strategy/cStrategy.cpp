#include "cStrategy.h"

cStrategy::cStrategy() {
    m_timeSpan = 500;
}

cStrategy::cStrategy(const string& strategyID) {
    m_timeSpan           = 500;
    this->m_strategyName = strategyID;
}

cStrategy::~cStrategy() {}

void cStrategy::start() {
    if (!this->m_isRuning.load()) {
        this->m_isRuning.store(true, std::memory_order_release);
        m_thread = std::thread(&cStrategy::AutoTrading, this);
        ILOG("Strategy Inst:{},timeMode:{},Lots{},Start.", m_inst, m_timeMode, m_lots);

    } else {
        ILOG("Strategy status: {}.", this->m_isRuning.load());
    }
}

void cStrategy::stop() {
    m_isRuning.store(false, std::memory_order_release);
    if (m_thread.joinable()) {
        m_thread.join();
    }
    ILOG("Strategy Inst:{},timeMode:{},Lots{},Stop.", m_inst, m_timeMode, m_lots);
}

int cStrategy::AutoTrading() {
    init();
    while (this->m_isRuning.load(std::memory_order_relaxed)) {
        this->run();
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(500ms);
    }
    unInit();
    return 0;
}
void cStrategy::init() {
    cerr << this->m_strategyName << " init" << endl;
};

void cStrategy::unInit() {
    cerr << this->m_strategyName << " unInit" << endl;
};

void cStrategy::sendStopOrder(string                inst,
                              traderTag::DIRECTION  inDirection,
                              traderTag::OFFSETFLAG inOffset,
                              double                price,
                              int                   volume,
                              string                strategy,
                              int                   slipNum) {
    cStopOrder order;
    order.instrument   = inst;
    order.direction    = inDirection;
    order.offset       = inOffset;
    order.price        = price;
    order.volume       = volume;
    order.strategyName = strategy;
    order.orderType    = "";
    order.status       = true;
    order.slipTickNum  = slipNum;
    order.orderTime    = std::chrono::system_clock::now();
    // add order to working list
    m_workingStopOrderList.push_back(order);
}

void cStrategy::processStopOrder(string inst, double lastPrice) {
    for (auto var = m_workingStopOrderList.begin(); var != m_workingStopOrderList.end(); var++) {
        if (inst == var->instrument && var->status) {
            bool longTriggered  = var->direction == traderTag::DIRECTION::buy && lastPrice >= var->price;
            bool shortTriggered = var->direction == traderTag::DIRECTION::sell && lastPrice <= var->price;

            if (longTriggered || shortTriggered) {
                // this->m_pTradeSpi->insertOrder(inst, var->direction, var->offset, var->volume, var->price,
                // var->slipTickNum);
                this->m_pTradeSpi->insertOrder(inst, var->direction, var->offset, var->volume, 0);
                var->status = false;
            }
        }
    }
}
bool cStrategy::isTradeTime() {

    struct tm*     timeInfo = getLocalNowTm();
    DateTimeFormat nowTime  = timeInfo->tm_hour * 100 + timeInfo->tm_min;
    if (m_timeMode == 1) {
        return mode1(nowTime);
    }
    if (m_timeMode == 2) {
        return mode2(nowTime);
    }
    if (m_timeMode == 3) {
        return mode3(nowTime);
    }
    if (m_timeMode == 4) {
        return mode4(nowTime);
    }
    if (m_timeMode == 5) {
        return mode5(nowTime);
    }
    return false;
}
bool cStrategy::mode1(DateTimeFormat nowTime) {

    DateTimeFormat s0900 = 900, s1015 = 1015, s1030 = 1030, s1130 = 1130, s1330 = 1330, s1500 = 1500;
    bool           newState;
    if ((nowTime >= s0900 && nowTime < s1015) || (nowTime >= s1030 && nowTime < s1130) ||
        (nowTime >= s1330 && nowTime < s1500)) {

        newState = true;
    } else {

        newState = false;
    }
    return newState;
    return true;
}
bool cStrategy::mode2(DateTimeFormat nowTime) {
    DateTimeFormat s0900 = 900, s1015 = 1015, s1030 = 1030, s1130 = 1130, s1330 = 1330, s1500 = 1500, s2100 = 2100,
                   s2300 = 2300;
    bool newState;
    if ((nowTime >= s0900 && nowTime < s1015) || (nowTime >= s1030 && nowTime < s1130) ||
        (nowTime >= s1330 && nowTime < s1500) || (nowTime >= s2100 && nowTime < s2300)) {

        newState = true;
    } else {

        newState = false;
    }
    return newState;
}
bool cStrategy::mode3(DateTimeFormat nowTime) {
    DateTimeFormat s0900 = 900, s1015 = 1015, s1030 = 1030, s1130 = 1130, s1330 = 1330, s1500 = 1500, s2100 = 2100,
                   s2330 = 2330;
    bool newState;
    if ((nowTime >= s0900 && nowTime < s1015) || (nowTime >= s1030 && nowTime < s1130) ||
        (nowTime >= s1330 && nowTime < s1500) || (nowTime >= s2100 && nowTime < s2330)) {

        newState = true;
    } else {

        newState = false;
    }
    return newState;
}
bool cStrategy::mode4(DateTimeFormat nowTime) {
    DateTimeFormat s0900 = 900, s1015 = 1015, s1030 = 1030, s1130 = 1130, s1330 = 1330, s1500 = 1500, s2100 = 2100,
                   s2359 = 2359, s0000 = 0, s0100 = 100;
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
bool cStrategy::mode5(DateTimeFormat nowTime) {
    DateTimeFormat s0900 = 900, s1015 = 1015, s1030 = 1030, s1130 = 1130, s1330 = 1330, s1500 = 1500, s2100 = 2100,
                   s2359 = 2359, s0000 = 0, s0200 = 200;
    bool newState;
    if ((nowTime >= s0900 && nowTime < s1015) || (nowTime >= s1030 && nowTime < s1130) ||
        (nowTime >= s1330 && nowTime < s1500) || (nowTime >= s2100 && nowTime < s2359) ||
        (nowTime >= s0000 && nowTime < s0200)) {

        newState = true;
    } else {

        newState = false;
    }
    return newState;
}

tm* cStrategy::getLocalNowTm() {
    auto       local_now    = std::chrono::system_clock::now();
    time_t     local_now_tm = std::chrono::system_clock::to_time_t(local_now);
    struct tm* timeInfo     = localtime(&local_now_tm);
    return timeInfo;
}