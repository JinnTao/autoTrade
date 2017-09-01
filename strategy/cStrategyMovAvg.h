#ifndef __CSTRATEGYMOVAVG_H__
#define __CSTRATEGYMOVAVG_H__

#include <cStrategy.h>

/*
Strategy description:
	@@
	two expoentially moving average are used, i.e. fast and slow
	BUY - when the fast moves above the slow
	SELL - when the fast moves below the slow

	parameters:
		-> lead, i.e. the fast moving average
		-> lag,	i.e. the slow moving average
		-> alpha, i.e. different type of moving average
		-> emaflag, i.e. expoential moving average flag
		-> sampleFrequency

	additional parameters used for calibration
		-> maxLead
		-> maxLag
		-> leadInterval
		-> lagInterval
*/

typedef enum SwStrategyMovAvgParameter
{
	SWSTRATEGYMOVAVGPARAMETER_INVALID = -1,
	SWSTRATEGYMOVAVGPARAMETER_LEAD,
	SWSTRATEGYMOVAVGPARAMETER_LAG,
	SWSTRATEGYMOVAVGPARAMETER_ALPHA,
	SWSTRATEGYMOVAVGPARAMETER_EMAFLAG,
	SWSTRATEGYMOVAVGPARAMETER_SAMPLEFREQUENCY,
	SWSTRATEGYMOVAVGPARAMETER_MAXLEAD,
	SWSTRATEGYMOVAVGPARAMETER_MAXLAG,
	SWSTRATEGYMOVAVGPARAMETER_LEADINTERVAL,
	SWSTRATEGYMOVAVGPARAMETER_LAGINTERVAL
} _SwStrategyMovAvgParameter;

inline int IsValidStrategyMovAvgParameter( int param )
{
	return( param >= SWSTRATEGYMOVAVGPARAMETER_LEAD && param <= SWSTRATEGYMOVAVGPARAMETER_LAGINTERVAL );
}

cString StrategyMovAvgParameterToString( const SwStrategyMovAvgParameter& param );
SwStrategyMovAvgParameter StringToStrategyMovAvgParameter( const cString& str );

typedef enum SwStrategyMovAvgIndicator
{
	SWSTRATEGYMOVAVGINDICATOR_INVALID = -1,
	SWSTRATEGYMOVAVGINDICATOR_MALEAD,
	SWSTRATEGYMOVAVGINDICATOR_MALAG,
} _SwStrategyMovAvgIndicator;

inline int IsValidStrategyMovAvgIndicator( int indicator )
{
	return( indicator >= SWSTRATEGYMOVAVGINDICATOR_MALEAD && indicator <= SWSTRATEGYMOVAVGINDICATOR_MALAG );
}

cString StrategyMovAvgIndicatorToString( const SwStrategyMovAvgIndicator& indicator );


class cStrategyMovAvg : public cStrategy
{
public:
	cStrategyMovAvg();
	cStrategyMovAvg( const cString& );
	cStrategyMovAvg( const cString&, const cArray< cString >& );
	virtual ~cStrategyMovAvg();

	virtual void Init( const cArray< cString >& );
	virtual void Train( const cArray< cTickTime >&, const cArray< cTickTime>& );
	virtual void BackTest( const cArray< cTickTime >&, const cArray< cTickTime >&, cArray< cStrategyResults>& );
	virtual bool RealTimeIndicatorSignalUpdate( const cString& instrumentID );
	virtual bool SimTimeIndicatorSignalUpdate( const cTick& tick );
	virtual cArray< cOrder* > GenNewOrders( const cString& instrumentID, cPositionCollection* positions );
	virtual bool GenNewOrders( cPositionCollection* positions );


	virtual void CancelPendingOrders( const cString& instrumentID, cOrderCollection* orders, cIvector& orderIDs );

	virtual void LoadStrategyParameter( const cString& fileName);


private:
	void calcPnL( double lead, double lag, double alpha, double isEMA, const cVector& px, cVector& pnl );
};

#endif
