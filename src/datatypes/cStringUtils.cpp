#include <cStringUtils.h>
#include <math_defs.h>
#include <cString.h>
//#include "cVariant.h"
#include <stl.h>
//include "regex.h"
//#include "cBuffer.h"
//#include "lexer.h"

cArray< cString > StringBlankStrings( const cArray< cString >& input )
{
    int i;
    cArray< cString > output;
    for( i = 0; i < input.getSize(); ++i )
    {
        if( !Compare( input[i], theString ) )
            output &= input[i];
    }
    return output;
}

int IsInt( const cString& inputString )
{
    cString tmp = inputString;
    tmp.UpperCase();
    int last = inputString.length() - 1;

    if( tmp.InString( "-" ) == 1 )
    {
        if( tmp.InString( "+" ) != 0 )
            return( 0 );
    }
    else if( tmp.InString( "+" ) == 1 )
    {
        if( tmp.InString( "-" ) != 0 )
            return( 0 );
    }
    else if( tmp.NumOccurrences( "+" ) > 1 || tmp.NumOccurrences( "-" ) > 1 || tmp.NumOccurrences( "L" ) > 1 
                || tmp.InString( "+" ) > 0 || tmp.InString( "-" ) > 0 || ( tmp.InString( "L" ) > 0 && tmp.InString( "L" ) < last ) )
    {
        return( 0 );
    }
    cString allowed = "1234567890L+-";
    for( int a = 0; a < allowed.length(); ++a )
        tmp.StripAllChar( allowed[a] );
    return Compare( tmp, "" );
}

int AsInt( const cString& inputString )
{
    if( IsInt( inputString ) )
    {
        cString tmp = inputString;
        tmp.StripAllChar( '+' );
        return atoi( ( const char* )tmp );
    }
    else
        yr_error( "String '%s' cannot be converted to integer!", inputString.c_str() );
}

int IsDouble( const cString& inputString )
{
    double d = 0.0;
    return IsDouble( inputString, d );
}

bool IsDouble( const cString& inputString, double& d )
{
    return IsDoubleStr( inputString.c_str(), d );
}

bool IsDoubleStr( const char* inputString, double& d )
{
    return false;
    // miss lexer.h here
}

double AsDouble( const cString& inputString )
{
    if( IsDouble( inputString ) )
    {
        cString tmp = inputString;
        tmp.StripAllChar( '+' );
        return atof( ( const char*)tmp );
    }
    else
        yr_error( "String '%s' cannot be converted to double!", inputString.c_str() );
}

int IsPercentage( const cString& inputString )
{
    int last = inputString.length() - 1;
    cString tmp = inputString;
    tmp.Left( last );
    return( inputString.InString( "%" ) == last && IsDouble( tmp ) );
}

double AsPercentage( const cString& inputString )
{
    if( IsPercentage( inputString ) )
    {
        int last = inputString.length() - 1;
        cString tmp = inputString;
        tmp.Left( last );
        return AsDouble( tmp ) / 100.0;
    }
    else
        yr_error( "String '%s' cannot be converted to percentage!", inputString.c_str() );
}

// IsBasicInterval will return true if the string equals an interval of basic type, i.e. nM or nY etc
// It returns 0 for combinations of basic types (3y5m etc ), integer, any old string
bool IsBasicInterval( const cString& inputString )
{
    int last = inputString.length() - 1;
    if( last < 0 )
        return false;
    cString tmp = inputString;
    tmp.Left( last );
    switch( inputString[last] )
    {
    case 'D':
    case 'd':
    case 'B':
    case 'b':
    case 'W':
    case 'w':
    case 'M':
    case 'm':
    case 'Y':
    case 'y':
        if( last > 0 && inputString[0] == '+' )
            yr_error( "+ sign not allowed before interval." );
        if( IsInt( tmp ) == 1 )
            return true;
        else
            return false;
    default:
        return false;
    }
}


cString To32( const double price )
{
    cString to256;

    const double posPrice = fabs( price );

    int i = ( int )posPrice;
    int frac256 = ( int )( ( posPrice - ( double )i ) * 256.0 + 0.5 );
    if( frac256 < 0 )
    {
        i--;
        frac256 = 256 + frac256;
    }
    int frac = ( int )( ( ( double )frac256 ) / 8.0 );
    int frac8 = ( int )( ( ( double )frac256 ) - ( 8.0 * ( double )frac ) );
    if( frac8 < 0 )
    {
        frac--;
        frac8 = ( int )( 8.0 + ( double )frac8 );
    }

    to256 = cString::Format( "%d", i );
    to256 += cString( "-" );
    if( frac < 10 )
        to256 += cString( "0" );
    to256 += cString::Format( "%d", frac );
    to256 += ' ';
    to256 += cString::Format( "%d", frac8 );

    if( price < 0 )
    {
        to256 = "-" + to256;
    }

    return to256;

}

