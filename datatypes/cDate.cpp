#include <cDate.h>
#include <cString.h>
#include <cStringUtils.h>
#include <cVector.h>
#include <time.h>

cDate::cDate()
{
	time_t timer;
	time( &timer);
	struct tm* timeinfo;
	timeinfo = localtime( &timer );
	m_year = timeinfo->tm_year + 1900;
	m_month = timeinfo->tm_mon + 1;
	m_day = timeinfo->tm_mday;
	m_julianDate = CalcJulianDate();
	m_georgianDate = CalcGeorgianDate();
}

cDate::cDate( const cString& datestr )
{
	/*
	date is as format of yyyymmdd;
	*/
	m_year = AsInt( cString::Left( datestr, 4 ) );
	m_month = AsInt( cString::Mid( datestr, 4,2) );
	m_day = AsInt( cString::Right( datestr,2) );
	yr_assert( m_month >= 1 && m_month <= 12 );
	yr_assert( m_day >= 1 && m_day <= 31 );
	m_julianDate = CalcJulianDate();
	m_georgianDate = CalcGeorgianDate();
}

cDate::cDate( const char* date )
{
	/*
	date is as format of yyyymmdd;
	*/
	cString datestr( date );
	m_year = AsInt( cString::Left( datestr, 4 ) );
	m_month = AsInt( cString::Mid( datestr, 4,2) );
	m_day = AsInt( cString::Right( datestr,2) );
	yr_assert( m_month >= 1 && m_month <= 12 );
	yr_assert( m_day >= 1 && m_day <= 31 );
	m_julianDate = CalcJulianDate();
	m_georgianDate = CalcGeorgianDate();
}

cDate::cDate( int year, int month, int day )
:m_year(year)
,m_month(month)
,m_day(day)
{
	yr_assert( m_month >= 1 && m_month <= 12 );
	yr_assert( m_day >= 1 && m_day <= 31 );
	m_julianDate = CalcJulianDate();
	m_georgianDate = CalcGeorgianDate();
}


cDate::~cDate()
{
}

cDate::cDate( const cDate& in )
{
	yr_assert( this != & in );
	m_year = in.m_year;
	m_month = in.m_month;
	m_day = in.m_day;
	m_julianDate = in.m_julianDate;
	m_georgianDate = in.m_georgianDate;
}

cDate& cDate::operator = ( const cDate& in )
{
	if( &in != this )
	{
		m_year = in.m_year;
		m_month = in.m_month;
		m_day = in.m_day;
		m_julianDate = in.m_julianDate;
		m_georgianDate = in.m_georgianDate;
	}
	return *this;
}

bool cDate::operator == ( const cDate& a ) const
{
	if( this == &a )
		return true;
	if( m_julianDate != a.m_julianDate )
		return false;
	return true;
}

bool cDate::operator != ( const cDate& a ) const
{
	return !( *this == a );
}

bool cDate::operator < ( const cDate& a ) const
{
	if( this == &a )
		return false;
	else
	{
		if( m_julianDate < a.m_julianDate )
			return true;
		else
			return false;
	}
}

bool cDate::operator <= ( const cDate& a ) const
{
	if( *this == a || *this < a )
		return true;
	else
		return false;
		
}

bool cDate::operator > ( const cDate& a ) const
{
	return !( *this <= a );
}

bool cDate::operator >= ( const cDate& a ) const
{
	return !(*this < a );
}

cDate cDate::NextBusinessDay() const
{
	int julianDate = m_julianDate + 1;
	cDate nextBD(JulianDateToString(julianDate));
	bool flag = nextBD.IsHoliday();
	if( flag )
	{
		cDate bd;
		while(flag)
		{
			julianDate += 1;
			bd = cDate(JulianDateToString(julianDate));
			flag = bd.IsHoliday();
		}
		return bd;
	}
	else
	{
		return nextBD;
	}

}

bool cDate::IsWeekend() const
{
	struct tm* timeinfo;
	time_t rawtime;
	time( &rawtime );
	timeinfo = localtime( &rawtime );
	timeinfo->tm_year = m_year - 1900;
	timeinfo->tm_mon = m_month - 1;
	timeinfo->tm_mday = m_day;
	mktime( timeinfo );
	int wkday = timeinfo->tm_wday;
	if( wkday == 0 || wkday == 6 )
		return true;
	else
		return false;
}


bool cDate::IsHoliday() const
{
	if( IsWeekend() )
	{
		return true;
	}
	else
	{
		//this is implemented for Shanghai exchange holidays until end of 2017
		//the table will be updated on a regular basis
		cVector buildInHolidays;
		buildInHolidays.push_back(42370);//2016-01-01
		buildInHolidays.push_back(42408);//2016-02-08
		buildInHolidays.push_back(42409);//2016-02-09
		buildInHolidays.push_back(42410);//2016-02-10
		buildInHolidays.push_back(42411);//2016-02-11
		buildInHolidays.push_back(42412);//2016-02-12
		buildInHolidays.push_back(42464);//2016-04-04
		buildInHolidays.push_back(42492);//2016-05-02
		buildInHolidays.push_back(42530);//2016-06-09
		buildInHolidays.push_back(42531);//2016-06-10
		buildInHolidays.push_back(42628);//2016-09-15
		buildInHolidays.push_back(42629);//2016-09-16
		buildInHolidays.push_back(42646);//2016-10-03
		buildInHolidays.push_back(42647);//2016-10-04
		buildInHolidays.push_back(42648);//2016-10-05
		buildInHolidays.push_back(42649);//2016-10-06
		buildInHolidays.push_back(42650);//2016-10-07
		buildInHolidays.push_back(42737);//2017-01-02
		buildInHolidays.push_back(42762);//2017-01-27
		buildInHolidays.push_back(42765);//2017-01-30
		buildInHolidays.push_back(42766);//2017-01-31
		buildInHolidays.push_back(42767);//2017-02-01
		buildInHolidays.push_back(42768);//2017-02-02
		buildInHolidays.push_back(42829);//2017-04-04
		buildInHolidays.push_back(42856);//2017-05-01
		buildInHolidays.push_back(42885);//2017-05-30
		buildInHolidays.push_back(43010);//2017-10-02
		buildInHolidays.push_back(43011);//2017-10-03
		buildInHolidays.push_back(43012);//2017-10-04
		buildInHolidays.push_back(43013);//2017-10-05
		buildInHolidays.push_back(43014);//2017-10-06
		
		int idx = buildInHolidays.find(m_julianDate);
		if( idx < 0 )
			return false;
		else
			return true;
	}
}

