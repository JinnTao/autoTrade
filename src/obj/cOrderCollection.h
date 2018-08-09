#ifndef __CORDERCOLLECTION_H__
#define __CORDERCOLLECTION_H__


#include <vector>
#include <map>
#include "cOrder.h"

typedef std::map< std::string, std::vector< const cOrder* > > orderStore;
typedef std::map< int, cOrderPtr > orderHandle;

class cOrderCollection
{
public:
    cOrderCollection();
    virtual ~cOrderCollection();

    void Clear();

    void Add( CThostFtdcOrderField* );
    void Add( cOrderPtr p_element );
    int Count() const;
    void GetInstrumentIDs( std::vector< std::string >& instrumentIDs ) const;
    void GetOrderIDs( std::vector<int>& orderIDs ) const;
    bool      getOrderByNo(TThostFtdcSequenceNoType orderSequenceNo, std::shared_ptr<cOrder>& pOrder);
    cOrder* GetOrderHandle( int );
    cOrderPtr GetOrderHandleSharedPtr( int );
    std::vector< const cOrder* > GetOrderByInstrument( const std::string& ) const;
    //
    void PrintPendingOrders() const;
    void PrintCancelledOrders() const;
    void PrintAllOrders() const;

    // 
    std::vector< cOrderPtr > GetAllOrder( ) const;
    //
    /*
    functions used for demo-trading, i.e. cTraderApi/cTraderSpi is not needed
        */
    void Remove( int orderID );

    typedef std::map< int, cOrderPtr > mapType;

protected:
    mapType _map_order;
    mapType::iterator _it;
    orderStore _m_order_instrument;
    orderHandle _m_order_handle;

private:
    void AddToMapInternal( std::shared_ptr< cOrder >& element );

};
typedef std::shared_ptr< cOrderCollection > cOrderCollectionPtr;

#endif
