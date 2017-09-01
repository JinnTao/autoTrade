#ifndef __CTRADINGPLATFORM_H__
#define __CTRADINGPLATFORM_H__

#include <yr_structs.h>
#include <Windows.h>

class cString;
class cTraderSpi;

#include <cMarketDataCollection.h>
#include <cStrategy.h>
#include <cSignalCollection.h>
#include <cOrderCollection.h>
#include <cTradeCollection.h>
#include <cPositionCollection.h>



class cTradingPlatform
{
public:
	cTradingPlatform();
	virtual ~cTradingPlatform();

	void RegisterTraderSpi( cTraderSpi* pTraderSpi );

	void RegisterMarketDataEngine( cMarketDataCollectionPtr pMarketDataEngine );

	void RegisterStrategy( cStrategyPtr pStrategy );
	
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



private:
	cArray< cString > m_instrumentIDs;
	cTraderSpi*	m_pTraderSpi;

	cMarketDataCollectionPtr m_pMarketDataEngine;
	cStrategyPtr m_pStrategy;

	cPositionCollectionPtr m_pPositions;
	cOrderCollectionPtr m_pOrders;
	cTradeCollectionPtr m_pTrades;
	cSignalCollectionPtr m_pSignals;

	cInstMessageMapPtr m_pInstMessageMap;

	int m_nRequestID;
	bool m_runAutoTrade;
	map< cString, double > m_closedPnL;

};

typedef shared_ptr< cTradingPlatform > cTradingPlatformPtr;



#endif
