#ifndef __YR_DOWNLOADMARKETDATA_H__
#define __YR_DOWNLOADMARKETDATA_H__

template< class T > class cArray;
class cString;

void RunMarketdataDownload( const cArray< cString >& InstrumentIDs );

//int CountInstruments( cArray< cString >& InstrumentIDs );

#endif