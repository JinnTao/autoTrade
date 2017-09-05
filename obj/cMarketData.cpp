// cMarketData.cpp

#include <time.h>
#include <cMarketData.h>
#include <cStringUtils.h>
#include <cTickTime.h>
#include <cSystem.h>
#include <stl_ctp.h>


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

	this->m_market.push_back(*pDepthMarketData);

	m_lastMarketData = *pDepthMarketData;

}
