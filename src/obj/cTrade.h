#ifndef __CTRADE_H__
#define __CTRADE_H__

#include <cString.h>
#include "ThostFtdcUserApiDataType.h"
#include <ThostFtdcUserApiStruct.h>
#include <memory>
using std::shared_ptr;



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
	string GetInstrumentID() const { return m_instrumentID; }
	string GetAccountID() const { return m_accountID; }
	char GetDirection() const { return m_direction; }
	char GetOffsetFlag() const { return m_offsetFlag; }
	double GetPrice() const { return m_price; }
	int GetVolume() const { return m_volume; }
	string GetTradeTime() const { return m_tradeTime; }
	string GetTradeDate() const { return m_tradeDate; }
	double GetCommission() const {return m_commission;}
	void setCommission(CThostFtdcInstrumentCommissionRateField*,CThostFtdcInstrumentField*);
	
	//
	void Print() const;

private:
	int m_tradeID;
	int m_orderID;
	string m_instrumentID;
	string m_accountID;

	char m_direction;
	char m_offsetFlag;

	double m_price;
	double m_commission;
	int	m_volume;
	string m_tradeDate;
	string m_tradeTime;
	string m_exchange;// exchange
};

typedef shared_ptr< cTrade > cTradePtr;


#endif
