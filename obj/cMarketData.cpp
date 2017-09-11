// cMarketData.cpp

#include <time.h>
#include <cMarketData.h>
#include <cStringUtils.h>
#include <cTickTime.h>
#include <cSystem.h>



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


void cMarketData::OnRtnDepthMarketData( CThostFtdcDepthMarketDataField* pDepthMarketData )
{
	CThostFtdcDepthMarketDataField marketData;
	memset(&marketData,0,sizeof(CThostFtdcDepthMarketDataField));
	//strcpy(&marketData,pDepthMarketData);
	memcpy(&marketData,pDepthMarketData,sizeof(CThostFtdcDepthMarketDataField));
	m_lastMarketData = marketData;
	this->m_marketDepthVector.push_back(marketData);

	m_lastPriceSeries.push_back(marketData.LastPrice);
	//cerr << pDepthMarketData->InstrumentID << " " << pDepthMarketData->LastPrice << endl;

}
