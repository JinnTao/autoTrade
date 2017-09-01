#include <cDateTime.h>
#include <cString.h>
#include <cStringUtils.h>
#include <cVector.h>
#include <time.h>

cDateTime::cDateTime()
{
	time_t timer;
	time( &timer);
	struct tm* timeinfo;
	timeinfo = localtime( &timer );
	m_year = timeinfo->tm_year + 1900;
	m_month = timeinfo->tm_mon + 1;
	m_day = timeinfo->tm_mday;
	m_date = m_year * 10000 + m_month * 100 + m_day;
	m_hour = timeinfo->tm_hour;
	m_minute = timeinfo->tm_min;
	m_second = timeinfo->tm_sec;
}

cDateTime::cDateTime( const char* date, const char* time )
{
	/*
	date is as format of yyyymmdd;
	time is as format of hh::mm::ss
	*/
	cString datestr( date );
	cString timestr( time );
	m_date = AsInt( datestr );
	m_year = AsInt( cString::Left( datestr, 4 ) );
	m_month = AsInt( cString::Mid( datestr, 4,2) );
	m_day = AsInt( cString::Right( datestr,2) );
	cArray< cString > tmp;
	cString::Split( tmp, timestr, ":" );
	m_hour = AsInt( tmp[0] );
	m_minute = AsInt( tmp[1] );
	m_second = AsInt( tmp[2] );
}

cDateTime::cDateTime( const char* datetime )
{
	/*
	datetime is as format of yyyymmdd hh:mm::ss
	*/
	cString datetimestr( datetime );
	cString datestr = cString::Left( datetimestr, 8 );
	int n = datetimestr.size();
	cString timestr = cString::Right( datetimestr, n - 9 );
	m_date = AsInt( datestr );
	m_year = AsInt( cString::Left( datestr, 4 ) );
	m_month = AsInt( cString::Mid( datestr, 4,2) );
	m_day = AsInt( cString::Right( datestr,2) );
	cArray< cString > tmp;
	cString::Split( tmp, timestr, ":" );
	m_hour = AsInt( tmp[0] );
	m_minute = AsInt( tmp[1] );
	m_second = AsInt( tmp[2] );
}


cDateTime::~cDateTime()
{
}

cDateTime::cDateTime( const cDateTime& in )
{
	yr_assert( this != & in );
	m_date = in.m_date;
	m_year = in.m_year;
	m_month = in.m_month;
	m_day = in.m_day;
	m_hour = in.m_hour;
	m_minute = in.m_minute;
	m_second = in.m_second;
}

cDateTime& cDateTime::operator = ( const cDateTime& in )
{
	if( &in != this )
	{
		m_date = in.m_date;
		m_year = in.m_year;
		m_month = in.m_month;
		m_day = in.m_day;
		m_hour = in.m_hour;
		m_minute = in.m_minute;
		m_second = in.m_second;
	}
	return *this;
}

bool cDateTime::operator == ( const cDateTime& a ) const
{
	if( this == &a )
		return true;
	if( m_date != a.m_date )
		return false;
	if( m_year != a.m_year )
		return false;
	if( m_month != a.m_month )
		return false;
	if( m_day != a.m_day )
		return false;
	if( m_hour != a.m_hour )
		return false;
	if( m_minute != a.m_minute )
		return false;
	if( m_second != a.m_second )
		return false;
	return true;
}

bool cDateTime::operator != ( const cDateTime& a ) const
{
	return !( *this == a );
}

bool cDateTime::IsWeekend() const
{
	struct tm* timeinfo;
	time_t rawtime;
	time( &rawtime );
	timeinfo = localtime( &rawtime );
	timeinfo->tm_year = m_year - 1900;
	timeinfo->tm_mon = m_month - 1;
	timeinfo->tm_mday = m_day;
	timeinfo->tm_hour = m_hour;
	timeinfo->tm_min = m_minute;
	timeinfo->tm_sec = m_second;
	mktime( timeinfo );
	int wkday = timeinfo->tm_wday;
	if( wkday == 0 || wkday == 6 )
		return true;
	else
		return false;
}


bool cDateTime::IsHoliday() const
{
	//todo: not implemented yet
	return IsWeekend();
}

double cDateTime::DateTimeToDouble() const
{
	// since 1900/01/01
	// the same as excel date
	struct tm* timeinfo;
	time_t rawtime;
	time( &rawtime );
	timeinfo = localtime( &rawtime );
	timeinfo->tm_year = m_year - 1900;
	timeinfo->tm_mon = m_month - 1;
	timeinfo->tm_mday = m_day;
	timeinfo->tm_hour = m_hour;
	timeinfo->tm_min = m_minute;
	timeinfo->tm_sec = m_second;
	mktime( timeinfo );

	int date_num;
	int year_left = timeinfo->tm_year;
	int m = year_left % 4;
	int n = ( year_left - m )/4;
	if( n == 0 )
	{
		// 1900 is a leap year
		date_num = m * 365 + 1;
	}
	else
	{
		date_num = n * 1461;
		if( m > 0 )
			date_num += m * 365 + 1;	// the first year of every loop is a leap year
		date_num += timeinfo->tm_yday + 1;
	}
	double second_last = SECONDSPERHOUR * m_hour + SECONDSPERMINUTE * m_minute + m_second;
	double second_inday = second_last / SECONDSPERDAY;

	double results = second_inday + date_num;

	return results;

}

double cDateTime::DateTimeToDouble( const cDateTime& dt ) const
{
	return dt.DateTimeToDouble();
}

