#ifndef __CSTRATEGY_H__
#define __CSTRATEGY_H__

#include <cString.h>
#include <map>
#include <cSignal.h>
#include <Windows.h>

#include <cMarketDataCollection.h>
#include <cSignalCollection.h>

#include <cTrade.h>
#include <cPositionCollection.h>
#include <cOrderCollection.h>

template< class T > class cArray;

struct cStrategyResults
{
	double pnl;
	double drawdown;	//maximum pnl
	double peak;		//peak pnl
	double sharpratio;

};

//
class cStrategy
{
private:
	// non-copyable
	cStrategy( const cStrategy& ) {}
	cStrategy& operator = ( const cStrategy& ) {}

public:
	cStrategy();
	cStrategy( const cString& );
	cStrategy( const cString& strategyID, const cArray< cString >& instrumentIDs );
	
	virtual ~cStrategy();

	virtual void Init( const cArray< cString >& );
	
	// train the strategy to optimize the strategy parameters
	virtual void Train( const cArray< cTickTime >&, const cArray< cTickTime >& ) = 0;
	
	// backtest the strategy with the optimized strategy parameters
	virtual void BackTest( const cArray< cTickTime >&, const cArray< cTickTime >&, cArray< cStrategyResults>& ) = 0;
	
	// realtime strategy indicator & signal update
	virtual bool RealTimeIndicatorSignalUpdate( const cString& instrumentID ) = 0;
	
	// trading simulation time strategy indicator & signal update
	virtual bool SimTimeIndicatorSignalUpdate( const cTick& tick ) = 0;
	
	// generate new orders given latest signals and positions
	virtual cArray< cOrder* > GenNewOrders( const cString& instrumentID, cPositionCollection* positions ) = 0;
	virtual bool GenNewOrders( cPositionCollection* positions ) = 0;
	
	// cancel existing orders specified by different strategies
	virtual void CancelPendingOrders( const cString& instrumentID, cOrderCollection* orders, cIvector& orderIDs ) = 0;
	
	// load parameters from file
	virtual void LoadStrategyParameter( const cString& fileName) = 0;

	// calculate running position PnL
	double PositionPnL( cPositionCollection* positions );

	virtual void onInit(){};

	virtual void onStart(){};

	virtual void onStop(){};

	virtual void onTick(){};

	virtual void onBar(){};

	virtual void onOrder(cOrderPtr	){};

	virtual void onTrade(cTradePtr ){};


	// ***************************************************************************
	void RegisterMarketDataCollection( cMarketDataCollectionPtr p );

	void RegisterSignalCollection( cSignalCollectionPtr p )
	{
		if( m_signals.get() )
			m_signals.reset();

		m_signals = p;
	}

	// get method
	cString GetStrategyID() const { return m_strategyID; }
	void GetInstrumentIDs( cArray< cString >& instrumentIDs ) const;

	double GetClosePnL() const { return m_closePnL; }

	//
	double GetParameter( const cString& paramName ) const;
	double GetIndicator( const cString& indName ) const;

	cArray< const cSignal* > GetSignals( const cString& instrumentID ) const;
	const cSignal* GetLastSignalConstPtr( const cString& instrumentID ) const;
	cSignal* GetLastSignal( const cString& instrumentID );
	cSignalPtr GetLastSignalSharedPtr( const cString& instrumentID );
	
	int GetPXLastUsedIndex() const { return m_pxidx; }
	void SetPXLastUsedIndex( int i ) { m_pxidx = i; }

	bool GetDisplaySignalFlag( const cString& ) const;
	void SetDisplaySignalFlag( const cString&, bool flag );
	void SetDisplaySignalFlags( const map< cString, bool >& flags );

	void UpdateClosePnL( double pnl ) { m_closePnL += pnl; }

	//
	bool HasInstrument( const cString& instrumentID ) const
	{
		return( m_instrumentIDs.find( instrumentID ) != m_instrumentIDs.end() );
	}

	//
	void AddInstrument( const cString& instrumentID )
	{
		if( !HasInstrument( instrumentID ) )
			m_instrumentIDs.push_back( instrumentID );
	}

	//
	void DropInstrument( const cString& instrumentID )
	{
		if( !HasInstrument( instrumentID ) )
			return;

		int i;
		for( i = 0; i < m_instrumentIDs.getSize(); ++i )
		{
			if( Compare( instrumentID, m_instrumentIDs[i] ) )
				break;
		}
		m_instrumentIDs.drop( i );
	}

	void SetParameter( const cString& paramName, double paramValue );
	void SetIndicator( const cString& indName, double indValue );
	void SetSignal( cSignalPtr signal );
	
	
protected:
	cArray< cString > m_instrumentIDs;
	cVector m_weights;

	cMarketDataCollectionPtr m_mc;

	cSignalCollectionPtr m_signals;	// strategy signals

	int m_pxidx;		// the index of price used for generating signal

	map< cString, bool > m_displaySignal;	// flag to display the signal

	double m_closePnL;					// Profit & Close for closed positions

private:
	cString m_strategyID;

};

typedef shared_ptr< cStrategy > cStrategyPtr;

#endif

