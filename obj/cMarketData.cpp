// cMarketData.cpp

#include <time.h>
#include <cMarketData.h>
#include <cStringUtils.h>
#include <cTickTime.h>
#include <cSystem.h>
#include <mutex>


cMarketData::cMarketData(string id)
{
    this->m_id = id;
}




cMarketData::cMarketData( const cMarketData& in )
{
    yr_assert( this != & in );
    m_id = in.m_id;
    

}


cMarketData::~cMarketData()
{
    
}


CThostFtdcDepthMarketDataField cMarketData::getLastMarketData() const
{
    lock_guard<std::mutex> guard(_mtx);
    return m_lastMarketData;
}

void cMarketData::OnRtnDepthMarketData( CThostFtdcDepthMarketDataField* pDepthMarketData )
{
    lock_guard<std::mutex> guard(_mtx);
    CThostFtdcDepthMarketDataField marketData;
    memset(&marketData,0,sizeof(CThostFtdcDepthMarketDataField));
    //strcpy(&marketData,pDepthMarketData);
    memcpy(&marketData,pDepthMarketData,sizeof(CThostFtdcDepthMarketDataField));

    m_lastMarketData = marketData;
    this->m_marketDepthVector.push_back(marketData);

    m_lastPriceSeries.push_back(marketData.LastPrice);
    //cerr << pDepthMarketData->InstrumentID << " " << pDepthMarketData->LastPrice << endl;

}
