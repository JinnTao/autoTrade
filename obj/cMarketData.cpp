// cMarketData.cpp

#include <time.h>
#include <cMarketData.h>
#include <cStringUtils.h>
#include <cTickTime.h>
#include <cSystem.h>
#include <stl_ctp.h>


cMarketData::cMarketData()
:m_id( theString )
,m_displayTick( false )
,m_displayCandle( false )
,m_outputDirectory( theString )
,m_tickDataFolder( theString )
,m_candleDataFolder( theString )
{
	m_weights.resize( 1,1.0 );
}

cMarketData::cMarketData( const cString& id, const cString& freq )
:m_id( id )
,m_displayTick( false )
,m_displayCandle( false )
,m_outputDirectory( theString )
,m_tickDataFolder( theString )
,m_candleDataFolder( theString )
{
	m_weights.resize( 1,1.0 );
	genIntradayBucket( m_timeBucket, m_tradingStart, m_tradingEnd, freq );
	m_timeBucketStartFlag.resize( m_timeBucket.getSize()-1, 0);
	m_timeBucketEndFlag.resize( m_timeBucket.getSize()-1, 0);
	genBreakIdxVec();

	if( m_ticks.getSize() )
		m_ticks.clear();
	
	if( m_candles.getSize() )
		m_candles.clear();
}


cMarketData::cMarketData( const cMarketData& in )
{
	yr_assert( this != & in );
	m_id = in.m_id;
	m_displayTick = in.m_displayTick;
	m_displayCandle = in.m_displayCandle;
	m_outputDirectory = in.m_outputDirectory;
	m_tickDataFolder = in.m_tickDataFolder;
	m_candleDataFolder = in.m_candleDataFolder;
	m_weights = in.m_weights;
	m_timeBucket = in.m_timeBucket;
	m_tradingStart = in.m_tradingStart;
	m_tradingEnd = in.m_tradingEnd;
	m_timeBucketStartFlag = in.m_timeBucketStartFlag;
	m_timeBucketEndFlag = in.m_timeBucketEndFlag;
	m_breakIdxVec = in.m_breakIdxVec;

	if( m_ticks.getSize() )
		m_ticks.clear();
	for( int i = 0; i < in.m_ticks.getSize(); ++i )
		m_ticks.push_back( in.m_ticks[i] );
	
	if( m_candles.getSize() )
		m_candles.clear();
	for( int i = 0; i < in.m_candles.getSize(); ++i )
		m_candles.push_back( in.m_candles[i] );

}

cMarketData& cMarketData::operator = ( const cMarketData& in )
{
	if( &in != this )
	{
		m_id = in.m_id;
		m_displayTick = in.m_displayTick;
		m_displayCandle = in.m_displayCandle;
		m_outputDirectory = in.m_outputDirectory;
		m_tickDataFolder = in.m_tickDataFolder;
		m_candleDataFolder = in.m_candleDataFolder;
		m_weights = in.m_weights;
		m_timeBucket = in.m_timeBucket;
		m_tradingStart = in.m_tradingStart;
		m_tradingEnd = in.m_tradingEnd;
		m_timeBucketStartFlag = in.m_timeBucketStartFlag;
		m_timeBucketEndFlag = in.m_timeBucketEndFlag;
		m_breakIdxVec = in.m_breakIdxVec;

		if( m_ticks.getSize() )
			m_ticks.clear();
		for( int i = 0; i < in.m_ticks.getSize(); ++i )
			m_ticks.push_back( in.m_ticks[i] );

		if( m_candles.getSize() )
			m_candles.clear();
		for( int i = 0; i < in.m_candles.getSize(); ++i )
			m_candles.push_back( in.m_candles[i] );

	}
	return *this;
}

cMarketData::~cMarketData()
{
	ClearAllData();
}

SwUnderlying cMarketData::GetUnderlying() const
{
	return StringToUnderlying( GetUnderlyingStr() );
}

