#ifndef __YR_LOADDATA_H__
#define __YR_LOADDATA_H__

class cString;
class cVector;
class cTick;
class cTickTime;
class cDate;
template< class T > class cArray;


void ListFileNamesInDirectory( const cString& directoryName, cArray< cString >& fileNameList );
void LoadTickDataFromFile( const cString& fileName, const cString& instrumentID, cArray< cTick >& ticks, cArray< cTickTime >& tickTimes, bool oldFormat = false, bool display = false );
void LoadTickDataFromFiles( const cArray< cString >& fileNames, const cString& instrumentID, cArray< cTick >& ticks, cArray< cTickTime >& tickTimes, bool oldFormat = false, bool display = false );
void LoadTickDataFromFiles( const cArray< cString >& fileNames, const cArray< cString >& instrumentIDs, cArray< cTick >& ticks, cArray< cTickTime >& tickTimes, bool oldFormat = false, bool display = false );
void SortFilesByDate( const cArray< cString >& fileListIn, const cDate& start, const cDate& end, cArray< cString >& fileListOut, cArray< cString >& instrumentIDs );

#endif