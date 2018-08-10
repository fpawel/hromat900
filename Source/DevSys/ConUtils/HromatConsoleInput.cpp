//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
#pragma hdrstop
//------------------------------------------------------------------------------
#include "HromatConsoleInput.h"
#include "bcd_.h"
#include "HromatModbus.h"
#include "AnsiStringUtils_.h"
// для вызова функции PerformModbusTransfer
#include "PerformTransfer.h"
#include "MyExcpt.hpp"
#include "MySynchronize.h"

#include "TaskList.h"
#include "TransferManage.h"
#include "guicon.h"
#include "con_math.h"
#include "hromat_sys.h"
//#include "MyAppProfile.h"
#include "MyAppWinPath.hpp"
#include "grdtns_.h"


//------------------------------------------------------------------------------
#pragma package(smart_init)
//------------------------------------------------------------------------------
//using std::cout;
//using std::endl;
using std::cin;
//------------------------------------------------------------------------------
unsigned int radix = 10;
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
class MyConsoleInputParseException : public MyException
{
public:
	MyConsoleInputParseException( const FileLine& loc, const AnsiString& msg,
    	PMyExcpt nest = PMyExcpt() ) : MyException(loc, msg, nest)
    {}
	virtual ~MyConsoleInputParseException(){}
    virtual AnsiString ClassName() const { return "MyConsoleInputParseException"; }
};
//------------------------------------------------------------------------------
#undef MY_THROW_(msg)
#define MY_THROW_(msg) throw PMyExcpt( new MyConsoleInputParseException(__FILE_LINE__, msg ) )
//------------------------------------------------------------------------------
void ThrowParseValue( const FileLine& loc, const AnsiString& prmName, const AnsiString& msg)
{
	throw PMyExcpt( new MyConsoleInputParseException(loc,
    MYSPRINTF_( "%s - %s", prmName, msg )
    ));
}
//------------------------------------------------------------------------------
void ThrowBadParam( const FileLine& loc, const AnsiString& s)
{
	throw PMyExcpt( new MyConsoleInputParseException(loc,
    MYSPRINTF_( "недопустимое значение: %s", s )
    ));
}
//------------------------------------------------------------------------------
MyByte GetTheByte(const char* s)
{
	const MyInt getVal = MyStrToInt( s, radix );
    const long val = getVal.first;
    const bool ret = getVal.second && val>-1 && val<(0xFF+1);
    return std::make_pair( val, ret );
}
//------------------------------------------------------------------------------
unsigned GetUnsignedIntVal( const AnsiString& prmName,	const AnsiString& sVal, long maxV = 0xFFFF-1, int rdx = -1)
{
	if(rdx==-1) rdx=radix;
    const MyInt getVal = MyStrToInt( sVal, radix );
    const long val = getVal.first;
    if( !getVal.second || val<0 || !(val < maxV) )
    	ThrowParseValue( __FILE_LINE__, prmName, MYSPRINTF_("недопустимое значение \"%s\"", sVal ));
    return val;
}
//------------------------------------------------------------------------------
double GetDoubleVal( const AnsiString& prmName, const AnsiString& sVal,
	double minV = MAX_BCD_FLOAT*-1.0, double maxV = MAX_BCD_FLOAT)
{
    const MyDouble getVal = MyStrToD( sVal );
    const double val = getVal.first;
    if( !getVal.second || val<minV || !(val < maxV) )
    {
    	ThrowParseValue( __FILE_LINE__, prmName, MYSPRINTF_("недопустимое значение \"%s\"", sVal ));
    }
    return val;
}
//------------------------------------------------------------------------------
typedef std::vector<AnsiString> ParamsList;
//------------------------------------------------------------------------------
void CheckParamsListLength(const ParamsList& prms, unsigned len)
{
	if( prms.size()<len ) MY_THROW_( MYSPRINTF_("не хватает параметров - %d из %d", prms.size(),len ));
}
//------------------------------------------------------------------------------
class InpCmd : public boost::noncopyable
{
public:
    virtual ~InpCmd() {}
    virtual AnsiString Name() const = 0;
    virtual AnsiString Dscr() const = 0;
    virtual void Perform(const ParamsList&) = 0;
};
//------------------------------------------------------------------------------
typedef boost::shared_ptr<InpCmd> PCmd;
typedef std::map<AnsiString, PCmd> ParseMap;
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
class ConversionsCmd : public InpCmd
{
public:
	AnsiString Name() const { return "math"; }
    AnsiString Dscr() const { return "Математические подпрограммы:\n"
    "\tBF - BCD во float\n"
    "\tHD - HEX в double\n"
    "\tDH - double в HEX\n"; }

