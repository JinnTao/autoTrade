#include <cOrderCollection.h>
#include "easylogging++.h"
cOrderCollection::cOrderCollection()
{
    _it = _map_order.end();
}

cOrderCollection::~cOrderCollection()
{
    Clear();
}

void cOrderCollection::Clear()
{
    _map_order.clear();
    _m_order_instrument.clear();
    _m_order_handle.clear();
}

void cOrderCollection::Add( CThostFtdcOrderField* pOrder )
{
    if( !pOrder )
        return;
    if( pOrder->VolumeTotalOriginal > 0 )
    {
        std::shared_ptr< cOrder > ptr = std::make_shared< cOrder >( pOrder );
        AddToMapInternal( ptr );
    }
}

void cOrderCollection::Add( cOrderPtr p_element )
{
    AddToMapInternal( p_element );
}

int cOrderCollection::Count() const
{
    int count = 0;
    for( mapType::const_iterator it = _map_order.begin(); it != _map_order.end(); ++it )
        ++count;
    return count;
}

void cOrderCollection::GetInstrumentIDs( std::vector< std::string >& instrumentIDs ) const
{
    orderStore::const_iterator it;
    for( it = _m_order_instrument.begin(); it != _m_order_instrument.end(); ++it )
        instrumentIDs.push_back( (*it).first );
}

void cOrderCollection::GetOrderIDs( std::vector<int>& orderIDs ) const
{
    orderHandle::const_iterator it;
    for( it = _m_order_handle.begin(); it != _m_order_handle.end(); ++it )
        orderIDs.push_back( (*it).first );
}

cOrder* cOrderCollection::GetOrderHandle( int tradeID )
{
    cOrderPtr p = GetOrderHandleSharedPtr( tradeID );
    if( p )
        return p.get();
    else
        return NULL;
}

cOrderPtr cOrderCollection::GetOrderHandleSharedPtr( int tradeID )
{
    orderHandle::const_iterator it = _m_order_handle.find( tradeID );
    if( it != _m_order_handle.end() )
        return (*it).second;
    else
        return cOrderPtr();
}

std::vector< const cOrder* > cOrderCollection::GetOrderByInstrument( const std::string& instrumentID ) const
{
    orderStore::const_iterator it = _m_order_instrument.find( instrumentID );
    if( it != _m_order_instrument.end() )
    {
        return (*it).second;
    }
    else
        return std::vector< const cOrder* >();
}
std::vector<cOrderPtr> cOrderCollection::GetAllOrder( ) const{
    orderHandle::const_iterator it;
    std::vector<cOrderPtr> vOrder;
    for( it = _m_order_handle.begin(); it != _m_order_handle.end(); ++it )
    {
        vOrder.push_back(it->second);
    }
    return vOrder;
}

void cOrderCollection::AddToMapInternal( std::shared_ptr< cOrder >& element )
{
    int orderID = element->GetOrderID();
    mapType::iterator it = _map_order.find( orderID );
    if( it != _map_order.end() )
        (*it).second = element;
    else
        _map_order.insert( mapType::value_type( orderID, element ) );

    std::string instrumentID = element->GetInstrumentID();
    orderStore::iterator itS = _m_order_instrument.find( instrumentID );
    if( itS == _m_order_instrument.end() )
    {
        std::vector< const cOrder* > v;
        v.push_back( element.get() );
        _m_order_instrument.insert( orderStore::value_type( instrumentID, v ) );
    }
    else
    {
        bool foundFlag = false;
        for( int i = 0; i < (*itS).second.size(); ++i )
        {
            if( element->GetOrderID() == (*itS).second[i]->GetOrderID() )
            {
                foundFlag = true;
                break;
            }
        }
        if( !foundFlag )
            (*itS).second.push_back( element.get() );
    }
    
    orderHandle::iterator itH = _m_order_handle.find( orderID );
    if( itH == _m_order_handle.end() )
        _m_order_handle.insert( orderHandle::value_type( orderID, element ) );
    else
        (*itH).second = element;
}

void cOrderCollection::PrintPendingOrders() const
{
    orderHandle::const_iterator it;
    int count = -1;
    for( it = _m_order_handle.begin(); it != _m_order_handle.end(); ++it )
    {
        if( (*it).second->IsPendingOrder() )
        {
            count++;
            if( count == 0 )
                printf( "\nPending Orders:\n" );
            (*it).second->Print();
        }
    }
    if( count < 0 )
        printf( "\nNo Pending Orders!\n" );
    else
        printf( "\n" );
}

void cOrderCollection::PrintCancelledOrders() const
{
    orderHandle::const_iterator it;
    int count = -1;
    for( it = _m_order_handle.begin(); it != _m_order_handle.end(); ++it )
    {
        if( (*it).second->IsCancelledOrder() )
        {
            count++;
            if( count == 0 )
                printf( "\nCancelled Orders:\n" );
            (*it).second->Print();
        }
    }
    if( count < 0 )
        printf( "\nNo Cancelled Orders!\n" );
    else
        printf( "\n" );
}


void cOrderCollection::PrintAllOrders() const
{
    orderHandle::const_iterator it;
    int count = -1;
    for( it = _m_order_handle.begin(); it != _m_order_handle.end(); ++it )
    {
        count++;
        if( count == 0 )
            printf( "All Orders:\n" );
        (*it).second->Print();
        
    }
}

void cOrderCollection::Remove( int orderID )
{
    orderHandle::iterator it = _m_order_handle.find( orderID );
    std::string instrumentID("null");
    if( it != _m_order_handle.end() )
    {
        instrumentID = (*it).second->GetInstrumentID();
        _m_order_handle.erase( it );
    }

    mapType::iterator it2 = _map_order.find( orderID );
    if( it2 != _map_order.end() )
        _map_order.erase( it2 );
    
    orderStore::iterator it3 = _m_order_instrument.find(instrumentID);
    if( it3 != _m_order_instrument.end() )
    {
        // fix delete order instrument by jinntao need test -- 2018/07/27
        std::vector<const cOrder* > v = (*it3).second;
        int index = -1;
        for( index = 0; index < v.size(); ++index )
        {
            if( v[index]->GetOrderID() == orderID )
                break;
        }
        if (index < v.size())
            (*it3).second.erase((*it3).second.begin() + index);
    }
}

bool cOrderCollection::getOrderByNo(TThostFtdcSequenceNoType orderSequenceNo,std::shared_ptr<cOrder> &pOrder){
    orderHandle::const_iterator it;
    bool exist = false;
    for( it = _m_order_handle.begin(); it != _m_order_handle.end(); ++it )
    {
        if( (*it).second->IsPendingOrder() && (*it).second->GetOrderID() == orderSequenceNo )
        {
            pOrder = it->second;
            exist = true;
            break;
        }
    }
    return exist;
}

