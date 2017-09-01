#include <SwFuturesCN.h>
#include <cFutures.h>
#include <cArray.h>
#include <cStringUtils.h>

cFutures::cFutures( const char* id )
: _id( id )
{
}

cFutures::cFutures( const string& id )
: _id( id.c_str() )
{
}

cFutures::cFutures( const cString& id )
: _id( id )
{
}

cFutures::cFutures( const cFutures& in )
{
	yr_assert( this != &in );
	_id = in._id;
}

cFutures::~cFutures()
{
}

cFutures& cFutures::operator = ( const cFutures& in )
{
	if( &in != this )
	{
		_id = in._id;
	}
	return *this;
}

cString cFutures::Underlying() const
{
	// remove the tenor information
	// i.e. IF1512 will return IF only
	
	// 1. need to check whether the underlying is a single asset or 1) combine of the contracts with the same underlying or not
	int l = _id.InString( "_" );
	if( l >= 0 )
	{
		cArray< cString > ids;
		_id.Split( ids, _id, "_" );
		cArray< cString > assets( ids.size() );
		for( int i = 0; i < ids.size(); ++i )
		{
			int size = ids[i].size();
			int j;
			for( j = 0; j < size; ++j )
			{
				cString copyid( ids[i] );
				copyid.SubString( j, size-1 );
				if( copyid.IsInteger() )
					break;
			}
			assets[i] = ids[i];
			assets[i].Left(j);
		}
		bool sameAsset = true;
		for( int i = 0; i < assets.size(); ++i )
		{
			for( int j = i+1; j < assets.size(); ++j )
			{
				if( !Compare( assets[i], assets[j] ) )
				{
					sameAsset = false;
					break;
				}
			}
		}
		if( !sameAsset )
			yr_error( "different underlying asset not supported" );
		return StringToUnderlying( assets[0] );
	}

	int i;
	int size = _id.size();
	for( i = 0; i < size; ++i )
	{
		cString copyStr( _id );
		copyStr.SubString( i, size-1 );
		if( copyStr.IsInteger() )
			break;
	}
	cString underlyingStr(_id);
	underlyingStr.Left(i);
	
	return underlyingStr;

}

cString cFutures::Exchange() const
{
	return GetExchange( Underlying() );
}

cString cFutures::TradingBreak() const
{
	return GetTradingBreak( Underlying() );
}

cArray< cString > cFutures::TradingHours() const
{
	return GetTradingHours( Underlying() );
}

double cFutures::ContractSize() const
{
	return GetContractSize( Underlying() );
}

double cFutures::TickSize() const
{
	return GetTickSize( Underlying() );
}

double cFutures::TickValue() const
{
	return ContractSize() * TickSize();
}
	

cTickTime cFutures::Expiry() const
{
	int l = _id.InString( "_" );
	if( l >= 0 )
		yr_error( "combined contract not supported!" );

	int i;
	int size = _id.size();
	for( i = 0; i < size; ++i )
	{
		cString copyStr( _id );
		copyStr.SubString( i, size-1 );
		if( copyStr.IsInteger() )
			break;
	}
	cString tenor(_id);
	tenor.Right( size - i );
	//TODO:

	cTickTime currentTime;
	return currentTime;


	

}