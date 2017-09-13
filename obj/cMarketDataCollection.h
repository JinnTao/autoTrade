#ifndef __CMARKETDATACOLLECTION_H__
#define __CMARKETDATACOLLECTION_H__

#include <cMarketData.h>
#include <map>
#include "cCandle.h"
#include "cTick.h"
using std::map;

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
	vector<double> loadSeriesHistory(string inst,string startDate,string endDate,DataFrequency dataFrequency,DataType dataType){}
	vector<cCandle> loadCandleHistory(string inst,string startDate,string endDate,DataFrequency dataFrequency,DataType dataType){}
	vector<cTick> loadTickHistory(string inst,string startDate,string endDate,DataFrequency dataFrequency,DataType dataType){}
	vector<CThostFtdcDepthMarketDataField> loadDepthHistory(string inst,string startDate,string endDate,DataFrequency dataFrequency,DataType dataType){}


protected:

	marketdataHandle _m_mkt_handle;

private:
	
};

typedef shared_ptr< cMarketDataCollection > cMarketDataCollectionPtr;

#endif