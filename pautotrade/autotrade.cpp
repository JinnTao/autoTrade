#include <autotrade.h>
#include <autotrade_config.h>
#include "easylogging++.h"

// initial easylogging
INITIALIZE_EASYLOGGINGPP

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
		//-------------------------------------easyLogging-----------------------------------------
		el::Configurations conf("conf/easyLog.conf");
		el::Loggers::reconfigureAllLoggers(conf);


		//-------------------------------------读取基本配置---------------------------------------
		AccountParam ctpAccount;
		mongoSetting mongoDbSetting;
		autoSetting autoTradeSetting;
		ParseSettingJson(ctpAccount,mongoDbSetting,autoTradeSetting);
		

		//-------------------------------------行情前置接口--------------------------------------
		/* MDApi & MDSpi */
		CThostFtdcMdApi* pMdUserApi = CThostFtdcMdApi::CreateFtdcMdApi(".\\MDflow\\");// 流控文件 防止阻塞
		cMdSpi* pMdUserSpi = new cMdSpi( pMdUserApi, ctpAccount.brokerId, ctpAccount.userId, ctpAccount.passwd );
		pMdUserApi->RegisterSpi( pMdUserSpi );
		pMdUserApi->RegisterFront( ctpAccount.mdAddress );

		//------------------------------------- 创建数据收集器 --------------------------------------------
		/* cMarketDataCollection */
		cMarketDataCollectionPtr pMdEngine = make_shared< cMarketDataCollection >();
		dynamic_cast< cMdSpi* >( pMdUserSpi )->RegisterMarketDataCollection( pMdEngine.get() );

		//
		/* TraderApi && TraderSpi */
		CThostFtdcTraderApi* pTraderUserApi = CThostFtdcTraderApi::CreateFtdcTraderApi(".\\TDflow\\");
		cTraderSpi* pTraderUserSpi = new cTraderSpi( pTraderUserApi,pMdUserSpi,pMdUserApi, ctpAccount.brokerId, ctpAccount.userId, ctpAccount.passwd );
		pTraderUserApi->RegisterSpi((CThostFtdcTraderSpi*) pTraderUserSpi );
		pTraderUserApi->SubscribePublicTopic( THOST_TERT_RESTART );	// subscribe public topic
		pTraderUserApi->SubscribePrivateTopic( THOST_TERT_QUICK );	// subscribe private topic
		pTraderUserApi->RegisterFront( ctpAccount.tdAddress ); 
		pTraderUserSpi->RegisterMarketDataEngine( pMdEngine );
		//-----------------------------------------人机交互线程---------------------------------------------------------------------------------
		cTradingPlatformPtr pTradingPlatform = make_shared< cTradingPlatform >();
		pTradingPlatform->RegisterMarketDataEngine( pMdEngine );
		pTradingPlatform->RegisterTraderSpi(  pTraderUserSpi);
		pTradingPlatform->RegisterMdSpi(pMdUserSpi);
		pTradingPlatform->initStrategy(autoTradeSetting);

		cThread< cTradingPlatform >* pTradingThread = new cThread< cTradingPlatform >( pTradingPlatform.get(), &cTradingPlatform::AutoTrading );



		pTraderUserApi->Init(); //应首先初始化TD 再进行MD初始化 最后初始化 交易平台 基础思考为获取完账户信息后 再启动行情
		

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