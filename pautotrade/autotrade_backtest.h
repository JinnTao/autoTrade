#ifndef __AUTOTRADE_BACKTESTING_H__
#define __AUTOTRADE_BACKTESTING_H__

#include <autotrade_config.h>
class cTick;
class cTickTime;

typedef map< cString, cArray< cTick > > tickUnderlyingData;
typedef map< cString, cArray< cTickTime >> tickTimeUnderlyingData;

void autotrade_backtest();

void autotrade_backtesting_loadtickdata( const sATBacktestConfig& configs, tickUnderlyingData& tickData, tickTimeUnderlyingData& tickTimeData );

void autotrade_backtesting_simulation( const tickUnderlyingData& tickData, const tickTimeUnderlyingData& tickTimeData, const sATBacktestConfig& configs  );

void autotrade_backtesting_trainstrategy( const cArray< cString >& underlyings, const cString& start, const cString& end );

#endif
