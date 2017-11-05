#include <cCandle.h>


cCandle::cCandle()
:m_set( false )
,m_pxVec( theVector )
,m_open( -9.99 )
,m_high( -9.99 )
,m_low( -9.99 )
,m_close( -9.99 )
,m_tickCount( -1 )
,m_datenumStart( -1 )
,m_datenumEnd( -1 )
,m_secStart( -1 )
,m_secEnd( -1 )
,m_start( theTickTime )
,m_end( theTickTime )
{
}

cCandle::cCandle( const cCandle& in )
{
	yr_assert( this != & in );
	m_set = in.m_set;
	m_pxVec = in.m_pxVec;
	m_open = in.m_open;
	m_high = in.m_high;
	m_low = in.m_low;
	m_close = in.m_close;
	m_tickCount = in.m_tickCount;
	m_datenumStart = in.m_datenumStart;
	m_datenumEnd = in.m_datenumEnd;
	m_secStart = in.m_secStart;
	m_secEnd = in.m_secEnd;
	m_start = in.m_start;
	m_end = in.m_end;

	if( m_timeVec.getSize() )
		m_timeVec.clear();
	for( int i = 0; i < in.m_timeVec.getSize(); ++i )
		m_timeVec.push_back( in.m_timeVec[i] );

}

cCandle& cCandle::operator = ( const cCandle& in )
{
	if( &in != this )
	{
		m_set = in.m_set;
		m_pxVec = in.m_pxVec;
		m_open = in.m_open;
		m_high = in.m_high;
		m_low = in.m_low;
		m_close = in.m_close;
		m_tickCount = in.m_tickCount;
		m_datenumStart = in.m_datenumStart;
		m_datenumEnd = in.m_datenumEnd;
		m_secStart = in.m_secStart;
		m_secEnd = in.m_secEnd;
		m_start = in.m_start;
		m_end = in.m_end;

		if( m_timeVec.getSize() )
			m_timeVec.clear();
		for( int i = 0; i < in.m_timeVec.getSize(); ++i )
			m_timeVec.push_back( in.m_timeVec[i] );
	}
	return *this;
}

cCandle::cCandle( int julianDateStart, int julianDateEnd, int secStart, int secEnd )
:m_set( false )
,m_pxVec( theVector )
,m_open( -9.99 )
,m_high( -9.99 )
,m_low( -9.99 )
,m_close( -9.99 )
,m_tickCount( -1 )
,m_datenumStart( julianDateStart )
,m_datenumEnd( julianDateEnd )
,m_secStart( secStart )
,m_secEnd( secEnd )
{
	int hhStart, hhEnd, mmStart, mmEnd, ssStart, ssEnd;
	hhStart = m_secStart / SECONDSPERHOUR;
	mmStart = ( m_secStart - hhStart * SECONDSPERHOUR ) / SECONDSPERMINUTE;
	ssStart = m_secStart - hhStart * SECONDSPERHOUR - mmStart * SECONDSPERMINUTE;
	
	if( ssStart == 60 )
	{
		mmStart += 1;
		ssStart = 0;
		if( mmStart == 60 )
		{
			hhStart += 1;
			mmStart = 0;
		}
	}
	yr_assert( hhStart < 24 && hhStart >= 0 );
	yr_assert( mmStart < 60 && mmStart >= 0 );
	yr_assert( ssStart < 60 && ssStart >= 0 );

	cString dateStart  = cDate::JulianDateToString( julianDateStart );
	cString timeStart = cTickTime::ToTimeString( hhStart, mmStart, ssStart );
	m_start = cTickTime( dateStart.c_str(), timeStart.c_str() );

	if( m_secEnd == SECONDSPERDAY )
	{
		cString dateEnd  = cDate::JulianDateToString( julianDateEnd );
		m_end = cTickTime( dateEnd.c_str(), "23:59:59", 999 );
		return;
	}

	hhEnd = m_secEnd / SECONDSPERHOUR;
	mmEnd = ( m_secEnd - hhEnd * SECONDSPERHOUR ) / SECONDSPERMINUTE;
	ssEnd = m_secEnd - hhEnd * SECONDSPERHOUR - mmEnd * SECONDSPERMINUTE;
	
	if( ssEnd == 60 )
	{
		mmEnd += 1;
		ssEnd = 0;
		if( mmEnd == 60 )
		{
			hhEnd += 1;
			mmEnd = 0;
		}
	}
	yr_assert( hhEnd < 24 && hhEnd >= 0 );
	yr_assert( mmEnd < 60 && mmEnd >= 0 );
	yr_assert( ssEnd < 60 && ssEnd >= 0 );

	cString dateEnd  = cDate::JulianDateToString( julianDateEnd );
	cString timeEnd = cTickTime::ToTimeString( hhEnd, mmEnd, ssEnd );
	m_end = cTickTime( dateEnd.c_str(), timeEnd.c_str() );

}

