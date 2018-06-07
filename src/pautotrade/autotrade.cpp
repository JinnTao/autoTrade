#include <autotrade.h>
#include <autotrade_config.h>
#include "easylogging++.h"
#include "global.h"
// initial easylogging
INITIALIZE_EASYLOGGINGPP

HANDLE g_hEvent;//事件句柄

int iRequestID = 0;//订单编号

#define  ROHON  0

namespace global{

    volatile std::sig_atomic_t is_running;

}  // namespace

extern "C" void signal_handler(int signal) {
    LOG(INFO) << "Detect signal: " << signal << endl;
    global::is_running = false;
}


int main(int32 argc,char ** argv)
{
    try{
        global::is_running = true;
        std::signal(SIGTERM, signal_handler); // program termination 
        std::signal(SIGINT, signal_handler); // interrupt by user

        //-------------------------------------读取基本配置---------------------------------------
        AccountParam ctpAccount;
        mongoSetting mongoDbSetting;
        autoSetting autoTradeSetting;
        ParseSettingJson(ctpAccount, mongoDbSetting, autoTradeSetting);

        //-------------------------------------行情前置接口--------------------------------------
        /* MDApi & MDSpi */
        CThostFtdcMdApi* pMdUserApi = CThostFtdcMdApi::CreateFtdcMdApi(".\\MDflow\\");// 流控文件 防止阻塞
        cMdSpi* pMdUserSpi = new cMdSpi(pMdUserApi, ctpAccount.brokerId, ctpAccount.userId, ctpAccount.passwd);
        pMdUserApi->RegisterSpi(pMdUserSpi);
        pMdUserApi->RegisterFront(ctpAccount.mdAddress);

        //------------------------------------- 创建数据收集器 --------------------------------------------
        /* cMarketDataCollection */
        cMarketDataCollectionPtr pMdEngine = make_shared< cMarketDataCollection >();
        dynamic_cast< cMdSpi* >(pMdUserSpi)->RegisterMarketDataCollection(pMdEngine.get());

        //
        /* TraderApi && TraderSpi */
        CThostFtdcTraderApi* pTraderUserApi = CThostFtdcTraderApi::CreateFtdcTraderApi(".\\TDflow\\");
        cTraderSpi* pTraderUserSpi = new cTraderSpi(pTraderUserApi, pMdUserSpi, pMdUserApi, ctpAccount.brokerId, ctpAccount.userId, ctpAccount.passwd);
        pTraderUserApi->RegisterSpi((CThostFtdcTraderSpi*)pTraderUserSpi);
        pTraderUserApi->SubscribePublicTopic(THOST_TERT_RESTART);    // subscribe public topic
        pTraderUserApi->SubscribePrivateTopic(THOST_TERT_QUICK);    // subscribe private topic
        pTraderUserApi->RegisterFront(ctpAccount.tdAddress);

        //-----------------------------------------人机交互线程---------------------------------------------------------------------------------
        cTradingPlatformPtr pTradingPlatform = make_shared< cTradingPlatform >();
        pTradingPlatform->RegisterMarketDataEngine(pMdEngine);
        pTradingPlatform->RegisterTraderSpi(pTraderUserSpi);
        pTradingPlatform->RegisterMdSpi(pMdUserSpi);
        pTradingPlatform->RegisterParameters(&autoTradeSetting, &mongoDbSetting);

        cThread< cTradingPlatform >* pTradingThread = new cThread< cTradingPlatform >(pTradingPlatform.get(), &cTradingPlatform::AutoTrading);



        pTraderUserApi->Init(); //应首先初始化TD 再进行MD初始化 最后初始化 交易平台 基础思考为获取完账户信息后 再启动行情

        while (true) {
            if (pMdUserSpi->getSatus()) {
                pTradingThread->Init();
                break;
            }
        } // 防止Join无法停住，主线程退出

          //等待线程退出
        pTraderUserApi->Join();
        while (true) {}

        while (global::is_running) {
            if (global::need_reconnect.load(std::memory_order_relaxed)) {
            }
        
        
        }




    }
    catch (exception e)
    {
        printf("\n%s\n", e.what());
        exit(1);
    }



    autotrade_trade();

    return 0;
}

void autotrade_trade()
{
    try
    {
        printf( "\n" );
        printf( "running process to automatically trade with self-defined strategies...\n" );


        //std::cout << "1" << std::endl;
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
        pTraderUserApi->SubscribePublicTopic( THOST_TERT_RESTART );    // subscribe public topic
        pTraderUserApi->SubscribePrivateTopic( THOST_TERT_QUICK );    // subscribe private topic
        pTraderUserApi->RegisterFront( ctpAccount.tdAddress ); 

        //-----------------------------------------人机交互线程---------------------------------------------------------------------------------
        cTradingPlatformPtr pTradingPlatform = make_shared< cTradingPlatform >();
        pTradingPlatform->RegisterMarketDataEngine( pMdEngine );
        pTradingPlatform->RegisterTraderSpi(  pTraderUserSpi);
        pTradingPlatform->RegisterMdSpi(pMdUserSpi);
        pTradingPlatform->RegisterParameters(&autoTradeSetting,&mongoDbSetting);

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
    catch(exception e )
    {
    printf( "\n%s\n", e.what() );
    exit( 1 );
    }
}