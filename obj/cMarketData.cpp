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
	this->m_length = 100;
}





cMarketData::~cMarketData()
{
    
}


CThostFtdcDepthMarketDataField cMarketData::getLastMarketData() 
{
    std::unique_lock<std::mutex> guard(_mtx);
    return m_lastMarketData;
}


void cMarketData::OnRtnDepthMarketData( CThostFtdcDepthMarketDataField* pDepthMarketData )
{
	unique_lock<std::mutex> guard(_mtx);
    // Data initial
	CThostFtdcDepthMarketDataField marketData;
    memset(&marketData,0,sizeof(CThostFtdcDepthMarketDataField));
    //strcpy(&marketData,pDepthMarketData);
    memcpy(&marketData,pDepthMarketData,sizeof(CThostFtdcDepthMarketDataField));

	// Save Data
    m_lastMarketData = marketData;

    this->m_marketDepthVector.push_back(marketData);

    m_lastPriceSeries.push_back(marketData.LastPrice);


	// erase Data
	if (this->m_marketDepthVector.size() > m_length){
		this->m_marketDepthVector.erase(this->m_marketDepthVector.begin());

		this->m_lastPriceSeries.erase(this->m_lastPriceSeries.begin());
	}
	
}
