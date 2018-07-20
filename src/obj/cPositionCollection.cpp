#include <cPositionCollection.h>
#include <cVector.h>
#include <cTrade.h>
#include <easylogging++.h>

#ifndef _DEBUG
#define _DEBUG 0
#endif
cPositionCollection::cPositionCollection() {
    if (position_map_.size() != 0) {
        position_map_.clear();
    }


}

cPositionCollection::~cPositionCollection() {}

void cPositionCollection::PrintDetail() {
    double closeProfit = 0, positionProfit = 0;

    for_each(position_map_.begin(),
             position_map_.end(),
             [&closeProfit, &positionProfit](const std::multimap<string, cPositionDetailPtr>::value_type& elem) {
                 if (elem.second->getPosition() != 0) {
                     elem.second->Print();
                 }
                 closeProfit += elem.second->CloseProfit;
                 positionProfit += elem.second->PositionProfit;
             });
    LOG(INFO) << "TotalP&L: " << closeProfit + positionProfit << " positionP&L: " << positionProfit
              << " closeP&L: " << closeProfit;
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
void cPositionCollection::update(CThostFtdcTradeField* pTrade) {

    bool is_find_position = false;

    string                                              inst(pTrade->InstrumentID);
    std::multimap<string, cPositionDetailPtr>::iterator pos;
    for (pos = position_map_.lower_bound(inst); pos != position_map_.upper_bound(inst); ++pos) {
        if (this->posDireEqual(pos->second->getPosiDire(), pTrade->Direction)) {
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
    pos->second->update(pTrade);
}
void cPositionCollection::update(CThostFtdcDepthMarketDataField* pDepthMarket){
    bool is_find_position = false;

    string                                              inst(pDepthMarket->InstrumentID);
    std::multimap<string, cPositionDetailPtr>::iterator pos;
    for (pos = position_map_.lower_bound(inst); pos != position_map_.upper_bound(inst); ++pos) {
        pos->second->update(pDepthMarket);
    }
}


int cPositionCollection::getPosition(string inst, DIRE dire) {
    bool                                                is_find_position = false;
    std::multimap<string, cPositionDetailPtr>::iterator pos;
    for (pos = position_map_.lower_bound(inst); pos != position_map_.upper_bound(inst); ++pos) {
        //LOG(INFO) << inst << " " << pos->second->getPosition();
        if (pos->second->getPosiDire() ==  dire) {
            is_find_position = true;
            break;
        }
    }
    //LOG(INFO) << is_find_position;
    if (is_find_position) {
        return pos->second->getPosition();
    } else {
        return 0;
    }
}
int cPositionCollection::getYdPosition(string inst, DIRE dire) {
    bool                                                is_find_position = false;
    std::multimap<string, cPositionDetailPtr>::iterator pos;
    for (pos = position_map_.lower_bound(inst); pos != position_map_.upper_bound(inst); ++pos) {
        if (pos->second->getPosiDire() ==  dire) {
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