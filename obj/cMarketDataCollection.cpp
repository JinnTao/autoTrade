#include <stl_ctp.h>
#include <cMarketDataCollection.h>
#include <cTickTime.h>
#include <cSystem.h>
#include <global_defs.h>


cMarketDataCollection::cMarketDataCollection()
: m_genLog( false )
, m_sampleFreq( "1m" )
, m_runCalendarSpread( false )
, m_outputDirectory( theString )
, m_logFile( theString )
{
	_it = _map_mkt_data.end();
	
}

cMarketDataCollection::~cMarketDataCollection()
{
	Clear();
}

void cMarketDataCollection::Clear()
{
	_map_mkt_data.clear();
	_m_mkt_underlying.clear();
	_m_mkt_handle.clear();
	ClearMarketData();
}

void cMarketDataCollection::ClearMarketData()
{
	marketdataHandle::iterator it;
	for( it = _m_mkt_handle.begin(); it != _m_mkt_handle.end(); ++it )
	{
		if( (*it).second->GetNumRecords() > 0 )
			(*it).second->ClearAllData();
	}
}

int cMarketDataCollection::Count() const
{
	int count = 0;
	for( mapType::const_iterator it = _map_mkt_data.begin(); it != _map_mkt_data.end(); ++it )
		++count;
	return count;
}

void cMarketDataCollection::UniqueUnderlyings( cArray< cString >& output ) const
{
	marketdataStore::const_iterator itS;
	for( itS = _m_mkt_underlying.begin(); itS != _m_mkt_underlying.end(); ++itS )
	{
		output.push_back( itS->first );
	}
}

void cMarketDataCollection::GetInstrumentIDs( cArray< cString >& instrumentIDs ) const
{
	marketdataHandle::const_iterator it;
	for( it = _m_mkt_handle.begin(); it != _m_mkt_handle.end(); ++it )
		instrumentIDs.push_back( (*it).second->GetID() );
}

cMarketData* cMarketDataCollection::GetMarketDataHandle( const cString& name )
{
	cMarketDataPtr p = GetMarketDataHandleSharedPtr( name );
	if( p )
		return p.get();
	else
		return NULL;
}

cMarketDataPtr cMarketDataCollection::GetMarketDataHandleSharedPtr( const cString& name )
{
	marketdataHandle::iterator it = _m_mkt_handle.find( name );
	if( it != _m_mkt_handle.end() )
		return (*it).second;
	else
		return cMarketDataPtr();
}

cArray< const cMarketData* > cMarketDataCollection::GetMarketDataByUnderlying( const cString& underlying ) const
{
	marketdataStore::const_iterator itS = _m_mkt_underlying.find( underlying );
	if( itS != _m_mkt_underlying.end() )
	{
		return (*itS).second;
	}
	else
		return cArray< const cMarketData* >();
}

void cMarketDataCollection::Init( cArray< cString >& instrumentIDs )
{
	int nInstruments = instrumentIDs.getSize();
	if( nInstruments < 1 )
		return;

	for( int i = 0; i < nInstruments; ++i )
	{
		shared_ptr< cMarketData > ptr = make_shared< cMarketData >( instrumentIDs[i], m_sampleFreq );
		AddToMapInternal( ptr );
	}

	if( Count() == nInstruments && m_runCalendarSpread )
	{
		cArray< cString > underlyings;
		UniqueUnderlyings( underlyings );
		for( int i = 0; i < underlyings.getSize(); ++i )
		{
			cArray< const cMarketData* > markdataArray = GetMarketDataByUnderlying( underlyings[i] );
			if( markdataArray.getSize() == 2 )
			{
				cString lsid( markdataArray[0]->GetID() );
				lsid += "_";
				lsid += markdataArray[1]->GetID();
				shared_ptr< cMarketData > ptr = make_shared< cMarketData >( lsid, m_sampleFreq );
				cVector weights( 1.0, 2 );
				weights[1] = -1.0;
				ptr->SetWeights( weights );
				AddToMapInternal( ptr );
			}
		}
	}

}

void cMarketDataCollection::SetDisplayTickFlags( bool flags )
{
	for(auto i = this->_m_mkt_handle.begin();i!=this->_m_mkt_handle.end();i++){
		i->second->SetDisplayTick(flags);
	}
	//map< cString, bool >::const_iterator it;
	//for( it = flags.begin(); it != flags.end(); ++it )
	//{
	//	cArray< const cMarketData* > mds = GetMarketDataByUnderlying( (*it).first );
	//	for( int i = 0; i < mds.getSize(); ++i )
	//	{
	//		cString id = mds[i]->GetID();
	//		cMarketData* md = GetMarketDataHandle( id );
	//		if( md )
	//			md->SetDisplayTick( (*it).second );
	//	}
	//}
}

