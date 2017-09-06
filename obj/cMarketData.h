/***
*cVector.h - declarations/definitions for marketdata class
****/

#ifndef __CMARKETDATA_H__
#define __CMARKETDATA_H__
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include "ThostFtdcUserApiStruct.h"

using namespace std;

class cMarketData
{
public:
	cMarketData(string);
	cMarketData( const cMarketData& );
	~cMarketData();
	// get methods
	string GetID() const { return m_id; }
	CThostFtdcDepthMarketDataField getLastMarketData(){return m_lastMarketData;};
	vector<CThostFtdcDepthMarketDataField> getMarketDepthSeries(){return m_marketDepthVector;}
	vector<double>	getMarketLastSeries(){return m_lastPriceSeries;}

	void OnRtnDepthMarketData( CThostFtdcDepthMarketDataField* pDepthMarketData );
protected:
	string m_id;				// contract code
	vector<CThostFtdcDepthMarketDataField> m_marketDepthVector;
	vector<double> m_lastPriceSeries;
	CThostFtdcDepthMarketDataField m_lastMarketData;
private:
};

typedef shared_ptr< cMarketData > cMarketDataPtr;



#endif
