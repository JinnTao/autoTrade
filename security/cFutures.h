#ifndef __CFUTURES_H__
#define __CFUTURES_H__

#include <string.h>
#include <cString.h>
#include <cTickTime.h>

class cFutures
{
public:
	cFutures( const char* );
	cFutures( const string& );
	cFutures( const cString& );
	cFutures( const cFutures& );

	~cFutures();
	cFutures& operator = ( const cFutures& );

	cString Underlying() const;
	cString Exchange() const;
	cString TradingBreak() const;
	cArray< cString > TradingHours() const;
	double ContractSize() const;
	double TickSize() const;
	double TickValue() const;
	cTickTime Expiry() const;


protected:
	cString _id;




};

#endif