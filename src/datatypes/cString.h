#ifndef __CSTRING_H__
#define __CSTRING_H__

#include <stl_iostream.h>

#include <math.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#include <cArray.h>
//#include "cVariant.h"

#pragma warning( disable:4996 )

#ifdef WIN32
// Force the following functions to be generated inline...
#pragma intrinsic( abs, fabs, labs, memcmp, memcpy, memset, strcat, strcmp, strcpy, strlen )
#endif

#include "cErr.h"
#define NBSP -96

class cStringInternal
{
	friend class cConstCharStarWrapper;
	friend class cNormalizedString;

public:
	char* data_;
	int size_;
};

class cString : public cStringInternal
{
public:
	cString();
	cString( char );
	cString( const char* );
	cString( const cString& );
	/*
#ifdef ALTPROPVALUE
	cString( const cVariant& );
#endif
	*/
	cString( const char* str, int size );
#ifdef WIN32
	cString( const wchar_t* s, size_t size );
#endif

	~cString();

	cString& operator = ( const cString& );

	cString& operator += ( char );
	cString& operator += ( const cString& );

	char operator[] ( int i ) const
	{
		yr_assert( i >= 0 && i < size_ );
		return data_[i];
	}

	char& operator[] ( int i )
	{
		yr_assert( i >= 0 && i < size_ );
		return data_[i];
	}

	operator const char*() const { return data_; }
	const char* c_str() const { return data_; }
	char* c_str2() { return data_; }
	cString operator() ( int start, int end ) const;

	void reset();
	void resize( int i );
	int length() const { return size_; }
	int size() const { return size_; }

	void Left( int size );
	void Mid( int index, int size = -1 );
	void Right( int size );

	void SubString( const cString& str, int start, int end );
	void SubString( int start, int end );

	int InString( const char* str ) const	{ return InString( c_str(), str ); }
	int InString( const cString& str ) const	{ return InString( c_str(), str.c_str() ); }
	int NumOccurrences( const cString& str ) const	{ return NumOccurrences( *this, str ); }
	int FindChar( char ch, int start = 0 ) const;
	int ReverseFindChar( char ch ) const;
	void Replace( char ch, const cString& str );
	int IsInteger() const;
	
	int IsBlankString() const;

	cString& UpperCase();
	cString& LowerCase();
	cString& CapitalCase();

	void EscapeQuotes( char with = '\\' );
	void UnEscapeQuotes();

	static cString EscapeQuotes( const cString& s, char with = '\\' );

	void StripLeadingSpaces( void )	{ *this = StripLeadingSpaces( *this ); }
	void StripTrailingSpaces( void ) { *this = StripTrailingSpaces( *this ); }
	void StripAllSpaces();
	void StripAllChar( const char c );
	//cString GetObjectYRName() const;
	//bool IsComponentName() const;
	//bool IsGroupName() const;
	void NormaliseName();
	void Normalise();

	void StringToRange( int& range_start, int& range_end );

	static cString Format( const char* fmt, ... );
	static cString Left( const cString& str, int size );
	static cString Mid( const cString& str, int index, int size = -1 );
	static cString Right( const cString& str, int size );
	static int InString( const char* str, const char* pattern, int start = 0 );
	static int NumOccurrences( const cString& str, const cString& pattern );
	
	static cString UpperCase( const cString& str ) { return cString( str ).UpperCase(); }
	static cString LowerCase( const cString& str ) { return cString( str ).LowerCase(); }
	static cString CapitalCase( const cString& str ) { return cString( str ).CapitalCase(); }

	static cString StripLeadingSpaces( const cString& str );
	static cString StripTrailingSpaces( const cString& str );
	static cString StripAllSpaces( const cString& str );
	static void Split( cArray< cString >& splitresult,
						const cString& splitinput,
						const cString& delimiter ); 

	static bool IsStringInArray( const cArray< cString >& input, const cString& Name );

	static int StringIndexInArray( const cArray< cString >& input, const cString& Name );

	friend ostream operator << ( ostream&, cString& str );

	cString( const char* s1, int size1, const char* s2, int size2 );

	static void Swap( cString& a, cString& b )
	{
		int temp1 = a.size_;
		a.size_ = b.size_;
		b.size_ = temp1;

		char* temp2 = a.data_;
		a.data_ = b.data_;
		b.data_ = temp2;
	}

	static cString AppendPath( const cString& path, const cString& filename );

	//friend class cVariant;
	friend class cConstCharStarWrapper;

private:
	static const char* _defaultString;

	void _allocate();
	void _deallocate( char*& data );

};

