#include <cStrategyMovAvg.h>
#include <cMarketDataCollection.h>
#include <cStrategy_statistics.h>
#include <cTickTime.h>
#include <cPositionCollection.h>
#include <cOrderCollection.h>
#include <cSystem.h>
#include <FuturesCNUtils.h>

const int NUM_PARAMETER = 9;
const cString strategy_movavg_parameter[NUM_PARAMETER] = 
{
	"LEAD",
	"LAG",
	"ALPHA",
	"EMAFLAG",
	"SAMPLEFREQUENCY",
	"MAXLEAD",
	"MAXLAG",
	"LEADINTERVAL",
	"LAGINTERVAL"
};

cString StrategyMovAvgParameterToString( const SwStrategyMovAvgParameter& param )
{
	if( !IsValidStrategyMovAvgParameter( param ) )
		return "";

	return strategy_movavg_parameter[param];
}

SwStrategyMovAvgParameter StringToStrategyMovAvgParameter( const cString& str )
{
	int i;
	for( i = 0; i < NUM_PARAMETER; ++i )
	{
		if( Compare( strategy_movavg_parameter[i], str ) )
			break;
	}
	if( !IsValidStrategyMovAvgParameter( i ) )
	{
		return SWSTRATEGYMOVAVGPARAMETER_INVALID;
	}

	return (SwStrategyMovAvgParameter)i;
}

const int NUM_INDICATOR = 2;
const cString strategy_movavg_indicator[NUM_INDICATOR] = 
{
	"MALEAD",
	"MALAG"
};

cString StrategyMovAvgIndicatorToString( const SwStrategyMovAvgIndicator& indicator )
{
	if( !IsValidStrategyMovAvgIndicator( indicator ) )
		return "";

	return strategy_movavg_indicator[indicator];
}

cStrategyMovAvg::cStrategyMovAvg()
: cStrategy()
{
}

cStrategyMovAvg::cStrategyMovAvg( const cString& id )
: cStrategy( id )
{
}

cStrategyMovAvg::cStrategyMovAvg( const cString& id, const cArray< cString >& instrumentIDs )
: cStrategy( id )
{
	this->Init( instrumentIDs );
}

cStrategyMovAvg::~cStrategyMovAvg()
{
}

void cStrategyMovAvg::Init( const cArray< cString >& instrumentIDs )
{
	cStrategy::Init( instrumentIDs );
	//
	/*
	default parameter values
	*/
	double paramLead = 1.0;			// set 1 as the price itself
	double paramLag = 2.0;
	double paramAlpha = 0.0;		// used when isEMA == FALSE 
	double paramIsEMA = TRUE;
	double paramSampleFreq = 1;		// sample frequency is 1 minute
	int paramNLead = 20;
	int paramNLag = 50;
	double maLead = -9.99;		// set -9.99 as it is not calibrated
	double maLag = -9.99;

	for( int i = 0; i < m_instrumentIDs.getSize(); ++i )
	{
		// set default parameter values
		SetParameter( StrategyMovAvgParameterToString( SwStrategyMovAvgParameter::SWSTRATEGYMOVAVGPARAMETER_LEAD ), paramLead );
		SetParameter( StrategyMovAvgParameterToString( SwStrategyMovAvgParameter::SWSTRATEGYMOVAVGPARAMETER_LAG ), paramLag );
		SetParameter( StrategyMovAvgParameterToString( SwStrategyMovAvgParameter::SWSTRATEGYMOVAVGPARAMETER_ALPHA ), paramAlpha );
		SetParameter( StrategyMovAvgParameterToString( SwStrategyMovAvgParameter::SWSTRATEGYMOVAVGPARAMETER_EMAFLAG ), paramIsEMA );
		SetParameter( StrategyMovAvgParameterToString( SwStrategyMovAvgParameter::SWSTRATEGYMOVAVGPARAMETER_SAMPLEFREQUENCY ), paramSampleFreq );
		SetParameter( StrategyMovAvgParameterToString( SwStrategyMovAvgParameter::SWSTRATEGYMOVAVGPARAMETER_MAXLEAD ), paramNLead );
		SetParameter( StrategyMovAvgParameterToString( SwStrategyMovAvgParameter::SWSTRATEGYMOVAVGPARAMETER_MAXLAG ), paramNLag );
		// set default indicator values
		SetIndicator( StrategyMovAvgIndicatorToString( SwStrategyMovAvgIndicator::SWSTRATEGYMOVAVGINDICATOR_MALEAD ), maLead );
		SetIndicator( StrategyMovAvgIndicatorToString( SwStrategyMovAvgIndicator::SWSTRATEGYMOVAVGINDICATOR_MALAG ), maLag );
	}
}

