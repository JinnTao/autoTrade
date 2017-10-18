#pragma once
#include "cstrategy.h"
class cStrategyKingKeltner :
	public cStrategy
{
public:
	cStrategyKingKeltner(void);
	~cStrategyKingKeltner(void);
	virtual void init();
	virtual void unInit();

	virtual void run();
	void on5MBar();

	virtual void onOrder(cOrderPtr) {};

	virtual void onTrade(cTradePtr) {};

	bool isTradeTime();

	void setInst(string inst){ this->m_inst = inst; }
	void setInitDate(string startDate, string endDate){ this->m_startDate = startDate; this->m_endDate = endDate; }
private:

	vector<double> m_open;
	vector<double> m_high;
	vector<double> m_low;
	vector<double> m_close;
	vector<double> m_volume;

	double *m_buffer;

	int m_candleMinute;

	double m_lastOpen;
	double m_lastHigh;
	double m_lastLow;
	double m_lastClose;
	double m_lastVolume;

	bool m_oldState;

	string m_inst;
	// init data 
	string m_startDate;
	string m_endDate;
	// 策略参数变量
	double kkUp;
	double kkDown;
	double intraTradeHigh;
	double intraTradeLow;


};

