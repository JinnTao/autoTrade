/***
*
*       Copyright (c) Yiran Yang. All rights reserved.
*
****/


#include <test_defs.h>
#include <yr_loaddata.h>
#include <yr_marketdata_test.h>
#include <cVector.h>
#include <cArray.h>
#include <cMarketDataCollection.h>
#include <cSystem.h>
#include <stl_ctp.h>
#include <Windows.h>

HANDLE SimUpdateEvent = CreateEvent( NULL, TRUE, FALSE, NULL );

DWORD WINAPI ProcessLastCandle( LPVOID lpParam );

cMarketDataCollection* mdSimEngine = new cMarketDataCollection();

double t;

void RunMarketDataTest()
{
	char flag[256];
	printf("run market data simulation test (Y/N)?:");
	cin >> flag;
	while( !( Compare( flag, "Y" ) || Compare( flag, "N" ) ) )
	{
		printf( "invalid input, either 'Y' or 'N' is allowed. please re-enter:" );
		cin >> flag;
	}
	
	if( Compare( flag, "Y" ) )
	{
		HANDLE lastCandleThread = CreateThread( NULL, 0, ProcessLastCandle, NULL, 0, NULL );
		MarketDataTestSimulation();
	}
}

void MarketDataTestLoadData( cArray< cString >& instrumentIDs, TickData& tickData, TickTime& tickTime )
{
	//instrumentIDs.push_back( instrument1 );
	//instrumentIDs.push_back( instrument2 );
	instrumentIDs.push_back( instrument3 );
	//instrumentIDs.push_back( instrument4 );

	cArray< cString > tickFiles;
	//tickFiles.push_back( tickFile1 );
	//tickFiles.push_back( tickFile2 );
	tickFiles.push_back( tickFile3 );
	//tickFiles.push_back( tickFile4 );
	
	for( int i = 0; i < instrumentIDs.getSize(); ++i )
	{
		cArray< cTick > ticks;
		cArray< cTickTime > ticktimes;
		printf( "read %s's tick data from file...\n", instrumentIDs[i].c_str() );
		LoadTickDataFromFile( tickFiles[i], instrumentIDs[i], ticks, ticktimes );
		
		
		TickData::iterator it = tickData.find( instrumentIDs[i] );
		if( it == tickData.end() )
		{
			tickData.insert( TickData::value_type( instrumentIDs[i], ticks ) );
			tickTime.insert( TickTime::value_type( instrumentIDs[i], ticktimes ) );
		}
	}
}


void MarketDataTestSimulation()
{
	cout << "run 'MarketDataTestSimulation'..." << endl;

	cArray< cString > instrumentIDs;
	TickData tickData;
	TickTime tickTime;
	MarketDataTestLoadData( instrumentIDs, tickData, tickTime );

	cout << "all tick data loaded..." << endl;

	cSystem::Sleep( 1000 );
	cout << "simulation starts..." << endl;

	/*mdSimEngine->SetDisplayCandleFlag( true );*/
	mdSimEngine->Init( instrumentIDs );

	cTickTime startTT( "20160215", "21:00:00", 0 );
	cTickTime endTT( "20160216", "23:35:00", 0 );

	t = startTT.TickTimeToDouble();
	double tEnd = endTT.TickTimeToDouble();

	while( t < tEnd )
	{
		cTickTime tt = cTickTime::DoubleToTickTime( t );
		bool marketClose = true;
		for( int i = 0; i < instrumentIDs.getSize(); ++i )
		{
			TickTime::iterator ttit = tickTime.find( instrumentIDs[i] );
			if( ttit == tickTime.end() )
				continue;

			int idx = location( (*ttit).second, tt );
			if( idx == 0 && (*ttit).second[idx] > tt )
			{
				// the market has not started for instrumentIDs[i]
				continue;
			}
			int n = (*ttit).second.getSize();
			if( idx == n - 1 && (*ttit).second[idx] < tt )
			{
				// the market has closed for instrumentIDs[i]
				continue;
			}
			if( tt != (*ttit).second[idx] )
				continue;
			
			marketClose = false;
			TickData::iterator it = tickData.find( instrumentIDs[i] );
			cTick tick = (*it).second[idx];
			mdSimEngine->SimulationUpdate( tick );
			SetEvent( SimUpdateEvent );

		}
		t += 0.5 / SECONDSPERDAY;
		// we sleep much shorter in the sumulation run
		// only 5 milliseconds
		if( !marketClose )
			cSystem::Sleep(5);

	}
}

DWORD WINAPI ProcessLastCandle( LPVOID lpParam )
{
	UNREFERENCED_PARAMETER( lpParam );

	while( true )
	{
		DWORD dReturn = WaitForSingleObject( SimUpdateEvent, INFINITE );

		if( WAIT_OBJECT_0 == dReturn )
		{
			mdSimEngine->SimulationRtnLastCandle();
			SetEvent( SimUpdateEvent );
		}
	}
}