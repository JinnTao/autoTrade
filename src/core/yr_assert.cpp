#include <stl.h>
#include <stl_iostream.h>

#include <yr_assert.h>

namespace
{
    bool buffer = false;

    char bufferA[4096];
    char bufferB[4096];
}

#pragma warning(disable: 4996)

void throw_stream_contents( ostringstream& os, const char* file, int line )
{
    const char* str_to_throw = 0;
    std::string msg = os.str();
    if( msg.length() < 7 || strcmp( msg.c_str() + msg.length() - 7, "(error)" ) )
    {
        os << "(error)";
    }
    msg = os.str();

    buffer = !buffer;
    if( buffer )
    {
        strncpy( bufferA, msg.c_str(), sizeof(bufferA) - 1);
        str_to_throw = bufferA;
    }
    else
    {
        strncpy( bufferB, msg.c_str(), sizeof(bufferB) - 1);
        str_to_throw = bufferB;
    }

    throw str_to_throw;
}
