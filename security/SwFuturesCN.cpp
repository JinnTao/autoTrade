#include <SwFuturesCN.h>
#include <cString.h>
#include <cArray.h>

const int NUM_UNDERLYING = 36;

const cString underlying_code[NUM_UNDERLYING] = 
{
	"IF",
	"IH",
	"IC",
	"T",
	"TF",
	"AU",
	"AG",
	"CU",
	"AL",
	"ZN",
	"PB",
	"NI",
	"TA",
	"L",
	"PP",
	"J",
	"JM",
	"MA",
	"ZC",
	"SR",
	"CF",
	"C",
	"JD",
	"A",
	"M",
	"Y",
	"P",
	"OI",
	"RM",
	"RU",
	"RB",
	"I",
	"FG",
	"HC",
	"BU",
	"SN"
};

int HasTradingBreak( int testFuturesUnderlying )
{
	if( IsValidUnderlying( testFuturesUnderlying ) )
	{
		return !( testFuturesUnderlying >= SwUNDERLYING_IF && testFuturesUnderlying <= SwUNDERLYING_TF );
	}
	else
		yr_error( "Invalid Underlying" );
}

cString UnderlyingToString( const SwUnderlying& underlying )
{
	if( !IsValidUnderlying( underlying ) )
		return "";

	return underlying_code[underlying];
}

SwUnderlying StringToUnderlying( const cString& underlyingstr )
{
	int i;
	for( i = 0; i < NUM_UNDERLYING; ++i )
	{
		if( Compare( underlyingstr, underlying_code[i] ) )
			break;
	}
	if( !IsValidUnderlying( i ) )
	{
		yr_error( "Invalid Underlying" );
	}

	return (SwUnderlying)i;
}

void SwUnderlyingGetUnderlyings( cArray< cString >& output )
{
	output.resize( NUM_UNDERLYING );
	for( int i = 0; i < NUM_UNDERLYING; ++i )
		output[i] = underlying_code[i];
}

cString GetExchange( const SwUnderlying& underlying )
{
	switch( underlying )
	{
	case SwUNDERLYING_IF:
	case SwUNDERLYING_IH:
	case SwUNDERLYING_IC:
	case SwUNDERLYING_T:
	case SwUNDERLYING_TF:
		return cString( ".CFE" );
	//
	case SwUNDERLYING_AU:
	case SwUNDERLYING_AG:
	case SwUNDERLYING_CU:
	case SwUNDERLYING_AL:
	case SwUNDERLYING_ZN:
	case SwUNDERLYING_PB:
	case SwUNDERLYING_NI:
	case SwUNDERLYING_RU:
	case SwUNDERLYING_RB:
	case SwUNDERLYING_HC:
	case SwUNDERLYING_BU:
	case SwUNDERLYING_SN:
		return cString( ".SHF" );
	//
	case SwUNDERLYING_TA:
	case SwUNDERLYING_MA:
	case SwUNDERLYING_ZC:
	case SwUNDERLYING_SR:
	case SwUNDERLYING_CF:
	case SwUNDERLYING_OI:
	case SwUNDERLYING_RM:
	case SwUNDERLYING_FG:
		return cString( ".CZC" );
	//
	case SwUNDERLYING_L:
	case SwUNDERLYING_PP:
	case SwUNDERLYING_J:
	case SwUNDERLYING_JM:
	case SwUNDERLYING_C:
	case SwUNDERLYING_JD:
	case SwUNDERLYING_A:
	case SwUNDERLYING_M:
	case SwUNDERLYING_Y:
	case SwUNDERLYING_P:
	case SwUNDERLYING_I:
		return cString( ".DCE" );
	default:
		yr_error( "Unknown Underlying!" );		
	}
}

cString GetExchange( const cString& underlyingstr )
{
	SwUnderlying underlying = StringToUnderlying( underlyingstr );
	return GetExchange( underlying );
}

cArray< cString > GetTradingHours( const SwUnderlying& underlying )
{
	cArray< cString > tradingHours;
	tradingHours.resize(3);
	tradingHours[0] = "09:00-11:30";
	tradingHours[1] = "13:30-15:00";
	switch( underlying )
	{
	case SwUNDERLYING_IF:
	case SwUNDERLYING_IH:
	case SwUNDERLYING_IC:
		tradingHours[0] = "09:30-11:30";
		tradingHours[1] = "13:00-15:00";
		tradingHours[2] = "n/a";
		return tradingHours;
	case SwUNDERLYING_T:
	case SwUNDERLYING_TF:
		tradingHours[0] = "09:15-11:30";
		tradingHours[1] = "13:00-15:15";
		tradingHours[2] = "n/a";
		return tradingHours;

	case SwUNDERLYING_AU:
	case SwUNDERLYING_AG:
		tradingHours[2] = "21:00-02:30";
		return tradingHours;

	case SwUNDERLYING_CU:
	case SwUNDERLYING_AL:
	case SwUNDERLYING_ZN:
	case SwUNDERLYING_PB:
	case SwUNDERLYING_NI:
	case SwUNDERLYING_SN:
		tradingHours[2] = "21:00-01:00";
		return tradingHours;

	case SwUNDERLYING_C:
	case SwUNDERLYING_JD:
	case SwUNDERLYING_L:
	case SwUNDERLYING_PP:
		tradingHours[2] = "n/a";
		return tradingHours;

	case SwUNDERLYING_TA:
	case SwUNDERLYING_J:
	case SwUNDERLYING_JM:
	case SwUNDERLYING_MA:
	case SwUNDERLYING_ZC:
	case SwUNDERLYING_SR:
	case SwUNDERLYING_CF:
	case SwUNDERLYING_A:
	case SwUNDERLYING_M:
	case SwUNDERLYING_Y:
	case SwUNDERLYING_P:
	case SwUNDERLYING_OI:
	case SwUNDERLYING_RM:
	case SwUNDERLYING_FG:
	case SwUNDERLYING_I:
		tradingHours[2] = "21:00-23:30";
		return tradingHours;
	
	case SwUNDERLYING_RU:
		tradingHours[2] = "21:00-23:00";
		return tradingHours;
	
	case SwUNDERLYING_RB:
	case SwUNDERLYING_HC:
	case SwUNDERLYING_BU:
		tradingHours[2] = "21:00-01:00";
		return tradingHours;

	default:
		yr_error( "Unknown Underlying!" );
		
	}
}

