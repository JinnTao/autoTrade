#include <yr_tradingplatform_test.h>
#include <yr_marketdata_test.h>
#include <cTradingPlatform.h>
#include <cMarketDataCollection.h>
#include <cStrategyMovAvg.h>
#include <cSystem.h>
#include <cThread.h>

void RunTradingPlatformTest()
{
	char flag[256];
	printf("run tradingplatform test (Y/N)?:");
	cin >> flag;
	while( !( Compare( flag, "Y" ) || Compare( flag, "N" ) ) )
	{
		printf( "invalid input, either 'Y' or 'N' is allowed. please re-enter:" );
		cin >> flag;
	}
	
	if( Compare( flag, "Y" ) )
	{
		TradingPlatformTestTradingSimulation();
	}
}

void TradingPlatformTestTradingSimulation()
{
	cout << "run 'TradingPlatformTestTradingSimulation'..." << endl;
	cArray< cString > instrumentIDs;
	TickData tickData;
	TickTime tickTime;

	cTickTime startTT( "20160215", "21:00:00", 0 );
	cTickTime endTT( "20160216", "23:35:00", 0 );
	double tStart = startTT.TickTimeToDouble();
	double tEnd = endTT.TickTimeToDouble();

	// load market data
	MarketDataTestLoadData( instrumentIDs, tickData, tickTime );

	// init market data engine (collection )
	cMarketDataCollectionPtr pMdTestEngine = make_shared< cMarketDataCollection >();
	pMdTestEngine->Init( instrumentIDs );
	/*pMdTestEngine->SetDisplayCandleFlag( false );*/
	pMdTestEngine->SimulationSetTime( tStart );
	cThread< cMarketDataCollection >* pMdIOThread = new cThread< cMarketDataCollection>( pMdTestEngine.get(), &cMarketDataCollection::SimulationIOProcess );
	pMdIOThread->Init();

	// init strategy for test
	cStrategyPtr pStrategyTest = make_shared< cStrategyMovAvg >(); 
	pStrategyTest->Init( instrumentIDs );
	// use default parameters for the movavg strategy
	for( int i = 0; i < instrumentIDs.getSize(); ++i )
	{
		pStrategyTest->SetParameter( "EMAFLAG", 1.0 );
		pStrategyTest->SetParameter( "LEAD", 1.0 );
		pStrategyTest->SetParameter( "LAG", 5.0 );
	}
	// register strategy with market data engine
	pStrategyTest->RegisterMarketDataCollection( pMdTestEngine );

	// trading platform
	cTradingPlatform* pTraderTestEngine = new cTradingPlatform();
	pTraderTestEngine->RegisterMarketDataEngine( pMdTestEngine );
	pTraderTestEngine->RegisterStrategy( pStrategyTest );


	/*
		main thread of simulation
	*/
	double t = tStart;
	while( t < tEnd )
	{
		pMdTestEngine->SimulationSetTime( t );
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
			bool simUpdateFlag = pTraderTestEngine->SimulationUpdate( tick );
			if( simUpdateFlag )
			{
				pTraderTestEngine->SendNewOrders( tick.InstrumentID() );
				/*pTraderTestEngine->PrintPositionSummary();
				printf( "\n" );*/
				pTraderTestEngine->PrintClosePnL();
			}

		}

		t += 0.5 / SECONDSPERDAY;
		if( !marketClose )
		{
			
			cSystem::Sleep(5);
		}
	}
	cout << "'TradingPlatformTestTradingSimulation done'..." << endl;

}