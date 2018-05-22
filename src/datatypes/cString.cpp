#include <ctype.h>
#include <cString.h>
#include <cStringUtils.h>
#include <math_defs.h>
#include <stdio.h>
#ifndef WIN32
#define _vsnprintf vsnprintf
#endif

extern const cString YR_DefaultString;

// Uncomment the next line if you want to know
// how many strings are created and destroyed
// when you run your program
//#define STRING_DEBUG

#ifdef STRING_DEBUG
long create_string = 0;
long delete_string = 0;
#endif

const char* cString::_defaultString = "";

void cString::_allocate()
{
    if( size_ > 0 )
    {
        data_ = new char[ size_ + 1 ];
        if( !data_ )
            yr_error( "cString: No more memory for string objects!" );
        data_[size_] = '\0';
    }
    else
    {
        data_ = (char*)_defaultString;
    }
}

void cString::_deallocate( char*& data )
{
#ifdef _DEBUG
    if( data == ( char* ) - 1 )
        yr_error( "String already deleted!" );
#endif

    if( data != _defaultString )
        delete [] data;

#ifdef _DEBUG
    data = ( char* ) - 1;
#endif

}

// cString constructors
//
// The duplication of code here is optimize performance!
//
// The empty string is represented by:
// size_ = 0 and data_ pointing to a '\0' ( string terminator )
//
//
// Default inline constructor for
// cString s

cString::cString()
{
#ifdef STRING_DEBUG
    create_string++;
#endif
    size_ = 0;
    data_ = ( char* )_defaultString;
}

//
// Constructor for
// cString s( 'a' );
//
cString::cString( char a )
{
#ifdef STRING_DEBUG
    create_string++;
#endif
    // if the char a is '\0' then constructs an empty string,
    // otherwise constructs a string with one charactor in
    size_ = ( a ? 1 : 0 );
    _allocate();
    if( size_ )
        data_[0] = a;

}

// Constructor for
// cString s( "hello world" )
//
cString::cString( const char* str )
{
#ifdef STRING_DEBUG
    create_string++;
#endif
    // if the const char* str is NULL then constructs and empty string,
    // othersie constructa a string with a copy of the string in.
    size_ = ( str ? (int)strlen( str ) : 0 );
    _allocate();
    if( size_ )
        memcpy( data_, str, size_ );
}

/*
#ifdef ALTPROPVALUE
//
// Constructor for
// cString s( cVariant )
//
cString::cString( const cVariant& s )
{
#ifdef STRING_DEBUG
    create_string++;
#endif
    const char* str = NULL;
    if( !s.IsUnknown() )
        str = s.AsConstCharStar();
    // if the const char* str is NULL then constructs an empty string
    // otherwise constructs a string with a copy of the string in
    size_ = ( str ? strlen( str ) : 0 );
    _allocate();
    if( size_ )
        memset( data_, str, size_ );
}
#endif
*/

// Copy constructor
//
cString::cString( const cString& str )
{
    yr_assert( this != &str );

#ifdef STRING_DEBUG
    create_string++;
#endif
    // if the const cString& str is an empty string then constructs an empty string,
    // otherwise constructs a string with a copy of the string in
    size_ = str.size_;
    _allocate();
    if( size_ )
        memcpy( data_, str.data_, size_ );
}

cString::cString( const char* str, int size )
{
#ifdef STRING_DEBUG
    create_string++;
#endif
    size_ = size;
    _allocate();
    if( size_ )
        memcpy( data_, str, size_ );
}

cString::cString( const char* str1, int size1, const char* str2, int size2 )
{
#ifdef STRING_DEBUG
    create_string++;
#endif
    size_ = size1 + size2;
    _allocate();
    if( size1 )
        memcpy( data_, str1, size1 );
    if( size2 )
        memcpy( data_ + size1, str2, size2 );

}

#ifdef WIN32
cString::cString( const wchar_t* s, size_t size )
{
#ifdef STRING_DEBUG
    create_string++;
#endif
    size_ = (int)size;
    _allocate();
    if( size_ )
    {
        if( wcstombs( data_, s, size_ ) == ( size_t ) - 1 )
            data_[0] = 0;
        else
            data_[size] = 0;
    }
}
#endif

