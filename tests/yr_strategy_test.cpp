#include <yr_strategy_test.h>
#include <cStrategyMovAvg.h>
#include <yr_marketdata_test.h>
#include <cMarketDataCollection.h>

void RunStrategyTest()
{
	char flag[256];
	printf("run strategy test (Y/N)?:");
	cin >> flag;
	while( !( Compare( flag, "Y" ) || Compare( flag, "N" ) ) )
	{
		printf( "invalid input, either 'Y' or 'N' is allowed. please re-enter:" );
		cin >> flag;
	}
	
	if( Compare( flag, "Y" ) )
	{
		StrategyTestTrainParameters();
	}
}



void StrategyTestTrainParameters()
{
	cout << "run 'StrategyTestTrainParameters'..." << endl;
	cArray< cString > instrumentIDs;
	TickData tickData;
	TickTime tickTime;

	// load market data
	MarketDataTestLoadData( instrumentIDs, tickData, tickTime );
	// init market data engine (collection )
	cMarketDataCollectionPtr pMdTestEngine = make_shared< cMarketDataCollection >();
	pMdTestEngine->Init( instrumentIDs );
	// init strategy for test
	cStrategyPtr pStrategy = make_shared< cStrategyMovAvg >(); 
	pStrategy->Init( instrumentIDs );
	// use EMA for testing
	for( int i = 0; i < instrumentIDs.getSize(); ++i )
		pStrategy->SetParameter( "isEMA", 1.0 );
	// register strategy with market data engine
	pStrategy->RegisterMarketDataCollection( pMdTestEngine );
	
	cArray< cTickTime > ttStart;
	cArray< cTickTime > ttEnd;
	// upload market data engine with tick data
	for( int i = 0; i < instrumentIDs.getSize(); ++i )
	{
		ttStart.push_back( cTickTime( "20160215", "21:00:00", 0 ) );
		ttEnd.push_back( cTickTime( "20160216", "23:30:00", 0 ) );
		TickData::iterator it = tickData.find( instrumentIDs[i] );
		for( int j = 0; j < (*it).second.getSize(); ++j )
			pMdTestEngine->SimulationUpdate( (*it).second[j] );
	}
	// train parameters
	pStrategy->Train( ttStart, ttEnd );

}