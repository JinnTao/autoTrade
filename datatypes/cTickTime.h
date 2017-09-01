#ifndef __CTICKTIME_H__
#define __CTICKTIME_H__

#include <cDate.h>

typedef unsigned int TickTimeFormat;

#ifndef SECONDSPERDAY
#define SECONDSPERDAY 86400
#endif

#ifndef SECONDSPERHOUR
#define SECONDSPERHOUR 3600
#endif

#ifndef SECONDSPERMINUTE
#define SECONDSPERMINUTE 60
#endif

#ifndef MINUTESPERHOUR
#define MINUTESPERHOUR 60
#endif

#ifndef MINUTESPERDAY
#define MINUTESPERDAY 1440
#endif

#define ROUND_2_INT( f ) ( (int)( f >= 0.0 ? ( f + 0.5 ) : ( f - 0.5 ) ) )

template< class T > class cArray;
class cVector;
class cString;

class cTickTime
{
public:
	cTickTime();
	cTickTime( const char* date, const char* time, int millisec = 0 );
	cTickTime( const char* datetime, int millisec = 0 );
	~cTickTime();
	cTickTime( const cTickTime& );
	cTickTime& operator = ( const cTickTime& );

	bool operator == ( const cTickTime& ) const;
	bool operator != ( const cTickTime& ) const;
	bool operator < ( const cTickTime& ) const;
	bool operator > ( const cTickTime& ) const;
	bool operator >= ( const cTickTime& ) const;
	bool operator <= ( const cTickTime& ) const;

	cDate Date() const { return m_date; }
	TickTimeFormat Year() const { return m_date.Year(); }
	TickTimeFormat Month() const { return m_date.Month(); }
	TickTimeFormat Day() const { return m_date.Day(); }
	TickTimeFormat Hour() const { return m_hour; }
	TickTimeFormat Minute() const { return m_minute; }
	TickTimeFormat Second() const { return m_second; }
	TickTimeFormat MilliSecond() const { return m_millisec; }

	void SetMilliSecond( int millisec ) { m_millisec = millisec; }

	bool IsWeekend() const { return m_date.IsWeekend(); }
	bool IsHoliday() const { return m_date.IsHoliday(); }

	cString TickTimeFullString() const;
	cString DateString() const;
	cString TimeString() const;

	double TickTimeToDouble() const;
	static cTickTime DoubleToTickTime( double t );
	
	static void VectorToTickTime( const cVector&, cArray< cTickTime >& );
	static void VectorToTickTimeString( const cVector&, cArray< cString >& );

	static cString ToDateString( int, int, int );
	static cString ToTimeString( int, int, int );
	static TickTimeFormat JulianToGeorgian( TickTimeFormat );
	static cString JulianToGeorgianString( TickTimeFormat );
	static TickTimeFormat GeorgianToJulian( TickTimeFormat );
	
	static int TimeToSecInteger( double t );


protected:
	cDate m_date;
	TickTimeFormat m_hour;
	TickTimeFormat m_minute;
	TickTimeFormat m_second;
	TickTimeFormat m_millisec;
};

extern const cTickTime theTickTime;

int interval( const cArray< cTickTime >& tickTimeArray, const cTickTime& tickTime, int first = 0 );
int location( const cArray< cTickTime>& tickTimeArray, const cTickTime& tickTime, int first = 0 );

#endif