// cString destructor
cString::~cString()
{
    if( data_ != _defaultString )
        _deallocate( data_ );
#ifdef _DEBUG
    size_ = -1;
#endif

#ifdef STRING_DEBUG
    delete_string++;
    if( delete_string >= create_string )
    {
        printf( "strings: %d created, d% deleted.\n", create_string, delete_string );
    }
#endif
}

// Copy operator
cString& cString::operator = ( const cString& str )
{
    // if the const cString& str is an empty string then constructs an empty string
    // otherwise constructs a string with a copy of the string in

    if( this != &str )
    {
        // only re-allocate if the size changes
        if( size_ != str.size_ )
        {
            size_ = str.size_;
            _deallocate( data_ );
            _allocate();
        }
        if( size_ )
            memcpy( data_, str.data_, size_ );
    }

    return *this;
}

// Unary addition operator for s += 'a'
cString& cString::operator += ( char a )
{
    // if the char a is '\0' then does nothing!
    if( a )
    {
        int old_size = size_;
        char* old_data = data_;

        size_++;
        _allocate();
        if( old_size )
            memcpy( data_, old_data, old_size );
        data_[old_size] = a;

        _deallocate( old_data );
    }

    return *this;
}

// Unary addition operator for s += s
cString& cString::operator += ( const cString& str )
{
    // if the const cString& str is an empty string then does nothing
    if( str.size_ )
    {
        int old_size = size_;
        char* old_data = data_;
        char* str_data = str.data_;

        size_ += str.size_;
        _allocate();

        if( old_size)
            memcpy( data_, old_data, old_size );
        memcpy( data_ + old_size, str_data, size_ - old_size );

        _deallocate( old_data );
    }

    return *this;
}

// Function reinitialise a string to the empty string
void cString::reset()
{
    if( size_ > 0 )
    {
        _deallocate( data_ );
        size_ = 0;
        _allocate();
    }
}

// Function resize a string to a string of size spaces
void cString::resize( int size )
{
    if( size < 0 )
        yr_error( "cString::resize( %d, value )", size );

    // if size is 0 then returns an empty string
    if( size_ != size )
    {
        _deallocate( data_ );
        size_ = size;
        _allocate();
    }
    if( size_ )
        memset( data_, ' ', size_ );
}

// substring operator for s( a, b ) to return the substring starting at string[a] and ending at string[b]
//
cString cString::operator() ( int start, int end ) const
{
    if( start < 0 )
        start = 0;
    if( end >= size_ )
        end = size_ - 1;

    if( start > end ) 
        return cString();

    return cString( data_ + start, end - start + 1 );
}

// Other substring operators
void cString::Left( int size )
{
    if( size > size_ )
        return;
    if( size < 0 )
        size = 0;

    // if size is 0 then returns an empty string
    char* old_data = data_;

    size_ = size;
    _allocate();
    if( size_ )
        memcpy( data_, old_data, size_ );

    _deallocate( old_data );
}

void cString::Mid( int index, int size )
{
    if( index < 0 )
        index = 0;
    if( index >= size_ )
        index = size_;
    if( size < 0 || index + size > size_ )
        size = size_ - index;
    if( index == 0 && size == size_ )
        return;

    // if size is 0 then returns an empty string
    char* old_data = data_;

    size_ = size;
    _allocate();
    if( size_ ) memcpy( data_, old_data + index, size_ );

    _deallocate( old_data );
}

void cString::Right( int size )
{
    if( size >= size_ )
        return;
    if( size < 0 )
        size = 0;

    // if size is 0 then returns an empty string
    int old_size = size_;
    char* old_data = data_;

    size_ = size;
    _allocate();

    if( size_ )
        memcpy( data_, old_data + old_size - size_, size_ );

    _deallocate( old_data );
}

void cString::SubString( const cString& str, int start, int end )
{
    if( this == &str )
    {
        this->SubString( start, end );
    }
    else
    {
        if( start < 0 )
            start = 0;
        if( end >= str.size_ )
            end = str.size_ - 1;

        if( start > end )
        {
            _deallocate( data_ );
            size_ = 0;
            _allocate();
        }
        else
        {
            if( size_ != end - start + 1 )
            {
                _deallocate( data_ );
                size_ = end - start + 1;
                _allocate();
            }
            memcpy( data_, str.data_ + start, size_ );
        }
    }
}

