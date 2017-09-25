#include <cOrder.h>

cOrder::cOrder()
:m_orderID( -1 )
,m_instrumentID( theString )
,m_accountID( theString )
,m_orderRef( 0 )
,m_direction( '0' )
,m_offsetFlag( '0' )
,m_status( 'a' )
,m_price( 0.0 )
,m_volumeOriginal( 0 )
,m_volumeTraded( 0 )
,m_orderServerTime( theTickTime )
,m_orderLocalTime( theTickTime )
,m_frontID( -1 )
,m_sessionID( -1 )
{
	m_BrokerOrderSeq = 0;
	memset(&this->m_orderField,0,sizeof(CThostFtdcOrderField));
}

cOrder::cOrder( CThostFtdcOrderField* pOrder )
{
	m_orderID = atoi( pOrder->OrderSysID );
	m_instrumentID = cString( pOrder->InstrumentID );
	m_accountID = cString( pOrder->InvestorID );
	m_orderRef = atoi( pOrder->OrderRef );
	m_direction = pOrder->Direction;
	m_offsetFlag = pOrder->CombOffsetFlag[0];
	m_status = pOrder->OrderStatus;
	m_price = pOrder->LimitPrice;
	m_volumeOriginal = pOrder->VolumeTotalOriginal;
	m_volumeTraded = pOrder->VolumeTraded;
	m_orderServerTime = cTickTime( pOrder->InsertDate, pOrder->InsertTime );
	m_orderLocalTime = theTickTime;
	strcpy( m_orderSysID, pOrder->OrderSysID );
	strcpy( ExchangeID, pOrder->ExchangeID );// exchange Id
	m_frontID = pOrder->FrontID;
	m_sessionID = pOrder->SessionID;
	m_BrokerOrderSeq = pOrder->BrokerOrderSeq;

	memccpy(&this->m_orderField,pOrder,sizeof(CThostFtdcOrderField));
}

cOrder::cOrder( const cOrder& in )
{
	yr_assert( this != & in );
	m_orderID = in.m_orderID;
	m_instrumentID = in.m_instrumentID;
	m_accountID = in.m_accountID;
	m_orderRef = in.m_orderRef;
	m_direction = in.m_direction;
	m_offsetFlag = in.m_offsetFlag;
	m_status = in.m_status;
	m_price = in.m_price;
	m_volumeOriginal = in.m_volumeOriginal;
	m_volumeTraded = in.m_volumeTraded;
	m_orderServerTime = in.m_orderServerTime;
	m_orderLocalTime = in.m_orderLocalTime;
	strcpy( m_orderSysID, in.m_orderSysID );
	strcpy( ExchangeID, in.ExchangeID );// exchange Id
	m_frontID = in.m_frontID;
	m_sessionID = in.m_sessionID;
	m_BrokerOrderSeq = in.m_BrokerOrderSeq;
}

cOrder& cOrder::operator = ( const cOrder& in )
{
	if( &in != this )
	{
		m_orderID = in.m_orderID;
		m_instrumentID = in.m_instrumentID;
		m_accountID = in.m_accountID;
		m_orderRef = in.m_orderRef;
		m_direction = in.m_direction;
		m_offsetFlag = in.m_offsetFlag;
		m_status = in.m_status;
		m_price = in.m_price;
		m_volumeOriginal = in.m_volumeOriginal;
		m_volumeTraded = in.m_volumeTraded;
		m_orderServerTime = in.m_orderServerTime;
		m_orderLocalTime = in.m_orderLocalTime;
		strcpy( m_orderSysID, in.m_orderSysID );
		strcpy( ExchangeID, in.ExchangeID );// exchange Id
		m_frontID = in.m_frontID;
		m_sessionID = in.m_sessionID;
		m_BrokerOrderSeq = in.m_BrokerOrderSeq;
	}
	return *this;
}

void cOrder::Print() const
{
	if( m_orderID <= 0 )
		return;
	printf( "No:%d ", m_orderID );
	//printf( "OrderID:%d ", m_orderID );
	printf( "InstrumentID:%s ", m_instrumentID.c_str() );
	printf( "Direction:%s ", m_direction == '0' ? "B" : "S" );
	printf( "Price:%5.3f ", m_price );
	printf( "VolumeOriginal:%d ", m_volumeOriginal );
	printf( "VolumeNonTraded:%d ", m_volumeOriginal - m_volumeTraded );
	
	switch( m_offsetFlag )
	{
	case '0':
		printf( "Type:Open " );
		break;
	case '1':
		printf( "Type:Close " );
		break;
	case '3':
		printf( "Type:CloseToday " );
		break;
	case '4':
		printf( "Type:CloseYesterday " );
		break;
	default:
		break;
	}

	switch( m_status )
	{
	case '0':
		printf( "Status:%s ", "AllTraded" );
		break;
	case '1':
		printf( "Status:%s ", "PartTradedQueueing" );
		break;
	case '2':
		printf( "Status:%s ", "PartTradedNotQueueing" );
		break;
	case '3':
		printf( "Status:%s ", "NoTradeQueueing" );
		break;
	case '4':
		printf( "Status:%s ", "NoTradeNotQueueing" );
		break;
	case '5':
		printf( "Status:%s ", "Canceled" );
		break;
	case 'a':
		printf( "Status:%s ", "Unknown" );
		break;
	case 'b':
		printf( "Status: %s ", "NotTouched" );
		break;
	case 'c':
		printf( "Status: %s ", "Touched" );
		break;
	default:
		break;
	}
	
	printf( "\n" );
}

bool cOrder::IsPendingOrder() const
{
	return( ( m_status != THOST_FTDC_OST_PartTradedNotQueueing ) &&
			( m_status != THOST_FTDC_OST_Canceled ) && 
			( m_status != THOST_FTDC_OST_AllTraded ) && 
			( m_status != THOST_FTDC_OST_Unknown ) );
}

bool cOrder::IsCancelledOrder() const
{
	return( m_status == THOST_FTDC_OST_Canceled );
}