cString cMarketData::GetUnderlyingStr() const
{
		// remove the tenor information
	// i.e. IF1512 will return IF only
	
	// 1. need to check whether the underlying is a single asset or 1) combine of the contracts with the same underlying or not
	int l = m_id.InString( "_" );
	if( l >= 0 )
	{
		cArray< cString > ids;
		m_id.Split( ids, m_id, "_" );
		cArray< cString > assets( ids.size() );
		for( int i = 0; i < ids.size(); ++i )
		{
			int size = ids[i].size();
			int j;
			for( j = 0; j < size; ++j )
			{
				cString copyid( ids[i] );
				copyid.SubString( j, size-1 );
				if( copyid.IsInteger() )
					break;
			}
			assets[i] = ids[i];
			assets[i].Left(j);
		}
		bool sameAsset = true;
		for( int i = 0; i < assets.size(); ++i )
		{
			for( int j = i+1; j < assets.size(); ++j )
			{
				if( !Compare( assets[i], assets[j] ) )
				{
					sameAsset = false;
					break;
				}
			}
		}
		if( !sameAsset )
			yr_error( "different underlying asset not supported" );
		return assets[0];
	}

	int i;
	int size = m_id.size();
	for( i = 0; i < size; ++i )
	{
		cString copyStr( m_id );
		copyStr.SubString( i, size-1 );
		if( copyStr.IsInteger() )
			break;
	}
	cString underlyingStr(m_id);
	underlyingStr.Left(i);
	return underlyingStr;
}

void cMarketData::GetCandleStick( cVector& time, cVector& open, cVector& high, cVector& low, cVector& close ) const
{
	if( !m_candles.getSize() )
		return;
	for( int i = 0; i < m_candles.getSize(); ++i )
	{
		double dOpen = m_candles[i].Open();
		double dHigh = m_candles[i].High();
		double dLow = m_candles[i].Low();
		double dClose = m_candles[i].Close();
		//
		open.push_back( dOpen );
		high.push_back( dHigh );
		low.push_back( dLow );
		close.push_back( dClose );
		//
		cString dateStr, timeStr;
		m_candles[i].DateStart( dateStr );
		m_candles[i].TimeStart( timeStr );
		cTickTime tickTime( dateStr, timeStr );
		time.push_back( tickTime.TickTimeToDouble() );
	}
}

const cArray< cCandle >* cMarketData::GetCandleStick() const
{
	return &m_candles;
}

void cMarketData::SimulationRtnLastCandle()
{
	double tsec = m_simTime - (int)m_simTime;
	double sec = tsec * SECONDSPERDAY;

	bool runFlag = false;
	for( int i = 0; i < m_tradingEnd.getSize(); ++i )
	{
		if( sec - m_tradingEnd[i] >= SECONDSPERMINUTE && m_tradingEnd[i] != SECONDSPERDAY )
		{
			runFlag = true;
			break;
		}
	}

	if( !runFlag )
		return;
	
	if( !pRealTimeCandle.get() )
		return;
	
	// we skip this in case the last candle stick is set
	bool flag = pRealTimeCandle.get() ? pRealTimeCandle->IsSet() : true;
	if( flag )
		return;

	int secStart, secEnd;
	secEnd = pRealTimeCandle.get() ? pRealTimeCandle->TimeEnd() : -1;
	if( secEnd < 0 )
		return;

	bool lastBucketFlag = false;
	for( int i = 0; i < m_tradingEnd.getSize(); ++i )
	{
		if( secEnd == m_tradingEnd[i] )
		{
			lastBucketFlag = true;
			break;
		}
	}
	if( !lastBucketFlag )
		return;

	secStart = pRealTimeCandle.get() ? pRealTimeCandle->TimeStart() : -1;
	if( secStart < 0 )
		return;

	int idx = m_timeBucket.location( secStart );
	if( idx < 0 )
		return; 
	if( m_timeBucketStartFlag[idx] == 0 )		// NOT start to pop
		return;
	if( m_timeBucketEndFlag[idx] == 1 )			// poped
		return;
	if( idx == m_timeBucketStartFlag.getSize() - 1 && m_tradingStart[0] == 0 && m_tradingEnd[m_tradingEnd.getSize()-1] == SECONDSPERDAY )
		return;

	if( sec > secEnd + SECONDSPERMINUTE )
	{
		m_timeBucketEndFlag[idx] = 1;
		if( pRealTimeCandle.get() )
			pRealTimeCandle->Set();

		if( m_displayCandle && pRealTimeCandle->IsSet() )
		{
			cString dateStr, timeStr, candleInfo;
			pRealTimeCandle->DateStart( dateStr );
			pRealTimeCandle->TimeStart( timeStr );
			pRealTimeCandle->Print( candleInfo );
			cout << dateStr.c_str() << " " << timeStr.c_str() << ":" << m_id.c_str() << " >>> " << candleInfo.c_str() << endl;	
		}
		if( pRealTimeCandle.get() && pRealTimeCandle->IsSet() )
			m_candles.push_back( *pRealTimeCandle );

		pRealTimeCandle.reset();
	}
}


