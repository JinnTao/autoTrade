#include <FuturesCNUtils.h>
#include <SwFuturesCN.h>
#include <cSystem.h>
#include <cErr.h>
#include <cArray.h>
#include <cString.h>


void ListFirstInstrument( const cString& underlying, cString& instrument )
{
	if( Compare( underlying, "IF" ) || Compare( underlying, "IH" ) || Compare( underlying, "IC" ) )
		ListFirstEqIndexInstrument( underlying, instrument );
	
	if( Compare( underlying, "T" ) || Compare( underlying, "TF" ) )
		ListFirstBondInstrument( underlying, instrument );
	
	if( Compare( underlying, "ag" ) || Compare( underlying, "au" ) )
		ListFirstPreciousMetalInstrument( underlying, instrument );

	if( Compare( underlying, "cu" ) || Compare( underlying, "al" ) || Compare( underlying, "zn" ) || Compare( underlying, "pb" ) 
		|| Compare( underlying, "nl" ) || Compare( underlying, "sn" ) )
		ListFirstBaseMetalInstrument( underlying, instrument );

	if( Compare( underlying, "l" ) || Compare( underlying, "pp" ) || Compare( underlying, "TA" ) || Compare( underlying, "MA" ) )
		ListFirstEnergyInstrument( underlying, instrument );

	if( Compare( underlying, "c" ) || Compare( underlying, "jd" ) || Compare( underlying, "a" ) 
		|| Compare( underlying, "m" ) || Compare( underlying, "y" ) || Compare( underlying, "p" ) 
		|| Compare( underlying, "SR" ) || Compare( underlying, "CF" ) || Compare( underlying, "OI" ) 
		|| Compare( underlying, "RM" ) )
		ListFirstAgricultureInstrument( underlying, instrument );

	if( Compare( underlying, "j" ) || Compare( underlying, "jm" ) || Compare( underlying, "i" ) 
		|| Compare( underlying, "ru" ) || Compare( underlying, "rb" ) || Compare( underlying, "hc" )
		|| Compare( underlying, "FG" ) || Compare( underlying, "ZC" ) )
		ListFirstIndustryInstrument( underlying, instrument );

}

void ListFirstInstruments( const cArray< cString >& underlyings, cArray< cString >& instruments )
{
	instruments.clear();

	for( int i = 0; i < underlyings.getSize(); ++i )
	{
		cString instrument;
		ListFirstInstrument( underlyings[i], instrument );
		instruments.push_back( instrument );
	}
}

void ListActiveInstruments( cArray< cString >& instruments )
{
	//
	cArray< cString > eqInstruments;
	ListActiveEqIndexInstruments( eqInstruments );
	for( int i = 0; i < eqInstruments.getSize(); ++i )
		instruments.push_back( eqInstruments[i] );
	//
	cArray< cString > bondInstruments;
	ListActiveBondInstruments( bondInstruments );
	for( int i = 0; i < bondInstruments.getSize(); ++i )
		instruments.push_back( bondInstruments[i] );
	//
	cArray< cString > preciousMetalInstruments;
	ListActivePreciousMetalInstruments( preciousMetalInstruments );
	for( int i = 0; i < preciousMetalInstruments.getSize(); ++i )
		instruments.push_back( preciousMetalInstruments[i] );
	//
	cArray< cString > baseMetalInstruments;
	ListActiveBaseMetalInstruments( baseMetalInstruments );
	for( int i = 0; i < baseMetalInstruments.getSize(); ++i )
		instruments.push_back( baseMetalInstruments[i] );
	//
	cArray< cString > energyInstruments;
	ListActiveEnergyInstruments( energyInstruments );
	for( int i = 0; i < energyInstruments.getSize(); ++i )
		instruments.push_back( energyInstruments[i] );
	//
	cArray< cString > agricultureInstruments;
	ListActiveAgricultureInstruments( agricultureInstruments );
	for( int i = 0; i < agricultureInstruments.getSize(); ++i )
		instruments.push_back( agricultureInstruments[i] );
	//
	cArray< cString > industryInstruments;
	ListActiveIndustryInstruments( industryInstruments );
	for( int i = 0; i < industryInstruments.getSize(); ++i )
		instruments.push_back( industryInstruments[i] );
}


