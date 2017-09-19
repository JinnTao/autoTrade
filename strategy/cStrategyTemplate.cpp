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
		CThostFtdcDepthMarketDataField* plastData = this->m_marketData->GetMarketDataHandle(m_inst)->getLastMarketData();
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
			m_lastOpen = plastData->LastPrice;
			m_lastHigh = plastData->LastPrice;
			m_lastLow = plastData->LastPrice;
			m_lastClose = plastData->LastPrice;
			m_lastVolume = plastData->Volume;
			m_candleMinute = tickMinute;// update current candle Minute
	
		}else{
			m_lastHigh = max(m_lastHigh,plastData->LastPrice);
			m_lastLow = min(m_lastLow,plastData->LastPrice);
			m_lastClose = plastData->LastPrice;
			m_lastVolume += plastData->Volume;
		
		}
	}

}
void cStrategyTemplate::unInit(){


}
// 相对强弱策略
void cStrategyTemplate::on1MBar(){
		int outBegIdx[100]={};
		int outNBElement [100]={};
		double outReal[100]={};
	
		TA_RSI(m_open.size()-14,m_open.size()-1, &m_open[0],14,outBegIdx,outNBElement,outReal);


		double rsiValue = outReal[0];
		cout << cSystem::GetCurrentTimeBuffer() << " RSI: " << rsiValue << " "<< m_lastOpen << " " << m_lastHigh << " " << m_lastLow << " " << m_lastClose << endl;

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