cArray< cString > GetTradingHours( const cString& underlyingstr )
{
	SwUnderlying underlying = StringToUnderlying( underlyingstr );
	return GetTradingHours( underlying );
}

cString GetTradingBreak( const SwUnderlying& underlying )
{
	if( HasTradingBreak( underlying ) )
		return "10:15-10:30";
	else
		return "none";
}

cString GetTradingBreak( const cString& underlyingstr )
{
	SwUnderlying underlying = StringToUnderlying( underlyingstr );
	return GetTradingBreak( underlying );
}

double GetContractSize( const SwUnderlying& underlying )
{
	switch( underlying )
	{
	case SwUNDERLYING_IF:
	case SwUNDERLYING_IH:
		return 300.0;
	case SwUNDERLYING_IC:
		return 200.0;
	case SwUNDERLYING_T:
	case SwUNDERLYING_TF:
		return 10000.0;
	case SwUNDERLYING_AU:
		return 1000.0;
	case SwUNDERLYING_AG:
		return 15.0;
	case SwUNDERLYING_CU:
	case SwUNDERLYING_AL:
	case SwUNDERLYING_ZN:
	case SwUNDERLYING_PB:
		return 5.0;
	case SwUNDERLYING_NI:
	case SwUNDERLYING_SN:
		return 10.0;
	case SwUNDERLYING_TA:
	case SwUNDERLYING_L:
	case SwUNDERLYING_PP:
		return 5.0;
	case SwUNDERLYING_J:
		return 100.0;
	case SwUNDERLYING_JM:
		return 60.0;
	case SwUNDERLYING_MA:
		return 10.0;
	case SwUNDERLYING_ZC:
		return 200.0;
	case SwUNDERLYING_SR:
		return 10.0;
	case SwUNDERLYING_CF:
		return 5.0;
	case SwUNDERLYING_C:
	case SwUNDERLYING_JD:
	case SwUNDERLYING_A:
	case SwUNDERLYING_M:
	case SwUNDERLYING_Y:
	case SwUNDERLYING_P:
	case SwUNDERLYING_OI:
	case SwUNDERLYING_RM:
	case SwUNDERLYING_RU:
	case SwUNDERLYING_RB:
	case SwUNDERLYING_HC:
	case SwUNDERLYING_BU:
		return 10.0;
	case SwUNDERLYING_I:
		return 100.0;
	case SwUNDERLYING_FG:
		return 20.0;
	default:
		yr_error( "Unknown Underlying!" );
	}
}

double GetContractSize( const cString& underlyingstr )
{
	SwUnderlying underlying = StringToUnderlying( underlyingstr );
	return GetContractSize( underlying );
}

double GetTickSize( const SwUnderlying& underlying )
{
	switch( underlying )
	{
	case SwUNDERLYING_IF:
	case SwUNDERLYING_IH:
	case SwUNDERLYING_IC:
		return 0.2;
	case SwUNDERLYING_T:
	case SwUNDERLYING_TF:
		return 0.005;
	case SwUNDERLYING_AU:
		return 0.5;
	case SwUNDERLYING_AG:
		return 1.0;
	case SwUNDERLYING_CU:
		return 10.0;
	case SwUNDERLYING_AL:
	case SwUNDERLYING_ZN:
	case SwUNDERLYING_PB:
		return 5.0;
	case SwUNDERLYING_NI:
	case SwUNDERLYING_SN:
		return 1.0;
	case SwUNDERLYING_TA:
		return 2.0;
	case SwUNDERLYING_L:
		return 5.0;
	case SwUNDERLYING_PP:
		return 1.0;
	case SwUNDERLYING_J:
	case SwUNDERLYING_JM:
		return 0.5;
	case SwUNDERLYING_MA:
		return 1.0;
	case SwUNDERLYING_ZC:
		return 0.2;
	case SwUNDERLYING_SR:
		return 1.0;
	case SwUNDERLYING_CF:
		return 5.0;
	case SwUNDERLYING_C:
	case SwUNDERLYING_JD:
	case SwUNDERLYING_A:
	case SwUNDERLYING_M:
		return 1.0;
	case SwUNDERLYING_Y:
	case SwUNDERLYING_P:
	case SwUNDERLYING_OI:
		return 2.0;
	case SwUNDERLYING_RM:
		return 1.0;
	case SwUNDERLYING_RU:
		return 5.0;
	case SwUNDERLYING_RB:
		return 1.0;
	case SwUNDERLYING_I:
		return 0.5;
	case SwUNDERLYING_FG:
		return 1.0;
	case SwUNDERLYING_HC:
		return 1.0;
	case SwUNDERLYING_BU:
		return 2.0;
	default:
		yr_error( "Unknown Underlying!" );
	}
}

double GetTickSize( const cString& underlyingstr )
{
	SwUnderlying underlying = StringToUnderlying( underlyingstr );
	return GetTickSize( underlying );
}
