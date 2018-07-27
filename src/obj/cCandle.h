//#ifndef __CCANDLE_H__
//#define __CCANDLE_H__
//
//#include <cTick.h>
//#include <vector>
//
//// base class
//class cCandle {
//    using cTickTime = std::chrono::system_clock::time_point;
//
//public:
//    cCandle();
//    explicit cCandle(int julianDateStart, int julianDateEnd, int secStart, int secEnd);
//    cCandle(const cTickTime& startDT, const cTickTime& endDT);
//    cCandle(const cCandle&);
//    cCandle& operator=(const cCandle&);
//    virtual ~cCandle();
//
//    void   Clear();
//    bool   IsSet() const;
//    double Open() const;
//    double High() const;
//    double Low() const;
//    double Close() const;
//    int    TickCount() const;
//
//    double RealTimeOpen() const;
//    double RealTimeHigh() const;
//    double RealTimeLow() const;
//    double RealTimeClose() const;
//
//    int  DateStart(std::string& dateStr) const;
//    int  DateEnd(std::string& dateStr) const;
//    void TimeStart(std::string& timeStr) const;
//    void TimeEnd(std::string& timeStr) const;
//    int  TimeStart() const;
//    int  TimeEnd() const;
//
//    void Print(std::string& candleInfo) const;
//
//    void        Set();
//    void        UpdateWithTick(const cTick& tick);
//    std::string BarSize() const;
//
//private:
//    std::vector<cTickTime> m_timeVec;
//    std::vector<double>         m_pxVec;
//    bool                   m_set;
//    double                 m_open;
//    double                 m_high;
//    double                 m_low;
//    double                 m_close;
//    int                    m_tickCount;
//    int                    m_datenumStart;  // julian date start of the candle
//    int                    m_datenumEnd;    // julian date end of the candle
//    int       m_secStart;  // intraday candle starts on seconds of the day, e.g. 86400 seconds for an entire day
//    int       m_secEnd;    // intraday candle ends on seconds of the day, e.g. 86400 seconds for an entire day
//    cTickTime m_start;
//    cTickTime m_end;
//
//private:
//    // make the base class non-copyable
//};
//#endif