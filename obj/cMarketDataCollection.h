#ifndef __CMARKETDATACOLLECTION_H__
#define __CMARKETDATACOLLECTION_H__

#include <cMarketData.h>
#include <map>
using std::map;


// underlying string versus array of cMarketData, i.e. IF vs. IF1601 and IF1603
//typedef map< cString, cArray< const cMarketData* > > marketdataStore;
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


protected:

	marketdataHandle _m_mkt_handle;

private:
	
};

typedef shared_ptr< cMarketDataCollection > cMarketDataCollectionPtr;

#endif