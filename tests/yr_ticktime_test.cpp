#include <yr_ticktime_test.h>
#include <cTickTime.h>
#include <cString.h>
#include <cSystem.h>
#include <cVector.h>

void RunTickTimeTest()
{
	char runTicktimeTest[256];
	printf("run tick time class test (Y/N)?:");
	cin >> runTicktimeTest;
	while( !( Compare( runTicktimeTest, "Y" ) || Compare( runTicktimeTest, "N" ) ) )
	{
		printf( "invalid input, either 'Y' or 'N' is allowed. please re-enter:" );
		cin >> runTicktimeTest;
	}
	
	if( Compare( runTicktimeTest, "Y" ) )
	{
		TickTimeTestDoubleToTickTime();
		cSystem::Sleep(3000);
		TickTimeTestDoubleToTickTimeWithMilliSecond();
		cSystem::Sleep(3000);
		TickTimeTestDoubleToTickTime();
		cSystem::Sleep(3000);
	}
}

void TickTimeTestDoubleToTickTime()
{
	cout << "running TickTimeTestDoubleToTickTime" << endl;
	int hh_start = 0;
	int hh_end = 24;
	int mm_start = 0;
	int mm_end = 60;
	int ss_start = 0;
	int ss_end = 60;

	int i,j,k;
	int hh, mm, ss;
	bool flag = true;
	for( i = hh_start; i < hh_end; ++i )
	{
		hh = i;
		for( j = mm_start; j < mm_end; ++j )
		{
			mm = j;
			for( k = ss_start; k < ss_end; ++k )
			{
				ss = k;
				cString timestr = cTickTime::ToTimeString( hh, mm, ss );
				cTickTime tt( "20160222", timestr.c_str() );
				// the double is what we stored for reason of speed and memory
				double t = tt.TickTimeToDouble();
				cTickTime ttCheck = cTickTime::DoubleToTickTime( t );
				if( tt != ttCheck )
				{
					cout << "orignal tick time is: " << tt.TickTimeFullString().c_str()<<" ";
					cout << "converted tick time is: " << ttCheck.TickTimeFullString().c_str()<< endl;
					flag = false;
					cSystem::Sleep(5);
				}
			}
		}
	}
	if( flag )
		cout << "no error found running TickTimeTestDoubleToTickTime!" << endl;
}


void TickTimeTestDoubleToTickTimeWithMilliSecond()
{
	cout << "running TickTimeTestDoubleToTickTimeWithMilliSecond" << endl;
	int hh_start = 0;
	int hh_end = 24;
	int mm_start = 0;
	int mm_end = 60;
	int ss_start = 0;
	int ss_end = 60;

	int i,j,k;
	int hh, mm, ss;
	bool flag = true;
	for( i = hh_start; i < hh_end; ++i )
	{
		hh = i;
		for( j = mm_start; j < mm_end; ++j )
		{
			mm = j;
			for( k = ss_start; k < ss_end; ++k )
			{
				ss = k;
				cString timestr = cTickTime::ToTimeString( hh, mm, ss );
				cTickTime ttFirst( "20160222", timestr.c_str(), 0 );
				cTickTime ttSecond( "20160222", timestr.c_str(), 500 );
				// the double is what we stored for reason of speed and memory
				double tFirst = ttFirst.TickTimeToDouble();
				double tSecond = ttSecond.TickTimeToDouble();
				cTickTime ttCheckFirst = cTickTime::DoubleToTickTime( tFirst );
				cTickTime ttCheckSecond = cTickTime::DoubleToTickTime( tSecond );

				if( ttFirst != ttCheckFirst )
				{
					cout << "orignal first tick time is: " << ttFirst.TickTimeFullString().c_str() << " ";
					cout << "converted first tick time is: " << ttCheckFirst.TickTimeFullString().c_str()<< endl;
					flag = false;
					cSystem::Sleep(5);
				}
				if( ttSecond != ttCheckSecond )
				{
					cout << "orignal second time is: " << ttSecond.TickTimeFullString().c_str() << " ";
					cout << "converted second time is: " << ttCheckSecond.TickTimeFullString().c_str() << endl;
					flag = false;
					cSystem::Sleep(5);
				}
			}
		}
	}
	if( flag )
		cout << "no error found running TickTimeTestDoubleToTickTimeWithMilliSecond!" << endl;
}


void TickTimeTestVectorLocation()
{
	cout << "running TickTimeTestVectorLocation" << endl;
	int hh_start = 0;
	int hh_end = 24;
	int mm_start = 0;
	int mm_end = 60;
	int ss_start = 0;
	int ss_end = 60;

	int i,j,k;
	int hh, mm, ss;
	bool flag = true;
	cVector tickTimeVec;
	// push the tickTime into tickTimeVec;
	cDate date( "20160222" );

	for( i = hh_start; i < hh_end; ++i )
	{
		hh = i;
		for( j = mm_start; j < mm_end; ++j )
		{
			mm = j;
			for( k = ss_start; k < ss_end; ++k )
			{
				ss = k;
				double tFirst = double( hh * 3600 + mm * 60 + ss ) / SECONDSPERDAY;
				double tSecond = double( hh * 3600 + mm * 60 + ss + 0.5 ) / SECONDSPERDAY;
				tFirst += date.JulianDate();
				tSecond += date.JulianDate();
				tickTimeVec.push_back( tFirst );
				tickTimeVec.push_back( tSecond );
			}
		}
	}

	// now check whether the index matches
	for( i = hh_start; i < hh_end; ++i )
	{
		hh = i;
		for( j = mm_start; j < mm_end; ++j )
		{
			mm = j;
			for( k = ss_start; k < ss_end; ++k )
			{
				ss = k;
				cString timestr = cTickTime::ToTimeString( hh, mm, ss );
				cTickTime ttFirst( "20160222", timestr.c_str(), 0 );
				cTickTime ttSecond( "20160222", timestr.c_str(), 500 );
				// the double is what we stored for reason of speed and memory
				double tFirst = ttFirst.TickTimeToDouble();
				int idxFirst = tickTimeVec.find( tFirst, FUZZ );
				if( idxFirst < 0 )
				{
					cout << ttFirst.TickTimeFullString().c_str() << "not found!" << endl;
					flag = false;
				}
				else
				{
					if( !equal( tFirst, tickTimeVec[idxFirst] ) )
					{
						cout << ttFirst.TickTimeFullString().c_str() << "index mismatched!" << endl;
						flag = false;
					}
				}

				double tSecond = ttSecond.TickTimeToDouble();
				int idxSecond = tickTimeVec.find( tSecond, FUZZ );
				if( idxSecond < 0 )
				{
					cout << ttSecond.TickTimeFullString().c_str() << "not found!" << endl;
					flag = false;
				}
				else
				{
					if( !equal( tSecond, tickTimeVec[idxSecond] ) )
					{
						cout << ttSecond.TickTimeFullString().c_str() << "index mismatched!" << endl;
						flag = false;
					}
				}	
			}
		}
	}
	if( flag )
	cout << "no error found running TickTimeTestVectorLocation!" << endl;
}