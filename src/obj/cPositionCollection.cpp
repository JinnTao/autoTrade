#include "cPositionCollection.h"
#include "logger.h"

cPositionCollection::cPositionCollection() {
    if (position_map_.size() != 0) {
        position_map_.clear();
    }
}

cPositionCollection::~cPositionCollection() {}

void cPositionCollection::PrintDetail() {
    double closeProfit = 0, positionProfit = 0, commission = 0;

    for_each(position_map_.begin(),
             position_map_.end(),
             [&closeProfit, &positionProfit, &commission](
                 const std::multimap<string, cPositionDetailPtr>::value_type& elem) {
                 if (elem.second->getPosition() != 0) {
                     elem.second->Print();
                 }
                 // elem.second->Print();
                 closeProfit += elem.second->CloseProfit;
                 positionProfit += elem.second->PositionProfit;
                 commission += elem.second->getCommission();
             });
    ILOG("TotalP&L: {},PositionPnl:{},ClosePnl:{},commission:{}",
         closeProfit + positionProfit,
         positionProfit,
         closeProfit,
         commission);
}
void cPositionCollection::update(CThostFtdcInvestorPositionField* pInvestorPosition) {

    bool is_find_position = false;

    string                                              inst(pInvestorPosition->InstrumentID);
    std::multimap<string, cPositionDetailPtr>::iterator pos;
    for (pos = position_map_.lower_bound(inst); pos != position_map_.upper_bound(inst); ++pos) {
        if (this->posDireEqual(pos->second->getPosiDire(), pInvestorPosition->PosiDirection)) {
            is_find_position = true;
            break;
        }
    }
    if (!is_find_position) {
        cPositionDetailPtr position_detail = make_shared<cPositionDetail>(inst);
        pos = position_map_.insert(pair<string, cPositionDetailPtr>(pInvestorPosition->InstrumentID, position_detail));
    }
    pos->second->registerInstField(inst_field_map_->at(inst));
    // update position
    pos->second->update(pInvestorPosition);
}
void cPositionCollection::update(CThostFtdcTradeField* pTrade, shared_ptr<cTrade> pcTrade) {

    bool is_find_position = false;

    string                                              inst(pTrade->InstrumentID);
    std::multimap<string, cPositionDetailPtr>::iterator pos;
    for (pos = position_map_.lower_bound(inst); pos != position_map_.upper_bound(inst); ++pos) {
        // open should find  pos dire equal trade dire
        if (this->posDireEqual(pos->second->getPosiDire(), pTrade->Direction) &&
            pTrade->OffsetFlag == THOST_FTDC_OF_Open) {
            is_find_position = true;
            break;
        }
        // close should find pos dire not equal trade dire
        else if (pTrade->OffsetFlag != THOST_FTDC_OF_Open &&
                 !this->posDireEqual(pos->second->getPosiDire(), pTrade->Direction)) {
            is_find_position = true;
            break;
        }
    }
    if (!is_find_position) {
        cPositionDetailPtr position_detail = make_shared<cPositionDetail>(inst);
        pos = position_map_.insert(pair<string, cPositionDetailPtr>(inst, position_detail));
    }
    pos->second->registerInstField(inst_field_map_->at(inst));
    // update position
    pos->second->update(pTrade, pcTrade);
}
void cPositionCollection::update(CThostFtdcDepthMarketDataField* pDepthMarket) {
    bool is_find_position = false;

    string                                              inst(pDepthMarket->InstrumentID);
    std::multimap<string, cPositionDetailPtr>::iterator pos;
    for (pos = position_map_.lower_bound(inst); pos != position_map_.upper_bound(inst); ++pos) {
        pos->second->update(pDepthMarket);
    }
}
// 依据交易方向获取持仓
int cPositionCollection::getPosition(string inst, DIRE dire) {
    bool                                                is_find_position = false;
    std::multimap<string, cPositionDetailPtr>::iterator pos;
    for (pos = position_map_.lower_bound(inst); pos != position_map_.upper_bound(inst); ++pos) {
        if (pos->second->getPosiDire() == dire) {
            is_find_position = true;
            break;
        }
    }
    if (is_find_position) {
        return pos->second->getPosition();
    } else {
        return 0;
    }
}
// 获取净持仓 定义为多单为正 空单为负
int cPositionCollection::getPosition(string inst) {
    bool                                                is_find_position = false;
    std::multimap<string, cPositionDetailPtr>::iterator pos;
    int                                                 position = 0;
    for (pos = position_map_.lower_bound(inst); pos != position_map_.upper_bound(inst); ++pos) {
        if (pos->second->getPosiDire() == DIRE::AUTO_LONG) {
            position += pos->second->getPosition();
        }
        if (pos->second->getPosiDire() == DIRE::AUTO_SHORT) {
            position -= pos->second->getPosition();
        }
    }
    return position;
}
int cPositionCollection::getYdPosition(string inst, DIRE dire) {
    bool                                                is_find_position = false;
    std::multimap<string, cPositionDetailPtr>::iterator pos;
    for (pos = position_map_.lower_bound(inst); pos != position_map_.upper_bound(inst); ++pos) {
        if (pos->second->getPosiDire() == dire) {
            is_find_position = true;
            break;
        }
    }
    if (is_find_position) {
        return pos->second->getYdPostion();
    } else {
        return 0;
    }
}
int cPositionCollection::getTdPosition(string inst, DIRE dire) {
    bool                                                is_find_position = false;
    std::multimap<string, cPositionDetailPtr>::iterator pos;
    for (pos = position_map_.lower_bound(inst); pos != position_map_.upper_bound(inst); ++pos) {
        if (pos->second->getPosiDire() == dire) {
            is_find_position = true;
            break;
        }
    }
    if (is_find_position) {
        return pos->second->getTodayPosition();
    } else {
        return 0;
    }
}