void cMarketData::RtnLastCandle()
{
	struct tm* localtime = cSystem::GetTime();
	int currSec = SECONDSPERHOUR * localtime->tm_hour + SECONDSPERMINUTE * localtime->tm_min + localtime->tm_sec;
	
	bool runFlag = false;
	for( int i = 0; i < m_tradingEnd.getSize(); ++i )
	{
		if( currSec - m_tradingEnd[i] == SECONDSPERMINUTE && m_tradingEnd[i] != SECONDSPERDAY )
		{
			runFlag = true;
			break;
		}
	}

	if( !runFlag )
		return;
	
	if( !pRealTimeCandle.get() )
		return;

	// we skip this in case the last candle stick is set
	bool flag = pRealTimeCandle.get() ? pRealTimeCandle->IsSet() : true;
	if( flag )
		return;

	int secStart, secEnd;
	secEnd = pRealTimeCandle.get() ? pRealTimeCandle->TimeEnd() : -1;
	if( secEnd < 0 )
		return;

	bool lastBucketFlag = false;
	for( int i = 0; i < m_tradingEnd.getSize(); ++i )
	{
		if( secEnd == m_tradingEnd[i] )
		{
			lastBucketFlag = true;
			break;
		}
	}
	if( !lastBucketFlag )
		return;

	secStart = pRealTimeCandle.get() ? pRealTimeCandle->TimeStart() : -1;
	if( secStart < 0 )
		return;

	int idx = m_timeBucket.location( secStart );
	if( idx < 0 )
		return; 
	if( m_timeBucketStartFlag[idx] == 0 )		// NOT start to pop
		return;
	if( m_timeBucketEndFlag[idx] == 1 )			// poped
		return;
	// move over 00:00
	if( idx == m_timeBucketStartFlag.getSize() - 1 && m_tradingStart[0] == 0 && m_tradingEnd[m_tradingEnd.getSize()-1] == SECONDSPERDAY )
		return;

	
	if( currSec >= secEnd + SECONDSPERMINUTE )
	{
		m_timeBucketEndFlag[idx] = 1;
		if( pRealTimeCandle.get() )
			pRealTimeCandle->Set();

		if( m_displayCandle && pRealTimeCandle->IsSet() )
		{
			cString dateStr, timeStr, candleInfo;
			pRealTimeCandle->DateStart( dateStr );
			pRealTimeCandle->TimeStart( timeStr );
			pRealTimeCandle->Print( candleInfo );
			cout << dateStr.c_str() << " " << timeStr.c_str() << ":" << m_id.c_str() << " >>> " << candleInfo.c_str() << endl;	
		}
		if( pRealTimeCandle.get() && pRealTimeCandle->IsSet() )
			m_candles.push_back( *pRealTimeCandle );

		pRealTimeCandle.reset();
	}
}


bool cMarketData::DoRealTimeUpdate( const cTickTime& updateTime ) const
{
	if( updateTime.IsHoliday() )
		return false;

	int t_mm = SECONDSPERHOUR * updateTime.Hour() + SECONDSPERMINUTE * updateTime.Minute() + updateTime.Second();
	
	if( t_mm == 0 || t_mm == SECONDSPERDAY )
		return true;

	if( !m_tradingStart.getSize() || !m_tradingEnd.getSize() )
	{
		yr_error( "Internal error" );
	}
	bool flag = false;
	for( int i = 0; i < m_tradingStart.getSize(); ++i )
	{
		if( t_mm > m_tradingStart[i] && t_mm < m_tradingEnd[i] )
		{
			// there are Tick before or on tradingStart
			// also there are Tick on or after tradingEnd
			// we shall exclude these ticks
			flag = true;
			break;
		}
	}
	return flag;
}

