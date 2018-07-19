#include <autotrade.h>
#include <autotrade_config.h>
#include "easylogging++.h"
#include "global.h"
#include <future>
#include <chrono>
#include <thread>

INITIALIZE_EASYLOGGINGPP
#define ELPP_THREAD_SAFE
#define TRADE_CONFIG_FILE "setting.ini"
#define LOG_CONFIG_FILE "easyLog.conf"

HANDLE g_hEvent;
int    iRequestID;

extern "C" void signal_handler(int signal) {
    LOG(INFO) << "Detect signal: " << signal << endl;
    global::is_running = false;
}

int main(int32 argc, char** argv) {
    try {
        global::is_running = true;

        el::Configurations conf(LOG_CONFIG_FILE);
        el::Loggers::reconfigureAllLoggers(conf);

        std::signal(SIGTERM, signal_handler);  // program termination
        std::signal(SIGINT, signal_handler);   // interrupt by user

        auto  trader = std::make_unique<cTradingPlatform>();
        int32 result = 0;

        result = trader->loadConfig(TRADE_CONFIG_FILE);
            if (result != 0) {
                LOG(ERROR) << "Trader load config failed! Result: " << result;
                return -1;
            }
            LOG(INFO) << "Trader load config success!";

            result = trader->createPath();
            if (result != 0) {
                LOG(ERROR) << "Trader create path failed! Result: " << result;
                return -2;
            }
            LOG(INFO) << "Trader create path success!";

            result = trader->init();
            if (result != 0) {
                LOG(ERROR) << "Trader init failed! Result: " << result;
                return -3;
            }
            LOG(INFO) << "Trader init success!";

            result = trader->start();
            if (result != 0) {
                LOG(ERROR) << "Trader start failed! Result: " << result;
                return -4;
            }
            //LOG(INFO) << "Trader start success!";

            while (global::is_running) {
                if (global::need_reconnect.load(std::memory_order_relaxed)) {
                    auto result = trader->reConnect();
                    if (result == 0) {
                        LOG(INFO) << "Trader reconnect success!";
                    }
                }
                using namespace std::chrono_literals;
                std::this_thread::sleep_for(10s);
            }
            LOG(INFO) << "trader try stop.";
            result = trader->stop();
            LOG(INFO) << "trader stop, exited! Result:" << result;
        return result;
    } catch (exception e) {
        LOG(ERROR) << "error:" << e.what();
        exit(1);
    } catch (...) {
        LOG(ERROR) << "error!...";
    }
}
//
// void autotrade_trade()
//{
//    try
//    {
//        printf( "\n" );
//        printf( "running process to automatically trade with self-defined strategies...\n" );
//
//
//        //std::cout << "1" << std::endl;
//        //-------------------------------------读取基本配置---------------------------------------
//        AccountParam ctpAccount;
//        mongoSetting mongoDbSetting;
//        autoSetting autoTradeSetting;
//        ParseSettingJson(ctpAccount,mongoDbSetting,autoTradeSetting);
//
//        //-------------------------------------行情前置接口--------------------------------------
//        /* MDApi & MDSpi */
//        CThostFtdcMdApi* pMdUserApi = CThostFtdcMdApi::CreateFtdcMdApi(".\\MDflow\\");// 流控文件 防止阻塞
//        cMdSpi* pMdUserSpi = new cMdSpi( pMdUserApi, ctpAccount.brokerId, ctpAccount.userId, ctpAccount.passwd );
//        pMdUserApi->RegisterSpi( pMdUserSpi );
//        pMdUserApi->RegisterFront( ctpAccount.mdAddress );
//
//        //------------------------------------- 创建数据收集器 --------------------------------------------
//        /* cMarketDataCollection */
//        cMarketDataCollectionPtr pMdEngine = make_shared< cMarketDataCollection >();
//        dynamic_cast< cMdSpi* >( pMdUserSpi )->RegisterMarketDataCollection( pMdEngine.get() );
//
//        //
//        /* TraderApi && TraderSpi */
//        CThostFtdcTraderApi* pTraderUserApi = CThostFtdcTraderApi::CreateFtdcTraderApi(".\\TDflow\\");
//        cTraderSpi* pTraderUserSpi = new cTraderSpi( pTraderUserApi,pMdUserSpi,pMdUserApi, ctpAccount.brokerId,
//        ctpAccount.userId, ctpAccount.passwd ); pTraderUserApi->RegisterSpi((CThostFtdcTraderSpi*) pTraderUserSpi );
//        pTraderUserApi->SubscribePublicTopic( THOST_TERT_RESTART );    // subscribe public topic
//        pTraderUserApi->SubscribePrivateTopic( THOST_TERT_QUICK );    // subscribe private topic
//        pTraderUserApi->RegisterFront( ctpAccount.tdAddress );
//
//        //-----------------------------------------人机交互线程---------------------------------------------------------------------------------
//        cTradingPlatformPtr pTradingPlatform = make_shared< cTradingPlatform >();
//        pTradingPlatform->RegisterMarketDataEngine( pMdEngine );
//        pTradingPlatform->RegisterTraderSpi(  pTraderUserSpi);
//        pTradingPlatform->RegisterMdSpi(pMdUserSpi);
//        pTradingPlatform->RegisterParameters(&autoTradeSetting,&mongoDbSetting);
//
//        cThread< cTradingPlatform >* pTradingThread = new cThread< cTradingPlatform >( pTradingPlatform.get(),
//        &cTradingPlatform::AutoTrading );
//
//
//
//        pTraderUserApi->Init(); //应首先初始化TD 再进行MD初始化 最后初始化 交易平台 基础思考为获取完账户信息后
//        再启动行情
//
//        while(true){
//            if(pMdUserSpi->getSatus()){
//                pTradingThread->Init();
//                break;
//            }
//        } // 防止Join无法停住，主线程退出
//
//    //等待线程退出
//        pTraderUserApi->Join();
//        while(true){}
//    }
//    catch(exception e )
//    {
//        printf( "\n%s\n", e.what() );
//        exit( 1 );
//    }
//}

