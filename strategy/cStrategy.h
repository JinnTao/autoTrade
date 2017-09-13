#ifndef __CSTRATEGY_H__
#define __CSTRATEGY_H__


#include <map>

#include <Windows.h>

#include <cTick.h>
#include <cCandle.h>
#include <cMarketDataCollection.h>
#include <cTradeCollection.h> 
#include <cPositionCollection.h>
#include <cOrderCollection.h>
#include "cthread.h"

#include "cTraderSpi.h"
#include "cMdSpi.h"



//
class cStrategy
{
private:
	// non-copyable
	cStrategy( const cStrategy& ) {}
	cStrategy& operator = ( const cStrategy& ) {}

public:
	cStrategy();

	cStrategy(const string&);
	
	virtual ~cStrategy();

	virtual void init(){};

	void start();

	void stop();

	virtual void onOrder(cOrderPtr	){};

	virtual void onTrade(cTradePtr ){};

	virtual void run(){cerr << this->m_strategyName << " runing" << endl;};


	// ***************************************************************************
	void RegisterMarketDataCollection( cMarketDataCollectionPtr p );

protected:
	// base collection
	cPositionCollectionPtr m_pPositionC;
	cOrderCollectionPtr m_pOrderC;
	cMarketDataCollectionPtr m_mc;
	cTradeCollectionPtr m_tradeC;

	// base mdptr tdptr
	cTraderSpi* m_pTradeSpi;
	cMdSpi* m_pMdSpi;

	// run status;
	bool m_status;

	string m_strategyName;

	int m_timeSpan;
private:
	
	DWORD AutoTrading();

	cThread< cStrategy >* m_pTradingThread;
};

typedef shared_ptr< cStrategy > cStrategyPtr;

#endif

