#ifndef __CMARKETDATACOLLECTION_H__
#define __CMARKETDATACOLLECTION_H__

#include <cMarketData.h>
#include <map>
#include <vector>

#include "cCandle.h"
#include "cTick.h"
#include "cHistoryMarketData.h"

using std::map;
using std::vector;

// futures ID versus cMarketDataPtr, i.e. IF1601
typedef map< string, cMarketDataPtr > marketdataHandle;


class cMarketDataCollection
{
public:
	cMarketDataCollection();
	virtual ~cMarketDataCollection();

	cMarketData* GetMarketDataHandle( const string& );
	cMarketDataPtr GetMarketDataHandleSharedPtr( const string& );

	//real time update
	void OnRtnDepthMarketData( CThostFtdcDepthMarketDataField* pDepthMarketData );


	// history series from old to newest
	// data get from api http://www.tq18.cn/ or https://www.juhe.cn/docs/api/id/21
	void loadSeriesHistory(string inst,string startDate,string endDate,vector<double>& open, vector<double>& high,vector<double> &low,vector<double>& close,vector<double> &volume);
	vector<cCandle> loadCandleHistory(string inst,string startDate,string endDate,DataFrequency dataFrequency,DataType dataType);

	void setTradeDayList(map<string,int> *p){m_pTradeDayList = p;}
protected:

	marketdataHandle _m_mkt_handle;
	
	cHistoryMarketDataPtr m_historyMarketDataPtr;
	
	// trade date List
	map<string,int> *m_pTradeDayList;
private:
	
};

typedef shared_ptr< cMarketDataCollection > cMarketDataCollectionPtr;

#endif