void cString::SubString( int start, int end )
{
    if( start < 0 )
        start = 0;
    if( end >= size_ )
        end = size_ - 1;

    if( start > end )
    {
        _deallocate( data_ );
        size_ = 0;
        _allocate();
    }
    else if( size_ != end - start + 1 )
    {
        char* old_data = data_;

        size_ = end - start + 1;
        _allocate();

        memcpy( data_, old_data + start, size_ );

        _deallocate( old_data );
    }
}

cString cString::Left( const cString& str, int size )
{
    if( size >= str.size_ )
        return str;
    if( size <= 0 )
        return cString();

    return cString( str.data_, size );
}

cString cString::Mid( const cString& str, int index, int size )
{
    if( index < 0 )
        index = 0;
    if( index >= str.size_ )
        index = str.size_;
    if( size < 0 || index + size > str.size_ )
        size = str.size_ - index;

    return cString( str.data_ + index, size );
}


void cString::Split( cArray< cString >& splitresult,
                    const cString& splitinput,
                    const cString& delimiter )
{
    if( delimiter.size() <= 0 )
    {
        splitresult.resize( 1, splitinput );
        return;
    }
    if( splitinput.size() <= 0 )
    {
        splitresult.resize( 1, theString );
        return;
    }
    cArray< cString > substrings;
    Fields( splitinput, substrings, *(delimiter.c_str() ) );
    splitresult.resize( substrings.getSize() );
    int i;
    for( i = 0; i < substrings.getSize(); ++i )
        splitresult[i] = substrings[i];
    return;
}


cString cString::Right( const cString& str, int size )
{
    if( size >= str.size_ )
        return str;
    if( size <= 0 )
        return cString();

    return cString( str.data_ + str.size_ - size, size );
}

int cString::IsBlankString() const
{
    char* s = data_;
    char* s_end = data_ + size_;
    while( s < s_end )
    {
        if( !IsSpace( *s ) )
            return FALSE;
        s++;
    }
    return TRUE;
}

//
// UpperCase and LowerCase operators
//
cString& cString::UpperCase()
{
    for( int i = 0; i < size_; ++i )
        data_[i] = ToUpper( data_[i] );
    return *this;
}

cString& cString::LowerCase()
{
    for( int i = 0; i < size_; ++i )
        data_[i] = ToLower( data_[i] );
    return *this;
}

cString& cString::CapitalCase()
{
    if( size_ > 0 )
    {
        data_[0] = ToUpper( data_[0] );
        for( int i = 1; i < size_; ++i )
        {
            if( data_[i-1] != ' ' )
                data_[i] = ToLower( data_[i] );
            else
                data_[i] = ToUpper( data_[i] );
        }
    }
    return *this;
}

// Functions to remove spaces from strings
cString cString::StripLeadingSpaces( const cString& str )
{
    int i;
    for( i = 0; i < str.size_ && IsSpace( str.data_[i] ); ++i );
    return str( i, str.size_ - 1 );
}

cString cString::StripTrailingSpaces( const cString& str )
{
    int i;
    for( i = str.size_ - 1; i >= 0 && IsSpace( str.data_[i] ); --i );
    return str( 0, i );
}

cString cString::StripAllSpaces( const cString& str )
{
    cString temp( str );
    temp.StripAllSpaces();
    return temp;
}

void cString::StripAllChar( const char c )
{
    if( size_ )
    {
        int j = 0;
        for( int i = 0; i < size_; ++i )
        {
            if( data_[i] == c )
            {
                j++;
            }
            else if( j > 0 )
            {
                data_[ i - j ] = data_[i];
            }
        }
        if( j > 0 )
        {
            size_ -= j;
            data_[size_] = '\0';
        }
    }
}

void cString::StripAllSpaces()
{
    if( size_ )
    {
        int j = 0;
        for( int i = 0; i < size_; ++i )
        {
            if( IsSpace( data_[i] ) )
            {
                ++j;
            }
            else if( j > 0 )
            {
                data_[ i-j ] = data_[i];
            }
        }
        if( j > 0 )
        {
            size_ -= j;
            data_[size_] = '\0';
        }
    }
}


void cString::Normalise()
{
    if( size_ )
    {
        int j = 0;
        for( int i = 0; i < size_; ++i )
        {
            if( IsSpace( data_[i] ) )
            {
                ++j;
            }
            else
            {
                data_[ i-j ] = ToUpper( data_[i] );
            }
        }
        if( j > 0 )
        {
            size_ -= j;
            data_[size_] = '\0';
        }
    }
}

