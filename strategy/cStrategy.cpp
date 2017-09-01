#include <cStrategy.h>
#include <cTickTime.h>
#include <cSignalCollection.h>
#include <cPositionCollection.h>
#include <cStrategyMovAvg.h>
#include <FuturesCNUtils.h>


cStrategy::cStrategy()
:m_strategyID( theString )
,m_closePnL( 0.0 )
,m_pxidx( -1 )
{
	m_instrumentIDs.resize( 0 );
}

cStrategy::cStrategy( const cString& strategyID )
:m_strategyID( strategyID )
,m_closePnL( 0.0 )
,m_pxidx( -1 )
{
	m_instrumentIDs.resize( 0 );
}


cStrategy::cStrategy( const cString& strategyID, const cArray< cString >& instrumentIDs )
:m_strategyID( strategyID )
,m_closePnL( 0.0 )
,m_pxidx( -1 )
{
	this->Init( instrumentIDs );
}

cStrategy::~cStrategy()
{
	m_closePnL = 0.0;
	if( m_signals.get() )
	{
		m_signals->Clear();
		m_signals.reset();
	}
}

void cStrategy::Init( const cArray< cString >& instrumentIDs )
{
	m_closePnL = 0.0;
	m_pxidx = -1;
	int n = instrumentIDs.getSize();
	if( n > 1 )
		yr_error( "todo: strategy with more than one instrument" );

	m_instrumentIDs.resize( n );
	for( int i = 0; i < n; ++i )
		m_instrumentIDs[i] = instrumentIDs[i];
}

void cStrategy::RegisterMarketDataCollection( cMarketDataCollectionPtr p )
{
	int n = m_instrumentIDs.getSize();
	if( n > 1 )
		yr_error( "todo: strategy with more than one instrument" );

	if( n == 1 )
	{
		if( m_mc.get() )
			m_mc.reset();
		m_mc = p;
	}
}


double cStrategy::PositionPnL( cPositionCollection* positions )
{
	//if( m_instrumentIDs.getSize() == 0 )
	//	return 0.0;

	//if( positions->Count() == 0 )
	//	return 0.0;

	//double pnl = 0.0;
	//for( int i = 0; i < m_instrumentIDs.getSize(); ++i )
	//{
	//	cArray< const cPositionDetail* > pos = positions->GetPositionDetailByInstrument( m_instrumentIDs[i] );
	//	double contractSize = InstrumentContractSize( m_instrumentIDs[i] );
	//	cMarketData* md = m_mc->GetMarketDataHandle( m_instrumentIDs[i] );
	//	
	//	if( !md )
	//		yr_error( "missing market data for %s", m_instrumentIDs[i].c_str() );

	//	double px = md->GetLastTick().Last();
	//	for( int j = 0; j < pos.getSize(); ++j )
	//	{
	//		double pxOpen = pos[j]->GetPrice();
	//		int volume = pos[j]->GetVolume();
	//		if( pos[j]->GetDirection() == '0' )
	//		{
	//			// long
	//			pnl += ( px - pxOpen ) * volume * contractSize;
	//		}
	//		else
	//		{
	//			// short
	//			pnl += ( pxOpen - px ) * volume * contractSize;
	//		}
	//	}
	//}

	//return pnl;
	return 0;
}

void cStrategy::GetInstrumentIDs( cArray< cString >& instrumentIDs ) const
{
	for( int i = 0; i < m_instrumentIDs.getSize(); ++i )
		instrumentIDs.push_back( m_instrumentIDs[i] );
}


double cStrategy::GetParameter( const cString& paramName ) const
{
	//StrategyParameters::const_iterator it = m_params.find( paramName );
	//if( it == m_params.end() )
	//	yr_error( "parameter %s not found!", paramName.c_str() );
	//
	//return( (*it).second );
	return 0;
}


double cStrategy::GetIndicator( const cString& indName ) const
{
	//StrategyIndicators::const_iterator it = m_indicators.find( indName );
	//if( it == m_indicators.end() )
	//	yr_error( "indicator %s not found!", indName.c_str() );
	//
	//return( (*it).second );
	return 0;
}

cArray< const cSignal* > cStrategy::GetSignals( const cString& instrumentID ) const
{
	return m_signals->GetSignalByInstrument( instrumentID );
}


const cSignal* cStrategy::GetLastSignalConstPtr( const cString& instrumentID ) const
{
	cArray< const cSignal* > v = GetSignals( instrumentID );
	if( v.getSize() )
	{
		const cSignal* s = v.getLast();
		return s;
	}
	else
		return NULL;
}

cSignal* cStrategy::GetLastSignal( const cString& instrumentID )
{
	cSignalPtr p = GetLastSignalSharedPtr( instrumentID );
	if( p )
		return p.get();
	else
		return NULL;
}

cSignalPtr cStrategy::GetLastSignalSharedPtr( const cString& instrumentID )
{
	const cSignal* p = GetLastSignalConstPtr( instrumentID );
	if( p )
	{
		return m_signals->GetSignalHandleSharedPtr( p->GetSignalID() );
	}
	else
		return cSignalPtr();
}


void cStrategy::SetParameter( const cString& paramName, double paramValue )
{
	//// find whether instrumentID is included in m_instrumentIDs
	//StrategyParameters::iterator it = m_params.find( paramName );
	//if( it != m_params.end() )
	//	(*it).second = paramValue;
	//else
	//	m_params.insert( map< cString, double >::value_type( paramName, paramValue ) );
}

void cStrategy::SetIndicator( const cString& indName, double indValue )
{
	//StrategyIndicators::iterator it = m_indicators.find( indName );
	//if( it != m_indicators.end() )
	//	(*it).second = indValue;
	//else
	//	m_indicators.insert( map< cString, double >::value_type( indName, indValue ) );
}

void cStrategy::SetSignal( cSignalPtr signal )
{
	m_signals->Add( signal );
}


bool cStrategy::GetDisplaySignalFlag( const cString& instrumentID ) const
{
	map< cString, bool >::const_iterator it = m_displaySignal.find( instrumentID );
	if( it == m_displaySignal.end() )
		yr_error( "%s not found!", instrumentID.c_str() );

	return( (*it).second );
}

void cStrategy::SetDisplaySignalFlag( const cString& underlying, bool flag )
{
	map< cString, bool >::iterator it = m_displaySignal.find( underlying );
	if( it == m_displaySignal.end() )
		m_displaySignal.insert( map< cString, bool >::value_type( underlying, flag ) );
	else
		(*it).second = flag;
}

void cStrategy::SetDisplaySignalFlags( const map< cString, bool >& flags )
{
	map< cString, bool >::const_iterator it;
	for( it = flags.begin(); it != flags.end(); ++it )
		SetDisplaySignalFlag( (*it).first, (*it).second );
}
