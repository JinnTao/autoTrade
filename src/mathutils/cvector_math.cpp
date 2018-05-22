#include <math.h>
#include "cErr.h"

#include "cvector_math.h"

static double normal_error( double x );
static double normal_error_comp( double a );
static double polynomial_1( double x, double coef[], int N );

const double SQRT2OVER2 = 7.07106781186547524401E-1;
const double MAXLOG = 8.8029691931113054295988E1;

static double Mtools_nep[] = {
    2.46196981473530512524E-10, 5.64189564831068821977E-1, 7.46321056442269912687E0,
    4.86371970985681366614E1, 1.96520832956077098242E2, 5.26445194995477358631E2,
    9.34528527171957607540E2, 1.02755188689515710272E3, 5.57535335369399327526E2};

static double Mtools_neq[] = {
    1.32281951154744992508E1, 8.67072140885989742329E1, 3.54937778887819891062E2,
    9.75708501743205489753E2, 1.82390916687909736289E3, 2.24633760818710981792E3,
    1.65666309194161350182E3, 5.57535340817727675546E2};

static double Mtools_ner[] = {
    5.64189583547755073984E-1, 1.27536670759978104416E0, 5.01905042251180477414E0,
    6.16021097993053585195E0, 7.40974269950448939160E0,2.97886665372100240670E0};

static double Mtools_nes[] = {
    2.26052863220117276590E0, 9.39603524938001434673E0, 1.20489539808096656605E1,
    1.70814450747565897222E1, 9.60896809063285878198E0, 3.36907645100081516050E0};

static double Mtools_nip0[5] = {
    -5.99633501014107895267E1, 9.80010754185999661536E1, -5.66762857469070293439E1,
    1.39312609387279679503E1, -1.23916583867381258016E0,};

static double Mtools_niq0[8] = {
    1.95448858338141759834E0, 4.67627912898881538453E0, 8.63602421390890590575E1,
   -2.25462687854119370527E2, 2.00260212380060660359E2, -8.20372256168333339912E1,
    1.59056225126211695515E1, -1.18331621121330003142E0,};

static double Mtools_nip1[9] = {
    4.05544892305962419923E0, 3.15251094599893866154E1, 5.71628192246421288162E1,
    4.40805073893200834700E1, 1.46849561928858024014E1, 2.18663306850790267539E0,
   -1.40256079171354495875E-1, -3.50424626827848203418E-2, -8.57456785154685413611E-4,};

static double Mtools_nip2[9] = {
    3.23774891776946035970E0, 6.91522889068984211695E0, 3.93881025292474443415E0,
    1.33303460815807542389E0, 2.01485389549179081538E-1, 1.23716634817820021358E-2,
    3.01581553508235416007E-4, 2.65806974686737550832E-6, 6.23974539184983293730E-9,};

static double Mtools_niq1[8] = {
    1.57799883256466749731E1, 4.53907635128879210584E1, 4.13172038254672030440E1, 
    1.50425385692907503408E1, 2.50464946208309415979E0, -1.42182922854787785574E-1,
    -3.80806407691578277194E-2, -9.33259480895457427372E-4,};

static double Mtools_niq2[8] = {
    6.02427039364742014255E0, 3.67983563856160859403E0, 1.37702099489081330271E0,
    2.16236993594496635890E-1, 1.34204006088543189037E-2, 3.28014464682127739104E-4,
    2.89247864745380683936E-6, 6.79019408009981274425E-9,};

static double Mtools_T[] = {
    9.60497373987051638749E0, 9.00260197203842689217E1, 2.23200534594684319226E3,
    7.00332514112805075473E3, 5.55923013010394962768E4};

static double Mtools_U[] = {
    3.35617141647503099647E1, 5.21357949780152679795E2, 4.59432382970980127987E3,
    2.26290000613890934246E4, 4.92673942608635921086E4};


double yr_log( double x, double limit )
{
    if( x < 0.0 )
    {
        //print errors here
        yr_error( "Illegal log!" );
    }
    if( x < limit )
        return log( limit );

    return log( x );
}

