#include <autotrade.h>
#include <autotrade_config.h>
#include <autotrade_downloadmarketdata.h>


void autotrade_downloadmarketdata()
{
	try{
		printf( "\n" );
		printf( "running process to download market data...\n" );
	
		sATDownloadMarketDataConfig configs;
		autotrade_loadconfig_downloadmarketdata( configs );

		cArray< cString > instruments;
		ListFirstInstruments( configs.generalConfig.underlyings, instruments );
		int nCount = instruments.getSize();
		if( nCount <= 0 )
			return;

		if( configs.generalConfig.genLog )
		{
			cString fullDir( configs.generalConfig.dataoutputDirectory );
			fullDir += configs.generalConfig.logFileFolderName + "\\" + configs.logFileName;
			cSystem::CreateLogFile( fullDir.c_str() );
			char message[256];
			sprintf( message, "%s:start running process to download market data.", cSystem::GetCurrentTimeBuffer().c_str() );
			cString logFileFolder = configs.generalConfig.dataoutputDirectory + configs.generalConfig.logFileFolderName + "\\";
			cSystem::WriteLogFile( logFileFolder.c_str() , configs.logFileName.c_str(), message, true );
		}

		//
		/* MDApi & MDSpi */
		CThostFtdcMdApi* pMdUserApi = CThostFtdcMdApi::CreateFtdcMdApi();
		CThostFtdcMdSpi* pMdUserSpi = new cMdSpi( pMdUserApi, BROKER_ID_DEMO, INVESTOR_ID_DEMO, PASSWORD_DEMO );
		pMdUserApi->RegisterFront( QFRONT_ADDR_DEMO );
		pMdUserApi->RegisterSpi( pMdUserSpi );
	
		// cMarketDataCollection
		cMarketDataCollectionPtr pMdEngine = make_shared< cMarketDataCollection >();
		dynamic_cast< cMdSpi* >( pMdUserSpi )->RegisterMarketDataCollection( pMdEngine.get() );

		//pMdEngine->SetDisplayTickFlags( configs.generalConfig.displayTick );
		//pMdEngine->SetDisplayCandleFlags( configs.generalConfig.displayCandle );
		//pMdEngine->SetOutputDirectory( configs.generalConfig.dataoutputDirectory );
		//pMdEngine->SetTickDataFolder( configs.generalConfig.tickDataFolderName );
		//pMdEngine->SetCandleDataFolder( configs.generalConfig.candleDataFolderName );

		//if( configs.generalConfig.genLog )
		//{
		//	pMdEngine->SetGenLogFlag( true );
		//	pMdEngine->SetOutputDirectory( configs.generalConfig.dataoutputDirectory );
		//	pMdEngine->SetLogFileFolder( configs.generalConfig.logFileFolderName );
		//	pMdEngine->SetLogFileName( configs.logFileName );	
		//}

		// market data thread
		cThread< cMarketDataCollection >* pMdIOThread = new cThread< cMarketDataCollection>( pMdEngine.get(), &cMarketDataCollection::IOProcess );
	
		pMdUserApi->Init();
		pMdIOThread->Init();
	
		pMdUserApi->Join();
		pMdUserApi->Release();
	
		delete pMdIOThread;

	}
	catch( const char* err )
	{
		printf( "\n%s\n", err );
		exit( 1 );
	}
}