    void Perform(const ParamsList& prms)
    {
    	void (*foo)( const std::string&);
        CheckParamsListLength(prms, 1);
        if( prms.at(0)=="bf" )
        	foo = &ConMath::ConvertBCDFloat; else
        if( prms.at(0)=="hd" )
        	foo = &ConMath::ConvertByteDouble; else
        if( prms.at(0)=="dh" )
        	foo = &ConMath::ConvertDoubleBytes; else
        MY_THROW_( "недопустимый параметр - "+prms.at(0) );

        MyWCout( prms.at(0)+"-->" );
        std::string s = "";
    	while( std::getline(cin,s) && s!="q" )
    	{
        	foo(s);
        	MyWCout( prms.at(0)+"-->" );
    	}
    }
};

//------------------------------------------------------------------------------
class EditTaskListCmd : public InpCmd
{
public:
	AnsiString Name() const { return "tasks"; }
    AnsiString Dscr() const { return "Редактировать список трансферов"; }
    void Perform(const ParamsList&)
    {
    	Hromat900::Instance().GetTransferManager().GetTaskList().DebugEdit_console();
    }
};
//------------------------------------------------------------------------------

class ListRulesCmd : public InpCmd
{
public:
	explicit ListRulesCmd(const ParseMap& parseMap) : InpCmd(), parseMap_(parseMap) {}
	AnsiString Name() const { return "?"; }
    AnsiString Dscr() const { return "Список команд"; }
    void Perform(const ParamsList&)
    {
    	MyWCout( "Поддерживаемые команды:\n" );
    	for( ParseMap::const_iterator i=parseMap_.begin(); i!=parseMap_.end(); ++i )
        	MYWCOUT_( "%s:\t%s\n", i->first, i->second->Dscr() ));
        MyWCout("q или Q - выход.\n");
    }
private:
	const ParseMap& parseMap_;

};
//------------------------------------------------------------------------------
class ShellExecuteCmd : public InpCmd
{
public:
	AnsiString Name() const { return "system"; }
    AnsiString Dscr() const
    {
    	return "режим командной строки ДОС";
    }
    void Perform(const ParamsList& prms)
    {
    	MyWCout( "Выбран режим командной строки ДОС.\n\\>" );
        std::string s = "";
    	while( std::getline(cin,s) && s!="exit" && s!="q" )
    	{
        	std::system( s.c_str() );
        	MyWCout( "\\>" );
    	}
        MyWCout( "Выход из режима командной строки ДОС.\n\\>" );
    }
};
//------------------------------------------------------------------------------
class SetRdxCmd : public InpCmd
{
public:
	AnsiString Name() const { return "rdx"; }
    AnsiString Dscr() const
    {
    	return "[base - 2,10,16] - установить основание представления целых";
    }
    void Perform(const ParamsList& prms);
};
//------------------------------------------------------------------------------
class WriteValueCmd : public InpCmd
{
public:
	AnsiString Name() const { return "send"; }
    AnsiString Dscr() const
    {
    	return
        "[cmd] [val] - REG32=cmd REG33,34=BCD(val)\n\t"
        "[cmd] [b1] [b2] [b3] [b4] - REG32=cmd REG33,34=b1,b2,b3,b4";
    }
    void Perform(const ParamsList& prms);
};
//------------------------------------------------------------------------------
class GetModbusRegBytesCmd : public InpCmd
{
public:
	AnsiString Name() const { return "rdd"; }
    AnsiString Dscr() const
    {
		return
        "[регистр][кол-во] - считать байтовый пулл";
	}
    void Perform(const ParamsList& prms);
};
//------------------------------------------------------------------------------
class GetModbusRegFloatsCmd : public InpCmd
{
public:
	AnsiString Name() const { return "rdv"; }
    AnsiString Dscr() const
    {
		return
        "[регистр][кол-во] - считать массив float";
	}
    void Perform(const ParamsList& prms);
};
//------------------------------------------------------------------------------
class ExploreAppDirs : public InpCmd
{
public:
	AnsiString Name() const { return "dirs"; }
    AnsiString Dscr() const
    {
		return "показать директории профилей";
	}
    void Perform(const ParamsList& prms)
    {
        const AnsiString
        path1 = GetAppDataDir(),
        path2 = GetPersonalDir();

        ::ShellExecute( NULL, "open", path1.c_str(), "", path1.c_str(), SW_SHOW );
        ::ShellExecute( NULL, "open", path2.c_str(), "", path2.c_str(), SW_SHOW );
    }

};
//------------------------------------------------------------------------------
class ExmplGrdtn : public InpCmd
{
public:
	AnsiString Name() const { return "exmplgrdtns"; }
    AnsiString Dscr() const
    {
		return "Создать пример градуировочных файлов";
	}
    void Perform(const ParamsList& prms)
    {
        Grdtn::CreateExample();
    }

};
//------------------------------------------------------------------------------







