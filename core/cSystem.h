#ifndef __CSYSTEM_H__
#define __CSYSTEM_H__

#include <stl.h>
#include <time.h>
#include <Windows.h>

class cSystem
{
public:
	// Date and Time
	static void Sleep( size_t delay );		// delay in milliseconds
	static struct tm* GetTime();
	static string GetCurrentTimeBuffer();
	static string GetTimeHMS();
	static int GetGregorianDateToday();
	static string TimeNow();
	static int HourNow();
	static int MinuteNow();
	static int SecondNow();
	
	// write data to local directory
	static bool DoWriteDataToFile();
	static bool DoWriteDataToFile( double t );
	static bool DoWriteDataToFile( const string&  );
	static bool DoWriteDataToFile( const char* );
	static bool DoRest();

	static string GetEnv( const string& key, const string& def = "" );
	static void PutEnv( const string& key, const string& value );

	
	// I/O
	static bool HasDir( const char* dir );
	static void MakeDir( const char* dir );
	static bool HasTxtFile( const char* fileName );
	static void CreateTxtFile( const char* fileName, bool replace = true );

	// log
	static string LogFile();
	static void CreateLogFile();
	static void CreateLogFile( const char* fullDir );
	static void WriteLogFile( const char* fileFolder, const char* fileName, const char* message, bool init = true );
	static void WriteLogFile( const char* fileName, const char* message, bool init = true );

	// run time 
	static void FirstTimeInit();
	static void FirstTimeInit( const char* dir, const char* tickFolderName, const char* candleFolderName, const char* logFolderName );


};


#endif
