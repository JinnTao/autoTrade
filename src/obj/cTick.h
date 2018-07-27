#ifndef __CTICK_H__
#define __CTICK_H__

#include <string>
#include <chrono>
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

#define ROUND_2_INT(f) ((int)(f >= 0.0 ? (f + 0.5) : (f - 0.5)))


class cTick {
public:
    cTick();
    explicit cTick(const std::string& instrumentID, const std::chrono::system_clock::time_point& tickTime);
    ~cTick();
    cTick(const cTick&);
    cTick& operator=(const cTick&);

    // set method
    void SetLast(double last) { m_last = last; }
    void SetBid(double bid) { m_bid = bid; }
    void SetBidVolume(double bidVol) { m_bidVol = bidVol; }
    void SetAsk(double ask) { m_ask = ask; }
    void SetAskVolume(double askVol) { m_askVol = askVol; }
    void SetVolume(double vol) { m_vol = vol; }
    void SetOpenInterest(double oi) { m_openInterest = oi; };

    // get method
    std::string   InstrumentID() const { return m_instrumentID; }
    std::chrono::system_clock::time_point TickTime() const { return m_tickTime; }
    double    Last() const { return m_last; }
    double    Ask() const { return m_ask; }
    double    AskVolume() const { return m_askVol; }
    double    Bid() const { return m_bid; }
    double    BidVolume() const { return m_bidVol; }
    double    Volume() const { return m_vol; }
    double    OpenInterest() const { return m_openInterest; }

private:
    std::string   m_instrumentID;
    std::chrono::system_clock::time_point m_tickTime;
    double    m_last;
    double    m_bid;
    double    m_bidVol;
    double    m_ask;
    double    m_askVol;
    double    m_vol;           // volume
    double    m_openInterest;  // open interest
};

#endif