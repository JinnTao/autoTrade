#include <stl_ctp.h>
#include <cMarketDataCollection.h>
#include <cTickTime.h>
#include <cSystem.h>
#include <global_defs.h>


cMarketDataCollection::cMarketDataCollection()
{

}

cMarketDataCollection::~cMarketDataCollection()
{

}


cMarketData* cMarketDataCollection::GetMarketDataHandle( const string& name )
{
	cMarketDataPtr p = GetMarketDataHandleSharedPtr( name );
	if( p )
		return p.get();
	else
		return NULL;
}

cMarketDataPtr cMarketDataCollection::GetMarketDataHandleSharedPtr( const string& name )
{
	marketdataHandle::iterator it = _m_mkt_handle.find( name );
	if( it != _m_mkt_handle.end() )
		return (*it).second;
	else
		return cMarketDataPtr();
}




void cMarketDataCollection::OnRtnDepthMarketData( CThostFtdcDepthMarketDataField* pDepthMarketData )
{
	string id( pDepthMarketData->InstrumentID );
	cMarketData* md = GetMarketDataHandle( id );

	if( !md ){
		// create new CmarketData
		shared_ptr< cMarketData > ptr = make_shared< cMarketData >( pDepthMarketData->InstrumentID);
		ptr->OnRtnDepthMarketData(pDepthMarketData);
	}else{
		md->OnRtnDepthMarketData( pDepthMarketData );
	}
}

