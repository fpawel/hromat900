//------------------------------------------------------------------------------
#pragma hdrstop
//------------------------------------------------------------------------------
#include "MyStrUtils.h"
//------------------------------------------------------------------------------
#include <fstream>
#include <iostream>
#include <string>
#include <iterator>
#include <deque>
#include <map>
#include <sstream>
//------------------------------------------------------------------------------
#include "boost/regex.hpp"
//------------------------------------------------------------------------------
#include <windows.h>
//------------------------------------------------------------------------------
#pragma package(smart_init)
//------------------------------------------------------------------------------
typedef std::map<std::string,unsigned> HtmlMemonics2StrMapT;
//------------------------------------------------------------------------------
HtmlMemonics2StrMapT CreateHtmlMemonics2StrMap()
{
    struct H { static unsigned GetStreamSize(std::istream& strm)
    {
	    const std::size_t posG = strm.tellg();
        strm.seekg(0, std::ios::beg);
        const std::size_t pos1 = strm.tellg();
        strm.seekg(0, std::ios::end);
        const std::size_t pos2 = strm.tellg();

        strm.seekg(posG, std::ios::beg);
        return pos2-pos1;
    }};


    HtmlMemonics2StrMapT ret;
    std::ifstream srcFile("html-мнемоники.txt", std::ios::binary | std::ios::in );
    std::size_t sz = H::GetStreamSize(srcFile);
    std::vector<char>dst(sz+1, '\0');
    srcFile.read( &dst.at(0), sz );
    srcFile.close();
    const char *xItStart = dst.begin(), *xItEnd = dst.end();
    //"&nbsp;"	"&#160;"
    boost::regex xRegEx("\"&([A-Za-z]+[a-z]*);\"\\s*\"&#([0-9]+);");
    for( boost::cregex_iterator xIt(xItStart, xItEnd, xRegEx); xIt != boost::cregex_iterator(); ++xIt)
    {
        const boost::cregex_iterator::value_type &xRes = *xIt;
        const std::string
            s1( xRes[1].first, xRes[1].second ), s2( xRes[2].first, xRes[2].second );
        ret[s1] = std::atoi( s2.c_str() );
    }
    return ret;    
}
//------------------------------------------------------------------------------
unsigned GetCodeOfHtmlMnemonic(const std::string& mnemonic)
{
    typedef HtmlMemonics2StrMapT::const_iterator IterT;
    static const HtmlMemonics2StrMapT mnem2int = CreateHtmlMemonics2StrMap();
    static const IterT beg = mnem2int.begin(), end = mnem2int.end();
    const IterT i = mnem2int.find(mnemonic);
    return i==end ? 0 : i->second; 
}
//------------------------------------------------------------------------------
std::string WideStrToAnsiStr(const wchar_t *ures)
{
	const unsigned multiByteBuffSize = WideCharToMultiByte( 1251, 0, ures, -1, 0, 0, 0, 0);
	if(!multiByteBuffSize) return "";
    std::vector<char> mChars(multiByteBuffSize, '\0');
	char *cres = &mChars.at(0);
	if( !WideCharToMultiByte( 1251, 0, ures, -1, cres, multiByteBuffSize, 0, 0)) return "";
	return cres;
}
//------------------------------------------------------------------------------
std::string UTF8To1251(const std::string &src)
{
    const unsigned char *str = src.c_str();
	const unsigned wideCharBuffSize = MultiByteToWideChar(CP_UTF8, 0, str, -1, 0, 0);
	if (!wideCharBuffSize) return "";
    std::vector<wchar_t> wChars(wideCharBuffSize, L'\0');
	wchar_t *ures = &wChars.at(0);
	if(!MultiByteToWideChar(CP_UTF8, 0, str, -1, ures, wideCharBuffSize) ) return "";
    return WideStrToAnsiStr(ures);
}
//------------------------------------------------------------------------------
std::string UTF8To1251(const char* b, const char* e)
{
    return UTF8To1251( std::string(b,e) );
}
//------------------------------------------------------------------------------

