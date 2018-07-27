#ifndef __CTRADECOLLECTION_H__
#define __CTRADECOLLECTION_H__

#include <cTrade.h>
#include <iostream>
#include <map>
#include <memory>
#include <vector>
#include <fstream>
using namespace std;
//template< class T > class cArray;
//
//typedef map< cString, cArray< const cTrade* > > tradeStoreByInstrument;
//typedef map< int, cArray< const cTrade* > > tradeStoreByOrder;
typedef map< int, cTradePtr > tradeHandle;
using tradeStoreByInstrument = std::map<std::string,std::vector<const cTrade*>>;
using tradeStoreByOrder      = std::map<int, std::vector<const cTrade*>>;
class cTradeCollection
{
public: 
    cTradeCollection();
    virtual ~cTradeCollection();

    void Clear();

    void Add( CThostFtdcTradeField* pTrade,CThostFtdcInstrumentCommissionRateField* ,CThostFtdcInstrumentField*);
    void Add( cTradePtr p_element ,CThostFtdcInstrumentCommissionRateField*,CThostFtdcInstrumentField*);
    int Count() const;
    void GetInstrumentIDs( std::vector< std::string >& instrumentIDs ) const;
    void GetOrderIDs(std::vector<int>& orderIDs ) const;
    void GetTradeIDs(std::vector<int>& tradeIDs ) const;

    cTrade* GetTradeHandle( int );
    cTradePtr GetTradeHandleSharedPtr( int );
    std::vector< const cTrade* > GetTradeByInstrument( const std::string& ) const ;
    std::vector< const cTrade* > GetTradeByOrder( int ) const;

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