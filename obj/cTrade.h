#ifndef __CTRADE_H__
#define __CTRADE_H__

#include <cString.h>
#include <cTickTime.h>
#include <stl.h>
#include <ThostFtdcUserApiStruct.h>




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
	cString GetInstrumentID() const { return m_instrumentID; }
	cString GetAccountID() const { return m_accountID; }
	char GetDirection() const { return m_direction; }
	char GetOffsetFlag() const { return m_offsetFlag; }
	double GetPrice() const { return m_price; }
	int GetVolume() const { return m_volume; }
	cTickTime GetTradeTime() const { return m_tradeTime; }
	
	//
	void Print() const;

private:
	int m_tradeID;
	int m_orderID;
	cString m_instrumentID;
	cString m_accountID;

	char m_direction;
	char m_offsetFlag;

	double m_price;
	int	m_volume;
	cTickTime m_tradeTime;
};

typedef shared_ptr< cTrade > cTradePtr;


#endif