void ListFirstEqIndexInstrument( const cString& underlying, cString& instrument )
{
	if( !( Compare( underlying, "IF" ) || Compare( underlying, "IH" ) || Compare( underlying, "IC" ) ) )
		yr_error( "invalid equity index underlying input..." );

	struct tm* ct = cSystem::GetTime();
	int currYear = ct->tm_year+1900;
	int currMonth = ct->tm_mon + 1;
	int currDay = ct->tm_mday;
	
	
	// check whether the current month contract is expired or not
	// the futures expires on the 3rd Friday of the week
	bool expired = false;
	int curwd = ct->tm_wday;
	if( curwd >= 5 )
	{
		int daypassed = currDay - ( curwd - 5 );
		if( daypassed - 14 >= 1 )
			expired = true;
	}
	else
	{
		int daypassed = currDay - curwd - 2;
		if( daypassed - 14 >= 1 )
			expired = true;
	}

	if( expired )
	{
		currMonth += 1;
		if( currMonth > 12 )
		{
			currYear += 1;
			currMonth -= 12;
		}
	}

	int tenor = ( currYear - 2000 ) * 100 + currMonth;
	char id[10];
	sprintf( id, "%s%4d", underlying.c_str(), tenor );
	instrument = cString( id );
}


void ListActiveEqIndexInstruments( cArray< cString >& instruments )
{
	struct tm* ct = cSystem::GetTime();
	
	int currYear = ct->tm_year+1900;
	int currMonth = ct->tm_mon + 1;
	int currDay = ct->tm_mday;
	//todo: check whether the current month contract is expired or not


	int m = currMonth % 3;
	int firstQuarter, secondQuarter;
	
	if( m > 0 )
	{
		firstQuarter = 3 * ( currMonth /3  + 1 );
		secondQuarter = 3 * ( currMonth /3  + 2 );
		if( m == 2 )
		{
			firstQuarter += 3;
			secondQuarter += 3;
		}
	}
	else
	{
		firstQuarter = currMonth + 3;
		secondQuarter = currMonth + 6;
	}

	cIvector tenors(4);
	tenors[0] = ( currYear - 2000 ) * 100 + currMonth;
	int nextMonth = currMonth + 1;
	if( nextMonth > 12 )
	{
		int tempYear = currYear + 1;
		nextMonth -= 12;
		tenors[1] = ( tempYear - 2000 ) * 100 + nextMonth;
	}
	else
		tenors[1] = ( currYear - 2000 ) * 100 + nextMonth;

	if( firstQuarter > 12 )
	{
		int tempYear = currYear + 1;
		firstQuarter -= 12;
		tenors[2] = ( tempYear - 2000 ) * 100 + firstQuarter;
	}
	else
		tenors[2] = ( currYear - 2000 ) * 100 + firstQuarter;

	if( secondQuarter > 12 )
	{
		int tempYear = currYear + 1;
		secondQuarter -= 12;
		tenors[3] = ( tempYear - 2000 ) * 100 + secondQuarter;
	}
	else
		tenors[3] = ( currYear - 2000 ) * 100 + secondQuarter;


	cArray< cString > eqUnderlyings;
	eqUnderlyings.push_back( "IF" );
	eqUnderlyings.push_back( "IH" );
	eqUnderlyings.push_back( "IC" );

	instruments.resize(0);

	for( int i = 0; i < eqUnderlyings.getSize(); ++i )
	{
		for( int j = 0; j < tenors.getSize(); ++j )
		{
			char id[256];
			sprintf( id, "%s%4d", eqUnderlyings[i].c_str(), tenors[j] );
			instruments.push_back( id );
		}
	}

}

