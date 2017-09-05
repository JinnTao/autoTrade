#include <autotrade.h>
#include <autotrade_backtest.h>
#include <yr_utils.h>
#include <cStrategyMovAvg.h>

//
//void autotrade_backtest()
//{
//	try{
//		sATBacktestConfig configs;
//		autotrade_loadconfig_backtest( configs );
//	
//		tickUnderlyingData tickData;
//		tickTimeUnderlyingData tickTimeData;
//
//		autotrade_backtesting_loadtickdata( configs, tickData, tickTimeData );
//	
//		autotrade_backtesting_simulation( tickData, tickTimeData, configs );
//
//		printf( "\n" );
//		printf( "backtesting finished...\n" );
//	}
//	catch( const char* err )
//	{
//		printf( "\n%s\n", err );
//		cSystem::Sleep(3000);
//	}
//}
//
//
//
//void autotrade_backtesting_loadtickdata( const sATBacktestConfig& configs, tickUnderlyingData& tickData, tickTimeUnderlyingData& tickTimeData )
//{
//	printf( "\n" );
//	printf( "it may take several minutes to load data and pls go for a cup of tea...\n" );
//	printf( "\n" );
//
//	cDate startDt( configs.dateStart );
//	cDate endDt( configs.dateEnd );
//	yr_assert( startDt <= endDt );
//
//	tickUnderlyingData::iterator it1;
//	tickTimeUnderlyingData::iterator it2;
//
//	for( int i = 0; i < configs.generalConfig.underlyings.getSize(); ++i )
//	{
//		cString thisUnderlying = configs.generalConfig.underlyings[i];
//		cArray< cString > fileListIn, fileListOut, instrumentIDs;
//		
//		cString dirName = configs.dataLoadDirectory;
//		dirName += configs.generalConfig.tickDataFolderName + "\\" + thisUnderlying + "\\";
//		
//		ListFileNamesInDirectory( dirName + "*.txt", fileListIn );
//
//		if( fileListIn.getSize() == 0 )
//			continue;
//
//		SortFilesByDate( fileListIn, startDt, endDt, fileListOut, instrumentIDs );
//
//		for( int j = 0; j < fileListOut.getSize(); ++j )
//			fileListOut[j] = dirName + fileListOut[j];
//
//		cArray< cTick > ticks;
//		cArray< cTickTime > tickTimes;
//		LoadTickDataFromFiles( fileListOut, instrumentIDs, ticks, tickTimes, configs.oldFormat, true );
//
//		it1 = tickData.find( configs.generalConfig.underlyings[i] );
//		if( it1 == tickData.end() )
//		{
//			tickData.insert( map< cString, cArray< cTick > >::value_type( thisUnderlying, ticks ) );
//		}
//
//		it2 = tickTimeData.find( thisUnderlying );
//		if( it2 == tickTimeData.end() )
//		{
//			tickTimeData.insert( map< cString, cArray< cTickTime > >::value_type( thisUnderlying, tickTimes ) );
//		}
//
//	}
//}
//
//void autotrade_backtesting_simulation( const tickUnderlyingData& tickData, const tickTimeUnderlyingData& tickTimeData, const sATBacktestConfig& configs )
//{
//	
//	cTickTime start, end;
//	tickUnderlyingData::const_iterator it1;
//	tickTimeUnderlyingData::const_iterator it2 = tickTimeData.begin();
//
//	if( it2 == tickTimeData.end() )
//	{
//		printf("\n");
//		printf( "no data found, goodbye..." );
//		cSystem::Sleep(1000);
//		return;
//	}
//
//	printf( "\n" );
//	printf( "backtesing simulation starts...\n" );
//
//	start = (*it2).second[0];
//	end = (*it2).second.getLast();
//
//	//
//	/* to find the earlist and latest time point for historical simulation
//	   also to
//	   find the traded instruments associated with all the underlyings */
//	map< cString, int > underlyingIdx;
//	map< cString, int >::iterator itr;
//	map< cString, cArray< cString > > underlyingInstrumentMap;
//	map< cString, cArray< cString > >::iterator itr2;
//	for( it1 = tickData.begin(); it1 != tickData.end(); ++it1 )
//	{
//		itr = underlyingIdx.find( (*it1).first );
//		if( itr == underlyingIdx.end() )
//			underlyingIdx.insert( map< cString, int >::value_type( (*it1).first, 0 ) );
//
//		// find the earlist tick time
//		if( (*it1).second[0].TickTime() < start )
//			start = (*it2).second[0];
//
//		// find the latest tick time
//		if( (*it1).second.getLast().TickTime() > end )
//			end = (*it2).second.getLast();
//
//		itr2 = underlyingInstrumentMap.find( (*it1).first );
//		if( itr2 == underlyingInstrumentMap.end() )
//		{
//			cArray< cString > v;
//			v.push_back( (*it1).second[0].InstrumentID() );
//			for( int i = 1; i < (*it1).second.getSize(); ++i )
//			{
//				cString id = (*it1).second[i].InstrumentID();
//				bool inFlag = false;
//				int j;
//				for( j = 0; j < v.getSize(); ++j )
//				{
//					if( Compare( v[j], id ) )
//					{
//						inFlag = true;
//						break;
//					}
//				}
//				if( !inFlag )
//					v.push_back( id );
//			}
//			underlyingInstrumentMap.insert( map< cString, cArray< cString > >::value_type( (*it1).first, v ) );
//		}
//	}
//
//	//
//	/* list all instruments used in the backtesting */
//	cArray< cString > instruments;
//	for( itr2 = underlyingInstrumentMap.begin(); itr2 != underlyingInstrumentMap.end(); ++itr2 )
//	{
//		for( int i = 0; i < (*itr2).second.getSize(); ++i )
//			instruments.push_back( (*itr2).second[i] );
//	}
//	
//	//
//	/* MarketData Engine*/
//	shared_ptr< cMarketDataCollection > pTestMdEngine = make_shared< cMarketDataCollection >();
//	pTestMdEngine->Init( instruments );
//	pTestMdEngine->SetDisplayCandleFlags( configs.generalConfig.displayCandle );
//	pTestMdEngine->SetDisplayTickFlags( true);
//
//	//
//	/* Strategy */
//	cStrategyPtr pStrategy = make_shared< cStrategyMovAvg >( "Mvg" );
//	//pStrategy->SetDisplaySignalFlags( configs.generalConfig.displaySignal );
//	//pStrategy->RegisterMarketDataCollection( pTestMdEngine );
//	//for( int i = 0; i < configs.strategyConfigFileNames.getSize(); ++i )
//	//{
//	//	StrategyParameters param;
//	//	pStrategy->LoadStrategyParameter( configs.strategyConfigFileNames[i], param );
//	//	pStrategy->SetParameters( param );
//	//}
//	
//	//
//	/* Trading platform*/
//	cTradingPlatform* pTestTradingPlatform = new cTradingPlatform();
//	pTestTradingPlatform->RegisterMarketDataEngine( pTestMdEngine );
//	pTestTradingPlatform->RegisterStrategy( pStrategy );
//
//	//
//	/* Threads */
//	shared_ptr< AutoTradeThread > pMdTestIOThread = make_shared< AutoTradeThread >( pTestTradingPlatform, &cTradingPlatform::SimulationIOProcess );
//	shared_ptr< AutoTradeThread > pAutoTradeThread = make_shared< AutoTradeThread >( pTestTradingPlatform, &cTradingPlatform::AutoTrading );
//
//	start = cTickTime( start.DateString().c_str(), start.TimeString().c_str() );
//	double tStart = start.TickTimeToDouble();
//	/* we extend the historical simultion with extra 10 mins after all the markets close*/
//	double tEnd = end.TickTimeToDouble() + 600.0 / SECONDSPERDAY;
//	double dt = 0.5 / SECONDSPERDAY;
//	double interval_start = tStart;
//	double interval_end = interval_start + dt;
//	cTickTime tt_start = cTickTime::DoubleToTickTime( interval_start );
//	cTickTime tt_end = cTickTime::DoubleToTickTime( interval_end );
//	pTestMdEngine->SimulationSetTime( interval_start );
//	pMdTestIOThread->Init();
//	pAutoTradeThread->Init();
//
//	while( interval_start <= tEnd )
//	{
//		pTestMdEngine->SimulationSetTime( interval_start );
//		bool marketClose = true;	
//		for( it2 = tickTimeData.begin(); it2 != tickTimeData.end(); ++it2 )
//		{
//			itr = underlyingIdx.find( (*it2).first );
//			int idx = (*itr).second;
//			int n = (*it2).second.getSize();
//			if( idx < 0 || idx >= n )
//				continue;
//			if( !( (*it2).second[idx] >= tt_start && (*it2).second[idx] < tt_end ) )
//			{
//				if( (*it2).second[idx] < tt_start )
//					(*itr).second += 1;
//				continue;
//			}
//
//			marketClose = false;
//			it1 = tickData.find( (*it2).first );
//			cTick tick = (*it1).second[idx];
//			pTestMdEngine->SimulationUpdate( tick );
//			(*itr).second += 1;
//
//			//check whether the next index is in the current time slot
//			idx += 1;
//			if( idx < 0 || idx >= n )
//				continue;
//			if( ( (*it2).second[idx] >= tt_start && (*it2).second[idx] < tt_end ) )
//				pTestMdEngine->SimulationUpdate( (*it1).second[idx] );
//		}
//
//		interval_start = interval_end;
//		interval_end = interval_start + dt;
//		tt_start = tt_end;
//		tt_end = cTickTime::DoubleToTickTime( interval_end );
//		if( !marketClose )
//			cSystem::Sleep(2);
//
//	}
//
//	pTestTradingPlatform->SetAutoTradeFlag( false );
//	pTestTradingPlatform->ClearPlatform();
//
//}