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
//保存读取的信息的结构体
struct AccountParam
{
	TThostFtdcBrokerIDType	brokerId;//经纪公司代码
	TThostFtdcInvestorIDType	userId;//用户名
	char	passwd[252];//密码

	char mdAddress[50];//行情服务器地址
	char tdAddress[50];//交易服务器地址

	//string m_read_contract;//合约代码
	AccountParam(){
		memset(this,0,sizeof(AccountParam));
		
	}
	void reset(){
		memset(this,0,sizeof(AccountParam));
	}
};

//保存读取的信息的结构体
struct mongoSetting
{
	char address[50];// Host地址
    char database[50];
	int mongoPort;// 端口
	bool mongoLogging;// 
	//string m_read_contract;//合约代码
	mongoSetting(){
		memset(this,0,sizeof(mongoSetting));
	}
	void reset(){
		memset(this,0,sizeof(mongoSetting));
	}
};

struct autoSetting
{
	char tradeDayDir[125];// 交易日路径
	char dataBaseDir[125];// 数据路径
	
    
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


	autoSetting(){
		memset(this, 0, sizeof(autoSetting));
	}
	void reset(){
		memset(this, 0, sizeof(autoSetting));
	}
};

int ParseSettingJson(AccountParam&,mongoSetting&,autoSetting&);





#endif