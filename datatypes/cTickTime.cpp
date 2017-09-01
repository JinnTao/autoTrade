#include <cTickTime.h>
#include <cString.h>
#include <cStringUtils.h>
#include <cVector.h>
#include <time.h>

cTickTime::cTickTime()
{
	time_t timer;
	time( &timer);
	struct tm* timeinfo;
	timeinfo = localtime( &timer );

	m_date = cDate();
	m_hour = timeinfo->tm_hour;
	m_minute = timeinfo->tm_min;
	m_second = timeinfo->tm_sec;
	m_millisec = 0;
}

cTickTime::cTickTime( const char* date, const char* time, int millisec )
{
	/*
	date is as format of yyyymmdd;
	time is as format of hh::mm::ss
	*/
	m_date = cDate( date );

	cString timestr( time );
	cArray< cString > tmp;
	cString::Split( tmp, timestr, ":" );
	m_hour = AsInt( tmp[0] );
	m_minute = AsInt( tmp[1] );
	m_second = AsInt( tmp[2] );

	m_millisec = millisec;

}

cTickTime::cTickTime( const char* datetime, int millisec  )
{
	/*
	datetime is as format of yyyymmdd hh:mm::ss
	*/
	cString datetimestr( datetime );
	cString datestr = cString::Left( datetimestr, 8 );
	int n = datetimestr.size();
	cString timestr = cString::Right( datetimestr, n - 9 );
	m_date = cDate( datestr );

	cArray< cString > tmp;
	cString::Split( tmp, timestr, ":" );
	m_hour = AsInt( tmp[0] );
	m_minute = AsInt( tmp[1] );
	m_second = AsInt( tmp[2] );

	m_millisec = millisec;
}


cTickTime::~cTickTime()
{
}

cTickTime::cTickTime( const cTickTime& in )
{
	yr_assert( this != & in );
	m_date = in.m_date;
	m_hour = in.m_hour;
	m_minute = in.m_minute;
	m_second = in.m_second;
	m_millisec = in.m_millisec;
}

cTickTime& cTickTime::operator = ( const cTickTime& in )
{
	if( &in != this )
	{
		m_date = in.m_date;
		m_hour = in.m_hour;
		m_minute = in.m_minute;
		m_second = in.m_second;
		m_millisec = in.m_millisec;
	}
	return *this;
}

bool cTickTime::operator == ( const cTickTime& a ) const
{
	if( this == &a )
		return true;
	if( m_date != a.m_date )
		return false;
	if( m_hour != a.m_hour )
		return false;
	if( m_minute != a.m_minute )
		return false;
	if( m_second != a.m_second )
		return false;
	if( m_millisec != a.m_millisec )
		return false;
	return true;
}

bool cTickTime::operator != ( const cTickTime& a ) const
{
	return !( *this == a );
}

bool cTickTime::operator < ( const cTickTime& a ) const
{
	if( this == &a )
		return false;

	if( m_date < a.m_date )
		return true;
	else if ( m_date > a.m_date )
		return false;
	else
	{
		bool flag;
		if( m_hour < a.m_hour )
			flag = true;
		else if( m_hour > a.m_hour )
			flag = false;
		else
		{
			if( m_minute < a.m_minute )
				flag = true;
			else if( m_minute > a.m_minute )
				flag = false;
			else
			{
				if( m_second < a.m_second )
					flag = true;
				else if( m_second > a.m_second )
					flag = false;
				else
				{
					if( m_millisec < a.m_millisec )
						flag = true;
					else
						flag = false;
				}
			}
		}
		return flag;
	}
}

bool cTickTime::operator <= ( const cTickTime& a ) const
{
	if( *this == a || *this < a )
		return true;
	return false;
		
}

bool cTickTime::operator > ( const cTickTime& a ) const
{
	return !(*this <= a );
}

bool cTickTime::operator >= ( const cTickTime& a ) const
{
	return !(*this < a );
}


double cTickTime::TickTimeToDouble() const
{
	// since 1900/01/01
	// the same as excel date
	int date_num = m_date.JulianDate();
	double second_last = (double)m_millisec / 1000;
	second_last += m_second;
	second_last += SECONDSPERMINUTE * m_minute;
	second_last += SECONDSPERHOUR * m_hour;
	
	double second_inday = second_last / SECONDSPERDAY;
	double results = second_inday + date_num;
	return results;

}


