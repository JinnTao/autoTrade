#include <cTick.h>

cTick::cTick() {
    m_instrumentID = "";
    m_tickTime     = std::chrono::system_clock::now();
}

cTick::cTick( const std::string& instrumentID, const std::chrono::system_clock::time_point& tickTime )
: m_instrumentID( instrumentID )
, m_tickTime( tickTime )
{
}

cTick::~cTick()
{
}

cTick::cTick( const cTick& in )
{

    m_instrumentID = in.m_instrumentID;
    m_tickTime = in.m_tickTime;
    m_last = in.m_last;
    m_bid = in.m_bid;
    m_bidVol = in.m_bidVol;
    m_ask = in.m_ask;
    m_askVol = in.m_askVol;
    m_vol = in.m_vol;
    m_openInterest = in.m_openInterest;
}
cTick& cTick::operator = ( const cTick& in )
{
    if( &in != this )
    {
        m_instrumentID = in.m_instrumentID;
        m_tickTime = in.m_tickTime;
        m_last = in.m_last;
        m_bid = in.m_bid;
        m_bidVol = in.m_bidVol;
        m_ask = in.m_ask;
        m_askVol = in.m_askVol;
        m_vol = in.m_vol;
        m_openInterest = in.m_openInterest; 
    }
    return *this;
}