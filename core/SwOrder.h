#ifndef __SWORDER_H__
#define __YR_STRUCTS_H__

class cString;

typedef enum SwOrder
{
	SwORDER_INVALID = -1,
	SwORDER_OPENLONG,
	SwORDER_OPENSHORT,
	SwORDER_CLOSELONG,
	SwORDER_CLOSESHORT
} _SwOrder;

inline int IsValidOrder( int testOrder )
{
	return( testOrder >= SwORDER_OPENLONG && testOrder <= SwORDER_CLOSESHORT );
}

cString OrderToString( const SwOrder& order );
SwOrder StringToOrder( const cString& orderstr );

#endif
