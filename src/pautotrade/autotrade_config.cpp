#include <autotrade_config.h>
#include <cSystem.h>
#include "IniFile.h"

int ParseSettingJson( AccountParam &account,mongoSetting &mongoDbSetting,autoSetting &autoTradeSetting){  
	try{
		IniFile ini("setting.ini");
		// account information
		strcpy_s(account.brokerId,sizeof(account.brokerId), ini.ReadString("account", "brokerID", "1").c_str());
		strcpy_s(account.tdAddress,sizeof(account.tdAddress), ini.ReadString("account", "tdAddress", "1").c_str());
		strcpy(account.passwd, ini.ReadString("account", "password", "1").c_str());
		strcpy_s(account.mdAddress,sizeof(account.mdAddress), ini.ReadString("account", "mdAddress", "1").c_str());
		strcpy_s(account.userId,sizeof(account.userId), ini.ReadString("account", "userID", "1").c_str());
		// mongoDB
		strcpy_s(mongoDbSetting.address,sizeof(mongoDbSetting.address), ini.ReadString("dbMongo", "address", "1").c_str());
        strcpy_s(mongoDbSetting.database, sizeof(mongoDbSetting.database), ini.ReadString("dbMongo", "dataBase", "1").c_str());
		mongoDbSetting.mongoPort =  ini.ReadInt("dbMongo", "mongoPort", 1);
		mongoDbSetting.mongoLogging = ini.ReadInt("dbMongo", "mongoLogging", 1);
		// txt database dir
		strcpy_s(autoTradeSetting.dataBaseDir, sizeof(autoTradeSetting.dataBaseDir), ini.ReadString("dataBase", "dataBaseDir", "1").c_str());
		strcpy_s(autoTradeSetting.tradeDayDir, sizeof(autoTradeSetting.tradeDayDir), ini.ReadString("dataBase", "tradeDayDir", "1").c_str());
		strcpy_s(autoTradeSetting.startDate, sizeof(autoTradeSetting.startDate), ini.ReadString("dataBase", "startDate", "1").c_str());
		strcpy_s(autoTradeSetting.endDate, sizeof(autoTradeSetting.endDate), ini.ReadString("dataBase", "endDate", "1").c_str());
		// strategy
		strcpy_s(autoTradeSetting.inst, sizeof(autoTradeSetting.inst), ini.ReadString("strategy", "inst", "1").c_str());
        // datasource
        strcpy_s(autoTradeSetting.collectionName, sizeof(autoTradeSetting.collectionName), ini.ReadString("KingKeltner", "collectionName", "").c_str());
        strcpy_s(autoTradeSetting.startDateTime, sizeof(autoTradeSetting.startDateTime), ini.ReadString("KingKeltner", "startDateTime", "").c_str());
        strcpy_s(autoTradeSetting.endDateTime, sizeof(autoTradeSetting.endDateTime), ini.ReadString("KingKeltner", "endDateTime", "").c_str());
		// KingKeltner
		autoTradeSetting.kkLength = ini.ReadDouble("KingKeltner", "kkLength", 1);
		autoTradeSetting.kkDev = ini.ReadDouble("KingKeltner", "kkDev", 1);
		autoTradeSetting.trailingPrcnt = ini.ReadDouble("KingKeltner", "trailingPrcnt", 1);
		autoTradeSetting.fixedSize = ini.ReadDouble("KingKeltner", "fixedSize", 1);
		autoTradeSetting.initDays = ini.ReadDouble("KingKeltner", "initDays", 1);

		return 0;
	}
	catch (...){
		cout << "Parse Setting Error..." << endl;
		system("pause");
	}
	return -1;

  // 解析json用Json::Reader   
  //Json::Reader reader;  
  // Json::Value是一种很重要的类型，可以代表任意类型。如int, string, object, array...   
 // Json::Value root;         
  
  //std::ifstream is;  
  //is.open ("CodeLib/config/setting.json", std::ios::binary );    
  //if (reader.parse(is, root))  
  //{  
	 // strcpy_s(account.brokerId,sizeof(account.brokerId),root["brokerID"].asString().c_str());
	 // strcpy_s(account.tdAddress,sizeof(account.tdAddress),root["tdAddress"].asString().c_str());
	 // strcpy_s(account.passwd,sizeof(account.passwd),root["password"].asString().c_str());
	 // strcpy_s(account.mdAddress,sizeof(account.mdAddress),root["mdAddress"].asString().c_str());
	 // strcpy_s(account.userId,sizeof(account.userId),root["userID"].asString().c_str());

	 // strcpy_s(mongoDbSetting.mongoHost,sizeof(mongoDbSetting.mongoHost),root["mongoHost"].asString().c_str());
	 // mongoDbSetting.mongoPort = root["mongoPort"].asInt();
	 // mongoDbSetting.mongoLogging = root["mongoLogging"].asBool();
	 // 
  //} else{
	 // is.close();
	 // return -1;
  //} 
  //is.close();  
  //return 0;  

}  


