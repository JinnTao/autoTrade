#include "cStrategy_statistics.h"
#include "cVector.h"
#include "cTickTime.h"

void datawindow( const cVector& time, const cVector& px_in, const cTickTime& startdt, const cTickTime& enddt, cVector& px_out )
{
    // return the px between startdt and endt
    double tstart = startdt.TickTimeToDouble();
    double tend = enddt.TickTimeToDouble();
    int istart = time.location( tstart );
    if( tstart < time[istart]  )
    {
        istart--;
        istart = max( 0, istart );
    }
    int iend = time.location( tend );
    if( time[iend] < tend )
    {
        iend++;
        iend = min( time.getSize()-1, iend );
    }
    yr_assert( istart <= iend );
    px_out.resize( iend - istart + 1 );
    for( int i = 0; i < iend - istart + 1; ++i )
        px_out[i] = px_in[istart+i];
}


double sharpratio( const cVector& pnl, double scale )
{
    double a;
    double b = standard_dev( pnl, a );
    return ( a / b * sqrt( 252 ) * scale );
}

double drawdown( const cVector& pnl, double& peak )
{
    int n = pnl.getSize();
    if( n < 1 )
        return 0.0;
    double cpnl = pnl[0];
    peak = cpnl;
    double dd = peak - cpnl;

    for( int i = 1; i < n; ++i )
    {
        cpnl += pnl[i];
        if( cpnl > peak )
            peak = cpnl;
        if( dd < peak - cpnl )
            dd = peak - cpnl;
    }
    return -dd;
}

void movavg( const cVector& input, cVector& output, int lag, double alpha, bool isexp )
{
    // alpha is the control parameter which determines what types of moving average are calulated
    // alpha = 0 (default) corresponds to a simple moving average
    // alpha = 0.5 to a square root weighted moving average
    // alpha = 1 to a linear moving average
    // alpha = 2 to a square weighted moving average
    // isexp = true to calculate the exponential moving

    if( isexp == true )
    {
        // calculate smoothing constant
        double alpha_ = 2.0 / ( 1.0 + lag );
        output.resize( input.getSize() );
        // the first expoential average is the first input entry
        output[0] = input[0];
        for( int i = 1; i < output.getSize(); ++i )
            output[i] = output[i-1] + alpha_ * (input[i] - output[i-1] );
        return;
    }


    cVector temp( lag );
    for( int i = 1; i <= lag; ++i )
    {
        if( equal( alpha, 0.0 ) ) 
            temp[i-1] = 1.0;
        else
            temp[i-1] = pow( lag - i + 1, alpha );
    }
    double weights_sum = temp.Sum();
    cVector weights = yr_divide( temp, weights_sum, 0.0 );
    output.resize( input.getSize() - lag + 1 );
    for( int i = lag-1; i < input.getSize(); ++i )
    {
        double y = 0.0;
        for( int j = lag-1; j >=0; j-- )
            y += weights[j] * input[i-j];
        output[i-lag+1] = y;
    }
    


}