void cStrategyMovAvg::Train( const cArray< cTickTime>& startdts, const cArray< cTickTime>& enddts )
{
	//// optimize the parameters
	//if( m_instrumentIDs.getSize() == 0 )
	//	return;
	//yr_assert( m_instrumentIDs.getSize() == startdts.getSize() && m_instrumentIDs.getSize() == enddts.getSize() );

	//// for multiple contracts
	//int i,j,k;
	//for( i = 0; i < m_instrumentIDs.getSize(); ++i )
	//{
	//	cString id( m_instrumentIDs[i] );
	//	double nLead = GetParameter( StrategyMovAvgParameterToString( SwStrategyMovAvgParameter::SWSTRATEGYMOVAVGPARAMETER_MAXLEAD ) );
	//	double nLag = GetParameter(	StrategyMovAvgParameterToString( SwStrategyMovAvgParameter::SWSTRATEGYMOVAVGPARAMETER_MAXLAG ) );
	//	double alpha = GetParameter( StrategyMovAvgParameterToString( SwStrategyMovAvgParameter::SWSTRATEGYMOVAVGPARAMETER_ALPHA ) );
	//	double isEMA = GetParameter( StrategyMovAvgParameterToString( SwStrategyMovAvgParameter::SWSTRATEGYMOVAVGPARAMETER_EMAFLAG ) );

	//	cVector leadVec( 1.0, (int)nLead );
	//	cVector lagVec( 1.0, (int)nLag );
	//	for( j = 1; j < (int)nLead; ++j )
	//		leadVec[j] = leadVec[j-1] + 1.0;
	//	for( j = 1; j < (int)nLag; ++j )
	//		lagVec[j] = lagVec[j-1] + 1.0;

	//	
	//	yr_assert( startdts[i].TickTimeToDouble() <= enddts[i].TickTimeToDouble() );
	//	
	//	double maxSharpRatio = -99.99;
	//	double leadCalibrated, lagCalibrated, totalpnl;
	//	cMarketData* md_i = m_mc->GetMarketDataHandle( id );
	//	cVector time, open, high, low, close, px;

	//	//@todo: incorporate with sample frequency
	//	md_i->GetCandleStick( time, open, high, low, close );
	//	
	//	datawindow( time, close, startdts[i], enddts[i], px );
	//	
	//	for( j = 0; j < leadVec.getSize(); ++j )
	//	{
	//		double lead = leadVec[j];
	//		for( k = 0; k < lagVec.getSize(); ++k )
	//		{
	//			double lag = lagVec[k];
	//			if( lag > lead )
	//			{
	//				cVector pnl;
	//				calcPnL( lead, lag, alpha, isEMA, px, pnl );
	//				double sr = sharpratio( pnl );
	//				if( equal( maxSharpRatio, -99.99 ) )
	//				{
	//					maxSharpRatio = sr;
	//					SetParameter( StrategyMovAvgParameterToString( SwStrategyMovAvgParameter::SWSTRATEGYMOVAVGPARAMETER_LEAD ), lead );
	//					SetParameter( StrategyMovAvgParameterToString( SwStrategyMovAvgParameter::SWSTRATEGYMOVAVGPARAMETER_LAG ), lag );
	//					leadCalibrated = lead;
	//					lagCalibrated = lag;
	//					totalpnl = pnl.Sum();
	//				}
	//				if( sr > maxSharpRatio )
	//				{
	//					maxSharpRatio = sr;
	//					SetParameter( StrategyMovAvgParameterToString( SwStrategyMovAvgParameter::SWSTRATEGYMOVAVGPARAMETER_LEAD ), lead );
	//					SetParameter( StrategyMovAvgParameterToString( SwStrategyMovAvgParameter::SWSTRATEGYMOVAVGPARAMETER_LAG ), lag );
	//					leadCalibrated = lead;
	//					lagCalibrated = lag;
	//					totalpnl = pnl.Sum();
	//				}
	//			}
	//		}
	//	}
	//	// debug
	//	cout << "contract: " << m_instrumentIDs[i].c_str() << endl;
	//	cout << "maximum sharp ratio: " << maxSharpRatio << endl;
	//	cout << "total pnl: " << totalpnl << endl;
	//	cout << "parameter lead " << leadCalibrated << endl;
	//	cout << "parameter lag " << lagCalibrated << endl;
	//	cout << endl;
	//}

}


