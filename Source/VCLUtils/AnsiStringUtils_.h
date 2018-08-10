//---------------------------------------------------------------------------
#ifndef AnsiStringUtils_H
#define AnsiStringUtils_H
//------------------------------------------------------------------------------
#define VCL_IOSTREAM
#include <SysUtils.hpp>
#include <Graphics.hpp>
//------------------------------------------------------------------------------
#include <utility>
#include <vector>
//------------------------------------------------------------------------------
#define MYSPRINTF_ AnsiString().sprintf
#define WCOUTSYNC_ my::SynchronizeWCout( AnsiString().sprintf
//------------------------------------------------------------------------------
typedef std::vector<AnsiString> MyStringList;
MyStringList GetMyStringList(const AnsiString& srcStr, const AnsiString& delim);
int MyStringListIndexOf(const MyStringList& list, const AnsiString& s);


// translates a specified number of characters in a string into the OEM-defined character set
AnsiString MyStringToOem( const AnsiString& );
AnsiString CutCellText(const AnsiString &origCellText, TCanvas& cnv, int width);
AnsiString BoolArrayToStr( const bool* b, const bool* e );
void StrToBoolArray( bool* b, bool* e, const AnsiString& s );

typedef std::pair<double,bool> MyDouble;
MyDouble MyStrToD( const AnsiString& s );
bool MyTryStrToFloat( const AnsiString& s, double* pVal = NULL  );
double MyStrToFloatDef( const AnsiString& s, double def  );

typedef std::pair<long,bool> MyInt;
MyInt MyStrToInt( const AnsiString& s, int radix = 10 );

AnsiString MyFormatFloat( double v, int n );
AnsiString MyDToStr( const MyDouble& v, int n );
AnsiString MyFloatTToStr( double );

void ShowUserInputError(const AnsiString& fld, const AnsiString& val);

AnsiString MyBuffToStr( const unsigned char *buffer, const unsigned char *bufferEnd,
    const AnsiString& ss = "" );

AnsiString MyBuffToStr1( const unsigned char *buffer, const unsigned char *bufferEnd );

AnsiString MakeFileNameAsDate( const AnsiString& dir, const AnsiString& ext,
    TDateTime date, const AnsiString& add_back = "" );

AnsiString DateToPath( TDateTime date );
AnsiString DateToFilename( TDateTime date );
AnsiString GetFileNameWithoutExtension(const AnsiString& ss);

AnsiString MyFormatDateTime1(TDateTime);

bool IsAlphaNumber( const AnsiString& s );

AnsiString NormString( const AnsiString& s )
{
    return s.IsEmpty() ? AnsiString(" ") : s;
}

unsigned CountAfterCommaDigits(double v);

AnsiString MyDateTimeToStr(TDateTime);
AnsiString MyFormatMonth( unsigned month );

void SkipNotIntegerInput( const AnsiString& s, char &keyChar);
void SkipNotFloatInput( const AnsiString& s, char &keyChar);
void SkipNotPrecentInput( const AnsiString& s, char &keyChar);

AnsiString VarTypeToStr(int code);
AnsiString StrReplaceSymbols( const AnsiString& s, char ch, char ch1 );

AnsiString ReplaceSubstring( const AnsiString& str, const AnsiString& oldSubString,
	const AnsiString& newSubstring);

unsigned GetLineNumberForPosition( const char *s, unsigned pos );

std::string cp1251_to_utf8(const char *str);




//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
