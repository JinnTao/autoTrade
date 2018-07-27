#ifndef __CMARKETDATACOLLECTION_H__
#define __CMARKETDATACOLLECTION_H__

#include <cMarketData.h>
#include <map>
#include <vector>
#include <fstream>
#include "cCandle.h"
#include "cTick.h"
#include "cHistoryMarketData.h"
#include "autotrade_config.h"
#include "mongo\mongostore.h"

using std::map;
using std::vector;

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

    // history series from old to newest; data from mongoDb
    void loadHistoryFromMongo(string collection, string sDateTime,string eDateTime,vector<double>& open, vector<double>& high, vector<double> &low, vector<double>& close, vector<double> &volume);


    void setTradeDayList(map<string,int> *p){m_pTradeDayList = p;}
    void registerMongoSetting(mongoConfig *p) {
        //m_mongoStore.init(*p); 
    }
protected:

    marketdataHandle _m_mkt_handle;
    
    cHistoryMarketDataPtr m_historyMarketDataPtr;
    std::chrono::time_point<std::chrono::system_clock> m_sDateTimePoint;
    std::chrono::time_point<std::chrono::system_clock> m_eDateTimePoint;
    // trade date List
    map<string,int> *m_pTradeDayList;
private:
   // MongoStore m_mongoStore;
    
};

typedef shared_ptr< cMarketDataCollection > cMarketDataCollectionPtr;

#endif