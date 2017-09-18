#pragma once
#include "cstrategy.h"
class cStrategyTemplate :
	public cStrategy
{
public:
	cStrategyTemplate(void);
	cStrategyTemplate(string strategyName);

	~cStrategyTemplate(void);
	virtual void init();
	virtual void unInit();

	virtual void run();

	virtual void onOrder(cOrderPtr	);

	virtual void onTrade(cTradePtr );

private:

	vector<double> m_open;
	vector<double> m_high;
	vector<double> m_low;
	vector<double> m_close;
	vector<double> m_volume;

	double *m_buffer;
};

