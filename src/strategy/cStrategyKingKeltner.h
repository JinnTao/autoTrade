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

	virtual void onTrade(CThostFtdcTradeField) ;


    virtual void setInst(string inst);
	virtual void setInitDate(string startDate, string endDate) { this->m_startDate = startDate; this->m_endDate = endDate; };
	
	bool keltner( int kkLength, double kkDev,double &kkUp,double &kkDown);

	void sendOcoOrder(double upPrice, double downPrice, int fixedSize);

	void printStatus();

protected:

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

    std::map<std::string, int32> m_instLotsList;
	// init data 
	string m_startDate;
	string m_endDate;

	//策略输入参数
	double m_kkLength;
	double m_kkDev;


	// 策略参数变量
	double m_kkUp;
	double m_kkDown;
	double m_intraTradeHigh;
	double m_intraTradeLow;

	int m_netPos;

	double m_orderBuyPirce;
	double m_orderBuySize;
	double m_orderSellPrice;
	double m_orderSellSize;

	double m_ocoOrderStaus;
};

