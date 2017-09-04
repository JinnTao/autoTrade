/***
*cVector.h - declarations/definitions for marketdata class
*
*       Copyright (c) Yiran Yang. All rights reserved.
*
****/

#ifndef __CMARKETDATA_H__
#define __CMARKETDATA_H__
#include <iostream>
#include <string>
#include <vector>
#include <windows.h>
#include <memory.h>
#include <cSharedPtr.h>
#include "ThostFtdcUserApiDataType.h"
#include "ThostFtdcUserApiStruct.h"
using namespace std;

class cMarketData
{
public:
	cMarketData();
	cMarketData( const cMarketData& );
	~cMarketData();
	// get methods
	string GetID() const { return m_id; }
	void OnRtnDepthMarketData( CThostFtdcDepthMarketDataField* pDepthMarketData );
protected:
	string m_id;				// contract code
	vector<CThostFtdcDepthMarketDataField> m_market;
	CThostFtdcDepthMarketDataField m_lastMarketData;
private:

	
};

typedef shared_ptr< cMarketData > cMarketDataPtr;



#endif