// HANDLE g_hEvent;//事件句柄
//
// int iRequestID = 0;//订单编号
//
//#define  ROHON  0

////-------------------------------------obtain config---------------------------------------
// AccountParam ctpAccount;
// mongoSetting mongoDbSetting;
// autoSetting autoTradeSetting;
// ParseSettingJson(ctpAccount, mongoDbSetting, autoTradeSetting);
//
////-------------------------------------marketdata api--------------------------------------
///* MDApi & MDSpi */
// CThostFtdcMdApi* pMdUserApi = CThostFtdcMdApi::CreateFtdcMdApi(".\\MDflow\\");// flow file,prevention of obstruction
// cMdSpi* pMdUserSpi = new cMdSpi(pMdUserApi, ctpAccount.brokerId, ctpAccount.userId, ctpAccount.passwd);
// pMdUserApi->RegisterSpi(pMdUserSpi);
// pMdUserApi->RegisterFront(ctpAccount.mdAddress);
// std::promise<bool> finish_tag;
// std::future<bool> is_Finished = finish_tag.get_future();
// pMdUserSpi->setOnFinished([&finish_tag]() {
//    finish_tag.set_value(true);
//});
////------------------------------------- data collector --------------------------------------------
///* cMarketDataCollection */
// cMarketDataCollectionPtr pMdEngine = make_shared< cMarketDataCollection >();
// dynamic_cast< cMdSpi* >(pMdUserSpi)->RegisterMarketDataCollection(pMdEngine.get());
//
////
///* TraderApi && TraderSpi */
// CThostFtdcTraderApi* pTraderUserApi = CThostFtdcTraderApi::CreateFtdcTraderApi(".\\TDflow\\");
// cTraderSpi* pTraderUserSpi = new cTraderSpi(pTraderUserApi, pMdUserSpi, pMdUserApi, ctpAccount.brokerId,
// ctpAccount.userId, ctpAccount.passwd);  pTraderUserApi->RegisterSpi((CThostFtdcTraderSpi*)pTraderUserSpi);
// pTraderUserApi->SubscribePublicTopic(THOST_TERT_RESTART);    // subscribe public topic
// pTraderUserApi->SubscribePrivateTopic(THOST_TERT_QUICK);    // subscribe private topic
// pTraderUserApi->RegisterFront(ctpAccount.tdAddress);
//
////-----------------------------------------trading platform
///---------------------------------------------------------------------------------
// cTradingPlatformPtr pTradingPlatform = make_shared< cTradingPlatform >();
// pTradingPlatform->RegisterMarketDataEngine(pMdEngine);
// pTradingPlatform->RegisterTraderSpi(pTraderUserSpi);
// pTradingPlatform->RegisterMdSpi(pMdUserSpi);
// pTradingPlatform->RegisterParameters(&autoTradeSetting, &mongoDbSetting);
//
// cThread< cTradingPlatform >* pTradingThread = new cThread< cTradingPlatform >(pTradingPlatform.get(),
// &cTradingPlatform::AutoTrading);
//
// pTraderUserApi->Init(); //应首先初始化TD 再进行MD初始化 最后初始化 交易平台 基础思考为获取完账户信息后 再启动行情
// is_Finished.wait();// get()
// pTradingThread->Init();
//
///*
// isFinished.wait();
// if(isFinished.get())
//
// while (true) {
// if (pMdUserSpi->getSatus()) {
// pTradingThread->Init();
// break;
//}
//}*/
//