void ListFirstBondInstrument( const cString& underlying, cString& instrument )
{
	//list the first 2 contracts
	struct tm* ct = cSystem::GetTime();
	
	int currYear = ct->tm_year+1900;
	int currMonth = ct->tm_mon + 1;
	
	int m = currMonth % 3;
	int firstQuarter;

	if( m > 0 )
	{
		firstQuarter = 3 * ( currMonth /3  + 1 );
	}
	else
	{
		firstQuarter = currMonth + 3;
	}

	int tenor;

	if( firstQuarter > 12 )
	{
		int tempYear = currYear + 1;
		firstQuarter -= 12;
		tenor = ( tempYear - 2000 ) * 100 + firstQuarter;
	}
	else
		tenor = ( currYear - 2000 ) * 100 + firstQuarter;

	
	char id[256];
	sprintf( id, "%s%4d", underlying.c_str(), tenor );
	instrument = cString( id );
}

void ListActiveBondInstruments( cArray< cString >& instruments )
{
	//list the first 2 contracts
	struct tm* ct = cSystem::GetTime();
	
	int currYear = ct->tm_year+1900;
	int currMonth = ct->tm_mon + 1;
	
	int m = currMonth % 3;
	int firstQuarter, secondQuarter;

	if( m > 0 )
	{
		firstQuarter = 3 * ( currMonth /3  + 1 );
		secondQuarter = 3 * ( currMonth /3  + 2 );
	}
	else
	{
		firstQuarter = currMonth + 3;
		secondQuarter = currMonth + 6;
	}

	cIvector tenors(2);

	if( firstQuarter > 12 )
	{
		int tempYear = currYear + 1;
		firstQuarter -= 12;
		tenors[0] = ( tempYear - 2000 ) * 100 + firstQuarter;
	}
	else
		tenors[0] = ( currYear - 2000 ) * 100 + firstQuarter;

	if( secondQuarter > 12 )
	{
		int tempYear = currYear + 1;
		secondQuarter -= 12;
		tenors[1] = ( tempYear - 2000 ) * 100 + secondQuarter;
	}
	else
		tenors[1] = ( currYear - 2000 ) * 100 + secondQuarter;


	cArray< cString > underlyings;
	underlyings.push_back( "TF" );
	underlyings.push_back( "T" );

	instruments.resize(0);

	for( int i = 0; i < underlyings.getSize(); ++i )
	{
		for( int j = 0; j < tenors.getSize(); ++j )
		{
			char id[256];
			sprintf( id, "%s%4d", underlyings[i].c_str(), tenors[j] );
			instruments.push_back( id );
		}
	}
}

void ListFirstPreciousMetalInstrument( const cString& underlying, cString& instrument )
{
	struct tm* ct = cSystem::GetTime();

	int currYear = ct->tm_year+1900;
	int currMonth = ct->tm_mon + 1;

	int firstSemi = currMonth < 6 ? 6 : 12;

	int tenor = ( currYear - 2000 ) * 100 + firstSemi;

	char id[256];
	sprintf( id, "%s%4d", underlying.c_str(), tenor );
	instrument = cString( id );
}


void ListActivePreciousMetalInstruments( cArray< cString >& instruments )
{
	struct tm* ct = cSystem::GetTime();
	
	int currYear = ct->tm_year+1900;
	int currMonth = ct->tm_mon + 1;

	int firstSemi = currMonth < 6 ? 6 : 12;
	int secondSemi = firstSemi + 6;

	cIvector tenors(2);

	tenors[0] = ( currYear - 2000 ) * 100 + firstSemi;

	if( secondSemi > 12 )
	{
		int tempYear = currYear + 1;
		secondSemi -= 12;
		tenors[1] = ( tempYear - 2000 ) * 100 + secondSemi;
	}
	else
		tenors[1] = ( currYear - 2000 ) * 100 + secondSemi;

	cArray< cString > underlyings;
	underlyings.push_back( "ag" );
	underlyings.push_back( "au" );

	instruments.resize(0);

	for( int i = 0; i < underlyings.getSize(); ++i )
	{
		for( int j = 0; j < tenors.getSize(); ++j )
		{
			char id[256];
			sprintf( id, "%s%4d", underlyings[i].c_str(), tenors[j] );
			instruments.push_back( id );
		}
	}
}

