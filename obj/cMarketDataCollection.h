#ifndef __CMARKETDATACOLLECTION_H__
#define __CMARKETDATACOLLECTION_H__

#include <cMarketData.h>
#include <Windows.h>

template< class T > class cArray;

// underlying string versus array of cMarketData, i.e. IF vs. IF1601 and IF1603
typedef map< cString, cArray< const cMarketData* > > marketdataStore;
// futures ID versus cMarketDataPtr, i.e. IF1601
typedef map< cString, cMarketDataPtr > marketdataHandle;


class cMarketDataCollection
{
public:
	cMarketDataCollection();
	virtual ~cMarketDataCollection();

	void Clear();
	void ClearMarketData();

	void Add( cMarketDataPtr p_element ) { AddToMapInternal( p_element ); }

	int Count() const;
	void UniqueUnderlyings( cArray< cString >& ) const;
	void GetInstrumentIDs( cArray< cString >& instrumentIDs ) const;

	cMarketData* GetMarketDataHandle( const cString& );
	cMarketDataPtr GetMarketDataHandleSharedPtr( const cString& );
	cArray< const cMarketData* > GetMarketDataByUnderlying( const cString& ) const;
	

	typedef map< cString, cMarketDataPtr> mapType;

	//initialize
	void Init( cArray< cString >& instrumentIDs );

	//set methods
	void SetGenLogFlag( bool flag ) { m_genLog = flag; }
	void SetSampleFrequency( const cString& sampleFreq ) { m_sampleFreq = sampleFreq; }
	void SetRunCalendarSpreadFlag( bool flag ) { m_runCalendarSpread = flag; }
	void SetDisplayTickFlags( bool flags );
	void SetDisplayCandleFlags( const map< cString, bool >& flags );
	void SetOutputDirectory( const cString& directory );
	void SetTickDataFolder( const cString& folderName );
	void SetCandleDataFolder( const cString& folderName );
	void SetLogFileFolder( const cString& folderName ) { m_logFileFolder = folderName; }
	void SetLogFileName( const cString& logFileName ) { m_logFile = logFileName; }

	//get methods
	bool GetGenLogFlag() const { return m_genLog; }
	cString GetLogFileName() const { return m_logFile; }
	cString GetOutputDirectory() const { return m_outputDirectory; }
	cString GetLogFolderName() const { return m_logFileFolder; }

	//real time update
	void OnRtnDepthMarketData( CThostFtdcDepthMarketDataField* pDepthMarketData );
	void RtnLastCandle();
	DWORD IOProcess();
	
	//simulation tests
	void SimulationSetTime( double t );
	void SimulationUpdate( const cTick& tick );
	void SimulationRtnLastCandle();
	DWORD SimulationIOProcess();
	
	//data O/I
	void WriteTickDataToFile();
	void WriteCandleStickDataToFile();
	void ReadTickDataFromFile( const cArray< cArray< cString > >& fileNames );
	void ReadCandleStickDataFromFile( const cArray< cArray< cString > >& fileNames );

protected:
	mapType _map_mkt_data;
	mapType::iterator _it;
	marketdataStore _m_mkt_underlying;
	marketdataHandle _m_mkt_handle;

private:
	void AddToMapInternal( shared_ptr< cMarketData >& element );
	bool m_genLog;
	cString m_sampleFreq;
	bool m_runCalendarSpread;
	double m_simTime;
	cString m_outputDirectory;
	cString m_logFileFolder;
	cString m_logFile;
	
};

typedef shared_ptr< cMarketDataCollection > cMarketDataCollectionPtr;

#endif