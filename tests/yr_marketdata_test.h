#ifndef __YR_MARKETDATA_TEST_H__
#define __YR_MARKETDATA_TEST_H__

#include <map>
#include <cTick.h>

template< class T > class cArray;

typedef map< cString, cArray< cTick > > TickData;
typedef map< cString, cArray< cTickTime > > TickTime;

void RunMarketDataTest();

void MarketDataTestLoadData( cArray< cString >&, TickData&, TickTime& );

// simulate the real market data environment
void MarketDataTestSimulation();




#endif
