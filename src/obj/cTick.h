#ifndef __CTICK_H__
#define __CTICK_H__

#include <cString.h>
#include <cTickTime.h>

class cTick
{
public:
	cTick();
	explicit cTick( const cString& instrumentID, const cTickTime& tickTime );
	~cTick();
	cTick( const cTick& );
	cTick& operator = ( const cTick& );

	//set method
	void SetLast( double last ) { m_last = last; }
	void SetBid( double bid ) { m_bid = bid; }
	void SetBidVolume( double bidVol ) { m_bidVol = bidVol; }
	void SetAsk( double ask ) { m_ask = ask; }
	void SetAskVolume( double askVol ) { m_askVol = askVol; }
	void SetVolume( double vol ) { m_vol = vol; }
	void SetOpenInterest( double oi ) { m_openInterest = oi; };

	//get method
	cString InstrumentID() const { return m_instrumentID; }
	cTickTime TickTime() const { return m_tickTime; }
	double Last() const { return m_last; }
	double Ask() const { return m_ask; }
	double AskVolume() const { return m_askVol; }
	double Bid() const { return m_bid; }
	double BidVolume() const { return m_bidVol; }
	double Volume() const { return m_vol; }
	double OpenInterest() const { return m_openInterest; }

private:
	cString m_instrumentID;
	cTickTime m_tickTime;
	double m_last;
	double m_bid;
	double m_bidVol;
	double m_ask;
	double m_askVol;
	double m_vol;				// volume
	double m_openInterest;		// open interest

};


#endif