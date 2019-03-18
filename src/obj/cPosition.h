#ifndef __CPOSITION_H__
#define __CPOSITION_H__
#include <iostream>
#include <map>

#include "ThostFtdcUserApiStruct.h"
#include "cTrade.h"
using namespace std;

enum DIRE { AUTO_LONG, AUTO_SHORT, AUTO_UNDEFINE };
enum PNL_TAG { CLOSE_PNL, POSI_PNL, FLOAT_PNL };
enum PRC_TAG { OPEN_COST, POSI_COST };
using cInstrumentFieldMapPtr = shared_ptr<std::map<std::string, std::shared_ptr<CThostFtdcInstrumentField>>>;
class cPositionDetail {
public:
    cPositionDetail(string inst);
    void update(CThostFtdcInvestorPositionField* pInvestorPositionDetail);
    void update(CThostFtdcTradeField* pTrade, shared_ptr<cTrade> pcTrade);
    void update(CThostFtdcDepthMarketDataField* pDepthMarketData);
    ~cPositionDetail() {}
    //
    /* Get Method */
    int    GetTradeID() const { return m_tradeID; }
    string GetInstrumentID() const { return instrument_id_; }
    int    getPosition() { return position_; }
    int    getTodayPosition() { return today_pos_; }
    int    getYdPostion() { return yd_pos_; }
    DIRE   getPosiDire() { return posi_direction_; }
    double getCommission() { return commission_; }
    void   registerInstField(std::shared_ptr<CThostFtdcInstrumentField> p) { inst_field_ = p; }
    void   Print();
    double CloseProfit    = 0;  //平仓盈亏
    double PositionProfit = 0;  //持仓盈亏
    double FloatProfit    = 0;  // 浮动盈亏 （累计盈亏）
    double Margin         = 0;  //持仓占用保证金
    double getPositionPrice() { return position_price_; }
    double getOpenPrice() { return open_price_; }

private:
    int                                        m_tradeID = 0;
    string                                     instrument_id_;       //合约代码
    double                                     position_price_ = 0;  //持仓成本
    double                                     open_price_     = 0;  //开仓成本
    int                                        position_       = 0;  //总持仓量
    int                                        today_pos_      = 0;  //今日持仓量
    int                                        yd_pos_         = 0;  //多单上日持仓
    string                                     exchange_id_;         // 交易所
    double                                     commission_     = 0;
    double                                     settle_price_   = 0;              // 结算价
    double                                     last_price_     = 0;              // 市场价格
    double                                     margin_rate_    = 0;              // 保证金率
    double                                     open_cost_      = 0;              // 开仓金额
    double                                     position_cost_  = 0;              // 持仓金额
    DIRE                                       posi_direction_ = AUTO_UNDEFINE;  //
    string                                     trade_date_;
    string                                     position_date_;
    string                                     update_time_;
    std::shared_ptr<CThostFtdcInstrumentField> inst_field_;  // 合约信息
};

typedef shared_ptr<cPositionDetail> cPositionDetailPtr;
#endif
