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
#include "autotrade_config.h"
#include "cthread.h"
#include "ta_libc.h"
#include "cTraderSpi.h"
#include "cMdSpi.h"
#include "cDateTime.h"
#include "cSystem.h"

//#pragma comment(lib,"CodeLib\\ta-lib\\lib\\ta_abstract_cdd.lib")
//#pragma comment(lib,"CodeLib\\ta-lib\\lib\\ta_abstract_cdr.lib.lib")
//#pragma comment(lib,"CodeLib\\ta-lib\\lib\\ta_abstract_cdd")
//#pragma comment(lib,"CodeLib\\ta-lib\\lib\\ta_abstract_cdd")
//#pragma comment(lib,"CodeLib\\ta-lib\\lib\\ta_abstract_cdd")
//#pragma comment(lib,"CodeLib\\ta-lib\\lib\\ta_abstract_cdd")
//#pragma comment(lib,"CodeLib\\ta-lib\\lib\\ta_abstract_cdd")
//#pragma comment(lib,"CodeLib\\ta-lib\\lib\\ta_abstract_cdd")
//#pragma comment(lib,"CodeLib\\ta-lib\\lib\\ta_abstract_cdd")
//#pragma comment(lib,"CodeLib\\ta-lib\\lib\\ta_abstract_cdd")
//#pragma comment(lib,"CodeLib\\ta-lib\\lib\\ta_abstract_cdd")
//#pragma comment(lib,"CodeLib\\ta-lib\\lib\\ta_abstract_cdd")
//#pragma comment(lib,"CodeLib\\ta-lib\\lib\\ta_abstract_cdd")
//#pragma comment(lib,"CodeLib\\ta-lib\\lib\\ta_abstract_cdd")
//#pragma comment(lib,"CodeLib\\ta-lib\\lib\\ta_abstract_cdd")
//#pragma comment(lib,"CodeLib\\ta-lib\\lib\\ta_abstract_cdd")
//#pragma comment(lib,"CodeLib\\ta-lib\\lib\\ta_abstract_cdd")
//#pragma comment(lib,"CodeLib\\ta-lib\\lib\\ta_abstract_cdd")
//#pragma comment(lib,"CodeLib\\ta-lib\\lib\\ta_abstract_cdd")
//#pragma comment(lib,"CodeLib\\ta-lib\\lib\\ta_abstract_cdd")
//#pragma comment(lib,"CodeLib\\ta-lib\\lib\\ta_abstract_cdd")
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

	virtual void init();

	virtual void unInit();

	void start();

	void stop();

	virtual void onOrder(cOrderPtr	){};

	virtual void onTrade(cTradePtr ){};

	virtual void run(){cerr << this->m_strategyName << " runing" << endl;};


	virtual void setInst(string inst) { this->m_inst = inst; }
	virtual void setInitDate(string startDate, string endDate) { this->m_startDate = startDate; this->m_endDate = endDate; }

	// ***************************************************************************
	void RegisterMarketDataCollection( cMarketDataCollectionPtr p ){m_marketData = p;}
	void RegisterTradeSpi(cTraderSpi *p){m_pTradeSpi = p;}
	void RegisterMdSpi(cMdSpi *p){m_pMdSpi = p;}
	void RegisterPositionCollectionPtr(cPositionCollectionPtr p){m_pPositionC = p;};
	void RegisterOrderCollectionPtr(cOrderCollectionPtr p){m_pOrderC = p;}
	void RegisterTradeCollectionPtr(cTradeCollectionPtr p){m_pTradeC = p;}
	void RegisterTxtDir(string tradeDayDir, string oneMinuteDataDir){ m_tradeDayDir = tradeDayDir; m_oneMinuteDataDir = oneMinuteDataDir; }
	void RegisterAutoSetting(autoSetting *p) { this->m_pAutoSetting = p; }
//	void RegisterTradePlatForm(cTradingPlatform *p){m_pTradePlatform = p;}
protected:
	// base collection
	cMarketDataCollectionPtr m_marketData;
	// base mdptr tdptr
	cTraderSpi* m_pTradeSpi;
	cMdSpi* m_pMdSpi;

	cPositionCollectionPtr m_pPositionC;
	cOrderCollectionPtr m_pOrderC;
	cTradeCollectionPtr m_pTradeC;
	
	//cTradingPlatform * m_pTradePlatform;

	// run status;
	bool m_status;

	string m_strategyName;

	int m_timeSpan;
	//txt database dir
	string m_tradeDayDir;
	string m_oneMinuteDataDir;

	string m_inst;
	string m_startDate;
	string m_endDate;

	autoSetting *m_pAutoSetting;

private:
	
	DWORD AutoTrading();

	cThread< cStrategy >* m_pTradingThread;
};

typedef shared_ptr< cStrategy > cStrategyPtr;

#endif

