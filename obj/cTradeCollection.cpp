#include <cTradeCollection.h>

cTradeCollection::cTradeCollection()
{
	_it = _map_trade.end();
}

cTradeCollection::~cTradeCollection()
{
	Clear();
}

void cTradeCollection::Clear()
{
	_map_trade.clear();
	_m_trade_instrument.clear();
	_m_trade_order.clear();
	_m_trade_handle.clear();
}

void cTradeCollection::Add( CThostFtdcTradeField* pTrade )
{
	if( pTrade->Volume > 0 )
	{
		shared_ptr< cTrade > ptr = make_shared< cTrade >( pTrade );
		AddToMapInternal( ptr );
	}
}
void cTradeCollection::Add(CThostFtdcOrderField* pOrder){
	if( pOrder->OrderStatus == THOST_FTDC_OST_AllTraded )
	{
		shared_ptr< cTrade > ptr = make_shared< cTrade >( pOrder );
		AddToMapInternal( ptr );
	}
}
void cTradeCollection::Add( cTradePtr p_element )
{
	AddToMapInternal( p_element ); 
}

int cTradeCollection::Count() const
{
	int count = 0;
	for( mapType::const_iterator it = _map_trade.begin(); it != _map_trade.end(); ++it )
		++count;
	return count;
}

void cTradeCollection::GetInstrumentIDs( cArray< cString >& instrumentIDs ) const
{
	for( tradeStoreByInstrument::const_iterator it = _m_trade_instrument.begin(); it != _m_trade_instrument.end(); ++it )
		instrumentIDs.push_back( (*it).first );
}

void cTradeCollection::GetOrderIDs( cIvector& orderIDs ) const
{
	for( tradeStoreByOrder::const_iterator it = _m_trade_order.begin(); it != _m_trade_order.end(); ++it )
		orderIDs.push_back( (*it).first );
}

void cTradeCollection::GetTradeIDs( cIvector& tradeIDs ) const
{
	for( mapType::const_iterator it = _map_trade.begin(); it != _map_trade.end(); ++it )
		tradeIDs.push_back( (*it).first );
}

cTrade* cTradeCollection::GetTradeHandle( int tradeID )
{
	cTradePtr p = GetTradeHandleSharedPtr( tradeID );
	if( p )
		return p.get();
	else
		return NULL;
}

cTradePtr cTradeCollection::GetTradeHandleSharedPtr( int tradeID )
{
	tradeHandle::iterator it = _m_trade_handle.find( tradeID );
	if( it != _m_trade_handle.end() )
		return (*it).second;
	else
		return cTradePtr();
}

cArray< const cTrade* > cTradeCollection::GetTradeByInstrument( const cString& instrumentID ) const
{
	tradeStoreByInstrument::const_iterator it = _m_trade_instrument.find( instrumentID );
	if( it != _m_trade_instrument.end() )
		return (*it).second;
	else
		return cArray< const cTrade* >();
}

cArray< const cTrade* > cTradeCollection::GetTradeByOrder( int orderID ) const
{
	tradeStoreByOrder::const_iterator it = _m_trade_order.find( orderID );
	if( it != _m_trade_order.end() )
		return (*it).second;
	else
		return cArray< const cTrade* >();
}

void cTradeCollection::AddToMapInternal( shared_ptr< cTrade >& element )
{
	//
	int tradeID = element->GetTradeID();
	mapType::iterator it1 = _map_trade.find( tradeID );
	if( it1 != _map_trade.end() )
		(*it1).second = element;
	else
		_map_trade.insert( mapType::value_type( tradeID, element ) );
	//
	cString instrumentID = element->GetInstrumentID();
	tradeStoreByInstrument::iterator it2 = _m_trade_instrument.find( instrumentID );
	if( it2 == _m_trade_instrument.end() )
	{
		cArray< const cTrade* > v;
		v.push_back( element.get() );
		_m_trade_instrument.insert( tradeStoreByInstrument::value_type( instrumentID, v ) );
	}
	else
	{
		bool foundFlag = false;
		for( int i = 0; i < (*it2).second.getSize(); ++i )
		{
			if( element->GetTradeID() == (*it2).second[i]->GetTradeID() )
			{
				foundFlag = true;
				break;
			}
		}
		if( !foundFlag )
			(*it2).second.push_back( element.get() );
	}
	//
	int orderID = element->GetOrderID();
	tradeStoreByOrder::iterator it3 = _m_trade_order.find( orderID );
	if( it3 == _m_trade_order.end() )
	{
		cArray< const cTrade* > v;
		v.push_back( element.get() );
		_m_trade_order.insert( tradeStoreByOrder::value_type( orderID, v ) );
	}
	else
	{
		bool foundFlag = false;
		for( int i = 0; i < (*it3).second.getSize(); ++i )
		{
			if( element->GetTradeID() == (*it3).second[i]->GetTradeID() )
			{
				foundFlag = true;
				break;
			}
		}
		if( !foundFlag )
			(*it3).second.push_back( element.get() );
	}

	//
	//
	tradeHandle::iterator it = _m_trade_handle.find( tradeID );
	if( it == _m_trade_handle.end() )
		_m_trade_handle.insert( tradeHandle::value_type( tradeID, element ) );
	else
		(*it).second = element;
}

void cTradeCollection::PrintAll() const
{
	tradeHandle::const_iterator it;
	for( it = _m_trade_handle.begin(); it != _m_trade_handle.end(); ++it )
		(*it).second->Print();
}

void cTradeCollection::PrintTrade( int tradeID )
{
	cTrade* p = GetTradeHandle( tradeID );
	if( p )
		p->Print();
}