void cMarketDataCollection::SetDisplayCandleFlags( const map< cString, bool >& flags )
{
	map< cString, bool >::const_iterator it;
	for( it = flags.begin(); it != flags.end(); ++it )
	{
		cArray< const cMarketData* > mds = GetMarketDataByUnderlying( (*it).first );
		for( int i = 0; i < mds.getSize(); ++i )
		{
			cString id = mds[i]->GetID();
			cMarketData* md = GetMarketDataHandle( id );
			if( md )
				md->SetDisplayCandle( (*it).second );
		}
	}
}

void cMarketDataCollection::SetOutputDirectory( const cString& directory )
{
	m_outputDirectory = directory;
	
	marketdataHandle::iterator it;
	for( it = _m_mkt_handle.begin(); it != _m_mkt_handle.end(); ++it )
		(*it).second->SetOutputDirectory( directory );

	mapType::iterator it2;
	for( it2 = _map_mkt_data.begin(); it2 != _map_mkt_data.end(); ++it2 )
		(*it2).second->SetOutputDirectory( directory );
}

void cMarketDataCollection::SetTickDataFolder( const cString& folderName )
{
	marketdataHandle::iterator it;
	for( it = _m_mkt_handle.begin(); it != _m_mkt_handle.end(); ++it )
		(*it).second->SetTickDataFolder( folderName );

	mapType::iterator it2;
	for( it2 = _map_mkt_data.begin(); it2 != _map_mkt_data.end(); ++it2 )
		(*it2).second->SetTickDataFolder( folderName );
}

void cMarketDataCollection::SetCandleDataFolder( const cString& folderName )
{
	marketdataHandle::iterator it;
	for( it = _m_mkt_handle.begin(); it != _m_mkt_handle.end(); ++it )
		(*it).second->SetCandleDataFolder( folderName );

	mapType::iterator it2;
	for( it2 = _map_mkt_data.begin(); it2 != _map_mkt_data.end(); ++it2 )
		(*it2).second->SetCandleDataFolder( folderName );
}


void cMarketDataCollection::OnRtnDepthMarketData( CThostFtdcDepthMarketDataField* pDepthMarketData )
{
	cString id( pDepthMarketData->InstrumentID );
	cMarketData* md = GetMarketDataHandle( id );

	if( !md ){
		// create new CmarketData
		shared_ptr< cMarketData > ptr = make_shared< cMarketData >( pDepthMarketData->InstrumentID, m_sampleFreq );
		AddToMapInternal( ptr );
		ptr->OnRtnDepthMarketData(pDepthMarketData);
	}else{
		md->OnRtnDepthMarketData( pDepthMarketData );
	}
}

void cMarketDataCollection::SimulationSetTime( double t )
{
	m_simTime = t;
	marketdataHandle::iterator it;
	for( it = _m_mkt_handle.begin(); it != _m_mkt_handle.end(); ++it )
		(*it).second->SimulationSetTime( t );
}

void cMarketDataCollection::SimulationUpdate( const cTick& tick )
{
	cMarketData* md = GetMarketDataHandle( tick.InstrumentID() );

	if( !md )
		return;

	md->SimulationUpdate( tick );
}


void cMarketDataCollection::RtnLastCandle()
{
	marketdataHandle::iterator it;
	for( it = _m_mkt_handle.begin(); it != _m_mkt_handle.end(); ++it )
		(*it).second->RtnLastCandle();
}

DWORD cMarketDataCollection::IOProcess()
{
	while( true )
	{
		if( cSystem::DoWriteDataToFile() )
		{
			cout << endl;
			this->WriteCandleStickDataToFile();
			this->WriteTickDataToFile();
		}
		this->RtnLastCandle();
	}
	return 0;
}

DWORD cMarketDataCollection::SimulationIOProcess()
{
	while( true )
	{
		if( cSystem::DoWriteDataToFile( m_simTime ) )
		{
			//cout << endl;
			//this->WriteCandleStickDataToFile();
			//this->WriteTickDataToFile();
			marketdataHandle::iterator it;
			for( it = _m_mkt_handle.begin(); it != _m_mkt_handle.end(); ++it )
				(*it).second->ClearAllData();
		}
		this->SimulationRtnLastCandle();
	}
	return 0;
}

void cMarketDataCollection::SimulationRtnLastCandle()
{
	marketdataHandle::iterator it;
	for( it = _m_mkt_handle.begin(); it != _m_mkt_handle.end(); ++it )
		(*it).second->SimulationRtnLastCandle();
}