// class cConstCharStarWrapper
// ---------------------------
// This class lets you get a const cString that is a wrapper around a const char*
// without actually making a real copy of it. It just points the data_ member of
// a cString to the const char*. Upon destruction of this class, _data.data_
// is pointed elsewhere so that when the cString is destructed, it doesn't take the
// const char* down with it.
// 
// ONLY USE THIS IF YOU ARE ABSOLUTELY SURE THAT THE CSTRING WILL NOT BE MODIFIED
// IN ANY WAY. If you'are not sure, don't use this class.
//
class cConstCharStarWrapper
{
public:
	// Construct from a const char*
	cConstCharStarWrapper( const char* const input );
	cConstCharStarWrapper( const char* const input, const int size );
	// Destructor
	// ~cConstCharStarWrapper();

	// Convert to a const cString&
	operator const cString& () const { return *static_cast< const cString* >( &_data ); }

private:
	cStringInternal	_data;		// The cString object
	// Kill these off:
	cConstCharStarWrapper( const cConstCharStarWrapper& rhs );
	cConstCharStarWrapper& operator = ( const cConstCharStarWrapper& rhs );
};

// cConstCharStarWrapper constructor
inline cConstCharStarWrapper::cConstCharStarWrapper( const char* const input )
{
	// Point _data to input
	_data.data_ = ( char* )input;
	_data.size_ = ( int )strlen( input );
}

inline cConstCharStarWrapper::cConstCharStarWrapper( const char* const input, const int size )
{
	// Point _data to put
	_data.data_ = ( char* )input;
	_data.size_ = size;
}

// cConstCharStarWrapper destructor
/*
inline cConstCharStarWrapper::~cConstCharStarWrapper()
{
	// Point _data at the default string so the original const char* doesn't destroyed
	//_data.data_ = ( char* )_defaultString;
	//_data.size_ = 0;
}
*/

inline cString operator + ( const cString& s1, const cString& s2 )
{
	return cString( ( const char* )s1, s1.size(), ( const char* )s2, s2.size() );
}

inline cString operator + ( const char* s1, const cString& s2 )
{
	return cString( s1, ( int )strlen( s1 ), ( const char* )s2, s2.size() );
}

inline cString operator + ( const cString& s1, const char* s2 )
{
	return cString( ( const char* )s1, s1.size(), s2, ( int )strlen( s2 ) );
}

inline bool operator == ( const cString& s1, const cString& s2 )
{
	return( !strcmp( ( const char* )s1, ( const char* )s2 ) );
}

inline bool operator == ( const char* s1, const cString& s2 )
{
	return(  !strcmp( s1, ( const char* )s2 ) );
}

inline bool operator == ( const cString& s1, const char* s2 )
{
	return( !strcmp( ( const char* )s1, s2 ) );
}

inline bool operator != ( const cString& s1, const cString& s2 )
{
	return( strcmp( ( const char* )s1, ( const char* )s2 ) != 0 );
}

inline bool operator != ( const char* s1, const cString& s2 )
{
	return( strcmp( s1, ( const char* )s2 ) != 0 );
}

inline bool operator != ( const cString& s1, const char* s2 )
{
	return( strcmp( ( const char* )s1, s2 ) != 0 );
}

inline bool operator < ( const cString& s1, const cString& s2 )
{
	return( strcmp( ( const char* )s1, ( const char*)s2 ) < 0 );
}

inline bool operator < ( const char* s1, const cString& s2 )
{
	return( strcmp( s1, ( const char* )s2 ) < 0 );
}

inline bool operator < ( const cString& s1, const char* s2 )
{
	return( strcmp( ( const char* )s1, s2 ) < 0 );
}

inline bool operator <= ( const cString& s1, const cString& s2 )
{
	return( strcmp( ( const char* )s1, ( const char*)s2 ) <= 0 );
}

inline bool operator <= ( const char* s1, const cString& s2 )
{
	return( strcmp( s1, ( const char* )s2 ) <= 0 );
}

inline bool operator <= ( const cString& s1, const char* s2 )
{
	return( strcmp( ( const char* )s1, s2 ) <= 0 );
}

inline bool operator > ( const cString& s1, const cString& s2 )
{
	return( strcmp( ( const char* )s1, ( const char*)s2 ) > 0 );
}

inline bool operator > ( const char* s1, const cString& s2 )
{
	return( strcmp( s1, ( const char* )s2 ) > 0 );
}

inline bool operator > ( const cString& s1, const char* s2 )
{
	return( strcmp( ( const char* )s1, s2 ) > 0 );
}

