#ifndef __CPOSITIONCOLLECTION_H__
#define __CPOSITIONCOLLECTION_H__

#include <map>
#include <list>
#include <string>

#include "cPosition.h"
#include "global.h"
// 主要用于实时的持仓量计算，持仓的盈亏计算，因为涉及到持仓的盈亏 手续费 保证金 开仓价格等等 账户汇总信息也在这里处理
class cPositionCollection {
public:
    cPositionCollection();
    ~cPositionCollection();
    void PrintDetail();
    // update position
    void update(CThostFtdcInvestorPositionField* pInvestorPositionDetail);
    void update(CThostFtdcTradeField* pTrade, shared_ptr<cTrade> pcTrade);
    void update(CThostFtdcDepthMarketDataField* pDepthMarket);
    int  getPosition(string instID, DIRE dire);
    // dire undefine,return net position
    int getPosition(string instID);
    int getYdPosition(string instID, DIRE dire);
    int getTdPosition(string instID, DIRE dire);
    // hold position pnl
    double getPositionPnl(string instId, PNL_TAG,DIRE);
    // hold position price
    double getPositionPrc(string instId, PRC_TAG,DIRE);

        bool               posDireEqual(DIRE, TThostFtdcPosiDirectionType);
    void                   registerInstFiledMap(cInstrumentFieldMapPtr p);
    std::list<std::string> getTradeButNotPositionInstList();

	sTradingAccountInfo Trade_account_info() const { return trade_account_info_; }
    void                Trade_account_info(sTradingAccountInfo val) { trade_account_info_ = val; }

protected:
    std::multimap<string, cPositionDetailPtr> position_map_;
    cInstrumentFieldMapPtr                    inst_field_map_;  // record instument field
    sTradingAccountInfo                       trade_account_info_;
};

typedef shared_ptr<cPositionCollection> cPositionCollectionPtr;

#endif
