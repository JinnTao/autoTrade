#include <cPosition.h>
#include <cTrade.h>
#include "logger.h"
cPositionDetail::cPositionDetail(string inst) {
    memset(this, 0, sizeof(this));

    instrument_id_ = inst;
    position_price_      = 0;  //持仓成本
    open_price_          = 0;  //开仓成本
    position_            = 0;  //总持仓量
    today_pos_           = 0;  //今日持仓量
    yd_pos_              = 0;  //多单上日持仓
    commission_          = 0;
    settle_price_        = 0;  // 结算价
    last_price_          = 0;  // 市场价格
    margin_rate_         = 0;  // 保证金率
    CloseProfit          = 0;  //平仓盈亏
    PositionProfit       = 0;  //持仓盈亏
    FloatProfit          = 0;  // 浮动盈亏 （累计盈亏）
    Margin               = 0;  //持仓占用保证金
}

void cPositionDetail::update(CThostFtdcInvestorPositionField* pInvestorPosition) {
    double lamda = 1;
    if (strcmp(pInvestorPosition->InstrumentID, instrument_id_.c_str()) == 0) {

        Margin += pInvestorPosition->UseMargin;
        CloseProfit += pInvestorPosition->CloseProfit;
        PositionProfit += pInvestorPosition->PositionProfit;
        commission_ += pInvestorPosition->Commission;

        // long postion
        if (pInvestorPosition->PosiDirection == THOST_FTDC_PD_Long) {
            posi_direction_ = DIRE::AUTO_LONG;
            lamda           = 1;
        }
        // short postion
        else if (pInvestorPosition->PosiDirection == THOST_FTDC_PD_Short) {
            posi_direction_ = DIRE::AUTO_SHORT;
            lamda           = -1;
        } else {
            ILOG("cPostionDetail PosiDirection error.");
        }
        if (pInvestorPosition->Position > 0) {
            position_ += pInvestorPosition->Position;
            today_pos_ += pInvestorPosition->TodayPosition;
            yd_pos_ += pInvestorPosition->YdPosition;
            open_cost_ += pInvestorPosition->OpenCost;
            position_cost_ += pInvestorPosition->PositionCost;
            settle_price_  = pInvestorPosition->SettlementPrice;
            margin_rate_   = pInvestorPosition->ExchangeMargin;
            position_date_ = pInvestorPosition->PositionDate;
            trade_date_    = pInvestorPosition->TradingDay;

            open_price_     = open_cost_ / double(inst_field_->VolumeMultiple) / double(position_);
            position_price_ = position_cost_ / double(inst_field_->VolumeMultiple) / double(position_);
            last_price_ =
                position_price_ + lamda * PositionProfit / double(position_ * double(inst_field_->VolumeMultiple));
            FloatProfit = lamda * (last_price_ - open_price_) * position_ * double(inst_field_->VolumeMultiple);

            if (position_ != (today_pos_ + yd_pos_)){
                ILOG(
                    "Inst:{},position:{},ydPos:{},tdPos{},margin:{},CloseProfit:{},PositionProfit:{},PositionDate:{},"
                    "comssition:{},settlePrice:{},Marginrate:{},OpenVolume:{},CloseVol{}.",
                     pInvestorPosition->InstrumentID,
                     pInvestorPosition->Position,
                     pInvestorPosition->YdPosition,
                     pInvestorPosition->TodayPosition,
                     pInvestorPosition->UseMargin,
                     pInvestorPosition->CloseProfit,
                     pInvestorPosition->PositionProfit,
                     pInvestorPosition->PositionDate,
                     pInvestorPosition->Commission,
                     pInvestorPosition->SettlementPrice,
                     pInvestorPosition->ExchangeMargin,
                     pInvestorPosition->OpenVolume,
                     pInvestorPosition->CloseVolume);
            }
        }
    } else {
        WLOG("cPostionDetail update error.");
    }
}