int cDate::CalcJulianDate() const
{
	int julian = 0;
	int year_left = m_year - 1900;
	int m = year_left % 4;
	int n = ( year_left - m )/4;

	int yday;
	if( m_month == 1 )
		yday = m_day;
	else if( m_month == 2 )
		yday = 31 + m_day;
	else if( m_month == 3 )
		yday = m == 0 ? 60 + m_day : 59 + m_day;
	else if( m_month == 4 )
		yday = m == 0 ? 91 + m_day : 90 + m_day;
	else if( m_month == 5 )
		yday = m == 0 ? 121 + m_day : 120 + m_day;
	else if( m_month == 6 )
		yday = m == 0 ? 152 + m_day : 151 + m_day;
	else if( m_month == 7 )
		yday = m == 0 ? 182 + m_day : 181 + m_day;
	else if( m_month == 8 )
		yday = m == 0 ? 213 + m_day : 212 + m_day;
	else if( m_month == 9 )
		yday = m == 0 ? 244 + m_day : 243 + m_day;
	else if( m_month == 10 )
		yday = m == 0 ? 274 + m_day : 273 + m_day;
	else if( m_month == 11 )
		yday = m == 0 ? 305 + m_day : 304 + m_day;
	else
		yday = m == 0 ? 335 + m_day : 334 + m_day;

	if( n == 0 )
	{
		// 1900 is a leap year
		if( m > 0 )
			julian = m * 365 + 1;
		julian += yday;
	}
	else
	{
		julian = n * 1461;
		if( m > 0 )
			julian += m * 365 + 1;	// the first year of every loop is a leap year
		julian += yday;
	}
	return julian;
}

int cDate::CalcGeorgianDate() const
{
	return( m_year * 10000 + m_month * 100 + m_day );
}

int cDate::JulianToGeorgian( int julian )
{
	int n = (int)( julian / 1461 );
	int year, month, day;
	if( julian % 1461 == 0 )
	{
		year = n * 4 + 1900 - 1;
		month = 12;
		day = 31;
	}
	else
	{
		int m = int( ( julian - n * 1461 ) / 365.25 );
		year = n * 4 + m + 1900;
		int days_last = julian - n * 1461 - m * 365;
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
		
	}
	return( year * 10000 + month * 100 + day );
}

int cDate::GeorgianToJulian( int georgian )
{
	int year = (int)(georgian/10000);
	int month = (int)((georgian - year*10000)/100);
	int day = georgian - year*10000 - month * 100;
	yr_assert( month >= 1 && month <= 12 );
	yr_assert( day >= 1 && day <= 31 );

	int julian = 0;
	int year_left = year - 1900;
	int m = year_left % 4;
	int n = ( year_left - m )/4;

	int yday;
	if( month == 1 )
		yday = day;
	else if( month == 2 )
		yday = 31 + day;
	else if( month == 3 )
		yday = m == 0 ? 60 + day : 59 + day;
	else if( month == 4 )
		yday = m == 0 ? 91 + day : 90 + day;
	else if( month == 5 )
		yday = m == 0 ? 121 + day : 120 + day;
	else if( month == 6 )
		yday = m == 0 ? 152 + day : 151 + day;
	else if( month == 7 )
		yday = m == 0 ? 182 + day : 181 + day;
	else if( month == 8 )
		yday = m == 0 ? 213 + day : 212 + day;
	else if( month == 9 )
		yday = m == 0 ? 244 + day : 243 + day;
	else if( month == 10 )
		yday = m == 0 ? 274 + day : 273 + day;
	else if( month == 11 )
		yday = m == 0 ? 305 + day : 304 + day;
	else
		yday = m == 0 ? 335 + day : 334 + day;

	if( n == 0 )
	{
		// 1900 is a leap year
		if( m > 0 )
			julian = m * 365 + 1;
		julian += yday;
	}
	else
	{
		julian = n * 1461;
		if( m > 0 )
			julian += m * 365 + 1;	// the first year of every loop is a leap year
		julian += yday;
	}
	return julian;
}


cString cDate::DateToString() const
{
	char date_buf[256];
	sprintf(date_buf, "%04d%02d%02d", m_year,m_month, m_day );
	return cString( date_buf );
}

cString cDate::DateToString( int year, int month, int day )
{
	char date_buf[256];
	sprintf(date_buf, "%04d%02d%02d", year, month, day );
	return cString( date_buf );
}

cString cDate::JulianDateToString( int julian )
{
	int georgian = JulianToGeorgian( julian );
	return( GeorgianDateToString( georgian ) );
}

cString cDate::GeorgianDateToString( int georgian )
{
	char buffer[10];
	return cString( itoa( georgian, buffer, 10 ) );
}


const cDate theDate = cDate();