void autotrade_loadconfig_general( sATGeneralConfig& configs, cString& fileName )
{
	printf( "\nplease save your configuration file in the folder %s", ( cSystem::GetEnv( "MYDATADIR" ) + "configs\\" ).c_str() );

	char configFileName[256];

	printf( "\nplease input the configuration file name:" );
	cin >> configFileName;

	fileName = cString( configFileName );
	
	cString configDir = cString( cSystem::GetEnv( "MYDATADIR" ).c_str() ) + "configs\\" + configFileName + ".txt";
	
	ifstream ifs;
	ifs.open( configDir.c_str(), ios::in );
	if( !ifs )
		yr_error( "file %s cannot be open!", configDir.c_str() );

	string line;
	bool foundGenLog = false;
	bool foundDataoutputDirectory = false;
	bool foundTickDataFolderName = false;
	bool foundCandleDataFolderName = false;
	bool foundLogFileFolderName = false;
	bool foundUnderlyings = false;
	bool foundDisplayTick = false;
	bool foundDisplayCandle = false;
	bool foundDisplaySignal = false;
	bool foundDisplayOrder = false;
	bool foundDisplayTrade = false;

	cArray< bool > displayTickArray;
	cArray< bool > displayCandleArray;
	cArray< bool > displaySignalArray;
	cArray< bool > displayOrderArray;
	cArray< bool > displayTradeArray;


	while( getline( ifs, line ) )
	{
		string buff;
		stringstream ss( line );
		cArray< string > sbu;
		while( ss >> buff )
			sbu.push_back( buff );

		if( sbu.getSize() < 1 )
			continue;

		if( Compare( "GenLog", sbu[0].c_str() ) )
		{
			configs.genLog = Compare( sbu[1].c_str(), "YES" ) ? true : false;
			foundGenLog = true;
		}

		if( Compare( "DataOutputDirectory", sbu[0].c_str() ) )
		{
			configs.dataoutputDirectory = cString( sbu[1].c_str() );
			foundDataoutputDirectory = true;
		}

		if( Compare( "TickDataFolderName", sbu[0].c_str() ) )
		{
			configs.tickDataFolderName = cString( sbu[1].c_str() );
			foundTickDataFolderName = true;
		}

		if( Compare( "CandleDataFolderName", sbu[0].c_str() ) )
		{
			configs.candleDataFolderName = cString( sbu[1].c_str() );
			foundCandleDataFolderName = true;
		}

		if( Compare( "LogFileFolderName", sbu[0].c_str() ) )
		{
			configs.logFileFolderName = cString( sbu[1].c_str() );
			foundLogFileFolderName = true;
		}

		if( Compare( "Underlyings", sbu[0].c_str() ) )
		{
			for( int i = 1; i < sbu.getSize(); ++i )
				configs.underlyings.push_back( cString( sbu[i].c_str() ) );
			foundUnderlyings = true;
		}


		if( Compare( "DisplayTick", sbu[0].c_str() ) )
		{
			for( int i = 1; i < sbu.getSize(); ++i )
				displayTickArray.push_back( Compare( "YES", sbu[i].c_str() ) ? true : false );
			foundDisplayTick = true;
		}

		if( Compare( "DisplayCandle", sbu[0].c_str() ) )
		{
			for( int i = 1; i < sbu.getSize(); ++i )
				displayCandleArray.push_back( Compare( "YES", sbu[i].c_str() ) ? true : false );
			foundDisplayCandle = true;
		}

		if( Compare( "DisplaySignal", sbu[0].c_str() ) )
		{
			for( int i = 1; i < sbu.getSize(); ++i )
				displaySignalArray.push_back( Compare( "YES", sbu[i].c_str() ) ? true : false );
			foundDisplaySignal = true;
		}

		if( Compare( "DisplayOrder", sbu[0].c_str() ) )
		{
			for( int i = 1; i < sbu.getSize(); ++i )
				displayOrderArray.push_back( Compare( "YES", sbu[i].c_str() ) ? true : false );
			foundDisplayOrder = true;
		}

		if( Compare( "DisplayTrade", sbu[0].c_str() ) )
		{
			for( int i = 1; i < sbu.getSize(); ++i )
				displayTradeArray.push_back( Compare( "YES", sbu[i].c_str() ) ? true : false );
			foundDisplayTrade = true;
		}
	}

	ifs.close();

	//
	/* Set default values */
	if( !foundDataoutputDirectory )
		configs.dataoutputDirectory = "C:\\Temp\\";

	if( !foundUnderlyings )
		yr_error( "invalid underlying input..." );

	if( !foundGenLog )
		configs.genLog = false;

	if( !foundDataoutputDirectory )
		configs.dataoutputDirectory = cString( cSystem::GetEnv( "MYDATADIR" ).c_str() );
	

	if( !foundTickDataFolderName )
		configs.tickDataFolderName = "tick";

	if( !foundCandleDataFolderName )
		configs.candleDataFolderName = "candle";

	if( !foundLogFileFolderName )
		configs.logFileFolderName = "log";

	if( !foundDisplayTick )
	{
		for( int i = 0; i < configs.underlyings.getSize(); ++i )
			displayTickArray.push_back( false );
	}

	if( !foundDisplayCandle )
	{
		for( int i = 0; i < configs.underlyings.getSize(); ++i )
			displayCandleArray.push_back( false );
	}

	if( !foundDisplaySignal )
	{
		for( int i = 0; i < configs.underlyings.getSize(); ++i )
			displaySignalArray.push_back( false );
	}

	if( !foundDisplayOrder )
	{
		for( int i = 0; i < configs.underlyings.getSize(); ++i )
			displayOrderArray.push_back( false );
	}

	if( !foundDisplayTrade )
	{
		for( int i = 0; i < configs.underlyings.getSize(); ++i )
			displayTradeArray.push_back( false );
	}

	for( int i = 0; i < configs.underlyings.getSize(); ++i )
	{
		map< cString, bool >::iterator it = configs.displayTick.find( configs.underlyings[i] );
		if( it == configs.displayTick.end() )
			configs.displayTick.insert( map< cString, bool >::value_type( configs.underlyings[i], displayTickArray[i] ) );
		else
			(*it).second = displayTickArray[i];

		map< cString, bool >::iterator it2 = configs.displayCandle.find( configs.underlyings[i] );
		if( it2 == configs.displayCandle.end() )
			configs.displayCandle.insert( map< cString, bool >::value_type( configs.underlyings[i], displayCandleArray[i] ) );
		else
			(*it2).second = displayCandleArray[i];

		map< cString, bool >::iterator it3 = configs.displaySignal.find( configs.underlyings[i] );
		if( it3 == configs.displaySignal.end() )
			configs.displaySignal.insert( map< cString, bool >::value_type( configs.underlyings[i], displaySignalArray[i] ) );
		else
			(*it3).second = displaySignalArray[i];

		map< cString, bool >::iterator it4 = configs.displayOrder.find( configs.underlyings[i] );
		if( it4 == configs.displayOrder.end() )
			configs.displayOrder.insert( map< cString, bool >::value_type( configs.underlyings[i], displayOrderArray[i] ) );
		else
			(*it4).second = displayOrderArray[i];

		map< cString, bool >::iterator it5 = configs.displayTrade.find( configs.underlyings[i] );
		if( it5 == configs.displayTrade.end() )
			configs.displayTrade.insert( map< cString, bool >::value_type( configs.underlyings[i], displayTradeArray[i] ) );
		else
			(*it5).second = displayTradeArray[i];
	}
	
	cSystem::FirstTimeInit( configs.dataoutputDirectory.c_str(), configs.tickDataFolderName.c_str(), configs.candleDataFolderName.c_str(), configs.logFileFolderName.c_str() );

}

