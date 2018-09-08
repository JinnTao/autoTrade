#pragma once
#include <string>
#include <chrono>
#include <vector>
#include "common.h"
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
    int                                             volume;
    int                                             openInterest;
};

class ArrayManager {
    // K线序列管理工具，负责：
    // 1. K线时间序列的维护
    // 2. 常用技术指标的计算
private:
    std::vector<double> high_;   // 最高价序列
    std::vector<double> low_;    // 最低价序列
    std::vector<double> close_;  // 收盘价序列
    std::vector<double> open_;
    std::vector<int32>  vol_;
    std::vector<int32>  open_interest_;//持仓量序列
    std::vector<std::chrono::system_clock::time_point> date_time_; 
    int                                                count_;  //缓存计数
    int                                                size_;   //缓存大小
    bool                                               inited_;
    bool                                               is_tradable_;

public:
    ArrayManager(int size = 100);
    void                                               update(barData bar);
    std::vector<double>                                high();
    std::vector<double>                                low();
    std::vector<double>                                close();
    std::vector<double>                                open();
    std::vector<int32>                                 vol();
    std::vector<int32>                                 open_interest();
    std::vector<std::chrono::system_clock::time_point> date_time();
    bool                                               is_tradable();
    // 技术指标
    bool keltner(int n, double dev, double& up, double& down);


};