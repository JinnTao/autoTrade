/***
*cVector.h - declarations/definitions for marketdata class
****/

#ifndef __CMARKETDATA_H__
#define __CMARKETDATA_H__
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include "ThostFtdcUserApiStruct.h"

using namespace std;
enum DataFrequency{D1,H1,M15,M5,M1,Tick};
enum DataType{Close,High,Low,Open};

class cMarketData
{
public:
    cMarketData(string);
    ~cMarketData();
    // get methods
    string GetID() const { return m_id; }
    CThostFtdcDepthMarketDataField getLastMarketData() ;
    vector<CThostFtdcDepthMarketDataField> getMarketDepthSeries(){return m_marketDepthVector;}
    vector<double>    getMarketLastSeries(){return m_lastPriceSeries;}

    void OnRtnDepthMarketData( CThostFtdcDepthMarketDataField* pDepthMarketData );
protected:
    string m_id;                // contract code
    // save DataFieldVector
    vector<CThostFtdcDepthMarketDataField> m_marketDepthVector;
    // save last close series data
    vector<double> m_lastPriceSeries;

    CThostFtdcDepthMarketDataField m_lastMarketData;
    mutex _mtx;//  locks access to marketData
private:
    int m_length;

};

typedef shared_ptr< cMarketData > cMarketDataPtr;

#endif
