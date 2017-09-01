#include <yr_utilities_test.h>
#include <yr_loaddata.h>
#include <cString.h>
#include <cStringUtils.h>
#include <cVector.h>
#include <cTick.h>
#include <cSystem.h>


void RunUtilityTest()
{
	RunUtilityTest_LoadTickDataFromFile();
	RunUtilityTest_LoadTickDataFromFiles();
}

void RunUtilityTest_LoadTickDataFromFile()
{
	char flag[256];
	printf("run utility test -> LoadTickDataFromFile (Y/N)?:");
	cin >> flag;
	while( !( Compare( flag, "Y" ) || Compare( flag, "N" ) ) )
	{
		printf( "invalid input, either 'Y' or 'N' is allowed. please re-enter:" );
		cin >> flag;
	}
	
	if( Compare( flag, "N" ) )
		return;
	
	cString fileName = cString( TESTFILENAME );
	cArray< cString > res;
	cString::Split( res, fileName, "_" );
	cString instrumentStr = res[0];

	int i;
	int size = instrumentStr.size();
	for( i = 0; i < size; ++i )
	{
		cString copyStr( instrumentStr );
		copyStr.SubString( i, size-1 );
		if( copyStr.IsInteger() )
			break;
	}
	cString underlyingStr( instrumentStr );
	underlyingStr.Left(i);

	cString fileFullDir = cString( cSystem::GetEnv( "MYDATADIR" ).c_str() ) + "tick\\" + underlyingStr + "\\" + fileName;

	cArray< cTick > ticks;
	cArray< cTickTime> tickTimes;

	LoadTickDataFromFile( fileFullDir, instrumentStr, ticks, tickTimes );

	ticks.clear();
	tickTimes.clear();

	fileFullDir = "C:\\yangyiran\\autotrade_testdata\\tick\\A\\a1605_20160324.txt";
	LoadTickDataFromFile( fileFullDir, instrumentStr, ticks, tickTimes, true );

	printf( "LoadTickDataFromFile test done!\n" );
	printf( "\n" );

}

void RunUtilityTest_LoadTickDataFromFiles()
{

	cString fileFullDir = "C:\\yangyiran\\autotrade_testdata\\tick\\A\\";
	
	cArray< cString > fileNames;

	ListFileNamesInDirectory( fileFullDir + "*.txt", fileNames );

	cArray< cTick > ticks;
	cArray< cTickTime> tickTimes;

	cArray< cString > res;
	cString::Split( res, fileNames[0], "_" );
	cString instrumentStr = res[0];

	for( int i = 0; i < fileNames.getSize(); ++i )
		fileNames[i] = fileFullDir + fileNames[i];

	LoadTickDataFromFiles( fileNames, instrumentStr, ticks, tickTimes, true );

	printf( "LoadTickDataFromFiles test done!\n" );
	printf( "\n" );
}