void cStrategyMovAvg::BackTest( const cArray< cTickTime >& startdts, const cArray< cTickTime >& enddts, cArray< cStrategyResults>& results )
{
	//yr_assert( startdts.getSize() == enddts.getSize() && startdts.getSize() == m_instrumentIDs.getSize() );
	//results.resize( m_instrumentIDs.getSize() );
	//
	//for( int i = 0; i < m_instrumentIDs.getSize(); ++i )
	//{
	//	StrategyParameters::iterator it = m_params.find( m_instrumentIDs[i] );
	//	if( it == m_params.end() )
	//		yr_error( "parameters not set for s%", m_instrumentIDs[i].c_str() );
	//	
	//	// assume the parameters are already trained
	//	double lead = GetParameter( StrategyMovAvgParameterToString( SwStrategyMovAvgParameter::SWSTRATEGYMOVAVGPARAMETER_LEAD ) );
	//	double lag = GetParameter( StrategyMovAvgParameterToString( SwStrategyMovAvgParameter::SWSTRATEGYMOVAVGPARAMETER_LAG ) );
	//	double alpha = GetParameter( StrategyMovAvgParameterToString( SwStrategyMovAvgParameter::SWSTRATEGYMOVAVGPARAMETER_ALPHA ) );
	//	double isEMA = GetParameter( StrategyMovAvgParameterToString( SwStrategyMovAvgParameter::SWSTRATEGYMOVAVGPARAMETER_EMAFLAG ) );
	//		
	//	cMarketData* md_i = m_mc->GetMarketDataHandle( m_instrumentIDs[i] );
	//	cVector time, open, high, low, close, px;

	//	//@todo: incorporate with sample frequency
	//	md_i->GetCandleStick( time, open, high, low, close );

	//	datawindow( time, close, startdts[i], enddts[i], px );
	//	cVector pnl;
	//	calcPnL( lead, lag, alpha, isEMA, px, pnl );
	//	struct cStrategyResults result;
	//	double peak;
	//	result.pnl = pnl.Sum();
	//	result.sharpratio = sharpratio( pnl );
	//	result.drawdown = drawdown( pnl, peak );
	//	result.peak = peak;
	//	results[i] = result;
	//}
}

