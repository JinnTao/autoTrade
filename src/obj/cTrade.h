#ifndef __CTRADE_H__
#define __CTRADE_H__

#include "ThostFtdcUserApiDataType.h"
#include <ThostFtdcUserApiStruct.h>
#include <memory>
#include <string>
#include <iostream>
class cTrade
{
public:
    cTrade();
    cTrade( CThostFtdcTradeField* );

    cTrade( const cTrade& );
    cTrade& operator = ( const cTrade& );

    ~cTrade() {}


    //
    /* Get Method */
    int GetTradeID() const { return m_tradeID; }
    int GetOrderID() const { return m_orderID; }
    std::string GetInstrumentID() const { return m_instrumentID; }
    std::string GetAccountID() const { return m_accountID; }
    char GetDirection() const { return m_direction; }
    char GetOffsetFlag() const { return m_offsetFlag; }
    double GetPrice() const { return m_price; }
    int GetVolume() const { return m_volume; }
    std::string GetTradeTime() const { return m_tradeTime; }
    std::string GetTradeDate() const { return m_tradeDate; }
    double GetCommission() const {return m_commission;}
    void setCommission(CThostFtdcInstrumentCommissionRateField*,CThostFtdcInstrumentField*);
    
    //
    void Print() const;

private:
    int m_tradeID;
    int m_orderID;
    std::string m_instrumentID;
    std::string m_accountID;

    char m_direction;
    char m_offsetFlag;

    double m_price;
    double m_commission;
    int    m_volume;
    std::string m_tradeDate;
    std::string m_tradeTime;
    std::string m_exchange;// exchange
};

typedef std::shared_ptr< cTrade > cTradePtr;


#endif
