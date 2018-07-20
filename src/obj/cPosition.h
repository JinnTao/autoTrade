#ifndef __CPOSITION_H__
#define __CPOSITION_H__
#include <iostream>
#include <ThostFtdcUserApiStruct.h>
#include <map>

using namespace std;

enum DIRE { AUTO_LONG, AUTO_SHORT, AUTO_UNDEFINE };
using cInstrumentFieldMapPtr = shared_ptr<std::map<std::string, std::shared_ptr<CThostFtdcInstrumentField>>>;
class cPositionDetail {
public:
    cPositionDetail(string inst);
    void update(CThostFtdcInvestorPositionField* pInvestorPositionDetail);
    void update(CThostFtdcTradeField* pTrade);
    void update(CThostFtdcDepthMarketDataField* pDepthMarketData);
    ~cPositionDetail() {}
    //
    /* Get Method */
    int    GetTradeID() const { return m_tradeID; }
    string GetInstrumentID() const { return m_instrumentID; }
    int    getPosition() { return position_; }
    int    getTodayPosition() { return today_pos_; }
    int    getYdPostion() { return yd_pos_; }
    DIRE   getPosiDire() { return posi_direction_; }
    void   registerInstField(std::shared_ptr<CThostFtdcInstrumentField> p) { inst_field_ = p; }
    void   Print();
    double CloseProfit;     //多单平仓盈亏
    double PositionProfit;  //多单浮动盈亏
    double Margin;          //持仓占用保证金
private:
    int    m_tradeID = 0;
    string m_instrumentID;            //合约代码
    double m_lastPrice          = 0;  //最新价，时刻保存合约的最新价，平仓用
    double m_PreSettlementPrice = 0;  //上次结算价，对隔夜仓有时候要用，快期有用
    double position_price_      = 0;  //持仓成本
    double open_price_          = 0;  //开仓成本
    int    position_            = 0;  //总持仓量
    int    today_pos_           = 0;  //今日持仓量
    int    yd_pos_              = 0;  //多单上日持仓
    string exchange_id_;              // 交易所
    double commission_     = 0;
    double settle_price_   = 0;              // 结算价
    double last_price_     = 0;              // 市场价格
    double margin_rate_    = 0;              // 保证金率
    DIRE   posi_direction_ = AUTO_UNDEFINE;  //
    string trade_date_;
    string position_date_;
    string update_time_;
    std::shared_ptr<CThostFtdcInstrumentField> inst_field_;
};

typedef shared_ptr<cPositionDetail> cPositionDetailPtr;
#endif
