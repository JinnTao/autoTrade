#ifndef __AUTOTRADE_CONFIG_H__
#define __AUTOTRADE_CONFIG_H__

#include <cString.h>
#include <cArray.h>
#include <map>
#include <cSystem.h>
#include <ThostFtdcMdApi.h>
#include <memory>
//
struct sATGeneralConfig
{
    bool genLog;
    cString dataoutputDirectory;
    cString tickDataFolderName;
    cString candleDataFolderName;
    cString logFileFolderName;
    cArray< cString > underlyings;
    map< cString, bool > displayTick;
    map< cString, bool > displayCandle;
    map< cString, bool > displaySignal;
    map< cString, bool > displayOrder;
    map< cString, bool > displayTrade;
};

void autotrade_loadconfig_general( sATGeneralConfig& configs, cString& fileName );

//
struct sATDownloadMarketDataConfig
{
    sATGeneralConfig generalConfig;
    cString logFileName;
};

void autotrade_loadconfig_downloadmarketdata( sATDownloadMarketDataConfig& configs );


struct sATBacktestConfig
{
    sATGeneralConfig generalConfig;
    
    cString strategyName;
    cArray< cString > strategyConfigFileNames;
    
    cString dateStart;
    cString dateEnd;
    bool oldFormat;
    cString dataLoadDirectory;
};

void autotrade_loadconfig_backtest( sATBacktestConfig& configs );

struct sATTradeConfig
{
    sATGeneralConfig generalConfig;

    cString logFileName;
    
    cArray< cString > strategyConfigFileNames;
    
};


void autotrade_loadconfig_trade( sATTradeConfig& configs );


struct ctpConfig
{
    TThostFtdcBrokerIDType    brokerId;
    TThostFtdcInvestorIDType    userId;
    char    passwd[252];

    char mdAddress[50];
    char tdAddress[50];
    char md_flow_path_[50];
    char td_flow_path_[50];
    ctpConfig(){
        memset(this,0,sizeof(ctpConfig));
        
    }
    void reset(){
        memset(this,0,sizeof(ctpConfig));
    }
};

struct mongoConfig
{
    char address[50];// Host address
    char database[50];
    int mongoPort;// 
    bool mongoLogging;// 

    mongoConfig(){
        memset(this,0,sizeof(mongoConfig));
    }
    void reset(){
        memset(this,0,sizeof(mongoConfig));
    }
};

struct strategyConfig
{
    char tradeDayDir[125];
    char dataBaseDir[125];
    
    
    char startDate[25];
    char endDate[25];

    // dataSource
    char startDateTime[150];
    char endDateTime[150];
    char collectionName[20];

    //kingKeltNer
    //;计算通道中值的窗口数
    double kkLength;// = 11
    //;计算通道宽度的偏差
    double kkDev;// = 1.6
    //;移动止损
    double trailingPrcnt;// = 0.8
    //;每次交易的数量
    double fixedSize;// = 1
    //;初始化数据所用的天数
    double initDays;// = 10

    // strategy base Information
    char inst[300]; // strategy Trading Inst
    char lots[300]; // strategy Trading Inst lost
    char timeMode[300]; // strategy Time filter mode
    char collectionList[500]; // strategy collection name

    strategyConfig(){
        memset(this, 0, sizeof(strategyConfig));
    }
    void reset(){
        memset(this, 0, sizeof(strategyConfig));
    }
};



#endif