bool cPositionCollection::posDireEqual(DIRE dire, TThostFtdcPosiDirectionType ftdc_dire) {

    bool is_equal = false;
    // LOG(INFO) << dire << " " << ftdc_dire;
    if (dire == DIRE::AUTO_LONG && (ftdc_dire == THOST_FTDC_PD_Long || ftdc_dire == THOST_FTDC_D_Buy)) {
        is_equal = true;
    }
    if (dire == DIRE::AUTO_SHORT && (ftdc_dire == THOST_FTDC_PD_Short || ftdc_dire == THOST_FTDC_D_Sell)) {
        is_equal = true;
    }
    return is_equal;
}
void cPositionCollection::registerInstFiledMap(cInstrumentFieldMapPtr p) {
    inst_field_map_ = p;
};
// 获取成交过但是并没有在当前持仓列表中的合约
std::list<std::string> cPositionCollection::getTradeButNotPositionInstList() {
    std::list<std::string>                              instList = {};
    std::multimap<string, cPositionDetailPtr>::iterator pos;
    for (pos = position_map_.begin(); pos != position_map_.end(); ++pos) {
        if (this->getPosition(pos->first) == 0) {
            instList.emplace_back(pos->second->GetInstrumentID());
        }
    }
    // unique list
    instList.unique();
    return instList;
}

// hold position pnl
double cPositionCollection::getPositionPnl(string inst, PNL_TAG pnl_tag, DIRE dire) {
    std::multimap<string, cPositionDetailPtr>::iterator pos;
    double                                              pnls          = 0;
    auto calculate_pnl = [&pnl_tag](std::multimap<string, cPositionDetailPtr>::iterator pos) -> double {
        double pnls;
        if (pnl_tag == PNL_TAG::CLOSE_PNL) {
            pnls = pos->second->CloseProfit;
        }
        if (pnl_tag == PNL_TAG::FLOAT_PNL) {
            pnls = pos->second->FloatProfit;
        }
        if (pnl_tag == PNL_TAG::POSI_PNL) {
            pnls = pos->second->PositionProfit;
        }
        return pnls;
    };
    for (pos = position_map_.lower_bound(inst); pos != position_map_.upper_bound(inst); ++pos) {
        if (dire == DIRE::AUTO_UNDEFINE) {
            pnls += calculate_pnl(pos);
        } else {
            if (dire == pos->second->getPosiDire()) {
                pnls += calculate_pnl(pos);
            }
        }
    }
    return pnls;
}
// hold position price 必须指明多单还是空单 否则无法获取持仓价格
double cPositionCollection::getPositionPrc(string inst, PRC_TAG prc_tag,DIRE dire) {
    std::multimap<string, cPositionDetailPtr>::iterator pos;
    double                                              price = 0;
    auto obtain_price = [&prc_tag](std::multimap<string, cPositionDetailPtr>::iterator pos) -> double {
        double price;
        if (prc_tag == PRC_TAG::OPEN_COST) {
            price = pos->second->getPositionPrice();
        }
        if (prc_tag == PRC_TAG::POSI_COST) {
            price = pos->second->getOpenPrice();
        }
        return price;
    };
    for (pos = position_map_.lower_bound(inst); pos != position_map_.upper_bound(inst); ++pos) { 
        if (dire == pos->second->getPosiDire()) {
            price = obtain_price(pos);
        }
    }
    return price;
}