double normal_inv2( double u )
{
    if( u < -FUZZ || u > ( 1.0 + FUZZ ) )
    {
        //print errors here
        yr_error( "Argument must be in [0,1]." );
    }
    if( u < 1e-16 )
        return -8.22208221613043;
    if( u > 1.0 - 1e-16 )
        return 8.2208221613043;

    double a[4] = {2.50662823884,
                 -18.61500062529,
                  41.39119773534,
                 -25.44106049637};

    double b[4] = {-8.47351093090,
                   23.08336743743,
                  -21.06224101826,
                    3.13082909833};

    double c[9] = {0.3374754822726147,
                   0.9761690190917186,
                   0.1607979714918209,
                   0.0276438810333863,
                   0.0038405729373609,
                   0.0003951896511919,
                   0.0000321767881768,
                   0.0000002888167364,
                   0.0000003960315187};

    double x = 0.;
    double r = 0.;

    x = u - 0.5;
    if( fabs(x) < 0.42 )
    {
        r = x*x;
        r = x*(((a[3]*r+a[2])*r+a[1])*r+a[0])/((((b[3]*r+b[2])*r+b[1])*r+b[0])*r+1.0);
        return( r );
    }
    r = u;
    if( x > 0.0 )
    {
        r = 1.0 - u;
    }
    r = log(-log(r));
    r = c[0]+r*(c[1]+r*(c[2]+r*(c[3]+r*(c[4]+r*(c[5]+r*(c[6]+r*(c[7]+r*c[8])))))));
    if( x < 0.0 )
        r = -r;
    
    return( r );
}

double polynomial( double x, double coef[], int N )
{
    double *p = coef;
    double ans = *p++;
    do ans = ans * x + *p++;
    while( --N );
    return( ans );
}

double normal_error_comp( double a )
{
    double p;
    double q;
    double x;
    double y;
    double z;

    if( a < 0.0 )
        x = -a;
    else
        x = a;

    if( x < 1.0 )
        return( 1.0 - normal_error(a) );
    z = -a * a;
    if( z < -MAXLOG )
        return( 0.0 );
    z = exp( z );
    if( x < 8.0 )
    {
        p = polynomial( x, Mtools_nep, 8 );
        q = polynomial( x, Mtools_neq, 8 );
    }
    else
    {
        p = polynomial( x, Mtools_ner, 5 );
        q = polynomial( x, Mtools_nes, 6 );
    }
    y = ( z * p ) / q;
    if( a < 0.0 )
        y = 2.0 - y;
    return( y );
}

double polynomial_1( double x, double coef[], int N )
{
    double *p = coef;
    double ans = x + *p++;
    int i = N - 1;
    do ans = ans * x + *p++;
    while( --i );
    return( ans );
}

double normal_error( double x )
{
    double y;
    double z;

    if( fabs( x ) > 1.0 )
        return( 1.0 - normal_error_comp( x ) );
    z = x * x;
    y = x * polynomial( z, Mtools_T, 4 ) / polynomial_1( z, Mtools_U, 5 );
    return( y );
}

double normal( double a )
{
    double y;
    double x = a * SQRT2OVER2;
    double z = fabs( x );
    if( z < SQRT2OVER2 )
        y = 0.5 + 0.5 * normal_error( x );
    else
    {
        y = 0.5 * normal_error_comp( z );
        if( x > 0.0 )
            y = 1.0 - y;
    }
    return( y );
}

double normal_inv( double y0 )
{
    double x;
    double y;
    double z;
    double y2;
    double x0;
    double x1;
    int code;

    if( y0 < 0.0 || y0 > 1.0 )
    {
        //print errors here
        yr_error( "Argument must be in [0,1]." );
    }
    if( y0 < 1e-16 )
        return -20.0;
    if( y0 > 1.0 - 1e-16 )
        return 20.0;

    code = 1;
    y = y0;
    if( y > (1.0 - 0.13533528323661269189) )
    {
        /* 0.135... = exp(-2)*/
        y = 1.0 - y;
        code = 0;
    }
    if( y > 0.13533528323661269189 )
    {
        y = y - 0.5;
        y2 = y * y;
        x = y + y * ( y2 + polynomial( y2, Mtools_nip0, 4) / polynomial_1( y2, Mtools_niq0, 8 ) );
        x = x * YR_SQRT2PI;
        return( x );
    }

    x = sqrt( -2.0 * log( y ) );
    x0 = x - log( x ) / x;
    z = 1.0 / x;
    if( x < 8.0 )
        x1 = z * polynomial( z, Mtools_nip1, 8 ) / polynomial_1( z, Mtools_niq1, 8 );
    else
        x1 = z * polynomial( z, Mtools_nip2, 8 ) / polynomial_1( z, Mtools_niq2, 8 );
    x = x0 - x1;
    if( code != 0 )
        x = -x;
    return( x );
}