void ListFirstBaseMetalInstrument( const cString& underlying, cString& instrument )
{
	struct tm* ct = cSystem::GetTime();
	
	int currYear = ct->tm_year+1900;
	int currMonth = ct->tm_mon + 1;

	if( Compare( underlying, "cu" ) || Compare( underlying, "al" ) || Compare( underlying, "zn" ) || Compare( underlying, "pb" ) )
	{
		int activeMonth = currMonth + 2;
		if( activeMonth > 12 )
		{
			currYear += 1;
			activeMonth -= 12;
		}
		int tenor = ( currYear - 2000 ) * 100 + activeMonth;

		char id[256];
		sprintf( id, "%s%4d", underlying, tenor );
		instrument = cString( id );
	}
	else if( Compare( underlying, "ni" ) || Compare( underlying, "sn" ) )
	{
		int activeMonth, tenor;
		if( currMonth < 5 && currMonth >= 1 )
		{
			activeMonth = 5;
			tenor = ( currYear - 2000 ) * 100 + activeMonth;
		}
		else if( currMonth < 9 && currMonth >= 5 )
		{
			activeMonth = 9;
			tenor = ( currYear - 2000 ) * 100 + activeMonth;
		}
		else
		{
			activeMonth = 1;
			tenor = ( currYear + 1 - 2000 ) * 100 + activeMonth;
		}

		char id[256];
		sprintf( id, "%s%4d", underlying, tenor );
		instrument = cString( id );
	}
}

void ListActiveBaseMetalInstruments( cArray< cString >& instruments )
{
	struct tm* ct = cSystem::GetTime();
	
	int currYear = ct->tm_year+1900;
	int currMonth = ct->tm_mon + 1;

	cIvector activeMonths(3);
	for( int i = 0; i < activeMonths.getSize(); ++i )
		activeMonths[i] = currMonth + i + 1;
	

	cIvector tenors;
	for( int i = 0; i < activeMonths.getSize(); ++i )
	{
		int thisMonth = activeMonths[i];
		if( thisMonth > 12 )
		{
			int tempYear = currYear + 1;
			thisMonth -= 12;
			int tenor = ( tempYear - 2000 ) * 100 + thisMonth;
			tenors.push_back( tenor );
		}
		else
		{
			int tenor = ( currYear - 2000 ) * 100 + thisMonth;
			tenors.push_back( tenor );
		}
	}

	cArray< cString > underlyings;
	underlyings.push_back( "cu" );
	underlyings.push_back( "al" );
	underlyings.push_back( "zn" );
	underlyings.push_back( "pb" );

	instruments.resize(0);

	for( int i = 0; i < underlyings.getSize(); ++i )
	{
		for( int j = 0; j < tenors.getSize(); ++j )
		{
			char id[256];
			sprintf( id, "%s%4d", underlyings[i].c_str(), tenors[j] );
			instruments.push_back( id );
		}
	}

	// ni and sn
	int firstMonth, secondMonth;
	cIvector tenors2(2);
	if( currMonth < 5 && currMonth >= 1 )
	{
		firstMonth = 5;
		secondMonth = 9;
		tenors2[0] = ( currYear - 2000 ) * 100 + firstMonth;
		tenors2[1] = ( currYear - 2000 ) * 100 + secondMonth;
	}
	else if( currMonth < 9 && currMonth >= 5 )
	{
		firstMonth = 9;
		secondMonth = 1;
		tenors2[0] = ( currYear - 2000 ) * 100 + firstMonth;
		tenors2[1] = ( currYear - 2000 + 1 ) * 100 + secondMonth;
	}
	else
	{
		firstMonth = 1;
		secondMonth = 5;
		tenors2[0] = ( currYear - 2000 + 1 ) * 100 + firstMonth;
		tenors2[1] = ( currYear - 2000 + 1 ) * 100 + secondMonth;
	}
	
	cArray< cString > underlyings2;
	underlyings2.push_back( "ni" );
	underlyings2.push_back( "sn" );
	
	for( int i = 0; i < underlyings2.getSize(); ++i )
	{
		for( int j = 0; j < tenors2.getSize(); ++j )
		{
			char id[256];
			sprintf( id, "%s%4d", underlyings2[i].c_str(), tenors2[j] );
			instruments.push_back( id );
		}
	}

}

