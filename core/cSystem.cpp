#include <cSystem.h>
#include <cErr.h>
#include <yr_assert.h>
#include <stl_iostream.h>
#include <Windows.h>
#include <direct.h>
#include <cString.h>
#include <cTickTime.h>

#ifndef _tzset
#define _tzset tzset
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

void cSystem::Sleep( size_t delay )
{
#ifdef WIN32
	::_sleep( (DWORD)delay );
#else
	struct timeval tv;
	tv.tv_sec = delay / 1000;
	tv.tv_usec = ( delay - tv.tv_sec * 1000 ) * 1000;
	select( 0, NULL, NULL, NULL, &tv );
#endif
}

struct tm* cSystem::GetTime()
{
	time_t now;
	struct tm* timeinfo;

	time(&now);
	timeinfo = localtime(&now);

	return timeinfo;
}

string cSystem::GetCurrentTimeBuffer()
{
	char time_buf[256];
	time_t now;
	struct tm * timeinfo;

	time(&now);
	timeinfo = localtime(&now);

	sprintf(time_buf, "%04d%02d%02d %02d:%02d:%02d", timeinfo->tm_year+1900, timeinfo->tm_mon+1, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec );

	return string( time_buf );
}

int cSystem::HourNow()
{
	struct tm* timeinfo = GetTime();
	return timeinfo->tm_hour;
}

int cSystem::MinuteNow()
{
	struct tm* timeinfo = GetTime();
	return timeinfo->tm_min;
}

int cSystem::SecondNow()
{
	struct tm* timeinfo = GetTime();
	return timeinfo->tm_sec;
}

string cSystem::GetTimeHMS()
{
	char time_buf[256];
	time_t now;
	struct tm* timeinfo;

	time(&now);
	timeinfo = localtime(&now);

	sprintf( time_buf, "02d%02d%02d%", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec );

	return string( time_buf );
}

string cSystem::GetEnv( const string& key, const string& def )
{
	YR_ASSERT( !key.empty(), "System::GetEnv is passed empty key" );

	char* value = ::getenv( key.c_str() );
	return value ? string( value ) : string( def );
}

void cSystem::PutEnv( const string& key, const string& value )
{
	YR_ASSERT( !key.empty(), "System::GetEnv is passed empty key" );

	string str = key + "=" + value;
	::putenv( strdup( str.c_str() ) );
}

int cSystem::GetGregorianDateToday()
{
	_tzset();
	time_t ltime;
	time( &ltime );
#ifdef WIN32
	struct tm* today = localtime(&ltime);
#else
	struct tm t;
	struct tm* today = localtime_r(&ltime, &t);
#endif

	int longToday = ( 1900 + today->tm_year ) * 10000 + ( 1 + today->tm_mon ) * 100 + today->tm_mday;
	return longToday;

}

string cSystem::TimeNow()
{
	_tzset();
	time_t ltime;
	time( &ltime );
#ifdef WIN32
	struct tm* today = localtime( &ltime );
#else
	struct tm t;
	struct tm* today = localtime_r( &ltime, &t );
#endif

	char tmpbuf[128];
	strftime( tmpbuf, sizeof( tmpbuf ), "%Y%m%d %H%M%S", today );
	return tmpbuf;
}

bool cSystem::HasDir( const char* dir )
{
	int path = chdir( dir );
	if( path < 0 )
		return false;
	else
		return true;
}

void cSystem::MakeDir( const char* dir )
{
	mkdir( dir );
}

bool cSystem::HasTxtFile( const char* fileName )
{
	cString fileNameStr( fileName );
	int l = fileNameStr.InString( ".txt" );
	if( l < 0 )
		fileNameStr += ".txt";
	ifstream fileRead;
	fileRead.open( fileNameStr.c_str() );

	if( fileRead )
		return true;
	else
		return false;
}


void cSystem::CreateTxtFile( const char* fileName, bool replace )
{
	cString fileNameStr( fileName );
	int l = fileNameStr.InString( ".txt" );
	if( l < 0 )
		fileNameStr += ".txt";

	ifstream fileRead;
	fileRead.open( fileNameStr.c_str() );

	if( fileRead && replace )
	{
		// file named fileName exits and is going to be replaced
		ofstream fileWrite;
		fileWrite.open( fileNameStr.c_str(), ios::trunc );
		fileWrite.close();
	}

	if( !fileRead )
	{
		// file named fileName not existed
		ofstream fileWrite;
		fileWrite.open( fileNameStr.c_str() );
	}

	fileRead.close();

}

string cSystem::LogFile()
{
	// log file name for each trading day
	// since some futures trade after midnight, e.g. ag, till 2:30 am
	// the file name updated after 8:00am
	struct tm* currentTime = GetTime();
	int hh = currentTime->tm_hour;
	
	char file[256];
	if( hh >= 8 && hh <= 23 )
		sprintf( file, "log_%d.txt", cSystem::GetGregorianDateToday() );
	else
	{
		// return the previous date
		int yy = currentTime->tm_year;
		int mm = currentTime->tm_mon;	// [0,11]
		int dd = currentTime->tm_mday;	// [1,31];
		int yy_new, mm_new, dd_new;
		if( mm == 0 && dd == 1 )
		{
			yy_new = yy - 1;
			mm_new = 11;
			dd_new = 31;
		}
		else if( mm > 0 && dd == 1 )
		{
			yy_new = yy;
			mm_new = mm - 1;
			dd_new;
			if( mm_new == 0 || mm_new == 2 || mm_new == 4 || mm_new == 6 || mm_new == 7 || mm_new == 9 || mm_new == 11 )
				dd_new = 31;
			else if( mm_new == 1 )
			{
				if( yy_new % 4 == 0 )
					dd_new = 29;
				else
					dd_new = 28;
			}
			else
				dd_new = 30;
		}
		else
		{
			yy_new = yy;
			mm_new = mm;
			dd_new = dd - 1;
		}
		sprintf( file, "log_%d.txt",  ( 1900 + yy_new ) * 10000 + ( 1 + mm_new ) * 100 + dd_new );
	}

	return string( file );
}

