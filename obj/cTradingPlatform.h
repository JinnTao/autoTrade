#ifndef __CTRADINGPLATFORM_H__
#define __CTRADINGPLATFORM_H__

#include <yr_structs.h>


class cString;
class cTraderSpi;
#include "cMdSpi.h"
#include <cMarketDataCollection.h>
#include <cStrategy.h>
#include <cOrderCollection.h>
#include <cTradeCollection.h>
#include <cPositionCollection.h>
#include "cStrategyTemplate.h"
#include "autotrade_config.h"

class cTradingPlatform
{
public:
	cTradingPlatform();
	virtual ~cTradingPlatform();

	void RegisterTraderSpi( cTraderSpi* pTraderSpi );
	void RegisterMdSpi( cMdSpi* p );
	void RegisterMarketDataEngine( cMarketDataCollectionPtr pMarketDataEngine );


	void RegisterStrategy( cStrategyPtr pStrategy );
	
	void initStrategy(autoSetting &);
	//
	/* send orders */
	void SendNewOrders( const cString& instrumentID );
	void SendNewOrder( cOrder* pOrder );
	
	
	/* cancle exisiting pending orders */
	void CancelPendingOrders();
	void CancelPendingOrders( const cString& instrumentID );
	void CancelPendingOrder( int orderID );
	
	//
	/* print functions */
	void PrintPendingOrders() const;
	void PrintCancelledOrders() const;
	void PrintAllOrders() const;
	void PrintPositionSummary() const;
	void PrintClosePnL() const;
	void PrintClosePnL( int ) const;

	//
	/* trading simulation */
	bool SimulationUpdate( const cTick& );

	//
	/* realtime trading */
	DWORD AutoTrading();

	DWORD ProcessOrderTest();

	//
	/* IO process */
	DWORD IOProcess();
	DWORD SimulationIOProcess();

	const sInstrumentInfo* GetInstrumentInfo( const cString& ) const;
	const sTradingAccountInfo* GetTradingAccountInfo() const;

	void Sleep();
	void WakeUp();

	void SetAutoTradeFlag( bool flag ) { m_runAutoTrade = flag; }
	void ClearPlatform();
	
	void insertOrder(string inst,string dire,string flag, int vol,double orderPrice);
	void cancleOrder(string order,int seqNo);
	void cancleAllOrder(string order,string tag);

private:
	cTraderSpi*	m_pTraderSpi;
	cMdSpi* m_pMdSpi;
	cMarketDataCollectionPtr m_pMarketDataEngine;
	cStrategyPtr m_pStrategy;

	cPositionCollectionPtr m_pPositions;
	cOrderCollectionPtr m_pOrders;
	cTradeCollectionPtr m_pTrades;
	// Instrument base Message
	map<string, CThostFtdcInstrumentField*>* m_pInstMessageMap;
	// instrument Commission Rate
	map<string, CThostFtdcInstrumentCommissionRateField*>* m_pInstCommissionRate;

	shared_ptr<vector<string>> m_pSubscribeInst;

	int m_nRequestID;
	bool m_runAutoTrade;
	map< cString, double > m_closedPnL;
	// ≤ﬂ¬‘¿‡
	cStrategyTemplate m_strategy;
};

typedef shared_ptr< cTradingPlatform > cTradingPlatformPtr;



#endif