void cMarketDataCollection::WriteTickDataToFile()
{
	marketdataHandle::iterator it;
	for( it = _m_mkt_handle.begin(); it != _m_mkt_handle.end(); ++it )
	{
		int iReturn = (*it).second->WriteTickDataToFile();
		if( iReturn == 1 )
		{
			char message[256];
			sprintf( message, "%s: write tick data of %s to file", cSystem::GetCurrentTimeBuffer().c_str(), (*it).second->GetID().c_str() );
			cout << message << endl;
			if( m_genLog )
			{
				if( m_logFile.IsBlankString() )
					m_logFile = "logtemp.txt";
				if( m_outputDirectory.IsBlankString() )
					cSystem::WriteLogFile( m_logFile.c_str(), message, false );
				else
				{
					cString folderDir = m_outputDirectory + m_logFileFolder + "//"; 
					cSystem::WriteLogFile( folderDir.c_str(), m_logFile.c_str(), message, false );
				}
			}
		}
	}
}

void cMarketDataCollection::WriteCandleStickDataToFile()
{
	marketdataHandle::iterator it;
	for( it = _m_mkt_handle.begin(); it != _m_mkt_handle.end(); ++it )
	{
		int iReturn = (*it).second->WriteCandleStickDataToFile();
		if( iReturn == 1 )
		{
			char message[256];
			sprintf( message, "%s: write candle data of %s to file", cSystem::GetCurrentTimeBuffer().c_str(), (*it).second->GetID().c_str() );
			cout << message << endl;
			if( m_genLog )
			{
				if( m_logFile.IsBlankString() )
					m_logFile = "logtemp.txt";
				if( m_outputDirectory.IsBlankString() )
					cSystem::WriteLogFile( m_logFile.c_str(), message, false );
				else
				{
					cString folderDir = m_outputDirectory + m_logFileFolder + "//"; 
					cSystem::WriteLogFile( folderDir.c_str(), m_logFile.c_str(), message, false );
				}
			}
		}
	}
}

void cMarketDataCollection::ReadTickDataFromFile( const cArray< cArray< cString > >& fileNames )
{
	marketdataHandle::iterator it;
	int i = 0;
	for( it = _m_mkt_handle.begin(); it != _m_mkt_handle.end(); ++it, ++i )
	{
		cArray< cString > files = fileNames[i];
		(*it).second->ReadTickDataFromFile( files );
		char message[256];
		sprintf( message, "%s: read tick data of %s from file", cSystem::GetCurrentTimeBuffer().c_str(), (*it).second->GetID().c_str() );
		if( m_genLog )
		{
			if( m_logFile.IsBlankString() )
				m_logFile = "logtemp.txt";
			if( m_outputDirectory.IsBlankString() )
				cSystem::WriteLogFile( m_logFile.c_str(), message, false );
			else
			{
				cString folderDir = m_outputDirectory + m_logFileFolder + "//"; 
				cSystem::WriteLogFile( folderDir.c_str(), m_logFile.c_str(), message, false );
			}
		}
	}
}

void cMarketDataCollection::ReadCandleStickDataFromFile(  const cArray< cArray< cString > >& fileNames  )
{
	marketdataHandle::iterator it;
	int i = 0;
	for( it = _m_mkt_handle.begin(); it != _m_mkt_handle.end(); ++it, ++i )
	{
		cArray< cString > files = fileNames[i];
		(*it).second->ReadCandleStickDataFromFile( files );
		char message[256];
		sprintf( message, "%s: read candle data of %s from file", cSystem::GetCurrentTimeBuffer().c_str(), (*it).second->GetID().c_str() );
		if( m_genLog )
		{
			if( m_logFile.IsBlankString() )
				m_logFile = "logtemp.txt";
			if( m_outputDirectory.IsBlankString() )
				cSystem::WriteLogFile( m_logFile.c_str(), message, false );
			else
			{
				cString folderDir = m_outputDirectory + m_logFileFolder + "//"; 
				cSystem::WriteLogFile( folderDir.c_str(), m_logFile.c_str(), message, false );
			}
		}
	}
}
	

void cMarketDataCollection::AddToMapInternal( shared_ptr< cMarketData >& element )
{
	cString id = element->GetID();
	mapType::iterator it = _map_mkt_data.find( id );
	if( it != _map_mkt_data.end() )
		(*it).second = element;
	else
		_map_mkt_data.insert( mapType::value_type( id, element ) );

	cString underlying = element->GetUnderlyingStr();
	marketdataStore::iterator itS = _m_mkt_underlying.find( underlying );
	if( itS == _m_mkt_underlying.end() )
	{
		cArray< const cMarketData* > v;
		v.push_back( element.get() );
		_m_mkt_underlying.insert( marketdataStore::value_type( underlying, v ) );
	}
	else
		(*itS).second.push_back( element.get() );

	// now we assume _m_mkt_handle and _map_mkt_data are the same
	// todo: handle can be different but not id
	marketdataHandle::iterator itH = _m_mkt_handle.find( id );
	if( itH == _m_mkt_handle.end() )
		_m_mkt_handle.insert( marketdataHandle::value_type( id, element ) );
	else
		(*itH).second = element;

}
