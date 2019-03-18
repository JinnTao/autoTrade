#ifndef __CTRADECOLLECTION_H__
#define __CTRADECOLLECTION_H__


#include <iostream>
#include <map>
#include <memory>
#include <vector>
#include <fstream>

#include "cTrade.h"

using namespace std;

using tradeHandle            = map<int, cTradePtr>;

class cTradeCollection
{
public: 
    cTradeCollection();
    virtual ~cTradeCollection();

    void Clear();

    shared_ptr<cTrade> Add(CThostFtdcTradeField* pTrade,
                           CThostFtdcInstrumentCommissionRateField*,
                           CThostFtdcInstrumentField*);
    void Add( cTradePtr p_element ,CThostFtdcInstrumentCommissionRateField*,CThostFtdcInstrumentField*);

    cTrade* GetTradeHandle( int );
    cTradePtr GetTradeHandleSharedPtr( int );


    void PrintAll() const;
    void PrintTrade( int );


protected:
    tradeHandle    _m_trade_handle;

private:
    shared_ptr<cTrade> AddToMapInternal(shared_ptr<cTrade>& element,
                                        CThostFtdcInstrumentCommissionRateField*,
                                        CThostFtdcInstrumentField*);
};

using cTradeCollectionPtr = shared_ptr<cTradeCollection>;

#endif