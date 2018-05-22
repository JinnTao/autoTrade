#include <stdio.h>
#include <stdarg.h>
#include <cErr.h>
#include <stl.h>
#include <stl_iostream.h>

#pragma warning( disable:4996 )

int buffer = 0;
char bufferA[4096];
char bufferB[4096];

coutorHandler::coutorHandler( int line, char const* file )
{
    _line = line;
    _file = new char[ strlen( file ) + 1 ];
    strcpy( _file, file );
}

coutorHandler::~coutorHandler()
{
    delete [] _file;
}

const char* coutorHandler::throwThis( char const* fmt, ... )
{
    string msg;
    if( strlen( fmt ) > 0 )
    {
        va_list marker;
        char m[4096];
        va_start( marker, fmt );
        _vsnprintf( m, sizeof( m ) - 1, fmt, marker );
        va_end( marker );
        msg = m;
    }
    if( msg.length() < 7 || strcmp( msg.c_str() + msg.length() - 7, "(error)" ) )
        msg += " (error)";

    buffer = !buffer;
    if( buffer )
    {
        strcpy( bufferA, msg.c_str() );
        return bufferA;
    }
    else
    {
        strcpy( bufferB, msg.c_str() );
        return bufferB;
    }
}

