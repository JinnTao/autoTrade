#include <cTrade.h>
#include <cOrder.h>

cTrade::cTrade()
:m_tradeID( -1 )
,m_orderID( -1 )
,m_instrumentID( theString )
,m_accountID( theString )
,m_direction( '0' )
,m_offsetFlag( '0' )
,m_price( 0.0 )
,m_volume( 0 )
,m_tradeTime( theTickTime )
{
}

cTrade::cTrade( CThostFtdcTradeField* pTrade )
{
	m_tradeID = atoi( pTrade->TradeID );
	m_orderID = atoi( pTrade->OrderSysID );
	m_instrumentID = cString( pTrade->InstrumentID );
	m_accountID = cString( pTrade->InvestorID );
	m_direction = pTrade->Direction;
	m_offsetFlag = pTrade->OffsetFlag;
	m_price = pTrade->Price;
	m_volume = pTrade->Volume;
	m_tradeTime = cTickTime( pTrade->TradeDate, pTrade->TradeTime );
}

cTrade::cTrade( CThostFtdcOrderField* pOrder )
{
	//m_tradeID = atoi( pOrder->TradeID );
	//m_orderID = atoi( pOrder->OrderSysID );
	//m_instrumentID = cString( pOrder->InstrumentID );
	//m_accountID = cString( pOrder->InvestorID );
	//m_direction = pOrder->Direction;
	//m_offsetFlag = pOrder->CombOffsetFlag;
	//m_price = pOrder->Price;
	//m_volume = pOrder->Volume;
	//m_tradeTime = cTickTime( pOrder->TradeDate, pOrder->TradeTime );
}
//cTrade::cTrade( cOrder* pOrder, int id )
//{
//	m_tradeID = id;
//	m_orderID = pOrder->GetOrderID();
//	m_instrumentID = pOrder->GetInstrumentID();
//	m_accountID = "demo";
//	m_direction = pOrder->GetDirection();
//	m_offsetFlag = pOrder->GetOffsetFlag();
//	m_price = pOrder->GetPrice();
//	m_volume = pOrder->GetVolumeTraded();
//	m_tradeTime = pOrder->GetOrderServerTime();
//}

cTrade::cTrade( const cTrade& in )
{
	yr_assert( this != & in );
	m_tradeID = in.m_tradeID;
	m_orderID = in.m_orderID;
	m_instrumentID = in.m_instrumentID;
	m_accountID = in.m_accountID;
	m_direction = in.m_direction;
	m_offsetFlag = in.m_offsetFlag;
	m_price = in.m_price;
	m_volume = in.m_volume;
	m_tradeTime = in.m_tradeTime;
}

cTrade& cTrade::operator = ( const cTrade& in )
{
	if( &in != this )
	{
		m_tradeID = in.m_tradeID;
		m_orderID = in.m_orderID;
		m_instrumentID = in.m_instrumentID;
		m_accountID = in.m_accountID;
		m_direction = in.m_direction;
		m_offsetFlag = in.m_offsetFlag;
		m_price = in.m_price;
		m_volume = in.m_volume;
		m_tradeTime = in.m_tradeTime;
	}
	return *this;
}

void cTrade::Print() const
{
	printf( "TradeID:%d ", m_tradeID );
	printf( "OrderID:%d ", m_orderID );
	printf( "InstrumentID:%s ", m_instrumentID.c_str() );
	printf( "Direction:%s ", m_direction == '0' ? "B" : "S" );
	printf( "Volume:%d;", m_volume );
	printf( "Price:%5.3f ", m_price );
	printf( "TradeTime:%s %s ", m_tradeTime.DateString().c_str(), m_tradeTime.TimeString().c_str() );
	switch( m_offsetFlag )
	{
	case '0':
		printf( "Type:Open" );
		break;
	case '1':
		printf( "Type:Close" );
		break;
	case '3':
		printf( "Type:CloseToday" );
		break;
	case '4':
		printf( "Type:CloseYesterday" );
		break;
	default:
		break;
	}
	printf( "\n" );
}