cTickTime cTickTime::DoubleToTickTime( double t )
{
	/*
	note: this method is not recommended to use because of rounding issues
	*/
	int date_num;
	int hh, mm, ss;
	int millisec;
	date_num = (int)t;
	double second_inday = t - date_num;
	double second_temp = second_inday * SECONDSPERDAY;
	/*char sec_char[256];
	sprintf( sec_char, "%10.1f", second_temp );
	double second_last = atof( sec_char );
	if( equal( second_last - int(second_last), 0.5 ) )
	{
		second_last -= 0.5;
		millisec = 500;
	}
	else
		millisec = 0;*/
	int second_last = (int)second_temp;
	double second_millisec = ( second_temp - second_last ) * 1000;
	millisec = int(second_millisec);

	hh = (int)( second_last / SECONDSPERHOUR );
	mm = (int)( ( second_last - hh * SECONDSPERHOUR ) / SECONDSPERMINUTE );
	ss = ROUND_2_INT( second_last - hh * SECONDSPERHOUR - mm * SECONDSPERMINUTE );
	if( ss == 60 )
	{
		mm += 1;
		ss = 0;
		if( mm == 60 )
		{
			hh += 1;
			mm = 0;
			if( hh == 24 )
			{
				date_num += 1;
				hh = 0;
			}
		}
	}
	if( mm == 60 )
	{
		hh += 1;
		mm = 0;
		if( hh == 24 )
		{
			date_num += 1;
			hh = 0;
		}
	}
	if( hh == 24 )
	{
		date_num += 1;
		hh = 0;
	}
	yr_assert( hh < 24 && hh >= 0 );
	yr_assert( mm < 60 && mm >= 0 );
	yr_assert( ss < 60 && ss >= 0 );
	//
	cString timestr = ToTimeString( hh, mm, ss );
	
	cString datestr = JulianToGeorgianString( date_num );
	
	return cTickTime( datestr.c_str(), timestr.c_str(), millisec );

}


cString cTickTime::JulianToGeorgianString( TickTimeFormat julian )
{
	// there are totally 1461 days in 4 years
	// and a leap year happens in every 4 years
	int geogian = JulianToGeorgian( julian );

	char buffer[10];
	return cString( itoa( geogian, buffer, 10 ) );
}

TickTimeFormat cTickTime::JulianToGeorgian( TickTimeFormat julian )
{
	return cDate::JulianToGeorgian( julian );
}

TickTimeFormat cTickTime::GeorgianToJulian( TickTimeFormat georgian )
{
	return cDate::GeorgianToJulian( georgian );
}

void cTickTime::VectorToTickTime( const cVector& tVec, cArray< cTickTime >& dtArray )
{
	int n = tVec.getSize();
	if( n < 1 )
		return;
	double tStart = tVec[0];
	int dateStart = (int)tStart;
	cTickTime dtStart = DoubleToTickTime( tStart );
	cString dtStartStr = dtStart.DateString();
	dtArray.resize( n );
	dtArray[0] = dtStart;
	for( int i = 1; i < n; ++i )
	{
		int dateNum = (int)tVec[i];
		if( dateNum == dateStart )
		{
			double secDiff = ( tVec[i] - dateNum ) * SECONDSPERDAY;
			int hh = (int)( secDiff / SECONDSPERHOUR );
			int mm = (int)( ( secDiff - hh * SECONDSPERHOUR ) / SECONDSPERMINUTE );
			int ss = ROUND_2_INT( secDiff - hh * SECONDSPERHOUR - mm * SECONDSPERMINUTE );
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
			//
			cString timestr = ToTimeString( hh, mm, ss );
			dtArray[i] = cTickTime( dtStartStr.c_str(), timestr.c_str() );

		}
		else
		{
			cTickTime dt = DoubleToTickTime( tVec[i] );
			dateStart = dateNum;
			dtStartStr = dt.DateString();
		}	
	}
}

