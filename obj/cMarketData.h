/***
*cVector.h - declarations/definitions for marketdata class
*
*       Copyright (c) Yiran Yang. All rights reserved.
*
****/

#ifndef __CMARKETDATA_H__
#define __CMARKETDATA_H__

#include <SwFuturesCN.h>
#include <cTick.h>
#include <cCandle.h>

class cTickTime;

struct CThostFtdcDepthMarketDataField;

class cMarketData
{
public:
	cMarketData();
	explicit cMarketData( const cString& id, const cString& freq );
	cMarketData( const cMarketData& );

	~cMarketData();

	cMarketData& operator = ( const cMarketData& );

	void ClearAllData();

	// set methods
	void SetID( const cString& id ) { m_id = id; };
	void SetTimeBucket( const cIvector& timeBucket ) { m_timeBucket = timeBucket; }
	void SetTimeBucketStartFlag( const cIvector& flags ) { m_timeBucketStartFlag = flags; }
	void SetTimeBucketEndFlag( const cIvector& flags ) { m_timeBucketEndFlag = flags; }
	void SetTradingBuckets( const cIvector& tradingStart, const cIvector& tradingEnd ) { m_tradingStart = tradingStart; m_tradingEnd = tradingEnd; }
	void SetWeights( const cVector& weights ) { m_weights = weights; }
	void SetDisplayTick( bool flag ) { m_displayTick = flag; }
	void SetDisplayCandle( bool flag ) { m_displayCandle = flag; }
	void SetOutputDirectory( const cString& directory ) { m_outputDirectory = directory; }
	void SetTickDataFolder( const cString& folderName ) { m_tickDataFolder = folderName; }
	void SetCandleDataFolder( const cString& folderName ) { m_candleDataFolder = folderName; }

	// get methods
	cString GetID() const { return m_id; }
	cIvector GetTimeBucket() const { return m_timeBucket; }
	cIvector GetTimeBucketStartFlag() const { return m_timeBucketStartFlag; }
	cIvector GetTimeBucketEndFlag() const { return m_timeBucketEndFlag; }
	int GetNumRecords() const { return m_ticks.getSize(); }
	cTick GetLastTick() const { return m_ticks.getLast(); }
	cVector GetWeigths() const { return m_weights; }
	void GetTradingBuckets( cIvector& tradingStart, cIvector& tradingEnd ) const { tradingStart = m_tradingStart; tradingEnd = m_tradingEnd; }
	void GetCandleStick( cVector& time, cVector& open, cVector& high, cVector& low, cVector& close ) const;
	const cArray< cCandle >* GetCandleStick() const;

	// append methods
	void AppendTick( const cTick& tick ) { m_ticks.push_back( tick ); }

	SwUnderlying GetUnderlying() const;
	cString GetUnderlyingStr() const;

	// Realtime update 
	bool DoRealTimeUpdate( const cTickTime& ) const;
	void OnRtnDepthMarketData( CThostFtdcDepthMarketDataField* pDepthMarketData );
	void RtnLastCandle();

	// Simulation test
	void SimulationSetTime( double t ) { m_simTime = t; }
	void SimulationUpdate( const cTick& tick );
	void SimulationRtnLastCandle();

	// file I/O
	int WriteTickDataToFile();
	int WriteCandleStickDataToFile();
	void ReadTickDataFromFile( const cArray< cString >& fileNames );
	void ReadCandleStickDataFromFile( const cArray< cString >& fileNames );

	void Reset();


protected:
	cString m_id;				// contract code
	cVector m_weights;
	cArray< cTick> m_ticks;
	cArray< cCandle > m_candles;
	//
	cIvector m_timeBucket;		// integer vector of second when the candle is supposed to be generated
	cIvector m_tradingStart;	// integer vector of second when trading starts
	cIvector m_tradingEnd;		// integer vector of second when trading ends
	cIvector m_timeBucketStartFlag;	// 0/1 vector to check whether candle is poped up for certain bucket
	cIvector m_timeBucketEndFlag; // 0/1 vector to check whether candle is poped up for certain bucket
	cIvector m_breakIdxVec;

private:
	void genIntradayBucket( cIvector& timeBucket, cIvector& tradingStartBucket, cIvector& tradingEndBucket, const cString& frequency );
	void propTradingBuckets( cIvector& tradingStartBucket, cIvector& tradingEndBucket, const cArray< cString >& tradingHours, const cString& tradingBreak );
	void clearTickData();
	void clearCandleData();
	
	void genBreakIdxVec();

	// run-time candle
	auto_ptr< cCandle > pRealTimeCandle;

	// variable used for simulation
	double m_simTime;

	// I/O
	bool m_displayTick;

	bool m_displayCandle;

	cString m_outputDirectory;

	cString m_tickDataFolder;

	cString m_candleDataFolder;
	
};

typedef shared_ptr< cMarketData > cMarketDataPtr;



#endif
