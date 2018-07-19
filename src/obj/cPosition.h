#ifndef __CPOSITION_H__
#define __CPOSITION_H__
#include <iostream>
#include <memory>
#include <ThostFtdcUserApiStruct.h>

using namespace std;

enum DIRE {LONG,SHORT,UNDEFINE};
class cPositionDetail {
public:
    cPositionDetail(string inst);
    void update(CThostFtdcInvestorPositionField* pInvestorPositionDetail);
    void update(CThostFtdcTradeField* pTrade);
    ~cPositionDetail() {}
    //
    /* Get Method */
    int                             GetTradeID() const { return m_tradeID; }
    string                          GetInstrumentID() const { return m_instrumentID; }
    int                             getPosition() { return position_; }
    int                             getTodayPosition() { return today_pos_; }
    int                             getYdPostion() { return yd_pos_; }
    int                             getPosiDire() { return posi_direction_; }
    void                            Print();
    double                          CloseProfit;     //多单平仓盈亏
    double                          PositionProfit;  //多单浮动盈亏
    double                          Margin;          //持仓占用保证金
private:
    int    m_tradeID            = 0;
    string m_instrumentID;            //合约代码
    double m_lastPrice          = 0;  //最新价，时刻保存合约的最新价，平仓用
    double m_PreSettlementPrice = 0;  //上次结算价，对隔夜仓有时候要用，快期有用
    double position_cost_       = 0;  //持仓成本
    double open_cost_           = 0;  //开仓成本
    int    position_            = 0;  //总持仓量
    int    today_pos_           = 0;  //今日持仓量
    int    yd_pos_              = 0;  //多单上日持仓
    DIRE    posi_direction_     = UNDEFINE;  //
    string trade_date_;
};

typedef shared_ptr<cPositionDetail> cPositionDetailPtr;
#endif
