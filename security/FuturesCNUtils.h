#ifndef __FUTURESCNUTILS_H__
#define __FUTURESCNUTILS_H__

class cString;
template< class T > class cArray;

void ListFirstInstrument( const cString& underlying, cString& instrument );
void ListFirstInstruments( const cArray< cString >& underlyings, cArray< cString >& instruments );
void ListActiveInstruments( cArray< cString >& instruments );

//equity index futures
void ListFirstEqIndexInstrument( const cString& underlying, cString& instrument );
void ListActiveEqIndexInstruments( cArray< cString >& instruments );

//bond futures
void ListFirstBondInstrument( const cString& underlying, cString& instrument );
void ListActiveBondInstruments( cArray< cString >& instruments );

//precious metals
void ListFirstPreciousMetalInstrument( const cString& underlying, cString& instrument );
void ListActivePreciousMetalInstruments( cArray< cString >& instruments );

//base metals
void ListFirstBaseMetalInstrument( const cString& underlying, cString& instrument );
void ListActiveBaseMetalInstruments( cArray< cString >& instruments );

//energy
void ListFirstEnergyInstrument( const cString& underlying, cString& instrument );
void ListActiveEnergyInstruments( cArray< cString >& instruments );

//agriculture
void ListFirstAgricultureInstrument( const cString& underlying, cString& instrument );
void ListActiveAgricultureInstruments( cArray< cString >& instruments );

//industry
void ListFirstIndustryInstrument( const cString& underlying, cString& instrument );
void ListActiveIndustryInstruments( cArray< cString >& instruments );

//
/**/
void InstrumentToUnderlying( const cString& instrument, cString& underlying );
double InstrumentContractSize( const cString& instrument );
double InstrumentTickSize( const cString& instrument );
double InstrumentCommission( const cString& instrument, int offset );
double InstrumentMargin( const cString& instrument );


#endif