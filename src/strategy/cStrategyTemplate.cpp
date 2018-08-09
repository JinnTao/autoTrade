#include "cStrategyTemplate.h"


cStrategyTemplate::cStrategyTemplate(){
    m_candleMinute = -1;
    m_lastOpen = -1;
    m_lastHigh = -1;
    m_lastLow = -1;
    m_lastClose = -1;
    m_lastVolume = -1;
    cStrategy::cStrategy();

    m_oldState = false;
}
cStrategyTemplate::cStrategyTemplate(string name){
    cStrategy::cStrategy(name);
}

cStrategyTemplate::~cStrategyTemplate(){

}
// 在策略类中完成数据文件名字的构成
// 1、第一步 初始化策略时候 获取交易日列表（在tradingPlatforn中）
// 2、轮询交易日列表生成文件名列表 （在init中）
// 3、循环文件名列表初始化策略需要的数据 （同上）

void cStrategyTemplate::init(){
    if(m_close.size() == 0){
        // Start Time 
        this->m_marketData->loadSeriesHistory(m_oneMinuteDataDir,m_startDate,m_endDate,m_open,m_high,m_low,m_close,m_volume);
    }
    this->m_pMdSpi->SubscribeMarketData(m_inst);// trade 1801
}

void cStrategyTemplate::onOrder(cOrderPtr p){
}

void cStrategyTemplate::onTrade(CThostFtdcTradeField p){

}
void cStrategyTemplate::run(){
    if(this->m_marketData->GetMarketDataHandle(m_inst) && isTradeTime()){
        CThostFtdcDepthMarketDataField lastData = this->m_marketData->GetMarketDataHandle(m_inst)->getLastMarketData();
        int tickMinute = 1;//cDateTime(cSystem::GetCurrentTimeBuffer().c_str()).Minute();
        // new Candle
        if(tickMinute != m_candleMinute){
            if(m_candleMinute != -1){
                //add
                m_open.push_back(m_lastOpen);
                m_high.push_back(m_lastHigh);
                m_low.push_back(m_lastLow);
                m_close.push_back(m_lastClose);
                m_volume.push_back(m_lastVolume);
                //erase
                m_open.erase(m_open.begin());
                m_high.erase(m_high.begin());
                m_low.erase(m_low.begin());
                m_close.erase(m_close.begin());
                m_volume.erase(m_volume.begin());
            }
            on1MBar();
            m_lastOpen = lastData.LastPrice;
            m_lastHigh = lastData.LastPrice;
            m_lastLow = lastData.LastPrice;
            m_lastClose = lastData.LastPrice;
            m_lastVolume = lastData.Volume;
            m_candleMinute = tickMinute;// update current candle Minute
    
        }else{
            m_lastHigh = max(m_lastHigh, lastData.LastPrice);
            m_lastLow = min(m_lastLow,lastData.LastPrice);
            m_lastClose = lastData.LastPrice;
            m_lastVolume += lastData.Volume;
        
        }
    }

}

// 相对强弱策略
// 策略说明 RSI > 90 卖出开仓 < 10 买入开仓;
// 持仓多头 RSI > 50 平仓
// 持仓空头 RSI < 50 平仓
void cStrategyTemplate::on1MBar(){
        // =================================================================  指标计算 =================================================
        int outBegIdx[100]={};
        int outNBElement [100]={};
        double outReal[100]={};    
        TA_RSI(int(m_close.size())-14,int(m_close.size())-1, &m_close[0],14,outBegIdx,outNBElement,outReal);

        //=============================================================取消前面所有未成交单 ==============================================
        this->m_pTradeSpi->cancleMyPendingOrder();

        // ==============================================================日志输出========================================================
        double rsiValue = outReal[0];
        //cout << cSystem::GetCurrentTimeBuffer() << " RSI: " << rsiValue << " "<< m_lastOpen << " " << m_lastHigh << " " << m_lastLow << " " << m_lastClose << endl;
        // ===========================================================下单逻辑============================================================
        int longPos = this->m_pPositionC.get()->getPosition(m_inst,DIRE::AUTO_LONG);
        int shortPos = this->m_pPositionC.get()->getPosition(m_inst,DIRE::AUTO_SHORT);

        int netPos = longPos - shortPos;
        
        if(netPos == 0){
            if(rsiValue>90){
                this->m_pTradeSpi->insertOrder(m_inst, traderTag::DIRECTION::sell, traderTag::OFFSETFLAG::open, 1, 0);
            }
            if(rsiValue<10){
                this->m_pTradeSpi->insertOrder(m_inst, traderTag::DIRECTION::buy, traderTag::OFFSETFLAG::open, 1, 0);
            }
        }else if(netPos > 0){
            if(rsiValue>50){
                this->m_pTradeSpi->insertOrder(m_inst, traderTag::DIRECTION::sell, traderTag::OFFSETFLAG::close, 1, 0);
            }
        }else if(netPos < 0){
            if(rsiValue<50){
                this->m_pTradeSpi->insertOrder(m_inst, traderTag::DIRECTION::buy, traderTag::OFFSETFLAG::close, 1, 0);
            }
        }
}

void cStrategyTemplate::unInit(){


}

bool cStrategyTemplate::isTradeTime(){
    DateTimeFormat s0900 = 900,s1015 = 1015,s1030 = 1030,s1130 = 1130,s1330 = 1330,s1500 = 1500,s2100 = 2100,s2330 = 2330;
    //cDateTime nowDateTime = cDateTime(cSystem::GetCurrentTimeBuffer().c_str());
    DateTimeFormat hour = 1;  // nowDateTime.Hour();
    DateTimeFormat min  = 1;  // nowDateTime.Minute();

    DateTimeFormat nowTime = hour *100 + min;
    bool newState;
    if( (nowTime>s0900 && nowTime <s1015 ) || 
        (nowTime>s1030 && nowTime <s1130 ) ||
        (nowTime>s1330 && nowTime <s1500 ) ||
        (nowTime>s2100 && nowTime <s2330 )
    )
    {

        newState =  true;
    }
    else{

        newState =  false;
    }
    //if(m_oldState && newState){
    //    cout << "tradeState open" << endl;
    //}else{
    //    cout << "tradeState open" << endl;

    //}
    return newState;

}