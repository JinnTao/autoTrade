#ifndef __AUTOTRADE_CONFIG_H__
#define __AUTOTRADE_CONFIG_H__

#include <cString.h>
#include <cArray.h>
#include <map>
#include <cSystem.h>
#include <ThostFtdcMdApi.h>

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
	char mongoHost[50];// Host地址
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

int ParseSettingJson(AccountParam&,mongoSetting&);





#endif