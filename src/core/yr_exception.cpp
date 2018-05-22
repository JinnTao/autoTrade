#include <yr_exception.h>

#define STRDUP(x) _strdup(x)

yr_exception::yr_exception( const char* text ) throw()
: exception()
{
    m_text = STRDUP( text );
}

yr_exception::yr_exception( const string& text ) throw()
: exception()
{
    m_text = STRDUP( text.c_str() );
}

yr_exception::yr_exception( const yr_exception& rhs ) throw()
: exception( rhs )
{
    m_text = STRDUP( rhs.m_text );
}

yr_exception::~yr_exception() throw()
{
    free( m_text );
}

const char* yr_exception::what() const throw()
{
    return m_text;
}