std::string HtmlEntitiesToStr( const std::string &src1)
{
    using boost::cregex_iterator;
    std::deque<std::string> replaces;
    std::string src = src1.c_str();
    boost::regex xRegEx("&(?:#([0-9]+)|([A-Za-z]+[a-z]*));");

    // ищем первое вхождение подстроки со спец. символами
    const char *xItStart = src.begin(), *xItEnd = src.end();
    for( cregex_iterator xIt(xItStart, xItEnd, xRegEx); xIt != cregex_iterator(); ++xIt)
    {
        const cregex_iterator::value_type &xRes = *xIt;
        const std::string sNum( xRes[1].first, xRes[1].second ),
            sStr( xRes[2].first, xRes[2].second );
        unsigned code = 0;
        if( !sStr.empty() )
            code = GetCodeOfHtmlMnemonic(sStr);
        else if( !sNum.empty() )
            code = std::atoi( sNum.c_str() );
        if(code)
        {
            wchar_t ws[2] = {L'\0'};
            ws[0] = code;
            std::string ss = WideStrToAnsiStr(ws);
            replaces.push_back( ss );
        }
        else
            replaces.push_back( "" );

    }
    for( ; !replaces.empty(); replaces.pop_front() )
    {
        src = boost::regex_replace(src, xRegEx, replaces[0],
            boost::match_default | boost::format_perl | boost::format_first_only );
        //src = src;
    }
    return src;
}
//------------------------------------------------------------------------------
std::string MyReplace(const std::string& src, const std::string& sRegEx,
    const std::string& fmtStr, bool replaceAll)
{
    const boost::match_flag_type flags = boost::match_default | boost::format_perl |
    ( replaceAll ? boost::format_all : boost::format_first_only );
    boost::regex xRegEx( sRegEx );
    return boost::regex_replace(src, xRegEx, fmtStr, flags );
}
AnsiString MyReplace1(const AnsiString& src, const AnsiString& sRegEx,
    const AnsiString& fmtStr, bool replaceAll)
{
    return MyReplace(src.c_str(), sRegEx.c_str(), fmtStr.c_str(), replaceAll).c_str();
}
//------------------------------------------------------------------------------
FloatBool Str2Float( const std::string& s )
{
    FloatBool res(0,false);
    const unsigned sz = s.size();
    if( sz==0 ) return res;
    const std::string s1 = MyReplace(s, ",", ".");

    const char *pS = s1.c_str() ;
    char *pEndS = "x";
    res.first = strtod( pS, &pEndS);
    res.second = ( pEndS-pS==(int)sz );
    return res;
}
//------------------------------------------------------------------------------
bool TryStr2Float( const std::string& s, double* pVal  )
{
	const FloatBool val = Str2Float( s );
    if( val.second && pVal ) *pVal = val.first;
    return val.second;
}
//------------------------------------------------------------------------------
double Str2FloatDef( const std::string& s, double def  )
{
    const FloatBool val = Str2Float( s );
    return val.second ? val.first : def;
}
//------------------------------------------------------------------------------
IntBool Str2Int( const std::string& s, int radix  )
{
    IntBool res(0,0);
    const unsigned len = s.size();
    if( len==0 ) return res;
    const char *pS = s.c_str() ;
    char *pEndS = "x";
    res.first = strtol( pS, &pEndS, radix);
    res.second = ( pEndS-pS==(int)len );
    return res;
}
//------------------------------------------------------------------------------
long Str2IntDef( const std::string& s, long def )
{
    IntBool v = Str2Int( s  );
    return v.second ? v.first : def;
}
//------------------------------------------------------------------------------
bool TryStr2Int( const std::string& s, long* pVal )
{
    const IntBool val = Str2Int( s );
    if( val.second && pVal ) *pVal = val.first;
    return val.second;
}
//------------------------------------------------------------------------------
std::string Int2Str( __int64 v, int radix )
{
    char s[100] = {0};
    _i64toa(v, s, radix);
    return std::string(s);
}
//------------------------------------------------------------------------------
std::string Float2Str( double v )
{
    std::stringstream sstr;
    sstr << v;
    return sstr.str();
}
//------------------------------------------------------------------------------
std::string ChangeFileExtension(const std::string& fileName, const std::string& extension)
{
    return MyReplace(fileName, "\\.[^$\\.]*$", "."+extension);
}
//------------------------------------------------------------------------------
unsigned UnsignedXResult(const boost::cmatch& xResults, unsigned n)
{
    return Str2IntDef( std::string( xResults[n].first, xResults[n].second ), 0 );
}
void ParseArrayOfIntervals(const std::string& src, std::vector<unsigned>& dst)
{
    dst.clear();
    std::string::const_iterator xItStart = src.begin(), xItEnd = src.end(), xIt = xItStart;
    boost::cmatch xResults;
    const char numRegEx[] = "^\\s*([0-9]+)\\s*";

    for( ; xIt!=xItEnd;  ) {
        unsigned v1, v2;
        if( !boost::regex_search(xIt, xItEnd, xResults, boost::regex(numRegEx) ) )
            return;
        xIt = xResults[0].second;
        v1 = UnsignedXResult(xResults, 1);
        if( boost::regex_search(xIt, xItEnd, xResults, boost::regex("^[^0-9]*\\-\\s*([0-9]+)\\s*") ) ) {
            v2 = UnsignedXResult(xResults, 1);
            xIt = xResults[0].second;
            for(unsigned n=v1; n<=v2; ++n)
                dst.push_back(n);
        } else {
            dst.push_back(v1);
        }
    }

}

