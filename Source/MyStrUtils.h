//------------------------------------------------------------------------------
#ifndef MyStrUtilsH
#define MyStrUtilsH
//------------------------------------------------------------------------------

#include <string>
#include <sstream>
#include <vector>
#include "boost\noncopyable.hpp"
#include "boost\shared_ptr.hpp"
#include "boost\lexical_cast.hpp"

//------------------------------------------------------------------------------
#include <system.hpp>
//------------------------------------------------------------------------------
typedef std::pair<double,bool> FloatBool;
typedef std::pair<long,bool> IntBool;
class StringAdapter;
typedef boost::shared_ptr<StringAdapter> PStringAdapter;
//------------------------------------------------------------------------------
std::string HtmlEntitiesToStr( const std::string &src1);
std::string WideStrToAnsiStr(const wchar_t *ures);
std::string UTF8To1251(const std::string &src);
std::string UTF8To1251(const char* b, const char* e);
unsigned GetCodeOfHtmlMnemonic(const std::string& mnemonic);
//------------------------------------------------------------------------------
std::string MyReplace(const std::string& src, const std::string& sRegEx,
    const std::string& fmtStr, bool replaceAll = true);
AnsiString MyReplace1(const AnsiString& src, const AnsiString& sRegEx,
    const AnsiString& fmtStr, bool replaceAll = true);
//------------------------------------------------------------------------------
void ParseArrayOfIntervals(const std::string& src, std::vector<unsigned>&);
//------------------------------------------------------------------------------
FloatBool   Str2Float   ( const std::string& s );
bool        TryStr2Float( const std::string& s, double* pVal = 0  );
double      Str2FloatDef( const std::string& s, double def );

IntBool     Str2Int     ( const std::string& s, int radix = 10 );
long        Str2IntDef  ( const std::string& s, long def );
bool        TryStr2Int  ( const std::string& s, long* pVal = 0  );
//------------------------------------------------------------------------------
std::string Int2Str( __int64 v, int radix = 10 );
std::string Float2Str( double v );
//------------------------------------------------------------------------------
std::string ChangeFileExtension(const std::string& fileName, const std::string& extension);
//------------------------------------------------------------------------------
template<typename IterT>
IterT find_n_serie_bound(IterT i, IterT end)
{
    for( ;i!=end; ++i) {
        IterT next = i;
        ++next;
        if(next==end || (*next != *i+1)&&(*next != *i) )
            return next;
    }
    return end;
}
//------------------------------------------------------------------------------
template<typename IterT> std::string format_sorted_intervals(IterT i, IterT e)
{
    std::string s;
    for( ; i!=e; )
    {
        IterT i1=find_n_serie_bound(i,e), i2 = i1;
        --i2;
        if(*i==*i2)
            s += Int2Str(*i);
        else
            s += Int2Str(*i)+"-"+Int2Str(*i2);
        i = i1;
        s += " ";
    }
    return s;
}
//------------------------------------------------------------------------------
template<typename Target, typename Source> Target lexical_cast_def(const Source &arg, const Target &def )
{
    try {
        Target ret = boost::lexical_cast<Target, Source>(arg);
        return ret;
    }
    catch(boost::bad_lexical_cast) {
        return def;
    }
}
//------------------------------------------------------------------------------
template<> std::string lexical_cast_def<std::string, std::string>(const std::string &arg, const std::string& def)
{
    return arg.empty() ? def : arg;
}
//------------------------------------------------------------------------------
template<> std::string lexical_cast_def<std::string, AnsiString>(const AnsiString &arg, const std::string &def)
{
    return arg.IsEmpty() ? "" : arg.c_str();
}
template<> AnsiString lexical_cast_def<AnsiString, std::string>(const std::string &arg, const AnsiString &def)
{
    return arg.empty() ? "" : arg.c_str();
}
//------------------------------------------------------------------------------


template<typename Target, typename Source> Target lexical_cast(const Source &arg)
{
    try {
        Target ret = boost::lexical_cast<Target, Source>(arg);
        return ret;
    }
    catch(boost::bad_lexical_cast) {
        return Target();
    }
}
template<> std::string lexical_cast<std::string, std::string>(const std::string &arg)
{
    return arg;
}
template<> AnsiString lexical_cast<AnsiString, std::string>(const std::string &arg)
{
    return arg.c_str();
}
template<> std::string lexical_cast<std::string, AnsiString>(const AnsiString &arg)
{
    return arg.c_str();
}
//------------------------------------------------------------------------------
class StringAdapter : public boost::noncopyable
{
public:
	virtual std::string Get() const = 0;
    virtual void Set(const std::string& ) = 0;
};
//------------------------------------------------------------------------------
template<typename T>
class DefaultStrAdpt : public StringAdapter
{
public:
    explicit DefaultStrAdpt(T& ref) : ref_(ref) {}
    virtual std::string Get() const
    {
        return lexical_cast<std::string>(ref_);
    }
    virtual void Set(const std::string& s)
    {
        ref_ = lexical_cast<T>(s, ref_);
    }
private:
    T &ref_;
};
template<typename T> StringAdapter* CreateDefStrAdpt(T& ref)
{
    return new DefaultStrAdpt<T>(ref);
}
//------------------------------------------------------------------------------
template<>
class DefaultStrAdpt<std::string> : public StringAdapter
{
public:
    explicit DefaultStrAdpt(std::string& ref) : ref_(ref) {}
    virtual std::string Get() const
    {
        return ref_;
    }
    virtual void Set(const std::string& s)
    {
        ref_ = s;
    }
private:
    std::string &ref_;
};
template<>
class DefaultStrAdpt<AnsiString> : public StringAdapter
{
public:
    explicit DefaultStrAdpt(AnsiString& ref) : ref_(ref) {}
    virtual std::string Get() const
    {
        return ref_.c_str();
    }
    virtual void Set(const std::string& s)
    {
        ref_ = s.c_str();
    }
private:
    AnsiString &ref_;
};
//------------------------------------------------------------------------------
template<typename IntegerType>
class Int2StrAdapter : public StringAdapter
{
public:
    explicit Int2StrAdapter(IntegerType& ref) : ref_(ref) {}
    virtual std::string Get() const{ return Int2Str(ref_); }
    virtual void Set(const std::string& s){ ref_ = Str2IntDef(s,ref_); }
private:
    IntegerType &ref_;
};
template<typename IntegerType> StringAdapter* CreateIntStrAdapter(IntegerType& ref)
{
    return new Int2StrAdapter<IntegerType>(ref);
}
//------------------------------------------------------------------------------
template<typename FloatType>
class Float2StrAdapter : public StringAdapter
{
public:
    explicit Float2StrAdapter(FloatType& ref) : ref_(ref) {}
    virtual std::string Get() const{ return Float2Str(ref_); }
    virtual void Set(const std::string& s){ ref_ = Str2FloatDef(s,ref_); }
private:
    FloatType &ref_;
};
template<typename FloatType> StringAdapter* CreateFloatStrAdapter(FloatType& ref)
{
    return new Float2StrAdapter<FloatType>(ref);
}
//------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------
