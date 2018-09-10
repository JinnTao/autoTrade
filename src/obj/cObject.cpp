#include "cObject.h"
#include "logger.h"
// barData
barData::barData() {
    // do something
}
barData::~barData() {
    // do something
}

ArrayManager::ArrayManager(int size = 100) {
    count_  = 0;
    size_   = size;
    inited_ = false;
}
void ArrayManager::update(barData bar) {
    count_++;
    if (!inited_ && count_ >= size_) {
        inited_ = true;
        open_.erase(open_.begin());
        high_.erase(high_.begin());
        low_.erase(low_.begin());
        close_.erase(close_.begin());
        vol_.erase(vol_.begin());
        open_interest_.erase(open_interest_.begin());
        date_time_.erase(date_time_.begin());
    }
    open_.push_back(bar.open);
    high_.push_back(bar.high);
    low_.push_back(bar.low);
    close_.push_back(bar.close);

    vol_.push_back(bar.volume);
    open_interest_.push_back(bar.openInterest);
    date_time_.push_back(bar.date_time);


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
bool ArrayManager::keltner(int n, double dev, double& up, double& down) {
    try {
        double mid = 0, atr = 0;
        int    outBegIdx_SMA[100]    = {};
        int    outNBElement_SMA[100] = {};
        double outReal_SMA[100]      = {};

        int    outBegIdx_ATR[100]    = {};
        int    outNBElement_ATR[100] = {};
        double outReal_ATR[100]      = {};
        // 输出的值 在out_real中的最后一个数据，前提要求输入数据从old到new
        TA_SMA(int(close_.size()) - n, int(close_.size()), &close_[0], n, outBegIdx_SMA, outNBElement_SMA, outReal_SMA);

        TA_ATR(int(close_.size()) - n,
               int(close_.size()),
               &high_[0],
               &low_[0],
               &close_[0],
               n,
               outBegIdx_ATR,
               outNBElement_ATR,
               outReal_ATR);

        up   = outReal_SMA[n - 1] + outReal_ATR[n - 1] * dev;
        down = outReal_SMA[n - 1] - outReal_ATR[n - 1] * dev;
        return true;
    } catch (...) {
        ELOG("keltner calculate error:");
        return false;
    }
}