void ListFirstEnergyInstrument( const cString& underlying, cString& instrument )
{
	struct tm* ct = cSystem::GetTime();

	int currYear = ct->tm_year+1900;
	int currMonth = ct->tm_mon + 1;

	int firstMonth, tenor1, tenor2;
	if( currMonth < 5 && currMonth >= 1 )
	{
		firstMonth = 5;
		tenor1 = ( currYear - 2000 ) * 100 + firstMonth;
		tenor2 = ( currYear - 2010 ) * 100 + firstMonth;
	}
	else if( currMonth < 9 && currMonth >= 5 )
	{
		firstMonth = 9;
		tenor1 = ( currYear - 2000 ) * 100 + firstMonth;
		tenor2 = ( currYear - 2010 ) * 100 + firstMonth;
	}
	else
	{
		firstMonth = 1;
		tenor1 = ( currYear - 2000 + 1 ) * 100 + firstMonth;
		tenor2 = ( currYear - 2010 + 1 ) * 100 + firstMonth;
	}

	if( Compare( underlying, "l" ) || Compare( underlying, "pp" ) )
	{
		char id[256];
		sprintf( id, "%s%4d", underlying.c_str(), tenor1 );
		instrument = cString( id );
	}
	else if( Compare( underlying, "TA" ) || Compare( underlying, "MA" ) )
	{
		char id[256];
		sprintf( id, "%s%3d", underlying.c_str(), tenor2 );
		instrument = cString( id );
	}

}


void ListActiveEnergyInstruments( cArray< cString >& instruments )
{
	struct tm* ct = cSystem::GetTime();
	
	int currYear = ct->tm_year+1900;
	int currMonth = ct->tm_mon + 1;

	int firstMonth, secondMonth;
	cIvector tenors(2);
	cIvector tenors2(2);
	if( currMonth < 5 && currMonth >= 1 )
	{
		firstMonth = 5;
		secondMonth = 9;
		tenors[0] = ( currYear - 2000 ) * 100 + firstMonth;
		tenors[1] = ( currYear - 2000 ) * 100 + secondMonth;
		tenors2[0] = ( currYear - 2010 ) * 100 + firstMonth;
		tenors2[1] = ( currYear - 2010 ) * 100 + secondMonth;
	}
	else if( currMonth < 9 && currMonth >= 5 )
	{
		firstMonth = 9;
		secondMonth = 1;
		tenors[0] = ( currYear - 2000 ) * 100 + firstMonth;
		tenors[1] = ( currYear - 2000 + 1 ) * 100 + secondMonth;
		tenors2[0] = ( currYear - 2010 ) * 100 + firstMonth;
		tenors2[1] = ( currYear - 2010 + 1 ) * 100 + secondMonth;
	}
	else
	{
		firstMonth = 1;
		secondMonth = 5;
		tenors[0] = ( currYear - 2000 + 1 ) * 100 + firstMonth;
		tenors[1] = ( currYear - 2000 + 1 ) * 100 + secondMonth;
		tenors2[0] = ( currYear - 2010 + 1 ) * 100 + firstMonth;
		tenors2[1] = ( currYear - 2010 + 1 ) * 100 + secondMonth;
	}
	
	cArray< cString > underlyings, underlyings2;
	underlyings.push_back( "l" );
	underlyings.push_back( "pp" );

	instruments.resize( 0 );

	for( int i = 0; i < underlyings.getSize(); ++i )
	{
		for( int j = 0; j < tenors.getSize(); ++j )
		{
			char id[256];
			sprintf( id, "%s%4d", underlyings[i].c_str(), tenors[j] );
			instruments.push_back( id );
		}
	}

	underlyings2.push_back( "TA" );
	underlyings2.push_back( "MA" );
	for( int i = 0; i < underlyings2.getSize(); ++i )
	{
		for( int j = 0; j < tenors2.getSize(); ++j )
		{
			char id[256];
			sprintf( id, "%s%3d", underlyings2[i].c_str(), tenors2[j] );
			instruments.push_back( id );
		}
	}


}