void cMarketData::OnRtnDepthMarketData( CThostFtdcDepthMarketDataField* pDepthMarketData )
{
	yr_assert( Compare( m_id, pDepthMarketData->InstrumentID ) );
	yr_assert( m_weights.getSize() == 1 );

	cTickTime tickTime;
	cString updateTimeStr( pDepthMarketData->UpdateTime );
	int hh = atoi( cString::Left( updateTimeStr, 2 ).c_str() );
	if( hh > 20 && hh < 24 )
	{
		cString localDate = cDate::GeorgianDateToString( cSystem::GetGregorianDateToday() );
		int hh_local = cSystem::HourNow();
		if( hh_local < 21 )
			return;

		tickTime = cTickTime( localDate.c_str(), pDepthMarketData->UpdateTime, pDepthMarketData->UpdateMillisec );
	}
	else if( hh >= 0 && hh < 3 )
	{
		tickTime = cTickTime( pDepthMarketData->TradingDay, pDepthMarketData->UpdateTime, pDepthMarketData->UpdateMillisec );
	}
	else
	{
		tickTime = cTickTime( pDepthMarketData->TradingDay, pDepthMarketData->UpdateTime, pDepthMarketData->UpdateMillisec );
	} 

	if( !DoRealTimeUpdate( tickTime ) )
		return;

	// update tick array
	cTick tick( m_id, tickTime );
	tick.SetBid( pDepthMarketData->BidPrice1 );
	tick.SetAsk( pDepthMarketData->AskPrice1 );
	tick.SetLast( pDepthMarketData->LastPrice );
	tick.SetBidVolume( pDepthMarketData->BidVolume1 );
	tick.SetAskVolume( pDepthMarketData->AskVolume1 );
	tick.SetVolume( pDepthMarketData->Volume );
	tick.SetOpenInterest( pDepthMarketData->OpenInterest );
	
	AppendTick( tick );
	if( m_displayTick )
	{
		cout << tick.TickTime().TickTimeFullString() << ":" << m_id.c_str() << " >>> trade:"  << tick.Last();
		cout << " change:" << tick.Last() - pDepthMarketData->PreSettlementPrice;
		cout << " bv:" << tick.BidVolume() << " bid:" << tick.Bid();
		cout << " ask:" << tick.Ask() << " av:" << tick.AskVolume();
		cout << endl;
	}
	//
	// candle
	// if the candle stick time,i.e.m_timeBucket, is 20160115 09:30
	// take a 1 min candle stick for example
	// the 1m candle stick shall include tick from 201600115 09:30:01 to 20160115 09:31:00
	int sec = tickTime.Hour() * SECONDSPERHOUR + tickTime.Minute() * SECONDSPERMINUTE + tickTime.Second();
	int date_num = tickTime.Date().JulianDate();
	int idx = m_timeBucket.location( sec );
	if( m_timeBucket[idx] >= sec )
		idx -= 1;
	// in case sec == 0 it shall be included in the previous day's last time bucket
	if( idx < 0 )
	{
		if( sec != 0 )
			return;
		idx = m_timeBucketStartFlag.getSize() - 1;
	}
	if( m_timeBucketStartFlag[idx] == 0 )
	{
		m_timeBucketStartFlag[idx] = 1;
		if( idx == 0 )
		{
			// move pass the 12:00 pm in clock
			int nBuckets = m_timeBucketStartFlag.getSize();
			if( m_timeBucketStartFlag[nBuckets-1] == 1 && pRealTimeCandle.get() )
			{
				m_timeBucketEndFlag[nBuckets-1] = 1;
				pRealTimeCandle->Set();
				if( m_displayCandle && pRealTimeCandle->IsSet() )
				{
					cString dateStr, timeStr, candleInfo;
					pRealTimeCandle->DateStart( dateStr );
					pRealTimeCandle->TimeStart( timeStr );
					pRealTimeCandle->Print( candleInfo );
					cout << dateStr.c_str() << " " << timeStr.c_str() << ":" << m_id.c_str() << " >>> " << candleInfo.c_str() << endl;	
				}
				if( pRealTimeCandle->IsSet() )
					m_candles.push_back( *pRealTimeCandle );
				pRealTimeCandle->Clear();
			}
		} 
		if( idx >= 1 && m_timeBucketStartFlag[idx-1] == 1 )
		{
			m_timeBucketEndFlag[idx-1] = 1;
			pRealTimeCandle->Set();
			if( m_displayCandle && pRealTimeCandle->IsSet() )
			{
				cString dateStr, timeStr, candleInfo;
				pRealTimeCandle->DateStart( dateStr );
				pRealTimeCandle->TimeStart( timeStr );
				pRealTimeCandle->Print( candleInfo );
				cout << dateStr.c_str() << " " << timeStr.c_str() << ":" << m_id.c_str() << " >>> " << candleInfo.c_str() << endl;	
			}
			if( pRealTimeCandle->IsSet() )
				m_candles.push_back( *pRealTimeCandle  );
			pRealTimeCandle->Clear();
		}
		cCandle* pCandle = new cCandle( date_num, date_num, m_timeBucket[idx], m_timeBucket[idx+1] );
		pRealTimeCandle = auto_ptr< cCandle >( pCandle );
	}
	if( pRealTimeCandle.get() )
		pRealTimeCandle->UpdateWithTick( tick );
}

