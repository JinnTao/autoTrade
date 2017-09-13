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

	virtual void run();

	virtual void onOrder(cOrderPtr	);

	virtual void onTrade(cTradePtr );

private:


};