void ListFirstAgricultureInstrument( const cString& underlying, cString& instrument )
{
	struct tm* ct = cSystem::GetTime();

	int currYear = ct->tm_year+1900;
	int currMonth = ct->tm_mon + 1;
	int firstMonth, tenor1, tenor2;

	if( currMonth < 5 && currMonth >= 1 )
	{
		firstMonth = 5;
		tenor1 = ( currYear - 2000 ) * 100 + firstMonth;
		tenor2 = ( currYear - 2010 ) * 100 + firstMonth;
	}
	else if( currMonth < 9 && currMonth >= 5 )
	{
		firstMonth = 9;
		tenor1 = ( currYear - 2000 ) * 100 + firstMonth;
		tenor2 = ( currYear - 2010 ) * 100 + firstMonth;
	}
	else
	{
		firstMonth = 1;
		tenor1 = ( currYear - 2000 + 1 ) * 100 + firstMonth;
		tenor2 = ( currYear - 2010 + 1 ) * 100 + firstMonth;
	}
	if( Compare( underlying, "c" ) || Compare( underlying, "jd" ) || Compare( underlying, "a" ) 
		|| Compare( underlying, "m" ) || Compare( underlying, "y" ) || Compare( underlying, "p" ) )
	{
		char id[256];
		sprintf( id, "%s%4d", underlying.c_str(), tenor1 );
		instrument = cString( id );
	}
	else if( Compare( underlying, "SR" ) || Compare( underlying, "CF" ) || Compare( underlying, "OI" ) || Compare( underlying, "RM" ) )
	{
		char id[256];
		sprintf( id, "%s%3d", underlying.c_str(), tenor2 );
		instrument = cString( id );
	}
}