void cMarketData::SimulationUpdate( const cTick& tick )
{
	yr_assert( Compare( m_id, tick.InstrumentID() ) );
	yr_assert( m_weights.getSize() == 1 );

	if( !DoRealTimeUpdate( tick.TickTime() ) )
		return;

	AppendTick( tick );

	cTickTime tickTime = tick.TickTime();

	if( m_displayTick )
	{
		cout << tickTime.TickTimeFullString() << ":" << m_id.c_str() << " >>> trade:"  << tick.Last();
		cout << " bv:" << tick.BidVolume() << " bid:" << tick.Bid();
		cout << " ask:" << tick.Ask() << " av:" << tick.AskVolume();
		cout << endl;
	}

	int sec = tickTime.Hour() * SECONDSPERHOUR + tickTime.Minute() * SECONDSPERMINUTE + tickTime.Second();
	int date_num = tickTime.Date().JulianDate();
	int idx = m_timeBucket.location( sec );
	if( m_timeBucket[idx] >= sec )
		idx -= 1;
	if( idx < 0 )
	{
		if( sec != 0 )
			return;
		idx = m_timeBucketStartFlag.getSize() - 1;
	}
	if( m_timeBucketStartFlag[idx] == 0 )
	{
		m_timeBucketStartFlag[idx] = 1;
		if( idx == 0 )
		{
			int nBuckets = m_timeBucketStartFlag.getSize();
			if( m_timeBucketStartFlag[nBuckets-1] == 1 && pRealTimeCandle.get() )
			{
				m_timeBucketEndFlag[nBuckets-1] = 1;
				pRealTimeCandle->Set();
				if( m_displayCandle && pRealTimeCandle->IsSet() )
				{
					cString dateStr, timeStr, candleInfo;
					pRealTimeCandle->DateStart( dateStr );
					pRealTimeCandle->TimeStart( timeStr );
					pRealTimeCandle->Print( candleInfo );
					cout << dateStr.c_str() << " " << timeStr.c_str() << ":" << m_id.c_str() << " >>> " << candleInfo.c_str() << endl;	
				}
				if( pRealTimeCandle->IsSet() )
					m_candles.push_back( *pRealTimeCandle );
				pRealTimeCandle->Clear();
			}
		}
		if( idx >= 1 && m_timeBucketStartFlag[idx-1] == 1 )
		{
			m_timeBucketEndFlag[idx-1] = 1;
			pRealTimeCandle->Set();
			if( m_displayCandle && pRealTimeCandle->IsSet() )
			{
				cString dateStr, timeStr, candleInfo;
				pRealTimeCandle->DateStart( dateStr );
				pRealTimeCandle->TimeStart( timeStr );
				pRealTimeCandle->Print( candleInfo );
				cout << dateStr.c_str() << " " << timeStr.c_str() << ":" << m_id.c_str() << " >>> " << candleInfo.c_str() << endl;	
			}
			if( pRealTimeCandle->IsSet() )
				m_candles.push_back( *pRealTimeCandle );
			pRealTimeCandle->Clear();
		}
		cCandle* pCandle = new cCandle( date_num, date_num, m_timeBucket[idx], m_timeBucket[idx+1] );
		pRealTimeCandle = auto_ptr< cCandle >( pCandle );

	}
	if( pRealTimeCandle.get() )
		pRealTimeCandle->UpdateWithTick( tick );
}


