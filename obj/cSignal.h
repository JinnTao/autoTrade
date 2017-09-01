#ifndef __CSIGNAL_H__
#define __CSIGNAL_H__

#include <cString.h>
#include <cTickTime.h>
#include <stl.h>

typedef enum SwSignalDirection
{
	SWSIGNALDIRECTION_INVALID = -1,
	SWSIGNALDIRECTION_SELL,
	SWSIGNALDIRECTION_BUY
} SwSignalDirection;

inline int IsValidOrderDirection( int direction )
{
	return( direction >= SWSIGNALDIRECTION_SELL && direction <= SWSIGNALDIRECTION_BUY );
}

cString SignalDirectionToString( const SwSignalDirection& direction );
SwSignalDirection StringToOrderDirection( const cString& directionstr );

//
//typedef enum SwOrderOffsetFlag
//{
//	SWORDEROFFSETFLAG_INVALID = -1,
//	SWORDEROFFSETFLAG_OPEN,
//	SWORDEROFFSETFLAG_CLOSE,
//	SWORDEROFFSETFLAG_CLOSETODAY,
//	SWORDEROFFSETFLAG_CLOSEYESTERDAY,
//} _SwOrderOffsetFlag;
//
//inline int IsValidOrderOffsetFlag( int offsetFlag )
//{
//	return( offsetFlag >= SWORDEROFFSETFLAG_OPEN && offsetFlag <= SWORDEROFFSETFLAG_CLOSEYESTERDAY );
//}
//
//cString OrderOffsetFlagToString( const SwOrderOffsetFlag& flag );
//SwOrderOffsetFlag StringToOrderOffsetFlag( const cString& flagstr );


class cSignal
{
public:
	cSignal();
	~cSignal();

	cSignal( const cSignal& );
	cSignal& operator = ( const cSignal& );

	bool operator == ( const cSignal& ) const;
	bool operator != ( const cSignal& ) const;

	// Get methods
	void SetInstrumentID( const cString& instrumentID ) { m_instrumentID = instrumentID; }
	
	void SetDirection( int direction );
	void SetDirection( const cString& directionstr );
	
	void SetPrice( double px ) { m_price = px; };
	void SetVolume( unsigned int volume ) { m_volume = volume; }
	void SetTime( const cTickTime& dt ) { m_time = dt; }

	// Get Methods
	cString GetInstrumentID() const;
	char GetDirection() const;
	double GetPrice() const;
	unsigned int GetVolume() const;
	cTickTime GetTime() const;
	cString GetSignalID() const;

private:
	cString m_instrumentID;
	double m_price;
	unsigned int m_volume;
	cTickTime m_time;

	SwSignalDirection m_direction;
};

typedef shared_ptr< cSignal > cSignalPtr;

#endif