void SetRdxCmd::Perform(const ParamsList& prms)
{
	CheckParamsListLength(prms, 1);
    const MyInt getVal = MyStrToInt( prms[0] );
    const int rdx = getVal.first;
    if( !getVal.second || !(rdx==10 || rdx==2 || rdx==16) )
    	ThrowParseValue( __FILE_LINE__, "base",
        MYSPRINTF_("недопустимое значение основания \"%s\"", prms[0] ));
    radix = rdx;
}
//------------------------------------------------------------------------------
void GetModbusRegBytesCmd::Perform(const ParamsList& prms)
{
	CheckParamsListLength(prms, 2);
    const unsigned
        	reg0 = GetUnsignedIntVal( "регистр", prms.at(0) ),
            count = GetUnsignedIntVal( "кол-во", prms.at(1), 1000, 10 );
    ByteBuffPtr rxd, rxdEnd;
    ReadModbusRegistersData( reg0, count, rxd, rxdEnd );
}
//------------------------------------------------------------------------------
void GetModbusRegFloatsCmd::Perform(const ParamsList& prms)
{
	CheckParamsListLength(prms, 2);
    const unsigned
    	reg0 = GetUnsignedIntVal( "регистр", prms.at(0) ),
        count = GetUnsignedIntVal( "кол-во", prms.at(1), 1000, 10 );
    VDouble result(count);
    ReadModbusFloat( reg0, count, &result[0] );
    std::cout << "[" << reg0 << "]: " << myIO::OutDoubleVec(result, ", ") << "\n";
}
//------------------------------------------------------------------------------
void WriteValueCmd::Perform(const ParamsList& prms)
{
    const unsigned len = prms.size();
    CheckParamsListLength(prms, 2);
	const unsigned cmd = GetUnsignedIntVal( "cmd", prms.at(0) );
    if( len==2 )
	{
        const double val = GetDoubleVal( "val", prms.at(1) );
    	WriteModbusFloat( cmd, val  );
    }
    else if( len==5 )
	{
        uInt8 dt[4];
    	dt[0] = GetUnsignedIntVal( "b1", prms.at(1), 256 );
    	dt[1] = GetUnsignedIntVal( "b2", prms.at(2), 256 );
    	dt[2] = GetUnsignedIntVal( "b3", prms.at(3), 256 );
        dt[3] = GetUnsignedIntVal( "b4", prms.at(4), 256 );
    	WriteModbus4Bytes( cmd, dt  );
    }
	else
		MY_THROW_( "недопустимое колличество параметров" );
}
//------------------------------------------------------------------------------
class HromatConsoleInput : public boost::noncopyable
{

public:
	HromatConsoleInput()
    {   
    	AddRule( new WriteValueCmd );
        AddRule( new SetRdxCmd );
        AddRule( new GetModbusRegFloatsCmd );
        AddRule( new GetModbusRegBytesCmd );
        AddRule( new ShellExecuteCmd );
        AddRule( new ListRulesCmd(praseMap_) );
        AddRule( new EditTaskListCmd );
        AddRule( new ConversionsCmd );
        AddRule( new ExploreAppDirs );
        AddRule( new ExmplGrdtn );
    }
    void Perform( const std::string&);

