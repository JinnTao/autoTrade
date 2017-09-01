#ifndef __CSTRINGDEFS_H__
#define __CSTRINGDEFS_H__

#include <string.h>
#include <set>
#include <cString.h>

struct InsensitiveStringCompare
{
	bool operator() ( const cString& s1, const cString& s2 ) const
	{
		return Norm( s1.c_str(), s2.c_str() ) < 0;
	}
};

typedef std::set< cString, InsensitiveStringCompare > InsensitiveStringSet;

#endif
