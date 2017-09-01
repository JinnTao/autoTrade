#include <cSignalCollection.h>

cSignalCollection::cSignalCollection()
{
	_it = _map_signal.end();
}

cSignalCollection::~cSignalCollection()
{
	Clear();
}

void cSignalCollection::Clear()
{
	_map_signal.clear();
	_m_signal_instrument.clear();
	_m_signal_handle.clear();
}

int cSignalCollection::Count() const
{
	int count = 0;
	for( mapType::const_iterator it = _map_signal.begin(); it != _map_signal.end(); ++it )
		count++;
	return count;
}

void cSignalCollection::GetInstrumentIDs( cArray< cString >& instrumentIDs ) const
{
	signalStore::const_iterator it; 
	for( it = _m_signal_instrument.begin(); it != _m_signal_instrument.end(); ++it )
		instrumentIDs.push_back( (*it).first );

}

void cSignalCollection::GetSignalIDs( cArray< cString >& signalIDs ) const
{
	signalHandle::const_iterator it;
	for( it = _m_signal_handle.begin(); it != _m_signal_handle.end(); ++it )
		signalIDs.push_back( (*it).first );
}

cSignal* cSignalCollection::GetSignalHandle( const cString& signalID )
{
	cSignalPtr p = GetSignalHandleSharedPtr( signalID );
	if( p )
		return p.get();
	else
		return NULL;
}

cSignalPtr cSignalCollection::GetSignalHandleSharedPtr( const cString& signalID )
{
	signalHandle::iterator it = _m_signal_handle.find( signalID );
	if( it != _m_signal_handle.end() )
		return (*it).second;
	else
		return cSignalPtr();
}

cArray< const cSignal* > cSignalCollection::GetSignalByInstrument( const cString& instrumentID ) const
{
	signalStore::const_iterator it = _m_signal_instrument.find( instrumentID );
	if( it != _m_signal_instrument.end() )
	{
		return (*it).second;
	}
	else
		return cArray< const cSignal* >();
}

void cSignalCollection::AddToMapInternal( shared_ptr< cSignal >& element )
{
	cString signalID = element->GetSignalID();
	mapType::iterator it1 = _map_signal.find( signalID );
	if( it1 != _map_signal.end() )
		(*it1).second = element;
	else
		_map_signal.insert( mapType::value_type( signalID, element ) );

	cString instrumentID = element->GetInstrumentID();
	signalStore::iterator it2 = _m_signal_instrument.find( instrumentID );
	if( it2 == _m_signal_instrument.end() )
	{
		cArray< const cSignal* > v;
		v.push_back( element.get() );
		_m_signal_instrument.insert( signalStore::value_type( instrumentID, v ) );
	}
	else
	{
		bool foundFlag = false;
		for( int i = 0; i < (*it2).second.getSize(); ++i )
		{
			if( element->GetSignalID() == (*it2).second[i]->GetSignalID() )
			{
				foundFlag = true;
				break;
			}
		}
		if( !foundFlag )
			(*it2).second.push_back( element.get() );
	}

	signalHandle::iterator it3 = _m_signal_handle.find( signalID );
	if( it3 == _m_signal_handle.end() )
		_m_signal_handle.insert( signalHandle::value_type( signalID, element ) );
	else
		(*it3).second = element;
}

void cSignalCollection::Add( cSignalPtr p_element )
{
	AddToMapInternal( p_element );
}




