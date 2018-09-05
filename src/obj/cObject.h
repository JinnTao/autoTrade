#pragma once
#include <string>
#include <chrono>
#include "ta-lib/include/ta_func.h"


class barData {
public:
    barData();
    ~barData();
    std::string collection_symbol;
    std::string symbol;
    std::string exchange;

    double open;
    double high;
    double low;
    double close;

    std::chrono::time_point<std::chrono::system_clock> date_time;  // bar的开始时间 日期
    double                                             volume;
    double                                             openInterest;
};

class ArrayManager {
    //K线序列管理工具，负责：
    //1. K线时间序列的维护
    //2. 常用技术指标的计算


};