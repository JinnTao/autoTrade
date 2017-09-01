#include <SwOrder.h>
#include <cString.h>

const int NUM_ORDER = 4;

const cString order_code[NUM_ORDER] = 
{
	"OPENLONG",
	"OPENSHORT",
	"CLOSELONG",
	"CLOSESHORT"
};

cString OrderToString( const SwOrder& order )
{
	if( !IsValidOrder( order ) )
		return "";

	return order_code[order];
}

SwOrder StringToOrder( const cString& orderstr )
{
	int i;
	for( i = 0; i < NUM_ORDER; ++i )
	{
		if( Compare( orderstr, order_code[i] ) )
			break;
	}
	if( !IsValidOrder( i ) )
	{
		yr_error( "Invalid order" );
	}

	return (SwOrder)i;
}