void autotrade_loadconfig_downloadmarketdata( sATDownloadMarketDataConfig& configs )
{

	cString fileName;
	
	autotrade_loadconfig_general( configs.generalConfig, fileName );
	
	cString configDir = cString( cSystem::GetEnv( "MYDATADIR" ).c_str() ) + "configs\\" + fileName + ".txt";
	ifstream ifs;
	ifs.open( configDir.c_str(), ios::in );
	string line;
	
	bool foundLogFileName = false;

	while( getline( ifs, line ) )
	{
		string buff;
		stringstream ss( line );
		cArray< string > sbu;
		while( ss >> buff )
			sbu.push_back( buff );

		if( sbu.getSize() < 1 )
			continue;

		if( Compare( "LogFileName", sbu[0].c_str() ) )
		{
			configs.logFileName = cString( sbu[1].c_str() );
			foundLogFileName = true;
		}	
	}

	ifs.close();

	if( !foundLogFileName )
		configs.logFileName = theString;
	
}

void autotrade_loadconfig_backtest( sATBacktestConfig& configs )
{
	cString fileName;

	autotrade_loadconfig_general( configs.generalConfig, fileName );
	
	cString configDir = cString( cSystem::GetEnv( "MYDATADIR" ).c_str() ) + "configs\\" + fileName + ".txt";
	ifstream ifs;
	ifs.open( configDir.c_str(), ios::in );
	if( !ifs )
		yr_error( "file %s cannot be open!", configDir.c_str() );
	
	string line;
	bool foundStart = false;
	bool foundEnd = false;
	
	bool foundStrategyName = false;
	bool foundStrategyConfigFile = false;
	bool foundDataFormat = false;
	bool foundDisplaySignal = false;
	bool foundDataloadDirectory = false;
	
	while( getline( ifs, line ) )
	{
		string buff;
		stringstream ss( line );
		cArray< string > sbu;
		while( ss >> buff )
			sbu.push_back( buff );
		
		if( sbu.getSize() < 1 )
			continue;

		if( Compare( "Start", sbu[0].c_str() ) )
		{
			configs.dateStart = cString( sbu[1].c_str() );
			foundStart = true;
		}

		if( Compare( "End", sbu[0].c_str() ) )
		{
			configs.dateEnd = cString( sbu[1].c_str() );
			foundEnd = true;
		}

		if( Compare( "StrategyName", sbu[0].c_str() ) )
		{
			configs.strategyName = cString( sbu[1].c_str() );
			foundStrategyName = true;
		}
		
		if( Compare( "StrategyConfigFile", sbu[0].c_str() ) )
		{
			for( int i = 1; i < sbu.getSize(); ++i )
				configs.strategyConfigFileNames.push_back( cString( sbu[i].c_str() ) );
			foundStrategyConfigFile = true;
		}

		if( Compare( "DataFormat", sbu[0].c_str() ) )
		{
			configs.oldFormat = Compare( "Old", sbu[1].c_str() ) ? true : false;
			foundDataFormat = true;
		}

		if( Compare( "DataLoadDirectory", sbu[0].c_str() ) )
		{
			configs.dataLoadDirectory = cString( sbu[1].c_str() );
			foundDataloadDirectory = true;
		}
	}

	ifs.close();

	if( !foundStart )
		yr_error( "start date missing for backtesting" );

	if( !foundEnd )
		yr_error( "start date missing for backtesting" );

	if( !foundStrategyName )
		yr_error( "strategy name missing for backtesting" );

	if( !foundStrategyConfigFile )
		yr_error( "strategy config missing for backtesting" );
	else
	{
		// check whether the strategy config file exists on file
		ifstream ifs2;
		for( int i = 0; i < configs.strategyConfigFileNames.getSize(); ++i )
		{
			cString stratFullDir = cString( cSystem::GetEnv( "MYDATADIR" ).c_str() ) + "configs\\" + configs.strategyConfigFileNames[i] + ".txt";
			ifs2.open( stratFullDir.c_str(), ios::in );
			if( !ifs2 )
				yr_error( "file %s cannot be open!", stratFullDir.c_str() );
			ifs2.close();
		}
	}

	if( !foundDataFormat )
		configs.oldFormat = false;

	if( !foundDataloadDirectory )
		configs.dataLoadDirectory = "C\\Temp\\";


}

