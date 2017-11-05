#ifndef __CDATE_H__
#define __CDATE_H__

template< class T > class cArray;
class cVector;
class cString;

class cDate
{
public:
	cDate();
	cDate( const char* date );	// date is as format of yyyymmdd;
	cDate( const cString& );
	cDate( int, int, int );
	~cDate();
	cDate( const cDate& );
	cDate& operator = ( const cDate& );

	bool operator == ( const cDate& ) const;
	bool operator != ( const cDate& ) const;
	bool operator < ( const cDate& ) const;
	bool operator > ( const cDate& ) const;
	bool operator >= ( const cDate& ) const;
	bool operator <= ( const cDate& ) const;
	cDate operator - ( const cDate& ) const;
	cDate operator + ( const cDate& ) const;
		
	cDate NextBusinessDay() const;

	int Year() const { return m_year; }
	int Month() const { return m_month; }
	int Day() const { return m_day; }
	int JulianDate() const { return m_julianDate; }
	int GeorgianDate() const { return m_georgianDate; }


	bool IsWeekend() const;
	bool IsHoliday() const;

	cString DateToString() const;	//return in yyyymmdd

	// static methods
	static cString DateToString( int, int, int );
	static cString JulianDateToString( int );
	static cString GeorgianDateToString( int );
	
	static int JulianToGeorgian( int );
	static int GeorgianToJulian( int );

private:
	int m_year;
	int m_month;
	int m_day;
	int m_julianDate;
	int m_georgianDate;
	int CalcJulianDate() const;
	int CalcGeorgianDate() const;
};

extern const cDate theDate;



#endif
