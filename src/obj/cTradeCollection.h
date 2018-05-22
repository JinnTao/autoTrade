#ifndef __CTRADECOLLECTION_H__
#define __CTRADECOLLECTION_H__

#include <cTrade.h>
#include <iostream>
#include <map>
using std::map;
template< class T > class cArray;

typedef map< cString, cArray< const cTrade* > > tradeStoreByInstrument;
typedef map< int, cArray< const cTrade* > > tradeStoreByOrder;
typedef map< int, cTradePtr > tradeHandle;

class cTradeCollection
{
public: 
    cTradeCollection();
    virtual ~cTradeCollection();

    void Clear();

    void Add( CThostFtdcTradeField* pTrade,CThostFtdcInstrumentCommissionRateField* ,CThostFtdcInstrumentField*);
    void Add( cTradePtr p_element ,CThostFtdcInstrumentCommissionRateField*,CThostFtdcInstrumentField*);
    int Count() const;
    void GetInstrumentIDs( cArray< cString >& instrumentIDs ) const;
    void GetOrderIDs( cIvector& orderIDs ) const;
    void GetTradeIDs( cIvector& tradeIDs ) const;

    cTrade* GetTradeHandle( int );
    cTradePtr GetTradeHandleSharedPtr( int );
    cArray< const cTrade* > GetTradeByInstrument( const cString& ) const;
    cArray< const cTrade* > GetTradeByOrder( int ) const;

    void PrintAll() const;
    void PrintTrade( int );

    typedef map< int, cTradePtr > mapType;

protected:
    mapType _map_trade;
    mapType::iterator _it;
    tradeStoreByInstrument _m_trade_instrument;
    tradeStoreByOrder    _m_trade_order;
    tradeHandle    _m_trade_handle;

private:
    void AddToMapInternal( shared_ptr< cTrade >& element ,CThostFtdcInstrumentCommissionRateField*,CThostFtdcInstrumentField*);
};

typedef shared_ptr< cTradeCollection > cTradeCollectionPtr;

#endif