int cMarketData::WriteTickDataToFile()
{
	int n = GetNumRecords();
	if( n == 0 )
		return 0;

	cTickTime dt = GetLastTick().TickTime();
	cString datestr = dt.DateString();
	cString fileName = m_id + "_tick_" + datestr + ".txt";
	cString underlyingstr = GetUnderlyingStr();
	cString folderDir;
	if( m_outputDirectory.IsBlankString() )
		folderDir = cString( cSystem::GetEnv( "MYDATADIR" ).c_str() ) + "historicaldata\\tick\\" + underlyingstr + "\\";
	else
	{
		if( m_tickDataFolder.IsBlankString() )
			folderDir = m_outputDirectory + "tick\\" + underlyingstr + "\\";
		else
			folderDir = m_outputDirectory + m_tickDataFolder + "\\" + underlyingstr + "\\";
	}

	cString exchange = GetExchange( underlyingstr );
	
	if( !cSystem::HasDir( folderDir.c_str() ) )
		cSystem::MakeDir( folderDir.c_str() );

	cString fileNamePath = folderDir + fileName;
	// DO NOT override the file in case it exists on drive
	bool replaceFlag = false;
	cSystem::CreateTxtFile( fileNamePath.c_str(), replaceFlag );
	
	ofstream ofs;
	ofs.open( fileNamePath.c_str(), ios::app );
	if( ofs )
	{
		int n = GetNumRecords();
		for( int i = 0; i < n; ++i )
		{
			if( i > 1 && Compare( exchange, ".CZC" ) )
			{
				cTickTime tt_prev = m_ticks[i-1].TickTime();
				cTickTime tt_this = m_ticks[i].TickTime();
				if( tt_prev == tt_this )
				{
					int millisec = tt_this.MilliSecond();
					millisec = max( millisec + 500, 999 );
					tt_this.SetMilliSecond( millisec );
					ofs << tt_this.TickTimeFullString().c_str() << "\t";
				}
				else
					ofs << m_ticks[i].TickTime().TickTimeFullString().c_str() << "\t";
			}
			else
				ofs << m_ticks[i].TickTime().TickTimeFullString().c_str() << "\t";

			ofs << m_ticks[i].Last() << "\t";
			ofs << m_ticks[i].Bid() << "\t";
			ofs << m_ticks[i].BidVolume() << "\t";
			ofs << m_ticks[i].Ask() << "\t";
			ofs << m_ticks[i].AskVolume() << "\t";
			ofs << m_ticks[i].Volume() << "\t";
			ofs << m_ticks[i].OpenInterest() << endl;
		}
		// clear tick data after writing them to file
		clearTickData();
	}
	else
	{
		yr_error( "file s% cannot be open!", fileNamePath.c_str() );
	}

	ofs.close();
	return 1;
}


int cMarketData::WriteCandleStickDataToFile()
{
	if( !GetNumRecords() || !m_candles.getSize() )
		return 0;

	int mm = ( m_timeBucket[1] - m_timeBucket[0] ) / SECONDSPERMINUTE;
	char mms[256];
	sprintf( mms, "%d", mm );
	cTickTime dt = GetLastTick().TickTime();
	cString datestr = dt.DateString();
	cString underlyingstr = GetUnderlyingStr();
	cString fileName = m_id + "_" + cString( mms ) + "m_candle_" + datestr + ".txt";
	cString folderDir;
	if( m_outputDirectory.IsBlankString() )
		folderDir = cString( cSystem::GetEnv( "MYDATADIR" ).c_str() ) + "historicaldata\\candle\\" + underlyingstr + "\\";
	else
	{
		if( m_candleDataFolder.IsBlankString() )
			folderDir = m_outputDirectory + "candle\\" + underlyingstr + "\\";
		else
			folderDir = m_outputDirectory + m_candleDataFolder + "\\" + underlyingstr + "\\";
	}
	
	if( !cSystem::HasDir( folderDir.c_str() ) )
		cSystem::MakeDir( folderDir.c_str() );

	cString fileNamePath = folderDir + fileName;
	// DO NOT override the file in case it exists on drive
	bool replaceFlag = false;
	cSystem::CreateTxtFile( fileNamePath.c_str(), replaceFlag );
	
	ofstream ofs;
	ofs.open( fileNamePath.c_str(), ios::app );
	if( ofs )
	{
		int n = m_candles.getSize();
		for( int i = 0; i < n; ++i )
		{
			cString timeStr;
			m_candles[i].TimeStart( timeStr );
			ofs << datestr.c_str() << " " << timeStr.c_str() << "\t";
			ofs << m_candles[i].Open() << "\t";
			ofs << m_candles[i].High() << "\t"; 
			ofs << m_candles[i].Low() << "\t";
			ofs << m_candles[i].Close() << endl;
		}
		// clear candle stick data after writing down them to file
		clearCandleData();
	}
	else
	{
		yr_error( "file %s cannot be open!", fileNamePath.c_str() );
	}

	ofs.close();
	return 1;
}

