#ifndef __YR_STRUCT_H__
#define __YR_STRUCT_H__

#include <cString.h>
#include <ThostFtdcUserApiDataType.h>

struct sInstrumentInfo
{
	char InstrumentID[32];
	char ExchangeID[32];
	char ProductID[32];
	int MaxMarketOrderVolume;
	int MinMarketOrderVolume;
	int MaxLimitOrderVolume;
	int MinLimitOrderVolume;
	int VolumeMultiple;
	double PriceTick;
	int IsTrading;
	double LongMarginRatio;
	double ShortMarginRatio;
};

struct sTradingAccountInfo
{
	char BrokerID[32];
	char AccountID[32];
	double PreBalance;
	double Balance;
	double Available;
	double WithdrawQuota;
	double CurrMargin;
	double CloseProfit;
	double PositionProfit;
	double Commission;
	double FrozenMargin;
};

//Direction Offset
enum  DIRECTION{buy,sell};
enum  OFFSETFLAG{open,close};

typedef	shared_ptr<map<string, CThostFtdcInstrumentField*>> cInstMessageMapPtr;//保存合约信息的map
#endif

