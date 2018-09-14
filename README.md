# CodeLib #
## 前期准备 ##
* 1 mongoDb:下载地址：https://www.mongodb.com/download-center
* 2 visual studio 2017 64
* 3 mongoDriver 和 boost 当然如果你有心也可以自己编译
* 4 依赖项目-marketDataCollector： https://github.com/drinkmystery/ctpmarketdatacollector.git  该项目作为行情收集
## 运行 ##
* 1 marketDataCollector ： 一般来说不需要管理，主要实时记录行情（频率-1min） 主力合约数据记录如：rb0000
* 2 codelib:内置几个简单的策略。1，2分开主要保证，运行期间数据收集不会出现断档。
* 3 回测：目前采用vnpy进行回测。参考地址：https://github.com/vnpy/vnpy/tree/master
## 策略管理更加简洁 ##
<pre><code
#include "cStrategyKingKeltner.h"
cStrategyKingKeltner::cStrategyKingKeltner(void) : cStrategy() {
    name_ = "KingKeltner";
}
// 策略初始化
void cStrategyKingKeltner::onInit(){
    std::vector<std::string> inst_list;
    inst_list.push_back("rb1901");
    subscribe(inst_list, 60, 100, STRATEGY_MODE::REAL);// 60秒数据频率 数据长度100个 模式：实盘
}
// 策略循环
void cStrategyKingKeltner::onLoop(contextPtr context_ptr){
    std::lock_guard<std::mutex> lock(global::run_mutex);
    double      up, down, dev = 1.6, trailingPrcnt = 0.8;
    int         n = 11, lots = 1;
    std::string trade_inst = "rb1901";
    context_ptr->at(trade_inst).keltner(n, dev, up, down);
    int     count    = context_ptr->at(trade_inst).count();
    barData last_bar = context_ptr->at(trade_inst).lastBarData(--count);
    cancelAllOrder();// 取消前面所有未成交单
    int netPos  = position_collection_->getPosition(trade_inst);// 获取净持仓
    // 核心逻辑
    if (netPos == 0) {
        intra_high_ = last_bar.high;
        intra_low_  = last_bar.low;
        sendOcoOrder(trade_inst, up, down, lots);
    } else if (netPos > 0) {
        intra_high_     = max(last_bar.high, intra_high_);
        intra_low_  = last_bar.low;
        sellClose(trade_inst, intra_high_ * (1 - trailingPrcnt / 100.0), lots, true);//移动止盈止损单
    } else if (netPos < 0) {
        intra_high_     = last_bar.high;
        intra_low_  = min(last_bar.low, intra_low_);
        buyClose(trade_inst, intra_low_ * (1 + trailingPrcnt / 100.0), lots, true);// 移动止盈止损单
    }
    ILOG("NetPos:{},Up:{},Down:{},LastClosePrice:{}.", netPos, up, down, last_bar.close);
    showStopOrders();
}
//   实现区间突破入场
void cStrategyKingKeltner::sendOcoOrder(std::string inst,double  upPrice, double downPrice, int fixedSize) {
    this->buyOpen(inst, upPrice, fixedSize, true);
    this->sellOpen(inst, downPrice, fixedSize, true);
}
void cStrategyKingKeltner::onOrder(cOrderPtr) {}
void cStrategyKingKeltner::onTrade(CThostFtdcTradeField) {}
cStrategyKingKeltner::~cStrategyKingKeltner(void) {}
</code></pre>