double From32( const cString& str )
{
    double from32 = 0.0;
    int multiplier;
    cString posString( cString::StripLeadingSpaces( str ) ), s1, s2, s3;
    if( !posString.length() )
    {
        return from32;
    }
    if( posString[0] == '-' )
    {
        posString.SubString( 1, posString.length() );
        multiplier = -1;
    }
    else
    {
        multiplier = 1;
    }
    s1 = s2 = s3 = posString;

    if( posString.length() > 0 )
    {
        if( posString.InString( '.' ) > 0 )
            from32 = AsDouble( posString );
        else
        {
            int i = posString.InString( '-' );
            int k = max( posString.InString( ' ' ), posString.InString( '+' ) );
            if( i > 0 )
            {
                s1.Left( i );
                from32 = AsDouble( s1 );
                if( k < 1 )
                {
                    s2.Right( ( int )( posString.length() - i - 1 ) );
                    if( s2.length() > 3 )
                        yr_error( "Illegal format for 32nd and 8th ( more than 3 characters)" );
                    if( s2.length() > 2 )
                    {
                        int j = s2.InString( '+' );
                        if( j > 0 )
                        {
                            s2.Left( ( int )( j - 1 ) );
                            from32 += 1 / 64.0;
                        }
                        else
                        {
                            s2.Left( 2 );
                            s3.Right( 1 );
                            from32 += AsDouble( s3 ) / 256.0;
                        }
                    }
                    from32 += AsDouble( s2 ) / 32.0;
                }
                // we found either space or a plus sign
                else
                {
                    s2.Mid( ( int )( i + 1 ), ( int )( k - i - 1 ) );
                    s3.Right( posString.length() - k );
                    from32 += AsDouble( s2 ) / 32.0;
                    if( s3.InString( '+' ) > -1 )
                        from32 += 4.0 / 256.0;
                    else
                        from32 += AsDouble( s3 ) / 256.0;
                }
            }
            else
            {
                from32 = AsDouble( posString.c_str() );
            }
        }
    }

    return from32 * multiplier;
}

cString To64( const double price )
{
    cString to64;
    const double posPrice = fabs( price );

    int i = ( int )posPrice;
    int frac64 = ( int )( ( posPrice - ( double )i ) * 64.0 );
    if( frac64 < 0 )
    {
        i--;
        frac64 += 64;
    }
    to64 = cString::Format( "%d", i );
    to64 += cString( " \'" );
    if( frac64 < 10 )
        to64 += '0';
    to64 += cString::Format( "%d", frac64 );

    if( price < 0 )
    {
        to64 = "-" + to64;
    }

    return to64;

}

double From64( const cString& str )
{
    double from64 = 0.0;
    int multiplier;
    cString posString( cString::StripLeadingSpaces( str ) ), s1, s2;
    if( !posString.length() )
    {
        return from64;
    }
    if( posString[0] == '-' )
    {
        posString.SubString( 1, posString.length() );
        multiplier = -1;
    }
    else
    {
        multiplier = 1;
    }
    s1 = s2 = posString;
    if( posString.length() > 0 )
    {
        int i = posString.InString( '\'' );
        if( i > 0 )
        {
            s1.Left( i );
            s2.Right( ( int )( posString.length() - i - 1 ) );
            from64 = AsDouble( s1 );
            from64 += AsDouble( s2 ) / 64;
        }
        else
            from64 = AsDouble( str );
    }
    return from64 * multiplier;
}

void Fields( const cString& value, cArray< cString >& values, const char& separator )
{
    values.clear();
    if( value.length() == 0 )
        return;

    const char* str = value.c_str();

    int count = 0;
    int last_pos = 0;

    while( last_pos < value.length() )
    {
        if( str[last_pos] == separator )
            count++;

        last_pos++;
    }

    values.resize( count + 1 );
    count = 0;

    last_pos = 0;
    while( TRUE )
    {
        int pos = last_pos;
        while( pos < value.length() && str[pos] != separator )
            pos++;

        values[count++].SubString( value, last_pos, pos - 1 );
        if( pos == value.length() )
            break;

        last_pos = pos + 1;
    }
}

void JoinStrings( const cArray< cString >& strings, const cString& separator, cString& result )
{
    cString res;
    if( strings.getSize() > 0 )
        res = strings[0];

    for( int i = 1; i < strings.getSize(); ++i )
    {
        res += separator;
        res += strings[i];
    }

    cString::Swap( res, result );
}