cDateTime cDateTime::DoubleToDateTime( double t )
{
	int date_num;
	int hh, mm, ss;
	char buffer[10];
	date_num = (int)t;
	double second_inday = t - date_num;
	double second_last = second_inday * SECONDSPERDAY;
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
		}
	}
	yr_assert( hh < 24 && hh >= 0 );
	yr_assert( mm < 60 && mm >= 0 );
	yr_assert( ss < 60 && ss >= 0 );
	//
	cString timestr = ToTimeString( hh, mm, ss );

	int n = (int)( date_num / 1461 );
	if( date_num % 1461 == 0 )
	{
		int year = n * 4 + 1900 - 1;
		cString datestr = cString( itoa( year, buffer, 10 ) ) + "1231";
		return cDateTime( datestr.c_str(), timestr.c_str() );
	}
	else
	{
		int m = int( ( date_num - n * 1461 ) / 365.25 );
		int year = n * 4 + m + 1900;
		int days_last = date_num - n * 1461 - m * 365;
		int month, day;
		if( m > 0 )
			days_last -= 1;
		if( days_last > ( m == 0 ? 335 : 334 ) )
		{
			month = 12;
			day = days_last - ( m == 0 ? 335 : 334 );
		}
		else if( days_last > ( m == 0 ? 305 : 304 ) && days_last <= ( m == 0 ? 335 : 334 ) )
		{
			month = 11;
			day = days_last - ( m == 0 ? 305 : 304 );
		}
		else if( days_last > ( m == 0 ? 274 : 273 ) && days_last <= ( m == 0 ? 305 : 304 ) )
		{
			month = 10;
			day = days_last - ( m == 0 ? 274 : 273 );
		}
		else if( days_last > ( m == 0 ? 244 : 243 ) && days_last <= ( m == 0 ? 274 : 273 ) )
		{
			month = 9;
			day = days_last - ( m == 0 ? 244 : 243 );
		}
		else if( days_last > ( m == 0 ? 213 : 212 ) && days_last <= ( m == 0 ?  244 : 243 ) )
		{
			month = 8;
			day = days_last - ( m == 0 ? 213 : 212 );
		}
		else if( days_last > ( m == 0 ? 182 : 181 ) && days_last <= ( m == 0 ?  213 : 212 ) )
		{
			month = 7;
			day = days_last - ( m == 0 ? 182 : 181 );
		}
		else if( days_last > ( m == 0 ? 152 : 151 ) && days_last <= ( m == 0 ?  182 : 181 ) )
		{
			month = 6;
			day = days_last - ( m == 0 ? 152 : 151 );
		}
		else if( days_last > ( m == 0 ? 121 : 120 ) && days_last <= ( m == 0 ?  152 : 151 ) )
		{
			month = 5;
			day = days_last - ( m == 0 ? 121 : 120 );
		}
		else if( days_last > ( m == 0 ? 91 : 90 ) && days_last <= ( m == 0 ?  121 : 120 ) )
		{
			month = 4;
			day = days_last - ( m == 0 ? 91 : 90 );
		}
		else if( days_last > ( m == 0 ? 60 : 59 ) && days_last <= ( m == 0 ?  91 : 90 ) )
		{
			month = 3;
			day = days_last - ( m == 0 ? 60 : 59 );
		}
		else if( days_last > 31 && days_last <= ( m == 0 ?  60 : 59 ) )
		{
			month = 2;
			day = days_last - 31;
		}
		else
		{
			month = 1;
			day = days_last;
		}
		cString datestr = ToDateString( year, month, day );
		return cDateTime( datestr.c_str(), timestr.c_str() );
	}
}

void cDateTime::VectorToDateTime( const cVector& tVec, cArray< cDateTime >& dtArray )
{
	int n = tVec.getSize();
	if( n < 1 )
		return;
	double tStart = tVec[0];
	int dateStart = (int)tStart;
	cDateTime dtStart = DoubleToDateTime( tStart );
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
			dtArray[i] = cDateTime( dtStartStr.c_str(), timestr.c_str() );

		}
		else
		{
			cDateTime dt = DoubleToDateTime( tVec[i] );
			dateStart = dateNum;
			dtStartStr = dt.DateString();
		}	
	}
}

void cDateTime::VectorToDateTimeString( const cVector& tVec, cArray< cString >& dtstrArray )
{
	int n = tVec.getSize();
	if( n < 1 )
		return;
	double tStart = tVec[0];
	int dateStart = (int)tStart;
	cDateTime dtStart = DoubleToDateTime( tStart );
	cString dtStartStr = dtStart.DateString();
	dtstrArray.resize( n );
	cString dtStartFullStr = dtStart.DateTimeFullString();
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
			cDateTime dt = DoubleToDateTime( tVec[i] );
			dateStart = dateNum;
			dtStartStr = dt.DateString();
		}	
	}
}


cString cDateTime::DateTimeFullString() const
{
	char datetime_buf[256];
	sprintf(datetime_buf, "%04d%02d%02d %02d:%02d:%02d", m_year,m_month, m_day, m_hour, m_minute, m_second );
	return cString(datetime_buf);
}

cString cDateTime::DateString() const
{
	char date_buf[256];
	sprintf(date_buf, "%04d%02d%02d", m_year,m_month, m_day );
	return cString( date_buf );
}

cString cDateTime::TimeString() const
{
	char time_buf[256];
	sprintf( time_buf, "%02d:%02d:%02d", m_hour, m_minute, m_second );
	return cString( time_buf );
}

cString cDateTime::ToDateString( int year, int month, int day )
{
	char date_buf[256];
	sprintf(date_buf, "%04d%02d%02d", year, month, day );
	return cString( date_buf );
}


cString cDateTime::ToTimeString( int hh, int mm, int ss )
{
	char time_buf[256];
	sprintf(time_buf, "%02d:%02d:%02d", hh, mm, ss );
	return cString( time_buf );
}
