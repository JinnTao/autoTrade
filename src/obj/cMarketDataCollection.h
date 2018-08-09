#ifndef __CMARKETDATACOLLECTION_H__
#define __CMARKETDATACOLLECTION_H__


#include <map>
#include <vector>
#include <fstream>
//
#include "cMarketData.h"
#include "global.h"
#include "mongo/mongostore.h"

using marketdataHandle = std::map<string, cMarketDataPtr>;

class cMarketDataCollection {
public:
    cMarketDataCollection();
    virtual ~cMarketDataCollection();

    cMarketData* GetMarketDataHandle( const string& );
    cMarketDataPtr GetMarketDataHandleSharedPtr( const string& );

    //real time update
    void OnRtnDepthMarketData( CThostFtdcDepthMarketDataField* pDepthMarketData );


    // history series from old to newest
    // data get from api http://www.tq18.cn/ or https://www.juhe.cn/docs/api/id/21
    void loadSeriesHistory(string               inst,
                           string               startDate,
                           string               endDate,
                           vector<double>&      open,
                           std::vector<double>& high,
                           std::vector<double>& low,
                           std::vector<double>& close,
                           std::vector<double>& volume);

    // history series from old to newest; data from mongoDb
    void loadHistoryFromMongo(string               collection,
                              string               sDateTime,
                              string               eDateTime,
                              std::vector<double>& open,
                              std::vector<double>& high,
                              std::vector<double>& low,
                              std::vector<double>& close,
                              std::vector<double>& volume);


    void setTradeDayList(std::map<string,int> *p){m_pTradeDayList = p;}
    void registerMongoSetting(mongoConfig *p) {
        m_mongoStore.init(*p); 
    }
protected:
   
    marketdataHandle _m_mkt_handle;
    
    std::chrono::time_point<std::chrono::system_clock> m_sDateTimePoint;
    std::chrono::time_point<std::chrono::system_clock> m_eDateTimePoint;
    // trade date List
    std::map<string,int> *m_pTradeDayList;
private:
   MongoStore m_mongoStore;
    
};

typedef shared_ptr< cMarketDataCollection > cMarketDataCollectionPtr;

#endif