void autotrade_loadconfig_trade( sATTradeConfig& configs )
{
	cString fileName;

	autotrade_loadconfig_general( configs.generalConfig, fileName );
	
	cString configDir = cString( cSystem::GetEnv( "MYDATADIR" ).c_str() ) + "configs\\" + fileName + ".txt";
	ifstream ifs;
	ifs.open( configDir.c_str(), ios::in );
	if( !ifs )
		yr_error( "file %s cannot be open!", configDir.c_str() );
	
	string line;
	bool foundStrategyConfigFile = false;
	bool foundLogFileName = false;
	
	while( getline( ifs, line ) )
	{
		string buff;
		stringstream ss( line );
		cArray< string > sbu;
		while( ss >> buff )
			sbu.push_back( buff );
		
		if( sbu.getSize() < 1 )
			continue;

		if( Compare( "StrategyConfigFile", sbu[0].c_str() ) )
		{
			for( int i = 1; i < sbu.getSize(); ++i )
				configs.strategyConfigFileNames.push_back( cString( sbu[i].c_str() ) );
			foundStrategyConfigFile = true;
		}

		if( Compare( "LogFileName", sbu[0].c_str() ) )
		{
			configs.logFileName = cString( sbu[1].c_str() );
			foundLogFileName = true;
		}
	}

	ifs.close();

	if( !foundStrategyConfigFile )
		yr_error( "invalid StrategyConfigFile input!" );

	if( !foundLogFileName )
		configs.logFileName = theString;
}