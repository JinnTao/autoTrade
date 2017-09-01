#include <cSignal.h>

const int NUM_ORDERDIRECTION = 2;

const cString order_direction[NUM_ORDERDIRECTION] = 
{
	"SELL",
	"BUY"
};

cString SignalDirectionToString( const SwSignalDirection& direction )
{
	if( !IsValidOrderDirection( direction ) )
		return "";

	return order_direction[ direction ];
}

SwSignalDirection StringToOrderDirection( const cString& directionstr )
{
	int i;
	for( i = 0; i < NUM_ORDERDIRECTION; ++i )
	{
		if( Compare( directionstr, order_direction[i] ) )
			break;
	}
	if( !IsValidOrderDirection( i ) )
		yr_error( "Invalid Direction" );

	return (SwSignalDirection)i;
}

////**********************************************************************
//const int NUM_ORDEROFFSETFLAG = 4;
//
//const cString order_offsetflag[NUM_ORDEROFFSETFLAG] = 
//{
//	"OPEN",
//	"CLOSE",
//	"CLOSETODAY",
//	"CLOSEYESTERDAY"
//};
//
//cString OrderOffsetFlagToString( const SwOrderOffsetFlag& flag )
//{
//	if( !IsValidOrderOffsetFlag( flag ) )
//		return "";
//
//	return order_offsetflag[ flag ];
//}
//
//SwOrderOffsetFlag StringToOrderOffsetFlag( const cString& flagstr )
//{
//	int i;
//	for( i = 0; i < NUM_ORDEROFFSETFLAG; ++i )
//	{
//		if( Compare( flagstr, order_offsetflag[i] ) )
//			break;
//	}
//	if( !IsValidOrderOffsetFlag( i ) )
//		yr_error( "Invalid OffsetFlag" );
//
//	return (SwOrderOffsetFlag)i;
//}


//**********************************************************************
cSignal::cSignal()
: m_instrumentID( theString )
, m_price( -9.99 )
, m_volume( 0 )
, m_time( theTickTime )
, m_direction( SwSignalDirection::SWSIGNALDIRECTION_INVALID )
{
}

cSignal::~cSignal()
{
}

cSignal::cSignal( const cSignal& in )
{
	yr_assert( this != & in );
	m_instrumentID = in.m_instrumentID;
	m_direction = in.m_direction;
	m_price = in.m_price;
	m_volume = in.m_volume;
	m_time = in.m_time;
}


cSignal& cSignal::operator = ( const cSignal& in )
{
	if( &in != this )
	{
		m_instrumentID = in.m_instrumentID;
		m_direction = in.m_direction;
		m_price = in.m_price;
		m_volume = in.m_volume;
		m_time = in.m_time;
	}
	return *this;
}


bool cSignal::operator == ( const cSignal& in ) const
{
	if( this == &in )
		return true;
	if( m_instrumentID != in.m_instrumentID )
		return false;
	if( m_direction != in.m_direction )
		return false;
	if( m_price != in.m_price )
		return false;
	if( m_volume != in.m_volume )
		return false;
	if( m_time != in.m_time )
		return false;
	return true;
}

bool cSignal::operator != ( const cSignal& in ) const
{
	return !( *this == in );
}

void cSignal::SetDirection( int direction )
{
	if( !IsValidOrderDirection( direction ) )
		yr_error( "Invalid Direction" );

	m_direction = SwSignalDirection( direction );
}

void cSignal::SetDirection( const cString& directionstr )
{
	int direction = StringToOrderDirection( directionstr );
	SetDirection( direction );
}

cString cSignal::GetInstrumentID() const
{
	return m_instrumentID;
}

char cSignal::GetDirection() const
{
	char c[5];
	switch( m_direction )
	{
	case SwSignalDirection::SWSIGNALDIRECTION_BUY:
		sprintf( c, "%d", 0 );
		break;
	case SwSignalDirection::SWSIGNALDIRECTION_SELL:
		sprintf( c, "%d", 1 );
		break;
	default:
		sprintf( c, "%d", -1 );
		break;
	}
	return c[0];
}

double cSignal::GetPrice() const
{
	return m_price;
}

unsigned int cSignal::GetVolume() const
{
	return m_volume;
}

cTickTime cSignal::GetTime() const
{
	return m_time;
}

cString cSignal::GetSignalID() const
{
	cString ans( m_instrumentID );
	ans += "_";
	ans += SignalDirectionToString( m_direction );
	ans += "_";
	ans += m_time.DateString() + " " + m_time.TimeString();
	return ans;
}