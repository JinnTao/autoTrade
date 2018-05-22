#ifndef __CORDERCOLLECTION_H__
#define __CORDERCOLLECTION_H__

#include <cOrder.h>

template< class T > class cArray;

typedef map< cString, cArray< const cOrder* > > orderStore;
typedef map< int, cOrderPtr > orderHandle;

class cOrderCollection
{
public:
    cOrderCollection();
    virtual ~cOrderCollection();

    void Clear();

    void Add( CThostFtdcOrderField* );
    void Add( cOrderPtr p_element );
    int Count() const;
    void GetInstrumentIDs( cArray< cString >& instrumentIDs ) const;
    void GetOrderIDs( cIvector& orderIDs ) const;

    bool getOrderByNo(int,shared_ptr<cOrder>& );
    cOrder* GetOrderHandle( int );
    cOrderPtr GetOrderHandleSharedPtr( int );
    cArray< const cOrder* > GetOrderByInstrument( const cString& ) const;
    //
    void PrintPendingOrders() const;
    void PrintCancelledOrders() const;
    void PrintAllOrders() const;

    // 
    vector< cOrderPtr > GetAllOrder( ) const;
    //
    /*
    functions used for demo-trading, i.e. cTraderApi/cTraderSpi is not needed
        */
    void Remove( int orderID );

    typedef map< int, cOrderPtr > mapType;

protected:
    mapType _map_order;
    mapType::iterator _it;
    orderStore _m_order_instrument;
    orderHandle _m_order_handle;

private:
    void AddToMapInternal( shared_ptr< cOrder >& element );

};

typedef shared_ptr< cOrderCollection > cOrderCollectionPtr;

#endif