void cTickTime::VectorToTickTimeString( const cVector& tVec, cArray< cString >& dtstrArray )
{
	int n = tVec.getSize();
	if( n < 1 )
		return;
	double tStart = tVec[0];
	int dateStart = (int)tStart;
	cTickTime dtStart = DoubleToTickTime( tStart );
	cString dtStartStr = dtStart.DateString();
	dtstrArray.resize( n );
	cString dtStartFullStr = dtStart.TickTimeFullString();
	dtstrArray[0] = dtStartFullStr;
	for( int i = 1; i < n; ++i )
	{
		int dateNum = (int)tVec[i];
		if( dateNum == dateStart )
		{
			double secDiff = ( tVec[i] - dateNum ) * SECONDSPERDAY;
			int hh = (int)( secDiff / SECONDSPERHOUR );
			int mm = (int)( ( secDiff - hh * SECONDSPERHOUR ) / SECONDSPERMINUTE );
			int ss = ROUND_2_INT( secDiff - hh * SECONDSPERHOUR - mm * SECONDSPERMINUTE );
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
			//
			cString timestr = ToTimeString( hh, mm, ss );
			dtstrArray[i] = dtStartStr + " " + timestr;

		}
		else
		{
			cTickTime dt = DoubleToTickTime( tVec[i] );
			dateStart = dateNum;
			dtStartStr = dt.DateString();
		}	
	}
}


cString cTickTime::TickTimeFullString() const
{
	char datetime_buf[256];
	sprintf(datetime_buf, "%08d %02d:%02d:%02d %03d", m_date.GeorgianDate(), m_hour, m_minute, m_second, m_millisec );
	return cString(datetime_buf);
}

cString cTickTime::DateString() const
{
	return m_date.DateToString();
}

cString cTickTime::TimeString() const
{
	char time_buf[256];
	sprintf( time_buf, "%02d:%02d:%02d", m_hour, m_minute, m_second );
	return cString( time_buf );
}

cString cTickTime::ToDateString( int year, int month, int day )
{
	char date_buf[256];
	sprintf(date_buf, "%04d%02d%02d", year, month, day );
	return cString( date_buf );
}


cString cTickTime::ToTimeString( int hh, int mm, int ss )
{
	char time_buf[256];
	sprintf(time_buf, "%02d:%02d:%02d", hh, mm, ss );
	return cString( time_buf );
}

int cTickTime::TimeToSecInteger( double t )
{
	int date_num = (int)t;
	double second_inday = t - date_num;
	double second_last = second_inday * SECONDSPERDAY;
	int hh = (int)( second_last / SECONDSPERHOUR );
	int mm = (int)( ( second_last - hh * SECONDSPERHOUR ) / SECONDSPERMINUTE );
	int ss = ROUND_2_INT( second_last - hh * SECONDSPERHOUR - mm * SECONDSPERMINUTE );
	int sec = hh * SECONDSPERHOUR + mm * SECONDSPERMINUTE + ss;
	return sec;
}


int interval( const cArray< cTickTime >& tickTimeArray, const cTickTime& tickTime, int first )
{
	int size = tickTimeArray.getSize();
	yr_assert( size > 0 );
	int il = first;
	if( tickTime >= tickTimeArray[size-1] )
		return size - 1;
	if( ( first >= 0 ) && ( first < size - 1 ) )
	{
		if( ( tickTimeArray[first] <= tickTime ) && ( tickTime < tickTimeArray[first+1] ) )
			return il;
	}

	int ih = size - 1;

	while( ih > il + 1 )
	{
		int midpoint = ( il + ih ) / 2;
		if( tickTime < tickTimeArray[midpoint] )
			ih = midpoint;
		else
			il = midpoint;
	}
	return il;
}

int location( const cArray< cTickTime>& tickTimeArray, const cTickTime& tickTime, int first )
{
	int il = interval( tickTimeArray, tickTime, first );
	int ih = il + 1;
	int n = tickTimeArray.getSize();

	if( il == n - 1 )
		return il;

	if( n > 1 )
	{
		double tl = tickTimeArray[il].TickTimeToDouble();
		double th = tickTimeArray[ih].TickTimeToDouble();
		double t = tickTime.TickTimeToDouble();
		if( ( fabs( t - tl ) ) < ( fabs( t - th ) ) )
			return il;
		else
			return ih;
	}
	else
		return il;
}

const cTickTime theTickTime = cTickTime();