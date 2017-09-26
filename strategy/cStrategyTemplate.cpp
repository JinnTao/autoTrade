#include "cStrategyTemplate.h"


cStrategyTemplate::cStrategyTemplate(){
	m_candleMinute = -1;
	m_lastOpen = -1;
	m_lastHigh = -1;
	m_lastLow = -1;
	m_lastClose = -1;
	m_lastVolume = -1;
	cStrategy::cStrategy();

	m_inst = "rb1801";
}
cStrategyTemplate::cStrategyTemplate(string name){
	cStrategy::cStrategy(name);
}

cStrategyTemplate::~cStrategyTemplate(){

}
void cStrategyTemplate::init(){
	if(m_close.size() == 0){
		this->m_marketData->loadSeriesHistory("rb","2017","2017",DataFrequency::M1,DataType::Open,m_open,m_high,m_low,m_close,m_volume,100);
	}
	this->m_pMdSpi->SubscribeMarketData(m_inst);// trade 1801
}

void cStrategyTemplate::onOrder(cOrderPtr p){
}

void cStrategyTemplate::onTrade(cTradePtr p){

}
void cStrategyTemplate::run(){
	if(this->m_marketData->GetMarketDataHandle(m_inst)){
		CThostFtdcDepthMarketDataField lastData = this->m_marketData->GetMarketDataHandle(m_inst)->getLastMarketData();
		int tickMinute = cDateTime(cSystem::GetCurrentTimeBuffer().c_str()).Second();
		// new Candle
		if(tickMinute != m_candleMinute){
			if(m_candleMinute != -1){
				
				m_open.push_back(m_lastOpen);
				m_high.push_back(m_lastHigh);
				m_low.push_back(m_lastLow);
				m_close.push_back(m_lastClose);
				m_volume.push_back(m_lastVolume);
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
		TA_RSI(m_close.size()-14,m_close.size()-1, &m_close[0],14,outBegIdx,outNBElement,outReal);

		//=============================================================取消前面所有未成交单 ==============================================
		this->m_pTradeSpi->cancleMyPendingOrder();

		// ==============================================================日志输出========================================================
		double rsiValue = outReal[0];
		cout << cSystem::GetCurrentTimeBuffer() << " RSI: " << rsiValue << " "<< m_lastOpen << " " << m_lastHigh << " " << m_lastLow << " " << m_lastClose << endl;
		// ===========================================================下单逻辑============================================================
		int longPos = this->m_pPositionC.get()->getHolding_long(m_inst);
		int shortPos = this->m_pPositionC.get()->getHolding_short(m_inst);

		int netPos = longPos - shortPos;
		
		if(netPos == 0){
			if(rsiValue>90){
				this->m_pTradeSpi->insertOrder(m_inst,DIRECTION::sell,OFFSETFLAG::open,1,0);
			}
			if(rsiValue<10){
				this->m_pTradeSpi->insertOrder(m_inst,DIRECTION::buy,OFFSETFLAG::open,1,0);
			}

		}else if(netPos > 0){
			if(rsiValue>50){
				this->m_pTradeSpi->insertOrder(m_inst,DIRECTION::sell,OFFSETFLAG::close,1,0);
			}
		}else if(netPos < 0){
			if(rsiValue<50){
				this->m_pTradeSpi->insertOrder(m_inst,DIRECTION::buy,OFFSETFLAG::close,1,0);
			}
		}
}

void cStrategyTemplate::unInit(){


}