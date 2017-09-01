#ifndef __STL_IOSTREAM_H__
#define __STL_IOSTREAM_H__

#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>

using namespace std;

class stl_string;

ostream& operator<<( ostream& ostr, const stl_string& str );
istream& operator>>( istream& istr, stl_string& str );

#endif
