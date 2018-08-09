#ifndef __CTRADECOLLECTION_H__
#define __CTRADECOLLECTION_H__


#include <iostream>
#include <map>
#include <memory>
#include <vector>
#include <fstream>

#include "cTrade.h"

using namespace std;

using tradeStoreByInstrument = std::map<std::string,std::vector<const cTrade*>>;
using tradeStoreByOrder      = std::map<int, std::vector<const cTrade*>>;
using tradeHandle            = map<int, cTradePtr>;

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


protected:
    std::map <int,cTradePtr> _map_trade;
    std::map <int,cTradePtr> ::iterator _it;
    tradeStoreByInstrument _m_trade_instrument;
    tradeStoreByOrder    _m_trade_order;
    tradeHandle    _m_trade_handle;

private:
    void AddToMapInternal( shared_ptr< cTrade >& element ,CThostFtdcInstrumentCommissionRateField*,CThostFtdcInstrumentField*);
};

using cTradeCollectionPtr = shared_ptr<cTradeCollection>;

#endif