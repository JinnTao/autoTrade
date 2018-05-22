#ifndef __CSTRINGUTILS_H__
#define __CSTRINGUTILS_H__

#include <cArray.h>
#include <cSharedPtr.h>
//#include "cVariant.h"

class cString;

// Regular expression
#define YRAX_RE_DEFAULT_SYNTAX 0
#define YRAX_RE_CASE_INSENSITIVE 1
#define YRAX_RE_UNIX_SYNTAX 2
#define YRAX_RE_PARTIAL_MATCH 4
#define YRAX_RE_NO_UNDERSCORE_SYNTAX 8  // same as default but _ and % are not reserved keyword
#define YRAX_RE_POSIX_SYNTAX 16

/*
Does the string s match the regular expression regex?
*/

//int Match( const cString& s, const cString& regex, const int syntax );
//int Match( const cString& s, const cArray< cString >& regex, const int syntax );
//int MatchVariant( const cVariant& var, const cVariant& regexvar, const int syntax );

//bool Match( const cString& s, const cString& regex, const int syntax, cArray< cString >* groups );
cArray< cString > StripBlankStrings( const cArray< cString >& input );

int IsDouble( const cString& );
bool IsDoubleStr( const char* inputString, double& d );
bool IsDouble( const cString& inputString, double& d );
double AsDouble( const cString& );
int IsInt( const cString& );
int AsInt( const cString& );
int IsPercentage( const cString& );
double AsPercentage( const cString& );
bool IsBasicInterval( const cString& inputString );
//cArray< SwCity > AsCitySetArray( const cString& inputString );

cString To32( const double price );
double From32( const cString& str );
cString To64( const double price );
double From64( const cString& str );

void Fields( const cString& value, cArray< cString >&values, const char& separator );
void JoinStrings( const cArray< cString >& strings, const cString& separator, cString& result );
int Parenthesis( const cString& value, cString& before, cString& inside, cString& after );

/*
void StripNumbers2( int mode, const cVariant& variant, double& num1, double& num2 );
void StripNumbers3( const cVariant& variant, double& num1, double& num2, double& num3 );
void CombineNumber3( cVariant& variant, double num1, double num2, double num3, int nr_decimals );
void CombineNumber2or3( cVariant& variant, double num1, double num2, double num3, int nr_decimals );
void StripPolyOrder( const cVariant& variant, int& polyOrder, double& stateMaxFraction );
*/

void EnsureString( cArray< cString >& stringarray, const cString& b );

// Unlike the template function this one is case and blank IN-sensitive
int ArrayLocationString( const cArray< cString >& a, const cString& b );

void Replace( cString& str, const cString& a, const cString& b );
void NormaliseNames( cArray< cString >& names );
cArray< cString > GetTags( const cString& str, const char& lefttagdelimiter, const char& righttagdelimiter );

cString InvertFXString( const cString& str );

/*
// Operator for YRGridMath REL function
enum eOperatorType {
    singular_operator = -1,
    or_operator,
    and_operator
};

// YRGridMath / OP:
// Restriction criteria for
// RESTRICT and CORESTRICT operations
// ===================================
// Note: DO NOT use operator == on cVariants!!!
class cRestrictionCriterion
{
    const eOperatorType _operatorType;

public:
    cRestrictionCriterion() : _operatorType( singular_operator ) {}
    cRestrictionCriterion( const eOperatorType Operator ) : _operatorType( Operator ) {}
    const eOperatorType& OperatorType() { return _operatorType; }
    static cSharedPtr< cRestrictionCriterion > Create( const cString& criterionType );
    virtual bool check( const cVariant& lhs, const cVariant& rhs ) const = 0;
};

class cRCEQ : public cRestrictionCriterion
{
    cRCEQ() : cRestrictionCriterion( or_operator ) {}
    virtual bool check( const cVariant& lhs, const cVariant& rhs ) const
    {
        return lhs.ComparableTo( rhs ) ? ! lhs.LessThan( rhs ) && !rhs.LessThan( lhs ) : false;
    }
};

class cRCLE : public cRestrictionCriterion
{
    virtual bool check( const cVariant& lhs, const cVariant& rhs ) const
    {
        return !rhs.LessThan(lhs);
    }
};

class cRCGE : public cRestrictionCriterion
{
    virtual bool check( const cVariant& lhs, const cVariant& rhs ) const
    {
        return !lhs.LessThan(rhs);
    }
};

class cRCLT : public cRestrictionCriterion
{
    virtual bool check( const cVariant& lhs, const cVariant& rhs ) const
    {
        return lhs.LessThan(rhs);
    }
};

class cRCGT : public cRestrictionCriterion
{
    virtual bool check( const cVariant& lhs, const cVariant& rhs ) const
    {
        return rhs.LessThan(lhs);
    }
};

class cRCNE : public cRestrictionCriterion
{
    cRCNE() : cRestrictionCriterion( and_operator ) {}
    virtual bool check( const cVariant& lhs, const cVariant& rhs ) const
    {
        return lhs.ComparableTo( rhs ) ? lhs.LessThan( rhs ) || rhs.LessThan( lhs ) : true;
    }
};
*/

#endif