void cPositionDetail::update(CThostFtdcDepthMarketDataField* pDepthMarketData) {
    if (strcmp(pDepthMarketData->InstrumentID, instrument_id_.c_str()) == 0) {
        last_price_  = pDepthMarketData->LastPrice;
        exchange_id_ = pDepthMarketData->ExchangeID;
        update_time_ = string(pDepthMarketData->TradingDay + string(" ") + pDepthMarketData->UpdateTime);
        if (position_ > 0) {
            if (posi_direction_ == DIRE::AUTO_LONG) {
                PositionProfit = (last_price_ - position_price_) * position_ * double(inst_field_->VolumeMultiple);
                FloatProfit    = (last_price_ - open_price_) * position_ * double(inst_field_->VolumeMultiple);
            }
            if (posi_direction_ == DIRE::AUTO_SHORT) {
                PositionProfit = (position_price_ - last_price_) * position_ * double(inst_field_->VolumeMultiple);
                FloatProfit    = (open_price_ - last_price_) * position_ * double(inst_field_->VolumeMultiple);
            }
        }
    } else {
        WLOG("cPositionDetail update marketData inst not match.");
    }
}
//  identification long & short at outer func..
void cPositionDetail::update(CThostFtdcTradeField* pTrade, shared_ptr<cTrade> pcTrade) {

    if (strcmp(pTrade->InstrumentID, instrument_id_.c_str()) == 0) {

        int old_pos = position_;
        // open
        if (pTrade->OffsetFlag == THOST_FTDC_OF_Open) {
            position_ += pTrade->Volume;
            today_pos_ += pTrade->Volume;
            // LOG(INFO) << pTrade->Price << "  " << pTrade->Volume << "  " << open_price_ << "  " << old_pos << "  "
            //         << position_;
            open_price_     = (pTrade->Price * pTrade->Volume + open_price_ * old_pos) / double(position_);
            position_price_ = (pTrade->Price * pTrade->Volume + position_price_ * old_pos) / double(position_);
            // long
            if (pTrade->Direction == THOST_FTDC_D_Buy) {
                posi_direction_ = DIRE::AUTO_LONG;
            }
            // short
            if (pTrade->Direction == THOST_FTDC_D_Sell) {
                posi_direction_ = DIRE::AUTO_SHORT;
            }
        }
        // close
        else {
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
            // buy close => sell open
            if (pTrade->Direction == THOST_FTDC_D_Buy) {
                CloseProfit += (position_price_ - pTrade->Price) * pTrade->Volume * double(inst_field_->VolumeMultiple);
            }
            // sell close => buy open
            if (pTrade->Direction == THOST_FTDC_D_Sell) {
                CloseProfit += (pTrade->Price - position_price_) * pTrade->Volume * double(inst_field_->VolumeMultiple);
            }
            if (position_ > 0) {
                // open_price_     = (open_price_ * old_pos - pTrade->Price * pTrade->Volume) / double(position_);
                // position_price_ = (position_price_ * old_pos - pTrade->Price * pTrade->Volume) / double(position_);

            } else {
                open_price_     = 0;
                position_price_ = 0;
                PositionProfit  = 0;
                FloatProfit     = 0;
            }

            if (position_ != (yd_pos_ + today_pos_)) {
                WLOG("Position error: pos:{},ydPos:{},tdPos:{},offset:{},vol:{}.",
                     position_,
                     yd_pos_,
                     today_pos_,
                     pTrade->OffsetFlag,
                     pTrade->Volume);
            }
        }

        position_date_ = pTrade->TradingDay;
        trade_date_    = pTrade->TradingDay;
        exchange_id_   = pTrade->ExchangeID;
        commission_ += pcTrade->GetCommission();
        // LOG(INFO) << "price :" << pTrade->Price << " pos: " << position_ << "tradding day" << trade_date_;

    } else {
        //LOG(INFO) << "cPostionDetail update error";
        WLOG("cPostionDetail update error!");
    }
}

void cPositionDetail::Print() {
    string posi_dire[] = {"Long", "Short", "error"};
    cerr << instrument_id_ << "\t Pos:" << position_ << "\t Dire: " << posi_dire[posi_direction_]
         << "\t Td:" << today_pos_ << "\t Yd:" << yd_pos_ << "\t PositionP&L:" << PositionProfit
         << "\t Float P&L:" << FloatProfit  //<< "\t CloseP&L:" << CloseProfit
         << "\t openPrice:" << open_price_ << "\t posiPrice: " << position_price_ << "\t lastPrice: " << last_price_
         << "\t updateTime:" << update_time_ << endl;
}