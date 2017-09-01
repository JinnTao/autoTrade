#ifndef __CSIGNALCOLLECTION_H__
#define __CSIGNALCOLLECTION_H__

#include <cSignal.h>
#include <map>

typedef map< cString, cArray< const cSignal* > > signalStore;	// map between instrument and array of signals associated with this instrument
typedef map< cString, cSignalPtr > signalHandle;				// map between signalID and the signal

class cSignalCollection
{
public:
	cSignalCollection();
	virtual ~cSignalCollection();

	void Clear();

	void Add( cSignalPtr p_element );

	int Count() const;

	void GetInstrumentIDs( cArray< cString >& instrumentIDs ) const;
	void GetSignalIDs( cArray< cString >& signalIDs ) const;

	cSignal* GetSignalHandle( const cString& );
	cSignalPtr GetSignalHandleSharedPtr( const cString& );
	cArray< const cSignal* > GetSignalByInstrument( const cString& ) const;

	typedef map< cString, cSignalPtr > mapType;

protected:
	mapType _map_signal;
	mapType::iterator _it;
	signalStore _m_signal_instrument;
	signalHandle _m_signal_handle;

private:
	void AddToMapInternal( shared_ptr< cSignal >& element );

};

typedef shared_ptr< cSignalCollection > cSignalCollectionPtr;


#endif