inline bool operator >= ( const cString& s1, const cString& s2 )
{
	return( strcmp( ( const char* )s1, ( const char*)s2 ) >= 0 );
}

inline bool operator >= ( const char* s1, const cString& s2 )
{
	return( strcmp( s1, ( const char* )s2 ) >= 0 );
}

inline bool operator >= ( const cString& s1, const char* s2 )
{
	return( strcmp( ( const char* )s1, s2 ) >= 0 );
}

int StringToCoords( const cString& coords, int& rows, int& cols );

inline void NormaliseName( cString& name )
{
	name.NormaliseName();
}

inline char& ToUpper( char& c )
{
	if( c >= 'a' && c < 'z' )
		c -= ' ';

	return c;
}

inline char ToUpperConst( const char c )
{
	const bool isLower = ( c >= 'a' && c <= 'z' );
	return isLower ? static_cast< char >( c - ' ' ) : c;
}

inline char& ToLower( char& c )
{
	if( c >= 'A' && c < 'Z' )
		c += ' ';

	return c;
}

inline int IsSpace( const char& c )
{
	return( c == 0x20 || c == NBSP || ( c >= 0x09 && c <= 0x0D ) );
}
#if _MSC_VER
#define strcasecmp _stricmp  //strcasecmp 找不到标识符
#define strncasecmp _strnicmp

#endif

#ifndef WIN32
#define stricmp strcasecmp
#define strnicmp strncasecmp
#endif

inline int Norm( const char* s1, const char* s2 )
{
	if( stricmp( s1, s2 ) == 0 )
		return 0;

	char c1, c2;
	while( *s1 || *s2 )
	{
		while( IsSpace( *s1 ) )
			s1++;
		while( IsSpace( *s2 ) )
			s2++;

		c1 = *s1;
		c2 = *s2;
		if( ToUpper( c1 ) != ToUpper( c2 ) )
		{
			if( c1 < c2 )
				return -1;
			else
				return 1;
		}
		if( c1 )
			s1++;
		if( c2 )
			s2++;
	}
	return 0;
}

// Return zero if and only if the first nChar black characters match
// not case sensitive, also returns zero if both strings have fewer
// than nChar black characters, and all chars agree
inline int Norm( const char* s1, const char* s2, int nChar )
{
	if( nChar > 0 && strnicmp( s1, s2, nChar ) == 0 )
		return 0;

	char c1, c2;
	int i = 0;

	while( *s1 || *s2 )
	{
		while( IsSpace( *s1 ) )
			s1++;
		while( IsSpace( *s2 ) )
			s2++;

		c1 = *s1;
		c2 = *s2;

		if( ToUpper( c1 ) != ToUpper( c2 ) )
		{
			if( c1 < c2 )
				return -1;
			else
				return 1;
		}
		i++;
		if( i >= nChar )
			return 0;

		if( c1 )
			s1++;
		if( c2 )
			s2++;
	}
	return 0;
}

class cStringLess
{
public:
	bool operator() ( const cString& x, const cString& y ) const
	{
		return( Norm( ( const char* )x, ( const char* )y ) < 0 );
	}
};

inline int Compare( const char* s1, const char* s2 )
{
	return Norm( s1, s2 ) == 0;
}

inline int Compare( const char* s1, const cString& s2 )
{
	return Norm( s1, ( const char* )s2 ) == 0;
}

inline int Compare( const cString& s1, const char* s2 )
{
	return Norm( ( const char* )s1, s2 ) == 0;
}

inline int Compare( const cString& s1, const cString& s2 )
{
	return Norm( ( const char* )s1, ( const char* )s2 ) == 0 ;
}

//*****************************
inline int Compare( const char* s1, const char* s2, int nChar )
{
	return Norm( s1, s2, nChar ) == 0;
}

inline int Compare( const char* s1, const cString& s2, int nChar )
{
	return Norm( s1, ( const char* )s2, nChar ) == 0;
}

inline int Compare( const cString& s1, const char* s2, int nChar )
{
	return Norm( ( const char* )s1, s2, nChar ) == 0;
}

inline int Compare( const cString& s1, const cString& s2, int nChar )
{
	return Norm( ( const char* )s1, ( const char* )s2, nChar ) == 0;
}

// Allow us to construct std::maps with strings as keys, where the
// less than operator is independent of case
struct nocase_less
{
	bool operator() ( const cString& s1, const cString& s2 ) const
	{
		return Norm( s1.c_str(), s2.c_str() ) < 0;
	}
};

#ifdef WIN32
#if _MSC_VER <= 1200
void getline( std::istream& in, std::string& line );
#endif
#endif

extern const cString theString;

#endif

