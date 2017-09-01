#include <cMdSpi.h>
#include <iostream>
#include <cSystem.h>
#include <cMarketDataCollection.h>

using namespace std;

extern HANDLE g_hEvent;
#pragma warning(disable : 4996)

cMdSpi::cMdSpi( CThostFtdcMdApi* pUserMdApi, TThostFtdcBrokerIDType brokerID, TThostFtdcInvestorIDType investorID, TThostFtdcPasswordType password, bool genLog )
: m_pUserMdApi( pUserMdApi )
, m_genLog( genLog )
, m_requestID( 0 )
, m_outputDirectory( theString )
, m_logFileFolder( theString )
, m_logFile( theString )
{
	strcpy_s( m_brokerID, sizeof( TThostFtdcBrokerIDType ), brokerID );
	strcpy_s( m_investorID, sizeof( TThostFtdcInvestorIDType ), investorID );
	strcpy_s( m_password, sizeof( TThostFtdcPasswordType ), password );
	m_status = false;
}

void cMdSpi::OnRspError( CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast )
{
	// copyright Yiran Yang
	char message[256];
	sprintf( message, "%s:called cMdSpi::OnRspError", cSystem::GetCurrentTimeBuffer().c_str() );
	cout << message << endl;
	IsErrorRspInfo( pRspInfo );
	
	// log info
	if( m_genLog )
	{
		if( m_outputDirectory.IsBlankString() )
			cSystem::WriteLogFile( m_logFile.c_str(), message, false );
		else
		{
			cString folderDir = m_outputDirectory + m_logFileFolder + "//"; 
			cSystem::WriteLogFile( folderDir.c_str(), m_logFile.c_str(), message, false );
		}
	}
}

void cMdSpi::OnFrontDisconnected( int nReason )
{
	// copyright Yiran Yang
	char message[256];
	sprintf( message, "%s:called cMdSpi::OnFrontDisconnected. Reason = %d", cSystem::GetCurrentTimeBuffer().c_str(), nReason );
	cout << message << endl;
	
	// log info
	if( m_genLog )
	{
		if( m_outputDirectory.IsBlankString() )
			cSystem::WriteLogFile( m_logFile.c_str(), message, false );
		else
		{
			cString folderDir = m_outputDirectory + m_logFileFolder + "//"; 
			cSystem::WriteLogFile( folderDir.c_str(), m_logFile.c_str(), message, false );
		}
	}
}
		
void cMdSpi::OnHeartBeatWarning(int nTimeLapse)
{
	// copyright Yiran Yang
	char message[256];
	sprintf( message, "%s:called cMdSpi::OnHeartBeatWarning. nTimerLapse = %d", cSystem::GetCurrentTimeBuffer().c_str(), nTimeLapse );
	cout << message << endl;

	// log info
	if( m_genLog )
	{
		if( m_outputDirectory.IsBlankString() )
			cSystem::WriteLogFile( m_logFile.c_str(), message, false );
		else
		{
			cString folderDir = m_outputDirectory + m_logFileFolder + "//"; 
			cSystem::WriteLogFile( folderDir.c_str(), m_logFile.c_str(), message, false );
		}
	}

}

void cMdSpi::OnFrontConnected()
{
	// copyright Yiran Yang
	char message[256];
	sprintf( message, "%s:called cMdSpi::OnFrontConnected.", cSystem::GetCurrentTimeBuffer().c_str() );
	cout << message << endl;

	// log info
	if( m_genLog )
	{
		if( m_outputDirectory.IsBlankString() )
			cSystem::WriteLogFile( m_logFile.c_str(), message, false );
		else
		{
			cString folderDir = m_outputDirectory + m_logFileFolder + "//"; 
			cSystem::WriteLogFile( folderDir.c_str(), m_logFile.c_str(), message, false );
		}
	}

	// request user login
	ReqUserLogin();
}

void cMdSpi::ReqUserLogin()
{
	// copyright Yiran Yang
	CThostFtdcReqUserLoginField req;
	memset( &req, 0, sizeof( req ) );
	strcpy_s( req.BrokerID, sizeof( TThostFtdcBrokerIDType ), m_brokerID );
	strcpy_s( req.UserID, sizeof( TThostFtdcInvestorIDType ), m_investorID );
	strcpy_s( req.Password, sizeof( TThostFtdcPasswordType ), m_password );

	int iResult = m_pUserMdApi->ReqUserLogin( &req, ++m_requestID );

	char message[256];
	sprintf( message, "%s:called cMdSpi::ReqUserLogin:%s", cSystem::GetCurrentTimeBuffer().c_str(), ( ( iResult == 0 ) ? "Success" : "Fail") );
	cout << message << endl;
	SetEvent(g_hEvent);

}

void cMdSpi::OnRspUserLogin(CThostFtdcRspUserLoginField* pRspUserLogin, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast )
{
	
	if( pRspUserLogin && !IsErrorRspInfo( pRspInfo ) )
	{
		char message[256];
		sprintf( message, "%s:called cMdSpi::OnRspUserLogin.", cSystem::GetCurrentTimeBuffer().c_str() );
		cout << message << endl;

	}
	if(bIsLast){ 
		SetEvent(g_hEvent);
		cerr << cSystem::GetCurrentTimeBuffer() << " Market init Finish" << endl;
		Sleep(1000);
		
		this->m_status = true;
	};
}