void cMarketData::ReadTickDataFromFile( const cArray< cString >& fileNames )
{
	//do nothing in case there is data exists in memory
	if( GetNumRecords() )
		return;
	//do nothing in case of empty fileNames array 
	if( fileNames.getSize() <= 0 )
		return;

	bool sameContractFlag = true;
	cString thisFileName = fileNames[0];
	cArray< cString > checks;
	cString::Split( checks, thisFileName, "_" );
	cString contract = checks[0];
	for( int i = 1; i < fileNames.getSize(); ++i )
	{
		cString::Split( checks, fileNames[i], "_" );
		if( !Compare( contract, checks[0] ) )
		{
			sameContractFlag = false;
			break;
		}
	}
	if( !sameContractFlag )
		yr_error( "files for different contracts supplied!" );

	cString underlyingStr = GetUnderlyingStr();

	// assuming the fileNames are already sorted
	ifstream ifs;
	for( int i = 0; i < fileNames.getSize(); ++i )
	{
		cString fileFullDir = cString( cSystem::GetEnv( "MYDATADIR" ).c_str() ) + "historicaldata\\tick\\" + underlyingStr + "\\" + fileNames[i];
		ifs.open( fileFullDir.c_str(), ios::in );
		if( !ifs )
			yr_error( "file s% cannot be open!", fileFullDir.c_str() );
		string line;
		while( getline( ifs, line ) )
		{
			string buff;
			stringstream ss( line );
			cArray< string > sbu;
			while( ss >> buff )
				sbu.push_back( buff );
			
			cTickTime dt( sbu[0].c_str(), sbu[1].c_str(), atoi( sbu[2].c_str() ) );
			double last = atof( sbu[3].c_str() );
			double bid = atof( sbu[4].c_str() );
			double bidVolume = atof( sbu[5].c_str() );
			double ask = atof( sbu[6].c_str() );
			double askVolume = atof( sbu[7].c_str() );
			double volume = atof( sbu[8].c_str() );
			double openinterest = atof( sbu[9].c_str() );

			cTick tick( contract, dt );
			tick.SetBid( bid );
			tick.SetAsk( ask );
			tick.SetLast( last );
			tick.SetBidVolume( bidVolume );
			tick.SetAskVolume( askVolume );
			tick.SetVolume( volume );
			tick.SetOpenInterest( openinterest );
			AppendTick( tick );
		}

		ifs.close();
	}
}

void cMarketData::ReadCandleStickDataFromFile( const cArray< cString >& fileNames  )
{
	//do nothing in case there are data exists in memory
	if( m_candles.getSize() )
		return;
	//do nothing in case of empty fileNames array 
	if( fileNames.getSize() <= 0 )
		return;

	bool sameContractFlag = true;
	cString thisFileName = fileNames[0];
	cArray< cString > checks;
	cString::Split( checks, thisFileName, "_" );
	cString contract = checks[0];
	for( int i = 1; i < fileNames.getSize(); ++i )
	{
		cString::Split( checks, fileNames[i], "_" );
		if( !Compare( contract, checks[0] ) )
		{
			sameContractFlag = false;
			break;
		}
	}
	if( !sameContractFlag )
		yr_error( "files for different contracts supplied!" );

	cString underlyingStr = GetUnderlyingStr();

	// assuming the fileNames are already sorted
	ifstream ifs;
	for( int i = 0; i < fileNames.getSize(); ++i )
	{
		cString fileFullDir = cString( cSystem::GetEnv( "MYDATADIR" ).c_str() ) + "historicaldata\\candle\\" + underlyingStr + "\\" + fileNames[i];
		ifs.open( fileFullDir.c_str(), ios::in );
		if( !ifs )
			yr_error( "file %s cannot be open!", fileFullDir.c_str() );
		string line;
		while( getline( ifs, line ) )
		{
			string buff;
			stringstream ss( line );
			cArray< string > sbu;
			while( ss >> buff )
				sbu.push_back( buff );
			
			double dtnum, open, high, low, close;
			
			if( sbu.getSize() == 6 )
			{
				cTickTime dt( sbu[0].c_str(), sbu[1].c_str() );
				dtnum = dt.TickTimeToDouble();
				open = atof( sbu[2].c_str() );
				high = atof( sbu[3].c_str() );
				low = atof( sbu[4].c_str() );
				close = atof( sbu[5].c_str() );
			}
			else if( sbu.getSize() == 5 )
			{
				dtnum = atof( sbu[0].c_str() );
				open = atof( sbu[1].c_str() );
				high = atof( sbu[2].c_str() );
				low = atof( sbu[3].c_str() );
				close = atof( sbu[4].c_str() );
			}
			else
				yr_error( "internal error" );

		}

		ifs.close();
	}

}

void cMarketData::Reset()
{
	m_ticks.clear();
	m_candles.clear();
}