void cStrategyMovAvg::calcPnL( double lead, double lag, double alpha, double isEMA, const cVector& px, cVector& pnl )
{
	yr_assert( lead < lag );
	yr_assert( px.getSize() >= lag );

	int ilead = (int)lead;
	int ilag = (int)lag;

	cVector leadMA, lagMA;
	movavg( px, leadMA, ilead, alpha, isEMA );
	movavg( px, lagMA, ilag, alpha, isEMA );
	cIvector signal( lagMA.getSize() );
	pnl.resize( lagMA.getSize() );
	for( int l = 0; l < lagMA.getSize(); ++l )
	{
		int idx_lead = equal( isEMA, 1.0 ) ? l : l + ilag - ilead;
		int idx_px = equal( isEMA, 1.0 ) ? l : l + ilag - 1;
		signal[l] = leadMA[idx_lead] > lagMA[l] ? 1 : leadMA[idx_lead] < lagMA[l] ? -1 : 0;
		// very simple way of calculate PnL
		// i.e. there is no change of position once the signal remains the same
		if( l == 0 )
			pnl[l] = 0.0;
		else
			pnl[l] = signal[l-1] * ( px[idx_px] - px[idx_px-1] );
	}
}

bool cStrategyMovAvg::RealTimeIndicatorSignalUpdate( const cString& instrumentID )
{
	//bool updateFlag = false;
	//
	//cMarketData* md = m_mc->GetMarketDataHandle( instrumentID );
	//if( !md )
	//	return false;

	//cString underlying; 
	//InstrumentToUnderlying( instrumentID, underlying );
	//bool displaySignal = GetDisplaySignalFlag( underlying );

	//const cArray< cCandle >* candles = md->GetCandleStick();
	////@todo: incorporate with sample frequency
	//int nCandle = candles->getSize();
	//if( nCandle < 1 )
	//{
	//	int idx = GetPXLastUsedIndex();
	//	if( idx >= 0 )
	//		SetPXLastUsedIndex( -1 );


	//	return false;
	//}

	//double maLead = GetIndicator( StrategyMovAvgIndicatorToString( SwStrategyMovAvgIndicator::SWSTRATEGYMOVAVGINDICATOR_MALEAD ) );
	//double maLag = GetIndicator( StrategyMovAvgIndicatorToString( SwStrategyMovAvgIndicator::SWSTRATEGYMOVAVGINDICATOR_MALAG ) );
	//double isEMA = GetParameter( StrategyMovAvgParameterToString( SwStrategyMovAvgParameter::SWSTRATEGYMOVAVGPARAMETER_EMAFLAG ) );
	//double lead = GetParameter( StrategyMovAvgParameterToString( SwStrategyMovAvgParameter::SWSTRATEGYMOVAVGPARAMETER_LEAD ) );
	//double lag = GetParameter( StrategyMovAvgParameterToString( SwStrategyMovAvgParameter::SWSTRATEGYMOVAVGPARAMETER_LAG ) );
	//
	//if( equal( isEMA, 1.0 ) )
	//{
	//	//EMA is used
	//	int idx = GetPXLastUsedIndex();
	//	if( idx < 0 && equal( maLead, -9.99 ) && equal( maLag, -9.99 ) )
	//	{	
	//		double dClose = (*candles)[0].Close();
	//		maLead = dClose;
	//		maLag = dClose;
	//		SetIndicator( StrategyMovAvgIndicatorToString( SwStrategyMovAvgIndicator::SWSTRATEGYMOVAVGINDICATOR_MALEAD ), maLead );
	//		SetIndicator( StrategyMovAvgIndicatorToString( SwStrategyMovAvgIndicator::SWSTRATEGYMOVAVGINDICATOR_MALAG ), maLag );
	//		SetPXLastUsedIndex( 0 );

	//		int direction = -1;
	//		if( maLead > maLag )
	//			direction = (int)SWSIGNALDIRECTION_BUY;
	//		else if( maLead < maLag )
	//			direction = (int)SWSIGNALDIRECTION_SELL;
	//		if( !IsValidOrderDirection( direction ) )
	//			return false;

	//		double px = md->GetLastTick().Last();
	//		cTickTime dt = md->GetLastTick().TickTime();
	//		
	//		shared_ptr< cSignal > signal = make_shared< cSignal >();
	//		signal->SetDirection( direction );
	//		signal->SetInstrumentID( instrumentID );
	//		signal->SetPrice( px );
	//		signal->SetTime( dt );
	//		signal->SetVolume( 1 );
	//		SetSignal( signal );
	//		if( displaySignal )
	//		{
	//			cout << dt.DateString().c_str() << " " << dt.TimeString().c_str() << ":";
	//			cout << instrumentID.c_str() << " >>> MA("<< lead << "): " << maLead << ":MA("<< lag << "): " << maLag;
	//			cout << " signal: " << SignalDirectionToString( SwSignalDirection( direction ) ).c_str() << " at " << px << endl;
	//		}
	//		updateFlag = true;
	//	}
	//	else
	//	{
	//		if( idx < nCandle - 1 )
	//		{
	//			double lastClose = candles->getLast().Close();
	//			double alpha_lead = 2.0 / ( 1.0 + lead );
	//			double alpha_lag = 2.0 / ( 1.0 + lag );
	//			maLead = maLead + alpha_lead * ( lastClose - maLead );
	//			maLag = maLag + alpha_lag * ( lastClose - maLag );
	//			SetIndicator( StrategyMovAvgIndicatorToString( SwStrategyMovAvgIndicator::SWSTRATEGYMOVAVGINDICATOR_MALEAD ), maLead );
	//			SetIndicator( StrategyMovAvgIndicatorToString( SwStrategyMovAvgIndicator::SWSTRATEGYMOVAVGINDICATOR_MALAG ), maLag );
	//			SetPXLastUsedIndex( nCandle-1 );

	//			int direction = -1;
	//			if( maLead > maLag )
	//				direction = (int)SWSIGNALDIRECTION_BUY;
	//			else if( maLead < maLag )
	//				direction = (int)SWSIGNALDIRECTION_SELL;
	//			
	//			if( !IsValidOrderDirection( direction ) )
	//				return false;

	//			double px = md->GetLastTick().Last();
	//			cTickTime dt = md->GetLastTick().TickTime();
	//				
	//			shared_ptr< cSignal > signal = make_shared< cSignal >();
	//			signal->SetDirection( direction );
	//			signal->SetInstrumentID( instrumentID );
	//			signal->SetPrice( px );
	//			signal->SetTime( dt );
	//			signal->SetVolume( 1 );
	//			SetSignal( signal );

	//			if( displaySignal )
	//			{
	//				cout << dt.DateString().c_str() << " " << dt.TimeString().c_str() << ":";
	//				cout << instrumentID.c_str() << " >>> MA("<< lead << "): " << maLead << ":MA("<< lag << "): " << maLag;
	//				cout << " signal: " << SignalDirectionToString( SwSignalDirection( direction ) ).c_str() << " at " << px << endl;
	//			}
	//			updateFlag = true;
	//		}
	//	}
	//}
	//else
	//{
	//	// non-exponential moving average
	//	// @todo
	//}
	//return updateFlag;
	return true;
}

