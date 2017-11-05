#pragma once
#include "cMarketData.h"

class cHistoryMarketData
{
public:
	cHistoryMarketData();
	virtual ~cHistoryMarketData();
protected:
	// history series from old to newest
	// data get from api http://www.tq18.cn/ or https://www.juhe.cn/docs/api/id/21
	vector<double> loadSeriesHistory(string inst, string startDate, string endDate, DataFrequency dataFrequency, DataType dataType){}
	vector<cCandle> loadCandleHistory(string inst, string startDate, string endDate, DataFrequency dataFrequency, DataType dataType){}
private:

};

typedef shared_ptr< cHistoryMarketData > cHistoryMarketDataPtr;
