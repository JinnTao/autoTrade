#include <autotrade.h>
#include <autotrade_downloadmarketdata.h>
#include <autotrade_backtest.h>
#include <cStrategyMovAvg.h>
#include <autotrade_config.h>

//#pragma comment(lib,"json_vc71_libmtd.lib")

HANDLE g_hEvent;//事件句柄

int iRequestID = 0;//订单编号

int main()
{

	autotrade_trade();

	return 0;
	
}

void autotrade_trade()
{
	try
	{
		printf( "\n" );
		printf( "running process to automatically trade with self-defined strategies...\n" );
		
		//-------------------------------------读取基本配置---------------------------------------
		AccountParam ctpAccount;
		mongoSetting mongoDbSetting;
		ParseSettingJson(ctpAccount,mongoDbSetting);
		

		//-------------------------------------行情前置接口--------------------------------------
		/* MDApi & MDSpi */
		CThostFtdcMdApi* pMdUserApi = CThostFtdcMdApi::CreateFtdcMdApi(".\\MDflow\\");// 流控文件 防止阻塞
		//CThostFtdcMdSpi* pMdUserSpi = new cMdSpi( pMdUserApi, instruments, BROKER_ID_DEMO, INVESTOR_ID_DEMO, PASSWORD_DEMO );
		cMdSpi* pMdUserSpi = new cMdSpi( pMdUserApi, ctpAccount.brokerId, ctpAccount.userId, ctpAccount.passwd );
		pMdUserApi->RegisterSpi( pMdUserSpi );
		pMdUserApi->RegisterFront( ctpAccount.mdAddress );

		//-------------------------------------创建数据收集器--------------------------------------------
		/* cMarketDataCollection */
		cMarketDataCollectionPtr pMdEngine = make_shared< cMarketDataCollection >();
		dynamic_cast< cMdSpi* >( pMdUserSpi )->RegisterMarketDataCollection( pMdEngine.get() );
		//
		/* Strategy */
		
		cStrategyPtr pStrategy = make_shared< cStrategyMovAvg >( "MovAvg" );
		pStrategy->RegisterMarketDataCollection( pMdEngine );

		//
		/* TraderApi && TraderSpi */
		CThostFtdcTraderApi* pTraderUserApi = CThostFtdcTraderApi::CreateFtdcTraderApi(".\\TDflow\\");
		cTraderSpi* pTraderUserSpi = new cTraderSpi( pTraderUserApi,pMdUserSpi,pMdUserApi, ctpAccount.brokerId, ctpAccount.userId, ctpAccount.passwd );
		//CThostFtdcTraderSpi* pTraderUserSpi = new cTraderSpi( pTraderUserApi, NULL, BROKER_ID_DEMO, INVESTOR_ID_DEMO, PASSWORD_DEMO );
		pTraderUserApi->RegisterSpi((CThostFtdcTraderSpi*) pTraderUserSpi );
		pTraderUserApi->SubscribePublicTopic( THOST_TERT_RESTART );	// subsribe public topic
		pTraderUserApi->SubscribePrivateTopic( THOST_TERT_QUICK );	// subscribe private topic
		pTraderUserApi->RegisterFront( ctpAccount.tdAddress ); 

		//-----------------------------------------人机交互线程---------------------------------------------------------------------------------
		cTradingPlatformPtr pTradingPlatform = make_shared< cTradingPlatform >();
		pTradingPlatform->RegisterMarketDataEngine( pMdEngine );
		pTradingPlatform->RegisterTraderSpi(  pTraderUserSpi);
		pTradingPlatform->RegisterMdSpi(pMdUserSpi);
		pTradingPlatform->RegisterStrategy( pStrategy );

		cThread< cTradingPlatform >* pTradingThread = new cThread< cTradingPlatform >( pTradingPlatform.get(), &cTradingPlatform::AutoTrading );



		//pMdUserApi->Init();
		pTraderUserApi->Init(); //应首先初始化TD 再进行MD初始化 最后初始化 交易平台 基础思考为获取完账户信息后 再启动行情


		//pTradingThread->Init();
		

		while(true){
			if(pMdUserSpi->getSatus()){
				pTradingThread->Init();
				break;
			}
		} // 防止Join无法停住，主线程退出

		//等待线程退出
		pTraderUserApi->Join();
		while(true){}
	}
	catch( const char* err )
	{
		printf( "\n%s\n", err );
		exit( 1 );
	}
}
//
//void autotrade_trade()
//{
//	try
//	{
//		printf( "\n" );
//		printf( "running process to automatically trade with self-defined strategies...\n" );
//		
//
//		AccountParam ctpAccount;
//		mongoSetting mongoDbSetting;
//		ParseSettingJson(ctpAccount,mongoDbSetting);
//
//		system("pause");
//
//		sATTradeConfig configs;
//
//		autotrade_loadconfig_trade( configs );
//
//		cArray< cString > instruments;
//		ListFirstInstruments( configs.generalConfig.underlyings, instruments );
//		int nCount = instruments.getSize();
//		if( nCount < 1 )
//		{
//			printf( "\nno tradable instrument found...\n" );
//			return;
//		}
//
//		map< cString, cArray< cString > > underlyingInstrumentMap;
//		map< cString, cArray< cString > >::iterator it;
//		for( int i = 0; i < configs.generalConfig.underlyings.getSize(); ++i )
//		{
//			cString thisUnderlying = configs.generalConfig.underlyings[i];
//			cArray< cString > v;
//			for( int j = 0; j < instruments.getSize(); ++j )
//			{
//				cString underlying_j;
//				InstrumentToUnderlying( instruments[j], underlying_j );
//				if( Compare( thisUnderlying, underlying_j ) )
//					v.push_back( instruments[j] );
//			}
//			it = underlyingInstrumentMap.find( thisUnderlying );
//			if( it == underlyingInstrumentMap.end() )
//			{
//				underlyingInstrumentMap.insert( map< cString, cArray< cString > >::value_type( thisUnderlying, v ) );
//			}
//		}
//
//		if( configs.generalConfig.genLog )
//		{
//			cString fullDir( configs.generalConfig.dataoutputDirectory );
//			if( !configs.logFileName.IsBlankString() )
//				fullDir += configs.generalConfig.logFileFolderName + "\\" + configs.logFileName;
//			else
//				yr_error( "invalid LogFileName input!" );
//			
//			cSystem::CreateLogFile( fullDir.c_str() );
//			
//			char message[256];
//			sprintf( message, "%s:start running process to automatically trade with self-defined strategies.", cSystem::GetCurrentTimeBuffer().c_str() );
//			cString logFileFolder = configs.generalConfig.dataoutputDirectory + configs.generalConfig.logFileFolderName + "\\";
//			cSystem::WriteLogFile( logFileFolder.c_str() , configs.logFileName.c_str(), message, true );
//		}
//		
//		
//		//
//		/* MDApi & MDSpi */
//		CThostFtdcMdApi* pMdUserApi = CThostFtdcMdApi::CreateFtdcMdApi();
//		//CThostFtdcMdSpi* pMdUserSpi = new cMdSpi( pMdUserApi, instruments, BROKER_ID_DEMO, INVESTOR_ID_DEMO, PASSWORD_DEMO );
//		CThostFtdcMdSpi* pMdUserSpi = new cMdSpi( pMdUserApi, instruments, ctpAccount.brokerId, ctpAccount.userId, ctpAccount.passwd );
//		pMdUserApi->RegisterSpi( pMdUserSpi );
//		pMdUserApi->RegisterFront( ctpAccount.mdAddress );
//
//		//
//		/* cMarketDataCollection */
//		cMarketDataCollectionPtr pMdEngine = make_shared< cMarketDataCollection >();
//		dynamic_cast< cMdSpi* >( pMdUserSpi )->RegisterMarketDataCollection( pMdEngine.get() );
//		pMdEngine->SetDisplayTickFlags( configs.generalConfig.displayTick );
//		pMdEngine->SetDisplayCandleFlags( configs.generalConfig.displayCandle );
//		pMdEngine->SetOutputDirectory( configs.generalConfig.dataoutputDirectory );
//		pMdEngine->SetTickDataFolder( configs.generalConfig.tickDataFolderName );
//		pMdEngine->SetCandleDataFolder( configs.generalConfig.candleDataFolderName );
//
//		if( configs.generalConfig.genLog )
//		{
//			pMdEngine->SetGenLogFlag( true );
//			pMdEngine->SetOutputDirectory( configs.generalConfig.dataoutputDirectory );
//			pMdEngine->SetLogFileFolder( configs.generalConfig.logFileFolderName );
//			pMdEngine->SetLogFileName( configs.logFileName );	
//		}
//
//		//
//		/* Strategy */
//		
//		cStrategyPtr pStrategy = CreateStrategy( "movavg", instruments );
//		pStrategy->SetDisplaySignalFlags( configs.generalConfig.displaySignal );
//		StrategyParameters param;
//		for( int i = 0; i < configs.strategyConfigFileNames.getSize(); ++i )
//		{
//			StrategyParameters param;
//			pStrategy->LoadStrategyParameter( configs.strategyConfigFileNames[i], param );
//			pStrategy->SetParameters( param );
//		}
//		pStrategy->RegisterMarketDataCollection( pMdEngine );
//
//		//
//		/* TraderApi && TraderSpi */
//		CThostFtdcTraderApi* pTraderUserApi = CThostFtdcTraderApi::CreateFtdcTraderApi();
//		CThostFtdcTraderSpi* pTraderUserSpi = new cTraderSpi( pTraderUserApi, instruments, BROKER_ID_DEMO, INVESTOR_ID_DEMO, PASSWORD_DEMO );
//		pTraderUserApi->RegisterSpi( pTraderUserSpi );
//		pTraderUserApi->SubscribePrivateTopic( THOST_TERT_QUICK );	// subscribe private topic
//		pTraderUserApi->SubscribePublicTopic( THOST_TERT_QUICK );	// subsribe public topic
//		pTraderUserApi->RegisterFront( TFRONT_ADDR_DEMO );
//		
//		
//		//
//		/* Trading platform */
//		cTradingPlatformPtr pTradingPlatform = make_shared< cTradingPlatform >();
//		pTradingPlatform->RegisterMarketDataEngine( pMdEngine );
//		pTradingPlatform->RegisterTraderSpi( dynamic_cast< cTraderSpi* >( pTraderUserSpi ) );
//		pTradingPlatform->RegisterStrategy( pStrategy );
//		//
//		/* other threads */
//		// market data thread
//		cThread< cTradingPlatform >* pIOThread = new cThread< cTradingPlatform>( pTradingPlatform.get(), &cTradingPlatform::IOProcess );
//		//
//		// automatic trading thread
//		cThread< cTradingPlatform >* pTradingThread = new cThread< cTradingPlatform >( pTradingPlatform.get(), &cTradingPlatform::AutoTrading );
//		//cThread< cTradingPlatform >* pTradingPlatformTestThread = new cThread< cTradingPlatform >( pTradingPlatform , &cTradingPlatform::ProcessOrderTest );
//	
//		pMdUserApi->Init();
//		pTraderUserApi->Init();
//		pIOThread->Init();
//		pTradingThread->Init();
//		//pTradingPlatformTestThread->Init();
//	
//		pTraderUserApi->Join();
//
//		pMdUserApi->Release();
//		pTraderUserApi->Release();
//		delete pMdUserSpi;
//		delete pTraderUserSpi;
//		pMdEngine->ClearMarketData();
//		pIOThread->Interrupt();
//		pTradingThread->Interrupt();
//	}
//	catch( const char* err )
//	{
//		printf( "\n%s\n", err );
//		exit( 1 );
//	}
//}
	