bool cStrategyMovAvg::SimTimeIndicatorSignalUpdate( const cTick& tick )
{
//	cString id = tick.InstrumentID();
//	cString underlying; 
//	InstrumentToUnderlying( id, underlying );
//	cMarketData* md = m_mc->GetMarketDataHandle( id );
//	if( !md )
//		return false;
//
//	bool displaySignal = GetDisplaySignalFlag( underlying );
//
//	const cArray< cCandle >* candles = md->GetCandleStick();
//	//@todo: incorporate with sample frequency
//	int nCandle = candles->getSize();
//	if( nCandle < 1 )
//	{
//		int idx = GetPXLastUsedIndex();
//		if( idx >= 0 )
//		SetPXLastUsedIndex( -1 );
//		return false;
//	}
//
//	double maLead = GetIndicator( StrategyMovAvgIndicatorToString( SwStrategyMovAvgIndicator::SWSTRATEGYMOVAVGINDICATOR_MALEAD ) );
//	double maLag = GetIndicator( StrategyMovAvgIndicatorToString( SwStrategyMovAvgIndicator::SWSTRATEGYMOVAVGINDICATOR_MALAG ) );
//	double isEMA = GetParameter( StrategyMovAvgParameterToString( SwStrategyMovAvgParameter::SWSTRATEGYMOVAVGPARAMETER_EMAFLAG ) );
//	double lead = GetParameter( StrategyMovAvgParameterToString( SwStrategyMovAvgParameter::SWSTRATEGYMOVAVGPARAMETER_LEAD ) );
//	double lag = GetParameter( StrategyMovAvgParameterToString( SwStrategyMovAvgParameter::SWSTRATEGYMOVAVGPARAMETER_LAG ) );
//	
//	if( equal( isEMA, 1.0 ) )
//	{
//			//EMA is used
//		int idx = GetPXLastUsedIndex();
//		if( idx < 0 && equal( maLead, -9.99 ) && equal( maLag, -9.99 ) )
//		{	
//			double dClose = (*candles)[0].Close();
//			maLead = dClose;
//			maLag = dClose;
//			SetIndicator( StrategyMovAvgIndicatorToString( SwStrategyMovAvgIndicator::SWSTRATEGYMOVAVGINDICATOR_MALEAD ), maLead );
//			SetIndicator( StrategyMovAvgIndicatorToString( SwStrategyMovAvgIndicator::SWSTRATEGYMOVAVGINDICATOR_MALAG ), maLag );
//			SetPXLastUsedIndex( 0 );
//
//			int direction = -1;
//			if( maLead > maLag )
//				direction = (int)SWSIGNALDIRECTION_BUY;
//			else if( maLead < maLag )
//				direction = (int)SWSIGNALDIRECTION_SELL;
//
//			if( !IsValidOrderDirection( direction ) )
//				return false;
//
//			double px = md->GetLastTick().Last();
//			cTickTime dt = md->GetLastTick().TickTime();
//
//			shared_ptr< cSignal > signal = make_shared< cSignal >();
//			signal->SetDirection( direction );
//			signal->SetInstrumentID( id );
//			signal->SetPrice( px );
//			signal->SetTime( dt );
//			signal->SetVolume( 1 );
//			SetSignal( signal );
//
//			if( displaySignal )
//			{
//				cout << dt.DateString().c_str() << " " << dt.TimeString().c_str() << ":";
//				cout << id.c_str() << " >>> MA("<< lead << "): " << maLead << ":MA("<< lag << "): " << maLag;
//				cout << " signal: " << SignalDirectionToString( SwSignalDirection( direction ) ).c_str() << " at " << px << endl;
//			}
//			return true;
//		}
//		else
//		{
//			if( idx < nCandle - 1 )
//			{
//				double lastClose = candles->getLast().Close();
//				double alpha_lead = 2.0 / ( 1.0 + lead );
//				double alpha_lag = 2.0 / ( 1.0 + lag );
//				maLead = maLead + alpha_lead * ( lastClose - maLead );
//				maLag = maLag + alpha_lag * ( lastClose - maLag );
//				SetIndicator( StrategyMovAvgIndicatorToString( SwStrategyMovAvgIndicator::SWSTRATEGYMOVAVGINDICATOR_MALEAD ), maLead );
//				SetIndicator( StrategyMovAvgIndicatorToString( SwStrategyMovAvgIndicator::SWSTRATEGYMOVAVGINDICATOR_MALAG ), maLag );
//				SetPXLastUsedIndex( nCandle-1 );
//
//				int direction = -1;
//				if( maLead > maLag )
//					direction = (int)SWSIGNALDIRECTION_BUY;
//				else if( maLead < maLag )
//					direction = (int)SWSIGNALDIRECTION_SELL;
//
//				if( !IsValidOrderDirection( direction ) )
//					return false;
//
//				double px = md->GetLastTick().Last();
//				cTickTime dt = md->GetLastTick().TickTime();
//
//				shared_ptr< cSignal > signal = make_shared< cSignal >();
//				signal->SetDirection( direction );
//				signal->SetInstrumentID( id );
//				signal->SetPrice( px );
//				signal->SetTime( dt );	
//				signal->SetVolume( 1 );
//				SetSignal( signal );
//
//				if( displaySignal )
//				{
//					cout << dt.DateString().c_str() << " " << dt.TimeString().c_str() << ":";
//					cout << id.c_str() << " >>> MA("<< lead << "): " << maLead << ":MA("<< lag << "): " << maLag;
//					cout << " signal: " << SignalDirectionToString( SwSignalDirection( direction ) ).c_str() << " at " << px << endl;
//				}
//
//				return true;
//			}
//		}
//		return false;
//	}
//	else
//	{
//		// non-exponential moving average
//		// @todo
//		return false;
//	}
	return 0;
}

