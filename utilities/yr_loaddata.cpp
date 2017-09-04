#include <yr_loaddata.h>
#include <yr_strconverter.h>
#include <cString.h>
#include <cArray.h>
#include <cTick.h>
#include <cTickTime.h>
#include <Windows.h>
#include <map>


void ListFileNamesInDirectory( const cString& directoryName, cArray< cString >& fileNameList )
{
//	HANDLE hFind;

//	wchar_t* file = CharToWchar( directoryName.c_str() );

//	WIN32_FIND_DATA FindFileData;

	//hFind = FindFirstFile( file, &FindFileData );

	//if (hFind != INVALID_HANDLE_VALUE) {
	//	do{
	//		cString fileName = cString( WcharToChar( FindFileData.cFileName ) );
	//		fileNameList.push_back( fileName );
	//	}
	//	while( FindNextFile( hFind, &FindFileData ) );
	//	FindClose( hFind );
	//}
	
}

void LoadTickDataFromFile( const cString& fileName, const cString& instrumentID, cArray< cTick >& ticks, cArray< cTickTime >& tickTimes, bool oldFormat, bool display )
{
	/*
	format in column:
		date		 
		time 
		millisecond
		last price
		bid price
		bid volume
		ask price
		ask volume
		volume
		open interest
	*/
	if( display )
		printf( "loading tick data from %s...\n", fileName.c_str() );

	ifstream ifs;
	ifs.open( fileName.c_str(), ios::in );
	if( !ifs )
		yr_error( "file cannot be found on this PC." );

	string line;
	while( getline( ifs, line ) )
	{
		string buff;
		stringstream ss( line );
		cArray< string > sbu;
		while( ss >> buff )
			sbu.push_back( buff );

		// sanity check here
		// o/w the code will crash
		if( sbu[0].size() != 8 )
			continue;

		int n = tickTimes.getSize();
		cTickTime tickTime( sbu[0].c_str(), sbu[1].c_str(), atoi( sbu[2].c_str() ) );
		if( n > 0 )
		{
			cTickTime tt_prev = tickTimes.getLast();
			if( tt_prev == tickTime )
				continue;
		}

		double last, bid, bidVol, ask, askVol, vol, oi;
		last = atof( sbu[3].c_str() );
		bid = atof( sbu[4].c_str() );
		bidVol = atof( sbu[5].c_str() );
		ask = atof( sbu[6].c_str() );
		askVol = atof( sbu[7].c_str() );
		if( !oldFormat )
		{
			vol = atof( sbu[8].c_str() );
			oi = atof( sbu[9].c_str() );
		}
		//
		cTick tick( instrumentID, tickTime );	
		tick.SetLast( last );
		tick.SetBid( bid );
		tick.SetBidVolume( bidVol );
		tick.SetAsk( ask );
		tick.SetAskVolume( askVol );
		if( !oldFormat )
		{
			tick.SetVolume( vol );
			tick.SetOpenInterest( oi );
		}

		ticks.push_back( tick );
		tickTimes.push_back( tickTime );
	}

	ifs.close();
}


void LoadTickDataFromFiles( const cArray< cString >& fileNames, const cString& instrumentID, cArray< cTick >& ticks, cArray< cTickTime >& tickTimes, bool oldFormat, bool display )
{
	// assume that fileNames are sorted with date information
	for( int i = 0; i < fileNames.getSize(); ++i )
	{
		cArray< cTick > ticks_i;
		cArray< cTickTime > tickTimes_i;
		LoadTickDataFromFile( fileNames[i], instrumentID, ticks_i, tickTimes_i, oldFormat, display );
		for( int j = 0; j < ticks_i.getSize(); ++j )
		{
			ticks.push_back( ticks_i[j] );
			tickTimes.push_back( tickTimes_i[j] );
		}

	}
}

void LoadTickDataFromFiles( const cArray< cString >& fileNames, const cArray< cString >& instrumentIDs, cArray< cTick >& ticks, cArray< cTickTime >& tickTimes, bool oldFormat, bool display )
{
	yr_assert( fileNames.getSize() == instrumentIDs.getSize() );
	
	for( int i = 0; i < fileNames.getSize(); ++i )
	{
		cArray< cTick > ticks_i;
		cArray< cTickTime > tickTimes_i;
		LoadTickDataFromFile( fileNames[i], instrumentIDs[i], ticks_i, tickTimes_i, oldFormat, display );
		for( int j = 0; j < ticks_i.getSize(); ++j )
		{
			ticks.push_back( ticks_i[j] );
			tickTimes.push_back( tickTimes_i[j] );
		}
	}

}


void SortFilesByDate( const cArray< cString >& fileListIn, const cDate& start, const cDate& end, cArray< cString >& fileListOut, cArray< cString >& instrumentIDs )
{
	int startDate = start.GeorgianDate();
	int endDate = end.GeorgianDate();
	
	map< int, map< cString, cArray< cString > > > mapType;
	map< int, map< cString, cArray< cString > > >::iterator it;
	map< cString, cArray < cString > >::iterator it2;
	
	for( int i = 0; i < fileListIn.getSize(); ++i )
	{
		cArray< cString > v;
		cString::Split( v, fileListIn[i], "_" );
		int fileDate = atoi( cString::Left( v.getLast(), v.getLast().size() - 4 ).c_str() );

		if( fileDate < startDate || fileDate > endDate )
			continue;

		it = mapType.find( fileDate );
		if( it == mapType.end() )
		{
			map< cString, cArray< cString > > m;
			cArray< cString > l;
			l.push_back( fileListIn[i] );
			m.insert( map< cString, cArray < cString > >::value_type( v[0], l ) );
			mapType.insert( map< int, map< cString, cArray < cString > > >::value_type( fileDate, m ) );
		}
		else
		{
			it2 = (*it).second.find( v[0] );
			if( it2 == (*it).second.end() )
			{
				cArray< cString > l;
				l.push_back( fileListIn[i] );
				(*it).second.insert( map< cString, cArray < cString > >::value_type( v[0], l ) );
			}
			else
				(*it2).second.push_back( fileListIn[i] );

		}
	}

	for( it = mapType.begin(); it != mapType.end(); ++it )
	{
		int count = 0;
		for( it2 = (*it).second.begin(); it2 != (*it).second.end(); ++it2 )
			count++;
		
		if( count > 1 )
		{
			//todo:
			//different instruments are found on the same business date
			//only load most active instrument
		}
		else
		{
			it2 = (*it).second.begin();
			for( int i = 0; i < (*it2).second.getSize(); ++i )
			{
				instrumentIDs.push_back( (*it2).first );
				fileListOut.push_back( (*it2).second[i] );
			}
		}
	}
}