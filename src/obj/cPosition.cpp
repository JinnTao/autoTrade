#include <cPosition.h>
#include <cTrade.h>
#include <easylogging++.h>
cPositionDetail::cPositionDetail(string inst) {
    memset(this, 0, sizeof(this));

    this->m_instrumentID = inst;
}

void cPositionDetail::update(CThostFtdcInvestorPositionField* pInvestorPosition) {

    if (strcmp(pInvestorPosition->InstrumentID, this->m_instrumentID.c_str()) == 0) {
        // long postion
        if (pInvestorPosition->PosiDirection == '2') {
            posi_direction_ = DIRE::LONG;
        }
        // short postion
        else if (pInvestorPosition->PosiDirection == '3') {
            posi_direction_ = DIRE::SHORT;
        } else {
            LOG(INFO) << "cPostionDetail PosiDirection error";
            return;
        }
        position_ = pInvestorPosition->Position;
        today_pos_ = pInvestorPosition->TodayPosition;
        yd_pos_ = pInvestorPosition->YdPosition;
        Margin = pInvestorPosition->UseMargin;
        CloseProfit = pInvestorPosition->CloseProfit;
        PositionProfit = pInvestorPosition->PositionProfit;
        trade_date_    = pInvestorPosition->PositionDate;
    } else {
        LOG(INFO) << "cPostionDetail update error";
    }
}
//  identification long & short at outer func.. 
void cPositionDetail::update(CThostFtdcTradeField* pTrade) {

    if (strcmp(pTrade->InstrumentID, this->m_instrumentID.c_str()) == 0) {
        if (pTrade->Direction == THOST_FTDC_D_Buy) {
            posi_direction_ = DIRE::LONG;
        }
        if (pTrade->Direction == THOST_FTDC_D_Sell) {
            posi_direction_ = DIRE::SHORT;
        }


        if (pTrade->OffsetFlag == THOST_FTDC_OF_Open) {
            position_ += pTrade->Volume;
            today_pos_ += pTrade->Volume;
        }
        if (pTrade->OffsetFlag == THOST_FTDC_D_Sell) {
            position_ -= pTrade->Volume;
            //今仓量和昨仓量，只对上期所有效
            if (pTrade->OffsetFlag == THOST_FTDC_OF_Close || pTrade->OffsetFlag == THOST_FTDC_OF_CloseYesterday ||
                pTrade->OffsetFlag == THOST_FTDC_OF_ForceClose)
                yd_pos_ -=  pTrade->Volume;  //昨仓
            else if (pTrade->OffsetFlag == THOST_FTDC_OF_CloseToday)
                today_pos_ -=  pTrade->Volume;  //今仓

            //假设今仓5手，昨仓1，平仓都是发'1'，假设平仓2手，导致昨仓是-1，今仓还是5手，实际应该是今仓5-1，昨仓0
            // 3手昨仓，5手今仓，，'1'平仓了4手,导致昨仓是-1，今仓还是5手，实际应该是今仓5-1，昨仓0
            if (this->yd_pos_ < 0) {
                this->today_pos_ += this->yd_pos_;
                this->yd_pos_              = 0;
            }
        }
    } 
    else {
        LOG(INFO) << "cPostionDetail update error";
    }
 }

void cPositionDetail::Print() {
    cerr << this->m_instrumentID 
        << "\t Pos:" << position_ 
        << "\t Td:" << today_pos_
        << "\t Yd:" << yd_pos_
        << "\t PositionP&L:" << PositionProfit
        << "\t CloseP&L:" << CloseProfit
        << "\t margin:" << Margin 
        << "\t tradeDate:" << trade_date_ << endl;
}