#include <yr_downloadmarketdata.h>
#include <cString.h>
#include <cSystem.h>
#include <cMdSpi.h>
#include <trading_account.h>
#include <cMarketDataCollection.h>
#include <cThread.h>
//
/* global variables */

//char* ppInstrumentID[] = {
//"IF1604",
//"IH1604",
//"IC1604",
//"TF1606",
//"T1606",
//"rb1610",
//"i1609",
//"a1609",
//"m1609",
//"y1609",
//"p1609",
//"OI609",
//"RM609",
//"CF609",
//"SR609",
//"ru1609",
//"jd1609",
//"l1609",
//"TA609",
//"j1605",
//"jm1605",
//"au1606",
//"ag1606"
//};


//int main( void )
//{
//	//***********************************************************
//	/*
//		running tests
//	*/
//	//RunUtilityTest();
//	//
//	//RunTickTimeTest();
//	//
//	//RunMarketDataTest();
//	//
//	//***********************************************************
//	
//	RunMarketdataDownload();
//	return 0;
//}
//
//void RunMarketdataDownload( const cArray< cString >& InstrumentIDs )
//{
//	char flag[256];
//	printf("run market data process(Y/N)?:");
//	cin >> flag;
//	while( !( Compare( flag, "Y" ) || Compare( flag, "N" ) ) )
//	{
//		printf( "invalid input, either 'Y' or 'N' is allowed. please re-enter:" );
//		cin >> flag;
//	}
//	
//	if( Compare( flag, "N" ) )
//	{
//		printf( "goodbye...\n" );
//		cSystem::Sleep(1000);
//		return;
//	}
//
//	cSystem::FirstTimeInit();
//
//	cMarketDataCollection* pMdEngine = new cMarketDataCollection();
//	//pMdEngine->SetDisplayCandleFlags( true );
//	//
//	/* MDApi & MDSpi */
//	CThostFtdcMdApi* pMdUserApi = CThostFtdcMdApi::CreateFtdcMdApi();
//	CThostFtdcMdSpi* pMdUserSpi = new cMdSpi( pMdUserApi, BROKER_ID, INVESTOR_ID, PASSWORD );
//	pMdUserApi->RegisterSpi( pMdUserSpi );
//	pMdUserApi->RegisterFront( QFRONT_ADDR );
//	dynamic_cast< cMdSpi* >( pMdUserSpi )->RegisterMarketDataCollection( pMdEngine );
//	
//
//	/* other threads */
//	// market data thread
//	cThread< cMarketDataCollection >* pMdIOThread = new cThread< cMarketDataCollection>( pMdEngine, &cMarketDataCollection::IOProcess );
//	
//	pMdUserApi->Init();
//	pMdIOThread->Init();
//	
//	pMdUserApi->Join();
//	pMdUserApi->Release();
//	delete pMdUserSpi;
//	pMdEngine->ClearMarketData();
//
//}
//
