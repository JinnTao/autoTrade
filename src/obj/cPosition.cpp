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
        if (pInvestorPosition->PosiDirection == THOST_FTDC_PD_Long) {
            posi_direction_ = DIRE::AUTO_LONG;
        }
        // short postion
        else if (pInvestorPosition->PosiDirection == THOST_FTDC_PD_Short) {
            posi_direction_ = DIRE::AUTO_SHORT;
        } else {
            LOG(INFO) << "cPostionDetail PosiDirection error";
            return;
        }
        position_      = pInvestorPosition->Position;
        today_pos_     = pInvestorPosition->TodayPosition;
        yd_pos_        = pInvestorPosition->YdPosition;
        Margin         = pInvestorPosition->UseMargin;
        CloseProfit    = pInvestorPosition->CloseProfit;
        PositionProfit = pInvestorPosition->PositionProfit;
        position_date_ = pInvestorPosition->PositionDate;
        trade_date_    = pInvestorPosition->TradingDay;
        open_price_     = pInvestorPosition->OpenCost / double(inst_field_->VolumeMultiple) / double(position_);
        position_price_ = pInvestorPosition->PositionCost / double(inst_field_->VolumeMultiple) / double(position_);
        commission_    = pInvestorPosition->Commission;
        settle_price_  = pInvestorPosition->SettlementPrice;
        margin_rate_   = pInvestorPosition->ExchangeMargin;
    } else {
        LOG(INFO) << "cPostionDetail update error";
    }
}

void cPositionDetail::update(CThostFtdcDepthMarketDataField* pDepthMarketData) {
    if (strcmp(pDepthMarketData->InstrumentID, this->m_instrumentID.c_str()) == 0) {
        last_price_ = pDepthMarketData->LastPrice;
        exchange_id_ = pDepthMarketData->ExchangeID;
        update_time_ = string(pDepthMarketData->TradingDay+string(" ")+pDepthMarketData->UpdateTime);
        if (posi_direction_ == DIRE::AUTO_LONG) {
            PositionProfit = (last_price_ - position_price_) * position_ * double(inst_field_->VolumeMultiple);
        }
        if (posi_direction_ == DIRE::AUTO_SHORT) {
            PositionProfit = (position_price_ - last_price_) * position_ * double(inst_field_->VolumeMultiple); 
        }
    } else {
        LOG(INFO) << "cPostionDetail update error";
    }
}
//  identification long & short at outer func..
void cPositionDetail::update(CThostFtdcTradeField* pTrade) {

    if (strcmp(pTrade->InstrumentID, this->m_instrumentID.c_str()) == 0) {


        if (pTrade->Direction == THOST_FTDC_D_Buy) {
            posi_direction_ = DIRE::AUTO_LONG;
        }
        if (pTrade->Direction == THOST_FTDC_D_Sell) {
            posi_direction_ = DIRE::AUTO_SHORT;
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
                yd_pos_ -= pTrade->Volume;  //昨仓
            else if (pTrade->OffsetFlag == THOST_FTDC_OF_CloseToday)
                today_pos_ -= pTrade->Volume;  //今仓

            //假设今仓5手，昨仓1，平仓都是发'1'，假设平仓2手，导致昨仓是-1，今仓还是5手，实际应该是今仓5-1，昨仓0
            // 3手昨仓，5手今仓，，'1'平仓了4手,导致昨仓是-1，今仓还是5手，实际应该是今仓5-1，昨仓0
            if (this->yd_pos_ < 0) {
                this->today_pos_ += this->yd_pos_;
                this->yd_pos_ = 0;
            }
        }

        CloseProfit     = 0;
        PositionProfit  = 0;
        position_date_  = pTrade->TradingDay;
        trade_date_     = pTrade->TradingDay;
        open_price_     = (pTrade->Price + open_price_) / double(position_);
        position_price_ = (pTrade->Price + position_price_) / double(position_);
        exchange_id_    = pTrade->ExchangeID;

    } else {
        LOG(INFO) << "cPostionDetail update error";
    }
}

void cPositionDetail::Print() {
    cerr << this->m_instrumentID << "\t Pos:" << position_ << "\t Td:" << today_pos_ << "\t Yd:" << yd_pos_
         << "\t PositionP&L:" << PositionProfit << "\t CloseP&L:" << CloseProfit << "\t margin:" << Margin 
         << "\t openPrice:" << open_price_ << "\t posiPrice: " << position_price_
         << "\t updateTime:" << update_time_ << endl;
}