void cString::NormaliseName()
{
    if( size_ )
    {
        int j = 0;
        int i;
        for( i = 0; i < size_; ++i )
        {
            if( data_[i] == ':' )
                break;

            if( IsSpace( data_[i] ) )
            {
                ++j;
            }
            else if( j > 0 )
            {
                data_[ i - j ] = data_[i];
            }
        }
        if( i - j < size_ )
        {
            size_ = i - j;
            data_[size_] = '\0';
        }
    }
}

// other stuff
//
ostream& operator << ( ostream& out, const cString& str )
{
    out << str.data_;
    return out;
}

cString cString::Format( const char* fmt, ... )
{
    va_list marker;
    char buf[4096];
    va_start( marker, fmt );
    _vsnprintf( buf, 4095, fmt, marker );
    va_end( marker );
    return buf;
}

int cString::InString( const char* str, const char* pattern, int start )
{
    if( start >=( int )strlen( str ) )
        return -1;
    
    const char* ptr = ::strstr( str + start, pattern );
    return( ( ptr ) ? ( int )( ( ptr - str ) * sizeof( char ) ) : -1 );
}

int cString::NumOccurrences( const cString& str, const cString& pattern )
{
    int count = 0;
    const char* ptr = str;
    while( ptr == ::strstr( ptr, pattern ) )
    {
        count++;
        ptr += sizeof( char );
    }

    return count;
}

int cString::FindChar( char ch, int start ) const
{
#ifdef _DEBUG
    if( start < 0 || start > size_ )
        yr_error( "Start index in FindChar is %d, string is of size %d2 ", start, size_ );
#endif
    char* ptr = strchr( data_ + start, ch );
    if( ptr == NULL )
        return -1;

    return ( ( int )( ptr - data_ ) * sizeof( char ) );
}

int cString::ReverseFindChar( char ch ) const
{
    char* ptr = strchr( data_, ch );
    if( ptr == NULL )
        return -1;

    return ( ( int )( ptr - data_ ) * sizeof( char ) );
}

int cString::IsInteger() const
{
    int isInt = TRUE;

    for( char* it = data_ + (int)( size_ && data_[0] == '-' ); isInt && it != data_ + size_; isInt = isdigit( *it++ ) );

    return isInt > 0;

}


// Takes a string in the form "(10,5)" and returns
// rows =  10, cols = 5, and FALSE if the string is in wrong format
int StringToCoords( const cString& coordsIn, int& rows, int& cols )
{
    cString coords( coordsIn );
    coords.StripAllSpaces();

    int retVal = FALSE;
    int index = 0;
    int size = coords.size();

    while( index < size )
    {
        if( coords[index] == '(' )
            break;
        index++;
    }

    if( index < size && coords[index++] == '(' )
    {
        int temp_rows = 0;
        while( index < size && coords[index] >= '0' && coords[index] <= '9' )
            temp_rows = 10 * temp_rows + ( coords[index++] - '0' );

        if( index < size && coords[index++] == ',' )
        {
            int temp_cols = 0;
            while( index < size && coords[index] >= '0' && coords[index] <= '9' )
                temp_cols = 10 * temp_cols + ( coords[index++] - '0' );

            if( index < size && coords[index++] == ')' && index == size )
            {
                rows = temp_rows;
                cols = temp_cols;
                retVal = TRUE;
            }
        }

    }

    return retVal;

}

// Takes a string in the form "n:m" and returns
// start = n, end = m, and ERROR if the string is wrong format
//
void cString::StringToRange( int& range_start, int& range_end )
{
    const char* Range = this->c_str();
    size_t rangeLength = strlen( Range );
    range_start = -1;
    range_end = -1;

    if( rangeLength )
    {
        int i = 0, j = 0;
        cString rangeElement;
        while( 1 )
        {
            if( isspace( Range[i] ) )
                continue;
            else if( Range[i] == 0 || Range[i] == ':' || Range[i] == '~' || Range[i] == 'x' )
            {
                if( !Compare( rangeElement, "" ) )
                {
                    if( j == 0 )
                        range_start = atoi( ( const char* )rangeElement );
                    else if( j == 1 )
                        range_end = atoi( ( const char* )rangeElement );
                    else if( j > 1 )
                    {
                        yr_error( "The BOOKRANGE value you specified is not valid: %s. Refter to the documentation for correct format!", Range );
                    }
                    rangeElement = "";
                    j++;
                }
                if( Range[i] == 0 )
                    break;
            }
            else if( Range[i] >= '0' && Range[i] <= '9' )
                rangeElement += Range[i];
            else
            {
                yr_error( "The RISKRANGE value you specified is not valid: %s. Refer to the documentation for correct format!", Range );
            }

            i++;
        }
    }
    if( range_start != -1 && range_end == -1 )
        range_end = range_start;
}

