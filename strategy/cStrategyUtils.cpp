#include <cStrategyUtils.h>
#include <cStrategyMovAvg.h>
//
//const int NUM_STRATEGY = 2;
//const cString strategy_code[NUM_STRATEGY] = 
//{
//	"MOVAVG",
//	"RSI"
//};
//
//SwStrategyName StringToStrategyName( const cString& strategyName )
//{
//	int i;
//	for( i = 0; i < NUM_STRATEGY; ++i )
//	{
//		if( Compare( strategyName, strategy_code[i] ) )
//			break;
//	}
//	if( !IsValidStrategyName( i ) )
//	{
//		yr_error( "Invalid Strategy Name" );
//	}
//
//	return (SwStrategyName)i;
//}
//
//cStrategyPtr CreateStrategy( const cString& strategyName )
//{
//	SwStrategyName name = StringToStrategyName( strategyName );
//
//	switch( name )
//	{
//	case SwStrategyName::SWSTRATEGYNAME_MOVAVG:
//		{
//			return( make_shared< cStrategyMovAvg >( strategyName ) );
//		}
//	case SwStrategyName::SWSTRATEGYNAME_RSI:	
//		{
//			yr_error( "not implemented yet" );
//		}
//	default:
//		return NULL;
//	}
//}
//
//cStrategyPtr CreateStrategy( const cString& strategyName, const cArray< cString >& instruments )
//{
//	SwStrategyName name = StringToStrategyName( strategyName );
//	
//	switch( name )
//	{
//	case SwStrategyName::SWSTRATEGYNAME_MOVAVG:
//		{
//			shared_ptr< cStrategy > p = make_shared< cStrategyMovAvg >( strategyName );
//			p->Init( instruments );
//			return p;
//		}
//	case SwStrategyName::SWSTRATEGYNAME_RSI:	
//		{
//			yr_error( "not implemented yet" );
//		}
//	default:
//		return NULL;
//	}
//}
//
//cStrategyPtr ( const cString& strategyName, const cArray< cString >& instruments, const StrategyParameters& strategyParams )
//{
//	SwStrategyName name = StringToStrategyName( strategyName );
//	
//	switch( name )
//	{
//	case SwStrategyName::SWSTRATEGYNAME_MOVAVG:
//		{
//			shared_ptr< cStrategy > p = make_shared< cStrategyMovAvg >( strategyName );
//			p->Init( instruments );
//			StrategyParameters::const_iterator it;
//			for( it = strategyParams.begin(); it != strategyParams.end(); ++it )
//				p->SetParameter( (*it).first, (*it).second );
//
//			return p;
//		}
//	case SwStrategyName::SWSTRATEGYNAME_RSI:	
//		{
//			yr_error( "not implemented yet" );
//		}
//	default:
//		return NULL;
//	}
//}