    void PrintList()
    {
        ListRulesCmd(praseMap_).Perform( ParamsList() );
    }

private:
    ParseMap praseMap_;

    void AddRule(InpCmd *rule)
    {
    	praseMap_[rule->Name()] = PCmd(rule);
    }
    void SendString( unsigned char, std::stringstream&);
};
//------------------------------------------------------------------------------
void HromatConsoleInput::Perform( const std::string& prfrmStr)
{
	std::stringstream strm( prfrmStr );
    std::string s1;
    strm >> s1;
    if( strm.fail() ) MY_THROW_("отсутсвует введённая команда");

    const MyByte getCmdCode = GetTheByte( s1.c_str() );
    if( getCmdCode.second )
    {
    	SendString(getCmdCode.first, strm);
        return;
    }
    const AnsiString sCmd = s1.c_str();
    const ParseMap::const_iterator eP1 = praseMap_.end(),
    fndP1 = praseMap_.find( sCmd );
    if( fndP1==eP1 ) MY_THROW_(MYSPRINTF_("недопустимая команда \"%s\"", sCmd ));

    // создать список параметров
    ParamsList prms;
    while( true )
    {
    	strm >> s1;
        if( strm.fail() ) break;
        prms.push_back( s1.c_str() );
    }

    if( prms.size()>=1 && prms[0]=="?")
    {
    	MyWCout( sCmd + ": " + fndP1->second->Dscr()+"\n" );
        return;
    }

    try
    {
    	fndP1->second->Perform(prms);
    }
    catch(...)
    {
     	PMyExcpt e = RethrowMyException(__FILE_LINE__);
        if( e->TypeOf<MyConsoleInputParseException>() )
        {
	        MYWCOUT_( "Недопустимый формат команды %s: %s\n%s\n",
            sCmd, e->Message(), "Введите ? для справки. "+sCmd + ": " + fndP1->second->Dscr() ));
        }
        else throw e;
    }
    SetCurrentDir( GetAppDataDir() );
}
//------------------------------------------------------------------------------
void HromatConsoleInput::SendString( unsigned char cmdCode, std::stringstream& strm)
{
    std::string s = "";
    VInt8 txd(0);
    strm >> s;
    while( !strm.fail()  )
    {
        const MyByte getVal = GetTheByte( s.c_str() );
        if( !getVal.second ) break;
        txd.push_back( getVal.first );
        strm >> s;
    }
    ByteBuffPtr rxd, rxdEnd;
    PerformModbusTransfer(cmdCode, txd.begin(), txd.end() , rxd, rxdEnd);
}
//------------------------------------------------------------------------------
void SetConsoleInput()
{
    TransferManager &tmngr = Hromat900::Instance().GetTransferManager();
    tmngr.SetConsoleinputMode( 1 );

	const GUIStdInputHelper stdInput;
    MyWCout( "\n\\>");
    std::string s = "";
    HromatConsoleInput hlpr;

    hlpr.PrintList();
    while( std::getline(cin,s) && s!="q" && s!="Q")
    {
    	std::transform( s.begin(), s.end(), s.begin(), std::tolower );
        try
        {
        	hlpr.Perform( s );
        } catch(...)
        {
            PMyExcpt e = RethrowMyException(__FILE_LINE__);
            MyWCout( e->Message()+"\n" );

        }
        MyWCout( "\\>" );
    }
    tmngr.SetConsoleinputMode( 0 );
}
//------------------------------------------------------------------------------