cString cString::EscapeQuotes( const cString& str, char with )
{
    cString s = str;
    s.EscapeQuotes( with );
    return s;
}

void cString::EscapeQuotes( char with )
{
    int i;
    int count = 0;
    int found = FALSE;

    for( i = 0; i < size_; ++i )
    {
        if( data_[i] == '\"' || data_[i] == with || data_[i] == 10 )
        {
            found = TRUE;
            count++;
        }
        if( data_[i] == 13 )
        {
            found = TRUE;
            count--;
        }
    }
    if( !found )
        return;

    int old_size = size_;
    char* old_data = data_;

    size_ += count;
    _allocate();

    count = 0;
    for( i = 0; i < old_size; i++ )
    {
        if( old_data[i] == '\"' || old_data[i] == with )
        {
            data_[i+count] = with;
            count++;
            data_[i+count] = old_data[i];
        }
        else if( old_data[i] == 10 )
        {
            data_[i+count] = '\\';
            count++;
            data_[i+count] = 'n';
        }
        else if( old_data[i] == 13 )
        {
            count--;
        }
        else
        {
            data_[i+count] = old_data[i];
        }
    }

    _deallocate( old_data );
}

void cString::UnEscapeQuotes()
{
    int count = 0;
    for( int i = 0; i < size_ - count; ++i )
    {
        if( data_[i+count] == '\\' && ( data_[i+count+1] == '\"' || data_[i+count+1] == '\\' ) )
            count++;
        else if( data_[i+count] == '\\' && data_[i+count+1] == 'n' )
        {
            count++;
            data_[i+count] = '\n';
        }

        if( count > 0 )
            data_[i] = data_[i+count];
    }
    if( count > 0 )
        *this = cString( data_, size_ - count );
}

void cString::Replace( char ch, const cString& str )
{
    int pos = FindChar( ch );
    while( pos != -1 )
    {
        int old_size = size_;
        char* old_data = data_;

        size_ += str.size_ - 1;
        _allocate();

        if( pos > 0 )
            memcpy( data_, old_data, pos );

        if( str.size_ > 0 )
            memcpy( data_ + pos, str.data_, str.size_ );

        if( old_size - pos - 1 > 0 )
            memcpy( data_ + pos + str.size_, old_data + pos + 1, old_size - pos - 1 );

        _deallocate( old_data );
        pos = FindChar( ch, pos + str.size_ );
    }
}


bool cString::IsStringInArray( const cArray< cString >& Input, const cString& Name )
{
    const int len = Input.getSize();
    for( int i = 0; i < len; ++i )
    {
        if( Compare( Input[i], Name ) )
            return true;
    }

    return false;
}



int cString::StringIndexInArray( const cArray< cString >& Input, const cString& Name )
{
    const int len = Input.getSize();
    for( int i = 0; i < len; ++i )
    {
        if( !Compare( Input[i], Name ) )
        {
            return i;
        }
    }

    return -1;
}


cString cString::AppendPath( const cString& path, const cString& filename )
{
#ifndef WIN32
    const char slash = '\\';
#else
    const char slash = '/';
#endif

    cString newPath = path;
    if( newPath.length() > 0 && newPath[ newPath.length() - 1 ] != '/' && newPath[ newPath.length() - 1 ] != '\\' )
        newPath += slash;
    newPath  += filename;

#ifndef WIN32
    newPath.Replace( '\\', "/" );
#else
    newPath.Replace( '/', "\\" );
#endif

    return newPath;
}

#ifdef WIN32
#if _MSC_VER <= 1200
void getline( std::istream& in, std::string& line )
{
    static char bfr[1024];
    in.getline( bfr, 1023 );
    line = bfr;
}
#endif
#endif

const cString theString = cString();


