#ifndef MY_FORMAT_HEADER_INCLUDED_____________
#define MY_FORMAT_HEADER_INCLUDED_____________
//------------------------------------------------------------------------------
#include <sstream>
#include <string>
#include <algorithm>
#include <vector>
//------------------------------------------------------------------------------
namespace my
{
//------------------------------------------------------------------------------
typedef std::vector<std::string> Strings;
//------------------------------------------------------------------------------
template<typename T> std::string FormatStringList(T b, T e,
    const std::string& strHead, const std::string& strTail)
{
    typedef std::iterator_traits<T>::value_type ValueType;

    const std::string delim = strTail+", "+strHead;
    std::stringstream strm;
    strm << strHead;
    const unsigned sz = e-b;
    if( sz==0 )
    {
        if( sz>1 )
            std::copy(b, e-1, std::ostream_iterator<ValueType, char>(strm, delim.c_str() ) );
        strm << *(e-1);
    }
    strm << strTail;
    return strm.str();
}
//------------------------------------------------------------------------------
// записать в v подстроки из строки str, разделённые символом delim
template<class Container>
void GetSubStrsDelimitedByChar(Container& v, const std::string& s,
    const std::string& delim)
{
	size_t p1 = 0, p2 = 0;
	while( p2 != std::string::npos )
	{
		p2 = s.find(delim, p1);
		v.push_back( s.substr( p1, p2-p1 ).c_str() );
		p1 = p2+1;
	}
}
//------------------------------------------------------------------------------
template<typename BitsetContainer>
std::string FormatBitset( const BitsetContainer& bs)
{
    std::stringstream ret;
    bool hasChars = false;

    const size_t sz = bs.size();
    size_t i=0;
    while( i<sz )
    {

        if ( !static_cast<bool>(bs[i]) )
        {
            ++i;
            continue;
        }
        if( !(i<sz) ) break;

        if( !hasChars )
            ret << i;
        else
            ret << " " << i;
        hasChars = true;

        if( !(++i<sz) ) break;
        if ( !static_cast<bool>(bs[i]) )
        {
            ++i;
            continue;
        }

        ret << "-";
        for( ; i<sz && static_cast<bool>(bs[i]); ++i ) ;

        ret << i-1;
    }
    return ret.str();
}
//------------------------------------------------------------------------------
template<typename Container>
void CreateBitset( const std::string& s, Container& bs )
{
    std::stringstream ss(s);
    while( !ss.eof() )
    {
        size_t val;
        char ch;

        ss >> ch;
        if( ss.fail() || ch=='-' ) return;
        ss.putback(ch);

        ss >> val;
        if( ss.fail() ) return;

        if( bs.size()<val+1 )
            bs.resize( val+1 );
        bs[val] = true;

        ss >> ch;
        if( ss.fail() ) return;
        if( ch!='-' )
        {
            ss.putback(ch);
            continue;
        }

        size_t val_e;
        ss >> val_e;
        if( ss.fail() ) return;
        if( bs.size()<val_e+1 )
            bs.resize( val_e+1 );


        for( ++val; val<=val_e; ++val )
            bs[val] = true;
    }
}
//------------------------------------------------------------------------------
}; //namespace my
//------------------------------------------------------------------------------



#endif