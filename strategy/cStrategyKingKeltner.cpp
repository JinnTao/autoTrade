#include "cStrategyKingKeltner.h"


cStrategyKingKeltner::cStrategyKingKeltner(void)
{
	m_candleMinute = -1;
	m_lastOpen = -1;
	m_lastHigh = -1;
	m_lastLow = -1;
	m_lastClose = -1;
	m_lastVolume = -1;
	m_strategyName == "KingKeltner";
	cStrategy::cStrategy();

	m_oldState = false;
}


cStrategyKingKeltner::~cStrategyKingKeltner(void)
{

}

void cStrategyKingKeltner::init(){
	if (m_close.size() == 0) {
		// Start Time 
		this->m_marketData->loadSeriesHistory(m_oneMinuteDataDir, m_startDate, m_endDate, m_open, m_high, m_low, m_close, m_volume);
	}
	this->m_pMdSpi->SubscribeMarketData(m_inst);// trade 1801

}

void cStrategyKingKeltner::unInit(){



}

void cStrategyKingKeltner::run(){
	if (this->m_marketData->GetMarketDataHandle(m_inst) && isTradeTime()) {
		CThostFtdcDepthMarketDataField lastData = this->m_marketData->GetMarketDataHandle(m_inst)->getLastMarketData();
		int tickMinute = cDateTime(cSystem::GetCurrentTimeBuffer().c_str()).Minute();
		// new Candle
		if (tickMinute != m_candleMinute) {
			if (m_candleMinute != -1) {
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
			on5MBar();
			// latest bar data
			m_lastOpen = lastData.LastPrice;
			m_lastHigh = lastData.LastPrice;
			m_lastLow = lastData.LastPrice;
			m_lastClose = lastData.LastPrice;
			m_lastVolume = lastData.Volume;
			m_candleMinute = tickMinute;// update current candle Minute

		}
		else {
			// update bar data
			m_lastHigh = max(m_lastHigh, lastData.LastPrice);

			m_lastLow = min(m_lastLow, lastData.LastPrice);

			m_lastClose = lastData.LastPrice;

			m_lastVolume += lastData.Volume;

		}
		// 处理停止单
		this->processStopOrder(m_inst, m_lastClose);
	}

}

void cStrategyKingKeltner::sendOcoOrder(double upPrice, double downPrice, int fixedSize) {
	
	//
	//	发送OCO委托

	//	OCO(One Cancel Other)委托：
	//	1. 主要用于实现区间突破入场
	//	2. 包含两个方向相反的停止单
	//	3. 一个方向的停止单成交后会立即撤消另一个方向的
	//
	this->sendStopOrder(m_inst, DIRECTION::buy, OFFSETFLAG::open, upPrice, fixedSize, this->m_strategyName);
	this->sendStopOrder(m_inst, DIRECTION::sell, OFFSETFLAG::open, downPrice, fixedSize, this->m_strategyName);

}

void cStrategyKingKeltner::on5MBar(){
	// =================================================================  指标计算 =================================================
	double up, down;
	if (!keltner(int(this->m_pAutoSetting->kkLength), this->m_pAutoSetting->kkDev, up, down)) {
		return;
	}

	
	//=============================================================取消前面所有未成交单 ==============================================
	this->m_pTradeSpi->cancleMyPendingOrder();

	// ==============================================================日志输出========================================================
	//double rsiValue = outReal[0];
	cout << cSystem::GetCurrentTimeBuffer() << " up: " << up << " down: " << down << " " << m_lastOpen << " " << m_lastHigh << " " << m_lastLow << " " << m_lastClose << endl;
	// ===========================================================下单逻辑============================================================


	int longPos = this->m_pPositionC.get()->getHolding_long(m_inst);
	int shortPos = this->m_pPositionC.get()->getHolding_short(m_inst);

	this->m_netPos = longPos - shortPos;



	if (netPos == 0) {
		this->m_intraTradeHigh = m_lastHigh;
		this->m_intraTradeLow = m_lastLow;

		this->sendOcoOrder(up, down, int(this->m_pAutoSetting->fixedSize));

	}
	else if (netPos > 0) {
		m_intraTradeHigh = max(m_lastHigh, m_intraTradeHigh);
		m_intraTradeLow = m_lastLow;
		this->sendStopOrder(m_inst, DIRECTION::sell, OFFSETFLAG::close, m_intraTradeHigh * (1 - m_pAutoSetting->trailingPrcnt / 100.0), UINT(m_pAutoSetting->fixedSize), this->m_strategyName);
		
	}
	else if (netPos < 0) {
		m_intraTradeHigh = m_lastHigh;
		m_intraTradeLow = min(m_lastLow, m_intraTradeLow);
		this->sendStopOrder(m_inst, DIRECTION::buy, OFFSETFLAG::close, m_intraTradeHigh * (1 + m_pAutoSetting->trailingPrcnt / 100.0), UINT(m_pAutoSetting->fixedSize), this->m_strategyName);
	}
}


bool cStrategyKingKeltner::isTradeTime() {
	return true;
	DateTimeFormat s0900 = 900, s1015 = 1015, s1030 = 1030, s1130 = 1130, s1330 = 1330, s1500 = 1500, s2100 = 2100, s2330 = 2330;
	cDateTime nowDateTime = cDateTime(cSystem::GetCurrentTimeBuffer().c_str());
	DateTimeFormat hour = nowDateTime.Hour();
	DateTimeFormat min = nowDateTime.Minute();

	DateTimeFormat nowTime = hour * 100 + min;
	bool newState;
	if ((nowTime>s0900 && nowTime <s1015) ||
		(nowTime>s1030 && nowTime <s1130) ||
		(nowTime>s1330 && nowTime <s1500) ||
		(nowTime>s2100 && nowTime <s2330)
		)
	{

		newState = true;
	}
	else {

		newState = false;
	}
	//if(m_oldState && newState){
	//	cout << "tradeState open" << endl;
	//}else{
	//	cout << "tradeState open" << endl;

	//}
	return newState;

}

bool cStrategyKingKeltner::keltner( int kkLength, double kkDev, double& kkUp,double &kkDown) {
	try {
		double mid = 0, atr = 0;
		int outBegIdx_SMA[100] = {};
		int outNBElement_SMA[100] = {};
		double outReal_SMA[100] = {};

		int outBegIdx_ATR[100] = {};
		int outNBElement_ATR[100] = {};
		double outReal_ATR[100] = {};
		// 输出的值 在out_real中的最后一个数据中，前提要求输入数据从old到new
		TA_SMA(m_close.size() - kkLength, m_close.size(), &m_close[0], kkLength, outBegIdx_SMA, outNBElement_SMA, outReal_SMA);

		TA_ATR(m_close.size() - kkLength, m_close.size(), &m_high[0], &m_low[0], &m_close[0], kkLength, outBegIdx_ATR, outNBElement_ATR, outReal_ATR);

		kkUp = outReal_SMA[kkLength - 1] + outReal_ATR[kkLength - 1] * kkDev;

		kkDown = outReal_SMA[kkLength - 1] - outReal_ATR[kkLength - 1] * kkDev;
		return true;
	}
	catch (...) {
		
		return false;
	
	}

}

void cStrategyKingKeltner::onTrade(CThostFtdcTradeField pTrade) {
	
}