void ListActiveAgricultureInstruments( cArray< cString >& instruments )
{
	struct tm* ct = cSystem::GetTime();
	
	int currYear = ct->tm_year+1900;
	int currMonth = ct->tm_mon + 1;

	int firstMonth, secondMonth;
	cIvector tenors(2);
	cIvector tenors2(2);
	if( currMonth < 5 && currMonth >= 1 )
	{
		firstMonth = 5;
		secondMonth = 9;
		tenors[0] = ( currYear - 2000 ) * 100 + firstMonth;
		tenors[1] = ( currYear - 2000 ) * 100 + secondMonth;
		tenors2[0] = ( currYear - 2010 ) * 100 + firstMonth;
		tenors2[1] = ( currYear - 2010 ) * 100 + secondMonth;
	}
	else if( currMonth < 9 && currMonth >= 5 )
	{
		firstMonth = 9;
		secondMonth = 1;
		tenors[0] = ( currYear - 2000 ) * 100 + firstMonth;
		tenors[1] = ( currYear - 2000 + 1 ) * 100 + secondMonth;
		tenors2[0] = ( currYear - 2010 ) * 100 + firstMonth;
		tenors2[1] = ( currYear - 2010 + 1 ) * 100 + secondMonth;
	}
	else
	{
		firstMonth = 1;
		secondMonth = 5;
		tenors[0] = ( currYear - 2000 + 1 ) * 100 + firstMonth;
		tenors[1] = ( currYear - 2000 + 1 ) * 100 + secondMonth;
		tenors2[0] = ( currYear - 2010 + 1 ) * 100 + firstMonth;
		tenors2[1] = ( currYear - 2010 + 1 ) * 100 + secondMonth;
	}
	
	cArray< cString > underlyings, underlyings2;
	underlyings.push_back( "c" );
	underlyings.push_back( "jd" );
	underlyings.push_back( "a" );
	underlyings.push_back( "m" );
	underlyings.push_back( "y" );
	underlyings.push_back( "p" );

	instruments.resize( 0 );

	for( int i = 0; i < underlyings.getSize(); ++i )
	{
		for( int j = 0; j < tenors.getSize(); ++j )
		{
			char id[256];
			sprintf( id, "%s%4d", underlyings[i].c_str(), tenors[j] );
			instruments.push_back( id );
		}
	}

	underlyings2.push_back( "SR" );
	underlyings2.push_back( "CF" );
	underlyings2.push_back( "OI" );
	underlyings2.push_back( "RM" );

	for( int i = 0; i < underlyings2.getSize(); ++i )
	{
		for( int j = 0; j < tenors2.getSize(); ++j )
		{
			char id[256];
			sprintf( id, "%s%3d", underlyings2[i].c_str(), tenors2[j] );
			instruments.push_back( id );
		}
	}
}

void ListFirstIndustryInstrument( const cString& underlying, cString& instrument )
{
	struct tm* ct = cSystem::GetTime();
	
	int currYear = ct->tm_year+1900;
	int currMonth = ct->tm_mon + 1;

	int firstMonth;
	int tenor, tenor2, tenor3;

	if( currMonth < 5 && currMonth >= 1 )
	{
		firstMonth = 5;
		tenor = ( currYear - 2000 ) * 100 + firstMonth;
		tenor2 = ( currYear - 2010 ) * 100 + firstMonth;
		tenor3 = tenor;
		
	}
	else if( currMonth < 9 && currMonth >= 5 )
	{
		firstMonth = 9;
		tenor = ( currYear - 2000 ) * 100 + firstMonth;
		tenor2 = ( currYear - 2010 ) * 100 + firstMonth;
		tenor3 = ( currYear - 2000 ) * 100 + firstMonth + 1;
		
	}
	else
	{
		firstMonth = 1;
		tenor = ( currYear - 2000 + 1 ) * 100 + firstMonth;
		tenor2 = ( currYear - 2010 + 1 ) * 100 + firstMonth;
		tenor3 = tenor;
	}

	if( Compare( underlying, "j" ) || Compare( underlying, "jm" ) || Compare( underlying, "i" ) || Compare( underlying, "ru" ) )
	{
		char id[256];
		sprintf( id, "%s%4d", underlying.c_str(), tenor );
		instrument = cString( id );
	}
	
	if( Compare( underlying, "rb" ) || Compare( underlying, "hc" ) )
	{
		char id[256];
		sprintf( id, "%s%4d", underlying.c_str(), tenor3 );
		instrument = cString( id );
	}

	if( Compare( underlying, "FG" ) || Compare( underlying, "ZC" ) )
	{
		char id[256];
		sprintf( id, "%s%3d", underlying.c_str(), tenor2 );
		instrument = cString( id );
	}
	
}