cArray< cOrder* > cStrategyMovAvg::GenNewOrders( const cString& instrumentID, cPositionCollection* positions )
{
	//cArray< cOrder* > orders;

	//cSignal* signal = GetLastSignal( instrumentID );
	//if( !signal )
	//	return orders;

	//char direction = signal->GetDirection();
	//int volume = signal->GetVolume();
	//double price = signal->GetPrice();
	//	
	//cArray< const cPositionDetail* > pos_i = positions->GetPositionDetailByInstrument( instrumentID );
	//int volumeLong, volumeShort, volumeLongToday, volumeShortToday;
	//positions->SummaryByInstrument( instrumentID, volumeLong, volumeShort, volumeLongToday, volumeShortToday );

	//if( direction == '0' )
	//{
	//	// long signal
	//	// unwind existing short positions first
	//	if( volumeShort > 0 )
	//	{
	//		//first unwind carry positions
	//		if( volumeShort - volumeShortToday > 0 )
	//		{
	//			cOrder* order = new cOrder();
	//			order->SetInstrumentID( instrumentID );
	//			order->SetDirection( 0 );
	//			order->SetOffsetFlag( 4 );
	//			order->SetVolumeOriginal( volumeShort - volumeShortToday );
	//			order->SetVolumeTraded( volumeShort - volumeShortToday );
	//			order->SetPrice( price );
	//			order->SetOrderTime( signal->GetTime() );
	//			orders.push_back( order );
	//		}
	//		//secondly unwind today's positions
	//		if( volumeShortToday > 0 )
	//		{
	//			cOrder* order = new cOrder();
	//			order->SetInstrumentID( instrumentID );
	//			order->SetDirection( 0 );
	//			order->SetOffsetFlag( 3 );
	//			order->SetVolumeOriginal( volumeShortToday );
	//			order->SetVolumeTraded( volumeShortToday );
	//			order->SetPrice( price );
	//			order->SetOrderTime( signal->GetTime() );
	//			orders.push_back( order );
	//		}
	//	}
	//	//
	//	if( volumeLong > 0 )
	//	{
	//		// nothing to do
	//	}
	//	else
	//	{
	//		// send orders to open new positions
	//		cOrder* order = new cOrder();
	//		order->SetInstrumentID( instrumentID );
	//		order->SetDirection( 0 );
	//		order->SetOffsetFlag( 0 );
	//		order->SetVolumeOriginal( volume );
	//		order->SetVolumeTraded( volume );
	//		order->SetPrice( price );
	//		order->SetOrderTime( signal->GetTime() );
	//		orders.push_back( order );
	//	}
	//}
	//else
	//{
	//	// short signal
	//	// // unwind existing long positions first 
	//	if( volumeLong > 0 )
	//	{
	//		//first unwind carry positions
	//		if( volumeLong - volumeLongToday > 0 )
	//		{
	//			cOrder* order = new cOrder();
	//			order->SetInstrumentID( instrumentID );
	//			order->SetDirection( 1 );
	//			order->SetOffsetFlag( 4 );
	//			order->SetVolumeOriginal( volumeLong - volumeLongToday );
	//			order->SetVolumeTraded( volumeLong - volumeLongToday );
	//			order->SetPrice( price );
	//			order->SetOrderTime( signal->GetTime() );
	//			orders.push_back( order );
	//		}
	//		//secondly unwind today's positions
	//		if( volumeLongToday > 0 )
	//		{
	//			cOrder* order = new cOrder();
	//			order->SetInstrumentID( instrumentID );
	//			order->SetDirection( 1 );
	//			order->SetOffsetFlag( 3 );
	//			order->SetVolumeOriginal( volumeLongToday );
	//			order->SetVolumeTraded( volumeLongToday );
	//			order->SetPrice( price );
	//			order->SetOrderTime( signal->GetTime() );
	//			orders.push_back( order );
	//		}
	//	}
	//	//
	//	if( volumeShort > 0 )
	//	{
	//		// nothing to do
	//	}
	//	else
	//	{
	//		cOrder* order = new cOrder();
	//		order->SetInstrumentID( instrumentID );
	//		order->SetDirection( 1 );
	//		order->SetOffsetFlag( 0 );
	//		order->SetVolumeOriginal( volume );
	//		order->SetVolumeTraded( volume );
	//		order->SetPrice( price );
	//		order->SetOrderTime( signal->GetTime() );
	//		orders.push_back( order );
	//	}
	//}
	//return orders;
	return 0;
}

