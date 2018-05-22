#ifndef __YR_EXCEPTION_H__
#define __YR_EXCEPTION_H__

#include <string>
#include <exception>

using namespace std;

class yr_exception : public std::exception
{
public:
    yr_exception( const char* ) throw();
    yr_exception( const string& s ) throw();
    yr_exception( const yr_exception& rhs ) throw();

    virtual ~yr_exception() throw();
    virtual const char* what() const throw();

private:
    char* m_text;
};

#endif