void cSystem::CreateLogFile()
{
	// in case the trading system runs uninterruptiblly
	// a log file is automatically created on 08:00 am
	struct tm* timeinfo = GetTime();
	char time_buf[256];
	sprintf(time_buf, "%02d:%02d:%02d", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec );

	if( Compare( time_buf, "08:00:00" ) )
	{
		cString fileFullPath( cSystem::GetEnv( "MYDATADIR" ).c_str() );
		fileFullPath += "log\\";
		fileFullPath = fileFullPath + LogFile().c_str();
		CreateTxtFile( fileFullPath.c_str() );
	}

}

void cSystem::CreateLogFile( const char* fullDir )
{
	CreateTxtFile( fullDir );
}

void cSystem::WriteLogFile( const char* fileFolder, const char* fileName, const char* message, bool init )
{
	if( !HasDir( fileFolder ) )
		MakeDir( fileFolder );

	string fileFullPath = string( fileFolder ) + fileName;

	if( init )
		CreateTxtFile( fileFullPath.c_str() );
	
	ofstream fileWrite;
	fileWrite.open( fileFullPath.c_str(), ios::app );

	if( fileWrite )
		fileWrite << message << endl;
	else
		yr_error( "file s% cannot be open!", fileFullPath.c_str() );
	
	fileWrite.close();
}

void cSystem::WriteLogFile( const char* fileName, const char* message, bool init )
{
	string logFolder = cSystem::GetEnv( "MYDATADIR" ) + "log\\";
	
	WriteLogFile( logFolder.c_str(), fileName, message, init );

}


bool cSystem::DoWriteDataToFile()
{
	struct tm* timeinfo = GetTime();
	char time_buf[256];
	sprintf(time_buf, "%02d:%02d:%02d", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec );

	// we do the write data 5 mins afer each trading session
	if( Compare( time_buf, "02:35:00" ) || Compare( time_buf, "11:35:00" ) || Compare( time_buf, "15:20:00" ) )
		return true;
	else
		return false;
}

bool cSystem::DoWriteDataToFile( double t )
{
	cTickTime tt = cTickTime::DoubleToTickTime( t );
	cString time_buf = tt.TimeString();
	if( Compare( time_buf, "02:35:00" ) || Compare( time_buf, "11:35:00" ) || Compare( time_buf, "15:20:00" ) )
		return true;
	else
		return false;
}

bool cSystem::DoRest()
{
	// the system sleeps between 02:45 am and 08:55am
	struct tm* timeinfo = GetTime();
	int hh = timeinfo->tm_hour;
	int mm = timeinfo->tm_min;
	if( hh == 2 && mm >= 45 )
		return true;
	if( hh > 2 && hh < 8 )
		return true;
	if( hh == 8 && mm <= 55 )
		return true;
	return false;
}

bool cSystem::DoWriteDataToFile( const string& in )
{
	return DoWriteDataToFile( in.c_str() );
}

bool cSystem::DoWriteDataToFile( const char* input )
{
	// input time is formatted as HH:MM:SS
	// e.g. 09:15:00
	struct tm* timeinfo = GetTime();
	char time_buf[256];
	sprintf(time_buf, "%02d:%02d:%02d", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec );
	if( Compare( time_buf, input ) )
		return true;
	else
		return false;


}


void cSystem::FirstTimeInit()
{
	string dataFolderDir = cSystem::GetEnv( "MYDATADIR" );
	
	if( !HasDir( dataFolderDir.c_str() ) )
		MakeDir( dataFolderDir.c_str() );

	string tickdataPath = dataFolderDir + "historicaldata\\tick\\";
	if( !HasDir( tickdataPath.c_str() ) )
		MakeDir( tickdataPath.c_str() );

	string candledataPath = dataFolderDir + "historicaldata\\candle\\";
	if( !HasDir( candledataPath.c_str() ) )
		MakeDir( candledataPath.c_str() );

	string logPath = dataFolderDir + "log\\";
	if( !HasDir( logPath.c_str() ) )
		MakeDir( logPath.c_str() );

}

void cSystem::FirstTimeInit( const char* dir, const char* tickFolderName, const char* candleFolderName, const char* logFolderName )
{
	if( !HasDir( dir ) )
		MakeDir( dir );

	string dataPath = string( dir );
	
	string tickdataPath = dataPath + tickFolderName + "\\";
	if( !HasDir( tickdataPath.c_str() ) )
		MakeDir( tickdataPath.c_str() );

	string candledataPath = dataPath + candleFolderName + "\\";
	if( !HasDir( candledataPath.c_str() ) )
		MakeDir( candledataPath.c_str() );

	string logPath = string( dir ) + logFolderName + "\\";
	if( !HasDir( logPath.c_str() ) )
		MakeDir( logPath.c_str() );
}