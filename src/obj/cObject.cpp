#include "cObject.h"
#include "logger.h"
// barData
barData::barData() {
    // do something
    date_time = std::chrono::system_clock::now();
}
barData::~barData() {
    // do something
}

ArrayManager::ArrayManager(int size) {
    count_  = 0;
    size_   = size;
    inited_ = false;
}
void ArrayManager::update(barData bar) {
    count_++;
    if (count_ == size_) {
        inited_ = true;
    }
    if (count_ > size_) {
        inited_ = true;
        open_.erase(open_.begin());
        high_.erase(high_.begin());
        low_.erase(low_.begin());
        close_.erase(close_.begin());
        vol_.erase(vol_.begin());
        open_interest_.erase(open_interest_.begin());
        date_time_.erase(date_time_.begin());
        count_--;
    }
    open_.push_back(bar.open);
    high_.push_back(bar.high);
    low_.push_back(bar.low);
    close_.push_back(bar.close);

    vol_.push_back(bar.volume);
    open_interest_.push_back(bar.openInterest);
    date_time_.push_back(bar.date_time);
}
void ArrayManager::fresh(barData bar) {
    open_[count_ - 1]          = bar.open;
    high_[count_ - 1]          = bar.high;
    low_[count_ - 1]           = bar.low;
    close_[count_ - 1]         = bar.close;
    date_time_[count_ - 1]     = bar.date_time;
    vol_[count_ - 1]           = bar.volume;
    open_interest_[count_ - 1] = bar.openInterest;
}
std::vector<double> ArrayManager::high() {
    return high_;
}
std::vector<double> ArrayManager::low() {
    return low_;
}
std::vector<double> ArrayManager::close() {
    return close_;
}
std::vector<double> ArrayManager::open() {
    return open_;
}
int ArrayManager::count() {
    return count_;
}
std::vector<int32> ArrayManager::vol() {
    return vol_;
}
std::vector<int32> ArrayManager::open_interest() {
    return open_interest_;
}
std::vector<std::chrono::system_clock::time_point> ArrayManager::date_time() {
    return date_time_;
}
bool ArrayManager::is_tradable() {
    return is_tradable_;
}
void ArrayManager::setTradable(bool tradable) {
    is_tradable_ = tradable;
}
barData ArrayManager::lastBarData(int index) {
    barData last_bar;
    if (count_ > 0) {
        int tag = (index == -1 ? count_ : index);

        last_bar.close        = (close_[tag - 1]);
        last_bar.open         = (open_[tag - 1]);
        last_bar.high         = (high_[tag - 1]);
        last_bar.low          = (low_[tag - 1]);
        last_bar.volume       = (vol_[tag - 1]);
        last_bar.date_time    = (date_time_[tag - 1]);
        last_bar.openInterest = (open_interest_[tag - 1]);
    }
    return last_bar;
}

bool ArrayManager::keltner(int n, double dev, double& up, double& down) {
    try {
        double mid = 0, atr = 0;
        int    outBegIdx_SMA;
        int    outNBElement_SMA;
        double outReal_SMA[100]      = {};

        int    outBegIdx_ATR;
        int    outNBElement_ATR;
        double outReal_ATR[100]      = {};
        // 输出的值 在out_real中的最后一个数据，前提要求输入数据从old到new
        // 通常close 数组最后一个数据是新bar，不纳入计算指标中
        // Ta学习网址 https://blog.csdn.net/htf15/article/details/9855119
        TA_SMA(0, close_.size()-2, &close_[0],n, &outBegIdx_SMA, &outNBElement_SMA, outReal_SMA);

        TA_ATR(0,close_.size()-2,
               &high_[0],
               &low_[0],
               &close_[0],
               n,
               &outBegIdx_ATR,
               &outNBElement_ATR,
               outReal_ATR);

        up   = outReal_SMA[outNBElement_SMA - 1] + outReal_ATR[outNBElement_ATR - 1] * dev;
        down = outReal_SMA[outNBElement_SMA - 1] - outReal_ATR[outNBElement_ATR - 1] * dev;
        return true;
    } catch (...) {
        ELOG("keltner calculate error:");
        return false;
    }
}
bool ArrayManager::is_inited() {
    return inited_;
}