void cMdSpi::SubscribeMarketData(char *instIdList)
{
	////
	//const int NUM_INSTRUMENT = 200;
	//char* p[NUM_INSTRUMENT];
	//for( int i = 0; i < NUM_INSTRUMENT; ++i )
	//{
	//	if( i < m_instrumentIDs.getSize() )
	//		p[i] = m_instrumentIDs[i].c_str2();
	//	else
	//		p[i] = NULL;
	//}
	//
	//int iResult = m_pUserMdApi->SubscribeMarketData( p, m_instrumentIDs.getSize() );
	//
	//char message[256];
	//sprintf( message, "%s:called cMdSpi::SubscribeMarketData:%s", cSystem::GetCurrentTimeBuffer().c_str(), ( ( iResult == 0 ) ? "Success" : "Fail" ) );
	//cout << message << endl;

	//if( m_genLog )
	//{
	//	if( m_outputDirectory.IsBlankString() )
	//		cSystem::WriteLogFile( m_logFile.c_str(), message, false );
	//	else
	//	{
	//		cString folderDir = m_outputDirectory + m_logFileFolder + "//"; 
	//		cSystem::WriteLogFile( folderDir.c_str(), m_logFile.c_str(), message, false );
	//	}
	//}
	
	vector<char*> list;

	char *token = strtok(instIdList, ",");

	while( token != NULL ){

		list.push_back(token); 

		token = strtok(NULL, ",");

	}

	unsigned int len = list.size();

	char** pInstId = new char* [len];  

	for(unsigned int i=0; i<len;i++)  pInstId[i]=list[i]; // vector list 转到 char **

	int ret=m_pUserMdApi->SubscribeMarketData(pInstId, len);

	cerr<<" 请求 | 发送行情订阅... "<<((ret == 0) ? "成功" : "失败")<< endl;
	

	SetEvent(g_hEvent);
}


void cMdSpi::SubscribeForQuoteRsp()
{
	/*int iResult = m_pUserMdApi->SubscribeForQuoteRsp( ppInstrumentID, m_instrumentIDs.getSize() );
	cout << "--->>> cMdSpi::SubscribeForQuoteRsp: " << ( ( iResult == 0 ) ? "Success" : "Fail") << endl;*/
}

void cMdSpi::OnRspSubMarketData( CThostFtdcSpecificInstrumentField* pSpecificInstrument, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast )
{
	if( bIsLast && !IsErrorRspInfo( pRspInfo ) )
	{
		char message[256];
		sprintf( message, "%s:called cMdSpi::OnRspSubMarketData:%s", cSystem::GetCurrentTimeBuffer().c_str(), pSpecificInstrument->InstrumentID );
		cout << message << endl;
		if( m_genLog )
		{
			if( m_outputDirectory.IsBlankString() )
				cSystem::WriteLogFile( m_logFile.c_str(), message, false );
			else
			{
				cString folderDir = m_outputDirectory + m_logFileFolder + "//"; 
				cSystem::WriteLogFile( folderDir.c_str(), m_logFile.c_str(), message, false );
			}
		}
	}
	
}

void cMdSpi::OnRspSubForQuoteRsp( CThostFtdcSpecificInstrumentField* pSpecificInstrument, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast )
{
	cout << "--->>> cMdSpi::OnRspSubForQuoteRsp" << endl;
	if( bIsLast && !IsErrorRspInfo( pRspInfo ) )
	{
	}
}

void cMdSpi::OnRspUnSubMarketData( CThostFtdcSpecificInstrumentField* pSpecificInstrument, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast )
{
	cout << "--->>> cMdSpi::OnRspUnSubMarketData" << endl;
	if( bIsLast && !IsErrorRspInfo( pRspInfo ) )
	{
	}
}

void cMdSpi::OnRspUnSubForQuoteRsp( CThostFtdcSpecificInstrumentField* pSpecificInstrument, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast )
{
	cout << "--->>> cMdSpi::OnRspUnSubForQuoteRsp" << endl;
	if( bIsLast && !IsErrorRspInfo( pRspInfo ) )
	{
	}
}

void cMdSpi::OnRtnDepthMarketData( CThostFtdcDepthMarketDataField* pDepthMarketData )
{
	//copy right Yiran Yang
	m_pMktDataCollection->OnRtnDepthMarketData( pDepthMarketData );

}

void cMdSpi::OnRtnForQuoteRsp( CThostFtdcForQuoteRspField* pForQuoteRsp )
{
	cout << "--->>> cMdSpi::OnRtnForQuoteRsp" << endl;
}

bool cMdSpi::IsErrorRspInfo( CThostFtdcRspInfoField* pRspInfo )
{
	// 如果ErrorID != 0, 说明收到了错误的响应
	bool bResult = ( ( pRspInfo ) && ( pRspInfo->ErrorID != 0 ) );
	if( bResult )
	{
		char message[256];
		sprintf( message, "%s:ERROR with ErrorID = %d and ErrorMsg = %s.", cSystem::GetCurrentTimeBuffer().c_str(), pRspInfo->ErrorID, pRspInfo->ErrorMsg );
		cout << message << endl;
		if( m_genLog )
		{
			if( m_outputDirectory.IsBlankString() )
				cSystem::WriteLogFile( m_logFile.c_str(), message, false );
			else
			{
				cString folderDir = m_outputDirectory + m_logFileFolder + "//"; 
				cSystem::WriteLogFile( folderDir.c_str(), m_logFile.c_str(), message, false );
			}
		}
	}
	return bResult;
}

void cMdSpi::RegisterMarketDataCollection( cMarketDataCollection* pMktDataCollection )
{
	m_pMktDataCollection = pMktDataCollection;
	m_pMktDataCollection->Init( m_instrumentIDs );

	m_genLog = m_pMktDataCollection->GetGenLogFlag();
	if( m_genLog )
	{
		m_logFile = m_pMktDataCollection->GetLogFileName();
		m_outputDirectory = m_pMktDataCollection->GetOutputDirectory();
		m_logFileFolder = m_pMktDataCollection->GetLogFolderName();
	}



}
