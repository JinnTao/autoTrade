/***
*math_defs.h - mathematical definition
*
*       Copyright (c) Yiran Yang. All rights reserved.
*
****/

#ifndef __MATH_DEFS_H__
#define __MATH_DEFS_H__

#include <stdlib.h>
#include <math.h>
#include <cErr.h>
#include <cvector_math.h>

#ifdef SIGN
#undef SIGN
#endif

inline double yr_sign( const double& a, const double& b )
{
    return fabs( a ) * ( b >= 0.0 ? 1 : -1 );
}

#ifdef ABS
#undef ABS
#endif

#ifndef FUZZ
#define FUZZ 1e-10
#endif

#define MFUZZ 1e-7

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#undef max
inline double max( double x, double y )
{
    return x > y ? x : y;
}

inline int max( int x, int y )
{
    return x > y ? x : y;
}

#undef min
inline double min( double x, double y )
{
    return x < y ? x : y;
}

inline int min( int x, int y )
{
    return x < y ? x : y;
}

#undef sqr
inline double sqr( double x )
{
    return x*x;
}

inline int sqr( int x )
{
    return x*x;
}

#define MINUS_INFINITY -1.0e100
#define PLUS_INFINITY 1.0e100

inline int equal( double a, double b )
{
    return( fabs( a - b ) <= FUZZ );
}

inline int equal( int a, int b, int tol = 0 )
{
    return( abs( a - b ) <= tol );
}


inline double yr_divide( double x, double y )
{
    if( fabs(y) < FUZZ )
    {
        //print errors here
        yr_error( "Division by zero" );
    }
    return( x / y );
}


inline double yr_divide( double x, double y, double default_value )
{
    if( fabs(y) < FUZZ )
        return default_value;
    else
        return( x / y );
}

inline double yr_divide( double x, double y, double default_value, double fuzz )
{
    if( fabs(y) < fuzz )
        return default_value;
    else
        return( x / y );
}

inline double yr_divide_smooth( double x, double y, double default_value, double fuzz )
{
    if( fabs(y) < FUZZ )
    {
        x /= fuzz;
        y /= fuzz;
        return ( 1. - y*y)*default_value + x * y;
    }
    else
        return( x / y );
}

inline int yr_int_divide( int x, int y, int default_value )
{
    if( y == 0 )
        return default_value;
    else
        return( x / y );
}

template< class T >
inline T yr_min( const T& t1, const T& t2 )
{
    if( t1 < t2 )
        return t1;
    else
        return t2;
}

template< class T >
inline void yr_min_max( const T& t1, const T& t2, T& minValue, T& maxValue )
{
    if( t1 < t2 )
    {
        minValue =  t1;
        maxValue = t2;
    }
    else
    {
        minValue = t2;
        maxValue = t1;
    }
}

#ifdef MIN
#undef MIN
#endif

template< class T >
inline T yr_max( const T& t1, const T& t2 )
{
    if( t1 > t2 )
        return t1;
    else
        return t2;
}

#ifdef MAX
#undef MAX
#endif

template< class T >
inline T yr_minmax( const T& theMin, const T& theMax, const T& theValue )
{
    if( theValue <= theMin )
        return theMin;
    else if( theValue >= theMax )
        return theMax;
    else
        return theValue;
}

template< class T >
inline T yr_abs( const T& x )
{
    return( x > 0 ? x : -x );
}

template< class X >
void yr_swap( X& a, X& b )
{
    if( &a != &b )
    {
        X tmp;
        tmp = a;
        a = b;
        b = tmp;
    }
}

inline double yr_asinh( double x )
{
    return log( x + sqrt( x * x + 1. ) );
}

inline double yr_acosh( double x )
{
    if( x < 1.0 )
    {
        // print erros here
        yr_error( "acosh parameter must be >= 1.0, not %s", x );
    }

    return log( x + sqrt( x * x - 1. ) );
}

inline double yr_atanh( double x )
{
    if( fabs( x ) >= 1.0 )
    {
        // print errors here
        yr_error( "atanh parameter must be > -1.0 and < 1.0, not %s", x );
    }
    return log( ( 1. + x ) / ( 1. - x ) ) / 2;
}

#define YR_PI        3.14159265358979323846
#define YR_SQRT2    1.41421356237309504880


#endif
