#ifndef __CCANDLE_H__
#define __CCANDLE_H__

#include <cVector.h>
//#include <cTickTime.h>
//#include <cString.h>
#include <cTick.h>

template< class T > class cArray;

// base class
class cCandle
{
public:
    cCandle();
    explicit cCandle( int julianDateStart, int julianDateEnd, int secStart, int secEnd );
    cCandle( const cTickTime& startDT, const cTickTime& endDT );
    cCandle( const cCandle& );
    cCandle& operator = ( const cCandle& );
    virtual ~cCandle();


    void Clear();
    bool IsSet() const;
    double Open() const;
    double High() const;
    double Low() const;
    double Close() const;
    int TickCount() const;

    double RealTimeOpen() const;
    double RealTimeHigh() const;
    double RealTimeLow() const;
    double RealTimeClose() const;

    int DateStart( cString& dateStr ) const;
    int DateEnd( cString& dateStr ) const;
    void TimeStart( cString& timeStr ) const;
    void TimeEnd( cString& timeStr ) const;
    int TimeStart() const;
    int TimeEnd() const;

    void Print( cString& candleInfo ) const;

    void Set();
    void UpdateWithTick( const cTick& tick );
    cString BarSize() const;

private:
    cArray< cTickTime > m_timeVec;
    cVector m_pxVec;
    bool m_set;
    double m_open;
    double m_high;
    double m_low;
    double m_close;
    int m_tickCount;
    int m_datenumStart;    // julian date start of the candle
    int m_datenumEnd;    // julian date end of the candle
    int m_secStart;        // intraday candle starts on seconds of the day, e.g. 86400 seconds for an entire day
    int m_secEnd;        // intraday candle ends on seconds of the day, e.g. 86400 seconds for an entire day
    cTickTime m_start;
    cTickTime m_end;


private:
    // make the base class non-copyable
    
};


//class cCandleIntraday : public cCandle
//{
//public:
//    cCandleIntraday();
//    explicit cCandleIntraday( int julianDate, int secStart, int secEnd );
//    ~cCandleIntraday();
//
//    cCandleIntraday( const cCandleIntraday& );
//    cCandleIntraday& operator = ( const cCandleIntraday& );
//
//    int Date( cString& dateStr ) const;
//    int Start( cString& timeStr ) const;
//    int End( cString& timeStr ) const;
//
//    void PrintTick() const;
//    
//    virtual void Set();
//    virtual void UpdateWithTick( const cTick& tick );
//    virtual cString BarSize() const;
//
//
//private:
//    int m_datenum;    // julian date of the candle
//    int m_secStart;    // intraday candle starts on seconds of the day, e.g. 86400 seconds for an entire day
//    int m_secEnd;   // intraday candle starts on seconds of the day, e.g. 86400 seconds for an entire day
//    cIvector m_secVec;
//
//};


//class cCandleGeneral : public cCandle
//{
//public:
//    explicit cCandleGeneral( const cTickTime& startDT, const cTickTime& endDT );
//    
//    virtual cString BarSize() const;
//
//    virtual void UpdateWithTick( const cTick& tick );
//
//private:
//    cTickTime m_startDT;
//    cTickTime m_endDT;
//};






#endif