void ListActiveIndustryInstruments( cArray< cString >& instruments )
{
	struct tm* ct = cSystem::GetTime();
	
	int currYear = ct->tm_year+1900;
	int currMonth = ct->tm_mon + 1;

	int firstMonth, secondMonth;
	cIvector tenors(2);
	cIvector tenors2(2);
	cIvector tenors3(2);

	if( currMonth < 5 && currMonth >= 1 )
	{
		firstMonth = 5;
		secondMonth = 9;
		tenors[0] = ( currYear - 2000 ) * 100 + firstMonth;
		tenors[1] = ( currYear - 2000 ) * 100 + secondMonth;
		tenors2[0] = ( currYear - 2010 ) * 100 + firstMonth;
		tenors2[1] = ( currYear - 2010 ) * 100 + secondMonth;
		tenors3[0] = tenors[0];
		tenors3[1] = ( currYear - 2000 ) * 100 + secondMonth + 1;
	}
	else if( currMonth < 9 && currMonth >= 5 )
	{
		firstMonth = 9;
		secondMonth = 1;
		tenors[0] = ( currYear - 2000 ) * 100 + firstMonth;
		tenors[1] = ( currYear - 2000 + 1 ) * 100 + secondMonth;
		tenors2[0] = ( currYear - 2010 ) * 100 + firstMonth;
		tenors2[1] = ( currYear - 2010 + 1 ) * 100 + secondMonth;
		tenors3[0] = ( currYear - 2000 ) * 100 + firstMonth + 1;
		tenors3[1] = tenors[1];
	}
	else
	{
		firstMonth = 1;
		secondMonth = 5;
		tenors[0] = ( currYear - 2000 + 1 ) * 100 + firstMonth;
		tenors[1] = ( currYear - 2000 + 1 ) * 100 + secondMonth;
		tenors2[0] = ( currYear - 2010 + 1 ) * 100 + firstMonth;
		tenors2[1] = ( currYear - 2010 + 1 ) * 100 + secondMonth;
		tenors3[0] = tenors[0];
		tenors3[1] = tenors[1];
	}
	
	cArray< cString > underlyings, underlyings2, underlyings3;
	underlyings.push_back( "j" );
	underlyings.push_back( "jm" );
	underlyings.push_back( "i" );
	underlyings.push_back( "ru" );

	instruments.resize( 0 );

	for( int i = 0; i < underlyings.getSize(); ++i )
	{
		for( int j = 0; j < tenors.getSize(); ++j )
		{
			char id[256];
			sprintf( id, "%s%4d", underlyings[i].c_str(), tenors[j] );
			instruments.push_back( id );
		}
	}

	underlyings3.push_back( "rb" );
	underlyings3.push_back( "hc" );
	for( int i = 0; i < underlyings3.getSize(); ++i )
	{
		for( int j = 0; j < tenors3.getSize(); ++j )
		{
			char id[256];
			sprintf( id, "%s%4d", underlyings3[i].c_str(), tenors3[j] );
			instruments.push_back( id );
		}
	}

	underlyings2.push_back( "FG" );
	underlyings2.push_back( "ZC" );

	for( int i = 0; i < underlyings2.getSize(); ++i )
	{
		for( int j = 0; j < tenors2.getSize(); ++j )
		{
			char id[256];
			sprintf( id, "%s%3d", underlyings2[i].c_str(), tenors2[j] );
			instruments.push_back( id );
		}
	}


}

void InstrumentToUnderlying( const cString& instrument, cString& underlying )
{
	// remove the tenor information
	// i.e. IF1512 will return IF only
	
	int i;
	int size = instrument.size();
	for( i = 0; i < size; ++i )
	{
		cString copyStr( instrument );
		copyStr.SubString( i, size-1 );
		if( copyStr.IsInteger() )
			break;
	}
	underlying = instrument;
	underlying.Left(i);
}

double InstrumentContractSize( const cString& instrument )
{
	cString underlying;
	InstrumentToUnderlying( instrument, underlying );
	return GetContractSize( underlying );
}

double InstrumentTickSize( const cString& instrument )
{
	cString underlying;
	InstrumentToUnderlying( instrument, underlying );
	return GetTickSize( underlying );
}

double InstrumentCommission( const cString& instrument, int offset )
{
	return 0.0;
	//todo:
	/*yr_error( "not implemented yet" );*/
}

double InstrumentMargin( const cString& instrument )
{
	return 0.08;
	//todo:
	/*yr_error( "not implemented yet" );*/
}

