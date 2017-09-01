#ifndef __SWFUTURESCN_H__
#define __SWFUTURESCN_H__

template< class T > class cArray;
class cString;


typedef enum SwUnderlying
{
	SwUNDERLYING_INVALID = -1,
	SwUNDERLYING_IF,
	SwUNDERLYING_IH,
	SwUNDERLYING_IC,
	SwUNDERLYING_T,
	SwUNDERLYING_TF,
	SwUNDERLYING_AU,
	SwUNDERLYING_AG,
	SwUNDERLYING_CU,
	SwUNDERLYING_AL,
	SwUNDERLYING_ZN,
	SwUNDERLYING_PB,
	SwUNDERLYING_NI,
	SwUNDERLYING_TA,
	SwUNDERLYING_L,
	SwUNDERLYING_PP,
	SwUNDERLYING_J,
	SwUNDERLYING_JM,
	SwUNDERLYING_MA,
	SwUNDERLYING_ZC,
	SwUNDERLYING_SR,
	SwUNDERLYING_CF,
	SwUNDERLYING_C,
	SwUNDERLYING_JD,
	SwUNDERLYING_A,
	SwUNDERLYING_M,
	SwUNDERLYING_Y,
	SwUNDERLYING_P,
	SwUNDERLYING_OI,
	SwUNDERLYING_RM,
	SwUNDERLYING_RU,
	SwUNDERLYING_RB,
	SwUNDERLYING_I,
	SwUNDERLYING_FG,
	SwUNDERLYING_HC,
	SwUNDERLYING_BU,
	SwUNDERLYING_SN
} _SwUnderlying;

inline int IsValidUnderlying( int testFuturesUnderlying )
{
	return( testFuturesUnderlying >= SwUNDERLYING_IF && testFuturesUnderlying <= SwUNDERLYING_SN );
}

int HasTradingBreak( int testFuturesUnderlying );

cString UnderlyingToString( const SwUnderlying& underlying );
SwUnderlying StringToUnderlying( const cString& underlyingstr );

void SwUnderlyingGetUnderlyings( cArray< cString >& output );

cString GetExchange( const SwUnderlying& underlying );
cString GetExchange( const cString& underlyingstr );

cArray< cString > GetTradingHours( const SwUnderlying& underlying );
cArray< cString > GetTradingHours( const cString& underlyingstr );

cString GetTradingBreak( const SwUnderlying& underlying );
cString GetTradingBreak( const cString& underlyingstr );

double GetContractSize( const SwUnderlying& underlying );
double GetContractSize( const cString& underlyingstr );

double GetTickSize( const SwUnderlying& underlying );
double GetTickSize( const cString& underlyingstr );




#endif

