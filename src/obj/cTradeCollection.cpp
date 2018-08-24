#include <direct.h>

#include "cTradeCollection.h"
cTradeCollection::cTradeCollection()
{
}

cTradeCollection::~cTradeCollection()
{
    Clear();
}

void cTradeCollection::Clear()
{
    _m_trade_handle.clear();
}

shared_ptr<cTrade> cTradeCollection::Add(CThostFtdcTradeField*                    pTrade,
                                         CThostFtdcInstrumentCommissionRateField* pCom,
                                         CThostFtdcInstrumentField*               pInstFiled) {
    if (pTrade->Volume > 0)
    {
        shared_ptr<cTrade> ptr      = std::make_shared<cTrade>(pTrade);
        auto               tradePtr = AddToMapInternal(ptr, pCom, pInstFiled);
        return tradePtr;
    }
    return nullptr;
}

void cTradeCollection::Add(cTradePtr p_element, CThostFtdcInstrumentCommissionRateField*pCom, CThostFtdcInstrumentField* pInstField)
{
    AddToMapInternal(p_element, pCom, pInstField);
}


cTrade* cTradeCollection::GetTradeHandle(int tradeID)
{
    cTradePtr p = GetTradeHandleSharedPtr(tradeID);
    if (p)
        return p.get();
    else
        return nullptr;
}

cTradePtr cTradeCollection::GetTradeHandleSharedPtr(int tradeID)
{
    tradeHandle::iterator it = _m_trade_handle.find(tradeID);
    if (it != _m_trade_handle.end())
        return (*it).second;
    else
        return cTradePtr();
}


shared_ptr<cTrade> cTradeCollection::AddToMapInternal(shared_ptr<cTrade>&                      element,
                                                      CThostFtdcInstrumentCommissionRateField* pCom,
                                                      CThostFtdcInstrumentField*               pInstField) {
    //
    int tradeID = element->GetTradeID();
    //
    element->setCommission(pCom, pInstField);
    tradeHandle::iterator it = _m_trade_handle.find(tradeID);
    if (it == _m_trade_handle.end())
        _m_trade_handle.insert(tradeHandle::value_type(tradeID, element));
    else
        (*it).second = element;
    return element;
}

void cTradeCollection::PrintAll() const
{
    using namespace std;
    tradeHandle::const_iterator it;
    if (_m_trade_handle.size() == 0) {
        cerr << "  No trade List" << endl;
        return;
    }
    _mkdir("output//tradeList");
    ofstream output;
    string s_userId = (*_m_trade_handle.begin()).second->GetAccountID();
    string date = (*_m_trade_handle.begin()).second->GetTradeDate();
    output.open("output/tradeList/" + s_userId  + "_" + date + ".csv", ios::ate) ;
    output <<"合约"<<","<< "日期"<< "," << "时间"<< "," << "方向"<< "," << "标志"<< "," << "成交笔数"<< ","<< "成交价格"<< ","<< "手续费"<< endl;
    for( it = _m_trade_handle.begin(); it != _m_trade_handle.end(); ++it ){
    (*it).second->Print();
    output
    <<(*it).second->GetInstrumentID()<< ","
    << (*it).second->GetTradeDate()<< ","
    << (*it).second->GetTradeTime() <<","
    << ((*it).second->GetDirection()=='0'?"买":"卖" )<< ","
    <<((*it).second->GetOffsetFlag()=='0'?"开仓":"平仓" )<< ","
    <<  (*it).second->GetVolume() << ","
    << (*it).second->GetPrice()<<","
    << (*it).second->GetCommission()
    <<endl;

    }
    output.close();
}

void cTradeCollection::PrintTrade(int tradeID)
{
    cTrade* p = GetTradeHandle(tradeID);
    if (p)
        p->Print();
}

