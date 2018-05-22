#ifndef __YR_ASSERT_H__
#define __YR_ASSERT_H__

#include <stl.h>
#include <stl_iostream.h>
#include <yr_exception.h>

void throw_stream_contents( ostringstream& os, const char* file, int line );

#define YR_ERROR( MESSAGE ) \
{ \
    stringstream os; \
    os << MESSAGE; throw yr_exception( os.str() ); \
}


#define YR_ERR( MESSAGE ) \
{ \
    ostringstream os;    \
    os << MESSAGE;        \
    throw_stream_contents( os, __FILE__, __LINE__ ); throw("noop");\
}

#define YR_THROW( EXPT, MESSAGE ) \
{ \
    ostringstream os; os << ( MESSAGE ); \
    throw ( EXPT )( os.str() ); \
}

#define YR_ASSERT( ASSERTION, MESSAGE ) \
{ \
    if( !( ASSERTION ) ) \
    { \
        ostringstream os;    \
        os << MESSAGE;    os << " (" << __FILE__ << "," << __LINE__ << ")";    \
        throw yr_exception( os.str() );    \
    } \
}

#endif