cCandle::cCandle( const cTickTime& startDT, const cTickTime& endDT )
:m_set( false )
,m_pxVec( theVector )
,m_open( -9.99 )
,m_high( -9.99 )
,m_low( -9.99 )
,m_close( -9.99 )
,m_tickCount( -1 )
{
	m_datenumStart = startDT.Date().JulianDate();
	m_datenumEnd = endDT.Date().JulianDate();
	m_secStart = startDT.Hour() * SECONDSPERHOUR + startDT.Minute() * SECONDSPERMINUTE + startDT.Second();
	m_secEnd = endDT.Hour() * SECONDSPERHOUR + endDT.Minute() * SECONDSPERMINUTE + endDT.Second();
}


cCandle::~cCandle()
{
	Clear();
}

void cCandle::Clear()
{
	m_set = false;
	m_pxVec.clear();
	m_timeVec.clear();
	m_open = -9.99;
	m_high = -9.99;
	m_low = -9.99;
	m_close = -9.99;
	m_tickCount = -1;
	m_datenumStart = -1;
	m_datenumEnd = -1;
	m_secStart = -1;
	m_secEnd = -1;
}

void cCandle::Set()
{
	if( m_set )
		return;

	m_set = true;
	if( m_pxVec.getSize() > 0 )
	{
		m_open = m_pxVec[0];
		m_high = m_pxVec.Max();
		m_low = m_pxVec.Min();
		m_tickCount = m_pxVec.getSize();
		m_close = m_pxVec.getLast();
		m_pxVec.clear();
		m_timeVec.clear();
	}
	else
	{
		m_set = false;
		/*yr_error( "candle can NOT be set with empty tick data." );*/
	}

}

bool cCandle::IsSet() const
{
	return m_set;
}

double cCandle::Open() const 
{ 
	if( m_set )
		return m_open;
	else
		yr_error( "candle NOT set." );
}

double cCandle::High() const
{ 
	if( m_set )
		return m_high;
	else
		yr_error( "candle NOT set." );
}

double cCandle::Low() const
{
	if( m_set )
		return m_low;
	else
		yr_error( "candle NOT set." );
}

double cCandle::Close() const
{
	if( m_set )
		return m_close;
	else
		yr_error( "candle NOT set." );
}

int cCandle::TickCount() const
{
	if( m_set )
		return m_tickCount;
	else
		yr_error( "candle NOT set." );
}

double cCandle::RealTimeOpen() const
{
	if( m_set || !( m_pxVec.getSize() > 0 ) )
		yr_error( "candle set and real-time tick data is cleared." );
	
	return m_pxVec[0];
}

double cCandle::RealTimeHigh() const
{
	if( m_set || !( m_pxVec.getSize() > 0 ) )
		yr_error( "candle set and real-time tick data is cleared." );

	return m_pxVec.Max();
}

double cCandle::RealTimeLow() const
{
	if( m_set || !( m_pxVec.getSize() > 0 ) )
		yr_error( "candle set and real-time tick data is cleared." );

	return m_pxVec.Min();
}

double cCandle::RealTimeClose() const
{
	if( m_set || !( m_pxVec.getSize() > 0 ) )
		yr_error( "candle set and real-time tick data is cleared." );

	return m_pxVec.getLast();
}

void cCandle::Print( cString& candleInfo ) const
{
	if( m_set )
	{
		char message[256];
		sprintf( message, "o:%5.3f h:%5.3f l:%5.3f c:%5.3f n:%d", m_open, m_high, m_low, m_close, m_tickCount );
		candleInfo = cString( message );
	}
	else
		yr_error( "candle NOT set." );

}