int Parenthesis( const cString& str, cString& before, cString& inside, cString& after )
{
    int pos_1 = str.FindChar( '(' );
    int pos_2 = str.ReverseFindChar( ')' );
    if( pos_1 >= pos_2 )
        return FALSE;

    if( pos_1 - 1 >= 0 )
        before.SubString( str, 0, pos_1 - 1 );
    inside.SubString( str, pos_1 + 1, pos_2 - 1 );
    if( pos_2 + 1 < str.length() - 1 )
        after.SubString( str, pos_2 + 1, str.length() - 1 );

    return TRUE;

}

void EnsureString( cArray< cString >& stringarray, const cString& string )
{
    for( int i = 0; i < stringarray.getSize(); ++i )
    {
        if( Compare( stringarray[i], string ) )
            return;
    }
    stringarray &= string;
}

// unlike the template function this one is case and blank IN-sensitive
int ArrayLocationString( const cArray< cString >& a, const cString& b )
{
    for( int i = 0; i < a.getSize(); ++i )
    {
        if( Compare( a[i], b ) )
            return i;
    }
    return -1;
}

cArray< cString > GetTags( const cString& str, const char& lefttagdelimiter, const char& righttagdelimiter )
{
    cArray< cString > tags;
    cString tmp;
    int poslch = -1;
    int posrch = -1;
    poslch = str.FindChar( lefttagdelimiter );
    if( poslch > -1 )
        posrch = str.FindChar( righttagdelimiter, poslch + 1 );
    const int len = str.length();

    while( poslch != -1 && posrch != -1 )
    {
        tmp.SubString( str, poslch + 1, posrch - 1 );
        tags &= tmp;
        poslch = str.FindChar( lefttagdelimiter, posrch + 1 );
        if( poslch > -1 )
            posrch = str.FindChar( righttagdelimiter, poslch + 1 );
    }
    return tags;
}


void Replace( cString& str, const cString& a, const cString& b )
{
    cString strupper = str;
    strupper.UpperCase();
    cString aupper = a;
    aupper.UpperCase();
    int start = strupper.InString( aupper );
    if( start < 0 )    // if substring a is not found in str, don't replace
        return;
    int end = start + a.length();
    cString right = str;
    right.Right( str.length() - end );
    cString tmp = str;
    tmp.Left( start );
    tmp += b;
    tmp += right;
    str = tmp;
}

void NormaliseNames( cArray< cString >& names )
{
    for( int i = 0; i < names.getSize(); ++i )
        names[i].NormaliseName();
}

/*
cSharedPtr< cRestrictionCriterion > cRestrictionCriterion::Create( const cString& criterionType )
{
    // TODO: replace with enum, if possible
    if( Compare( criterionType, "==" ) )
        return cSharedPtr< cRestrictionCriterion >( new cRCEQ );
    if( Compare( criterionType, "<=" ) )
        return cSharedPtr< cRestrictionCriterion >( new cRCLE );
    if( Compare( criterionType, ">=" ) )
        return cSharedPtr< cRestrictionCriterion >( new cRCGE );
    if( Compare( criterionType, "<" ) )
        return cSharedPtr< cRestrictionCriterion >( new cRCLT );
    if( Compare( criterionType, ">" ) )
        return cSharedPtr< cRestrictionCriterion >( new cRCGT );
    if( Compare( criterionType, "<>" ) )
        return cSharedPtr< cRestrictionCriterion >( new cRCNE );
    return cSharedPtr< cRestrictionCriterion >();
}
*/

cString InvertFXString( const cString& str )
{
    cString returnStr( str );

    // first strip away an ExtraStrings
    cArray< cString > extraStrings;
    Fields( str, extraStrings, '-' );

    const cString& fxString = extraStrings[0];
    bool isFX = fxString.NumOccurrences( "/" ) == 1;
    if( isFX )
    {
        cArray< cString > fxParts;
        Fields( fxString, fxParts, '/' );
        returnStr = fxParts[1] + "/" + fxParts[0];
        // Add back ExtraStrings
        for( int i = 1; i < extraStrings.getSize(); ++i )
            returnStr += "-" + extraStrings[i];
    }
    return returnStr;
}

/*
cArray< SwCity > AsCitySetArray( const cString& inputString )
{
    cArray< cString > individualCitySetStrings;
    Fields( inputString, individualCitySetStrings, '|' );

    int numCitySets = individualCitySetStrings.getSize();

    if( numCitySets > 3 )
        yr_error( "Invalid cities %s!", inputString.c_str() );

    cArray< SwCity > output( numCitySets );
    for( int i = 0; i < numCitySets; ++i )
        output[i] = individualCitySetStrings[i];

    return output;
}
*/