bool cStrategyMovAvg::GenNewOrders( cPositionCollection* positions )
{
	double m_stop = 10000.0;
	double m_loss = -10000.0;
	//
	//double pnl = PositionPnL( positions ) + m_closePnL;

	//if( pnl >= m_stop )
	//{
	//	if( positions->Count() > 1 )
	//	{
	//		// send close orders to unwind all existing deals
	//	}
	//	else
	//		return false;
	//}

	//if( pnl <= m_loss )
	//{
	//	if( positions->Count() > 1 )
	//	{
	//		// send close orders to unwind all existing deals
	//	}
	//	else
	//		return false;
	//}

	// pnl is between stop & loss range



	


	
	
	return false;
}

void cStrategyMovAvg::CancelPendingOrders( const cString& instrumentID, cOrderCollection* orders, cIvector& orderIDs )
{
	cArray< const cOrder* > orderArray = orders->GetOrderByInstrument( instrumentID );
	for( int i = 0; i < orderArray.getSize(); ++i )
	{
		if( orderArray[i]->IsPendingOrder() )
			orderIDs.push_back( orderArray[i]->GetOrderID() );
	}
}

void cStrategyMovAvg::LoadStrategyParameter( const cString& fileName )
{
	//cString fileFullDir = cString( cSystem::GetEnv( "MYDATADIR" ).c_str() ) + "configs\\" + fileName + ".txt";
	//ifstream ifs;
	//ifs.open( fileFullDir.c_str(), ios::in );
	//if( !ifs )
	//	yr_error( "file %s cannot be open!", fileFullDir.c_str() );

	//string line;
	//while( getline( ifs, line ) )
	//{
	//	string buff;
	//	stringstream ss( line );
	//	cArray< string > sbu;
	//	while( ss >> buff )
	//		sbu.push_back( buff );

	//	if( sbu.getSize() == 2 )
	//	{
	//		SwStrategyMovAvgParameter p = StringToStrategyMovAvgParameter( cString( sbu[0].c_str() ) );
	//		if( IsValidStrategyMovAvgParameter( p ) )
	//		{
	//			cString s = StrategyMovAvgParameterToString( p );
	//			param.insert( StrategyParameters::value_type( s, atof( sbu[1].c_str() ) ) );
	//		}
	//	}

	//}

	//ifs.close();
}