//cCandleIntraday::cCandleIntraday()
//:cCandle()
//,m_secStart(0)
//,m_secEnd(1)
//{
//
//}
//
//cCandleIntraday::cCandleIntraday( int julianDate, int secStart, int secEnd )
//:cCandle()
//,m_datenum( julianDate )
//,m_secStart( secStart )
//,m_secEnd( secEnd )
//{
//	yr_assert( m_secStart < m_secEnd );
//}
//
//cCandleIntraday::~cCandleIntraday()
//{
//}
//
//cCandleIntraday::cCandleIntraday( const cCandleIntraday& in )
//{
//	yr_assert( this != & in );
//	m_set = in.m_set;
//	m_pxVec = in.m_pxVec;
//
//	if( m_timeVec.getSize() )
//		m_timeVec.clear();
//	for( int i = 0; i < in.m_timeVec.getSize(); ++i )
//		m_timeVec.push_back( in.m_timeVec[i] );
//	
//	m_datenum = in.m_datenum;
//	m_secStart = in.m_secStart;
//	m_secEnd = in.m_secEnd;
//	m_secVec = in.m_secVec;
//	if( m_set )
//	{
//		m_open = in.m_open;
//		m_high = in.m_high;
//		m_low = in.m_low;
//		m_close = in.m_close;
//		m_tickCount = in.m_tickCount;
//	}
//}
//
//cCandleIntraday& cCandleIntraday::operator = ( const cCandleIntraday& in )
//{
//	if( &in != this )
//	{
//		m_set = in.m_set;
//		m_pxVec = in.m_pxVec;
//		
//		if( m_timeVec.getSize() )
//			m_timeVec.clear();
//		for( int i = 0; i < in.m_timeVec.getSize(); ++i )
//			m_timeVec.push_back( in.m_timeVec[i] );
//		
//		m_datenum = in.m_datenum;
//		m_secStart = in.m_secStart;
//		m_secEnd = in.m_secEnd;
//		m_secVec = in.m_secVec;
//		if( m_set )
//		{
//			m_open = in.m_open;
//			m_high = in.m_high;
//			m_low = in.m_low;
//			m_close = in.m_close;
//			m_tickCount = in.m_tickCount;
//		}
//	}
//	return *this;
//}
//
//void cCandleIntraday::Set()
//{
//	if( m_set )
//		return;
//
//	m_set = true;
//	if( m_pxVec.getSize() > 0 )
//	{
//		m_open = m_pxVec[0];
//		m_high = m_pxVec.Max();
//		m_low = m_pxVec.Min();
//		m_tickCount = m_pxVec.getSize();
//		m_close = m_pxVec[ m_tickCount - 1];
//		m_pxVec.clear();
//		m_secVec.resize( 0 );
//		m_timeVec.resize( 0 );
//	}
//	else
//	{
//		m_set = false;
//		yr_error( "candle can NOT be set with empty tick data." );
//	}
//	
//}
//
//void cCandleIntraday::PrintTick() const
//{
//	if( !m_set )
//	{
//		for( int i = 0; i < m_pxVec.getSize(); ++i )
//		{
//			cout << m_timeVec[i].TickTimeFullString();
//			cout << m_pxVec[i] << endl;
//		}	
//	}
//	else
//		yr_error( "candle set and real-time tick data is cleared." );
//}
//
int cCandle::DateStart( cString& dateStr ) const
{
	dateStr = cDate::JulianDateToString( m_datenumStart );
	return m_datenumStart;
}

int cCandle::DateEnd( cString& dateStr ) const
{
	dateStr = cDate::JulianDateToString( m_datenumEnd );
	return m_datenumEnd;
}


void cCandle::TimeStart( cString& timeStr ) const
{

	int hh = m_secStart / SECONDSPERHOUR;
	int mm = ( m_secStart - hh * SECONDSPERHOUR ) / SECONDSPERMINUTE;
	int ss = m_secStart - hh * SECONDSPERHOUR - mm * SECONDSPERMINUTE;
	
	if( ss == 60 )
	{
		mm += 1;
		ss = 0;
		if( mm == 60 )
		{
			hh += 1;
			mm = 0;
		}
	}
	yr_assert( hh < 24 && hh >= 0 );
	yr_assert( mm < 60 && mm >= 0 );
	yr_assert( ss < 60 && ss >= 0 );
	timeStr = cTickTime::ToTimeString( hh, mm, ss );
}

void cCandle::TimeEnd( cString& timeStr ) const
{
	if( m_secEnd == SECONDSPERDAY || m_secEnd == 0 )
	{
		timeStr = cTickTime::ToTimeString( 0, 0, 0 );
		return;
	}
	
	int hh = m_secEnd / SECONDSPERHOUR;
	int mm = ( m_secEnd - hh * SECONDSPERHOUR ) / SECONDSPERMINUTE;
	int ss = m_secEnd - hh * SECONDSPERHOUR - mm * SECONDSPERMINUTE;
	
	if( ss == 60 )
	{
		mm += 1;
		ss = 0;
		if( mm == 60 )
		{
			hh += 1;
			mm = 0;
		}
	}
	yr_assert( hh < 24 && hh >= 0 );
	yr_assert( mm < 60 && mm >= 0 );
	yr_assert( ss < 60 && ss >= 0 );
	timeStr = cTickTime::ToTimeString( hh, mm, ss );

}

int cCandle::TimeStart() const
{
	return m_secStart;
}

int cCandle::TimeEnd() const
{
	return m_secEnd;
}

cString cCandle::BarSize() const
{
	if( m_datenumStart == m_datenumEnd )
	{
		int mm = ( m_secEnd - m_secStart ) / SECONDSPERMINUTE;
		char barSize[256];
		if( mm < MINUTESPERHOUR )
		{
			sprintf( barSize, "%dm", mm );
			return cString(barSize);
		}
		else
		{
			int hh = mm / MINUTESPERHOUR;
			sprintf( barSize, "%dh", mm );
			return cString(barSize);
		}
	}
	else
	{
		int dd = m_datenumEnd - m_datenumStart;
		char barSize[256];
		sprintf( barSize, "%dd", dd );
		return cString(barSize);
	}
}


void cCandle::UpdateWithTick( const cTick& tick )
{
	cTickTime tt = tick.TickTime();
	if( tt > m_start && tt <= m_end )
	{
		m_timeVec.push_back( tt );
		m_pxVec.push_back( tick.Last() );
	}

}

