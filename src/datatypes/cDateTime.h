#ifndef __CDATETIME_H__
#define __CDATETIME_H__

typedef unsigned int DateTimeFormat;

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

class cDateTime
{
public:
    cDateTime();
    cDateTime( const char* date, const char* time );
    cDateTime( const char* datetime );
    ~cDateTime();
    cDateTime( const cDateTime& );
    cDateTime& operator = ( const cDateTime& );

    bool operator == ( const cDateTime& ) const;
    bool operator != ( const cDateTime& ) const;

    DateTimeFormat Date() const { return m_date; }
    DateTimeFormat Year() const { return m_year; }
    DateTimeFormat Month() const { return m_month; }
    DateTimeFormat Day() const { return m_day; }
    DateTimeFormat Hour() const { return m_hour; }
    DateTimeFormat Minute() const { return m_minute; }
    DateTimeFormat Second() const { return m_second; }

    bool IsWeekend() const;
    bool IsHoliday() const;

    double DateTimeToDouble() const;
    double DateTimeToDouble( const cDateTime& ) const;
    static cDateTime DoubleToDateTime( double t );
    static void VectorToDateTime( const cVector&, cArray< cDateTime >& );
    static void VectorToDateTimeString( const cVector&, cArray< cString >& );

    cString DateTimeFullString() const;
    cString DateString() const;
    cString TimeString() const;
    static cString ToDateString( int, int, int );
    static cString ToTimeString( int, int, int );


protected:
    DateTimeFormat m_date;
    DateTimeFormat m_year;
    DateTimeFormat m_month;
    DateTimeFormat m_day;
    DateTimeFormat m_hour;
    DateTimeFormat m_minute;
    DateTimeFormat m_second;
};



#endif
