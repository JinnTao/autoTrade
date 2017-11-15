#include <cMarketDataCollection.h>
#include <cTickTime.h>
#include <cSystem.h>
#include <cDateTime.h>


cMarketDataCollection::cMarketDataCollection()
{
    
}

cMarketDataCollection::~cMarketDataCollection()
{

}


cMarketData* cMarketDataCollection::GetMarketDataHandle( const string& name )
{
	cMarketDataPtr p = GetMarketDataHandleSharedPtr( name );
	if( p )
		return p.get();
	else
		return NULL;
}

cMarketDataPtr cMarketDataCollection::GetMarketDataHandleSharedPtr( const string& name )
{
	marketdataHandle::iterator it = _m_mkt_handle.find( name );
	if( it != _m_mkt_handle.end() )
		return (*it).second;
	else
		return cMarketDataPtr();
}




void cMarketDataCollection::OnRtnDepthMarketData( CThostFtdcDepthMarketDataField* pDepthMarketData )
{
	string id( pDepthMarketData->InstrumentID );
	cMarketData* md = GetMarketDataHandle( id );

	if( !md ){
		// create new CmarketData
		shared_ptr< cMarketData > ptr = make_shared< cMarketData >( id);
		ptr->OnRtnDepthMarketData(pDepthMarketData);
		_m_mkt_handle.insert(map< string, cMarketDataPtr >::value_type(id,ptr));
	}else{
		md->OnRtnDepthMarketData( pDepthMarketData );
	}
}
// 读取历史数据
void cMarketDataCollection::loadSeriesHistory(string inst,string startDate,string endDate,vector<double>& open,vector<double> &high,vector<double> &low,vector<double> &close,vector<double> &volume){
	try{
		
		auto iter = this->m_pTradeDayList->find(startDate);
		
		vector<string> fileNameList;
		while(iter != this->m_pTradeDayList->end()){
			fileNameList.push_back(iter->first);
			if(iter != this->m_pTradeDayList->find(endDate)){
				iter++;
			}else{
				break;
			}

		}

		for(auto i = fileNameList.begin();i!= fileNameList.end();i++){
			fstream dataFile;
			string date,time;
			double dOpen,dHigh,dLow,dClose,dVolume;
			dataFile.open( inst +"/" + *i + "_1m.txt", ios::_Nocreate | ios::in) ;
			if(dataFile)
			{
				while(!dataFile.eof()){
					dataFile >> date >> time>> dOpen >> dHigh>> dLow>>dClose>>dVolume;
					cerr << date<< " " << time<< " " << dOpen<< " "  << dHigh << " "<< dLow << " "<< dClose<< " " << dVolume <<endl;
					open.push_back(dOpen);
					high.push_back(dHigh);
					low.push_back(dLow);
					close.push_back(dClose);
					volume.push_back(dVolume);
				}
				// 弹出最后一个，因为上面会多加一个
				open.pop_back();
				high.pop_back();
				low.pop_back();
				close.pop_back();
				volume.pop_back();
				dataFile.close();	
			}

		}


	}
	catch(...){
	
	}

}
vector<cCandle> cMarketDataCollection::loadCandleHistory(string inst,string startDate,string endDate,DataFrequency dataFrequency,DataType dataType){
	vector<cCandle> t;
	return t;
}
void cMarketDataCollection::loadHistoryFromMongo(string collection, string sDateTime, string eDateTime, vector<double>& open, vector<double>& high, vector<double> &low, vector<double>& close, vector<double> &volume) {
    vector<string> dateTime;
    int sYear, sMon, sDay, eYear, eMon, eDay;
    int sH, sM, sS, eH, eM, eS;
    sscanf_s(sDateTime.c_str(), "%4d%2d%2d-%2d:%2d:%2d", &sYear, &sMon, &sDay, &sH, &sM, &sS);
    sscanf_s(eDateTime.c_str(), "%4d%2d%2d-%2d:%2d:%2d", &eYear, &eMon, &eDay, &eH, &eM, &eS);
    std::tm sTimeTm{sS,sM,sH,sDay,sMon-1,sYear-1900};
    std::tm eTimeTm{eS,eM,eH,eDay,eMon-1,eYear-1900};
   this->m_sDateTimePoint = std::chrono::system_clock::from_time_t(std::mktime(&sTimeTm));
   this->m_eDateTimePoint  = std::chrono::system_clock::from_time_t(std::mktime(&eTimeTm));
    m_mongoStore.getData(collection, m_sDateTimePoint, m_eDateTimePoint, close, open, high, low, volume, dateTime);

}