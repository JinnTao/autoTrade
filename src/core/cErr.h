/***
*cout.h - declarations/definitions for coutorHandler class
*
*       Copyright (c) Yiran Yang. All rights reserved.
*
****/

#ifndef __coutOR_H__
#define __coutOR_H__

typedef const char* coutor;

class coutorHandler
{
public:
    coutorHandler( int line, char const* file );
    ~coutorHandler();

    const char* throwThis( char const* fmt, ... );

private:
    int _line;
    char* _file;
};

#define yr_error throw coutorHandler( __LINE__, __FILE__ ).throwThis

#ifdef _DEBUG
#define yr_assert( exp ) { if( !bool( exp ) ) throw coutorHandler( __LINE__, __FILE__ ).throwThis( "yr_assert( %s )", #exp ); }
#else
#define yr_assert( exp )
#endif


#endif