// private functions / methods
void cMarketData::genIntradayBucket( cIvector& timeBucket, cIvector& tradingStartBucket, cIvector& tradingEndBucket, const cString& frequency )
{
	SwUnderlying underlying = GetUnderlying();
	cArray< cString > tradingHours = GetTradingHours( underlying );
	cString tradingBreak = GetTradingBreak( underlying );
	cString tenor = cString::Right( frequency, 1 );
	
	int interval = AsInt( cString::Left( frequency, frequency.size() - 1 ) );
	if( Compare( tenor, "m" ) )
	{
		interval = interval;
	}
	else if( Compare( tenor, "h" ) )
	{
		interval = MINUTESPERHOUR * interval;
	}
	else
		yr_error( "Invalid Frequency" );

	propTradingBuckets( tradingStartBucket, tradingEndBucket, tradingHours, tradingBreak );

	// now generate timeBucket
	for( int i = 0; i < tradingEndBucket.getSize(); ++i )
	{
		int timePoint = tradingStartBucket[i];
		timeBucket.push_back( timePoint );
		timePoint += interval;
		while( timePoint <= tradingEndBucket[i] )
		{
			timeBucket.push_back( timePoint );
			timePoint += interval;
		}
	}
	//translate into seconds
	for( int i = 0; i < timeBucket.getSize(); ++i )
		timeBucket[i] *= SECONDSPERMINUTE;

	for( int i = 0; i < tradingStartBucket.getSize(); ++i )
	{
		tradingStartBucket[i] *= SECONDSPERMINUTE;
		tradingEndBucket[i] *= SECONDSPERMINUTE;
	}
}


void cMarketData::propTradingBuckets( cIvector& tradingStartBucket, cIvector& tradingEndBucket, const cArray< cString >& tradingHours, const cString& tradingBreak )
{
	for( int i = 0; i < tradingHours.size(); ++i )
	{
		if( !Compare( tradingHours[i], "n/a" ) )
		{
			cArray< cString > tmp;
			cString::Split( tmp, tradingHours[i], "-" );
			int start_mm = MINUTESPERHOUR * AsInt( cString::Left( tmp[0], 2 ) ) + AsInt( cString::Right( tmp[0], 2 ) );
			int end_mm = MINUTESPERHOUR * AsInt( cString::Left( tmp[1], 2 ) ) + AsInt( cString::Right( tmp[1], 2 ) );
			if( i == 0  && !Compare( tradingBreak, "none" ) )
			{
				cString::Split( tmp, tradingBreak, "-" );
				int break_start = MINUTESPERHOUR * AsInt( cString::Left( tmp[0], 2 ) ) + AsInt( cString::Right( tmp[0], 2 ) );
				int break_end = MINUTESPERHOUR * AsInt( cString::Left( tmp[1], 2 ) ) + AsInt( cString::Right( tmp[1], 2 ) );
				tradingStartBucket.push_back( start_mm );
				tradingStartBucket.push_back( break_end );
				tradingEndBucket.push_back( break_start );
				tradingEndBucket.push_back( end_mm );
			}
			else if( i == 2 && end_mm < 540 )
			{
				tradingStartBucket.push_back( start_mm );
				tradingStartBucket.insert( 0, 0 );
				tradingEndBucket.push_back( MINUTESPERDAY );
				tradingEndBucket.insert( 0, end_mm );
			}
			else
			{
				tradingStartBucket.push_back( start_mm );
				tradingEndBucket.push_back( end_mm );
			}
		}
	}
	yr_assert( tradingStartBucket.getSize() == tradingEndBucket.getSize() );
	yr_assert( tradingStartBucket.IsIncreasing() && tradingEndBucket.IsIncreasing() );
}

void cMarketData::clearTickData()
{
	m_ticks.clear();
}

void cMarketData::clearCandleData()
{
	m_candles.clear();

	m_timeBucketStartFlag.resize( m_timeBucket.getSize()-1, 0);
	m_timeBucketEndFlag.resize( m_timeBucket.getSize()-1, 0);
}

void cMarketData::ClearAllData()
{
	clearTickData();
	clearCandleData();
}

void cMarketData::genBreakIdxVec()
{
	if( !m_timeBucket.getSize() || !m_tradingStart.getSize() || !m_tradingEnd.getSize() )
		return;

	int n = m_tradingEnd.getSize();
	m_breakIdxVec.resize( n );
	
	for( int i = 0; i < n; ++i )
		m_breakIdxVec[i] = m_timeBucket.location( m_tradingEnd[i] );
}
