//------------------------------------------------------------------------------
#pragma hdrstop
//------------------------------------------------------------------------------
#include "HromatUtils.h"
//------------------------------------------------------------------------------
#include "boost\noncopyable.hpp"
#include "boost\shared_ptr.hpp"
//------------------------------------------------------------------------------
#include <inifiles.hpp>
//------------------------------------------------------------------------------
#include <set>
//------------------------------------------------------------------------------
#include "AnsiStringUtils_.h"
#include "Hromat900.hpp"
#include "MshrmtArc.hpp"
#include "HromatGas.hpp"
#include "myconv.h"
#include "my_static_check.h"
#include "hrmtstat.h"
#include "Hromatogram.hpp"
#include "MyAppWinPath.hpp"
#include "MyFileCtrl.h"
#include "MyExcpt.hpp"
#include "myformat.hpp"
#include "MyNoDestroySingleton.hpp"
//------------------------------------------------------------------------------
#pragma package(smart_init)
//------------------------------------------------------------------------------
#include <DateUtils.hpp>
//------------------------------------------------------------------------------
MY_STATIC_CHECK(sizeof(HromatPhysChem)==8*3*sizeof(double),
	Nevernoe_predpolozhenie_o_razmere_HromatPhysChemValsT);

MY_STATIC_CHECK(sizeof(HromatBasePicData)==HROMAT_BASE_PIC_STRUCT_SIZE,
	Nevernoe_predpolozhenie_o_razmere_HromatBasePicData);

MY_STATIC_CHECK(sizeof(HromatPicData)==HROMAT_IDENTIFIED_PIC_STRUCT_SIZE,
	Nevernoe_predpolozhenie_o_razmere_HromatPicData);

MY_STATIC_CHECK(sizeof(TDateTime)==sizeof(double),
    Nevernoe_predpolozhenie_o_razmere_TDateTime);
    
MY_STATIC_CHECK(sizeof(HromatDateTimeInternal)==1+1+2+1+1+1+1,
	Nevernoe_predpolozhenie_o_razmere_HromatDateTimeInternal);

MY_STATIC_CHECK( sizeof(MeasureData)==
    sizeof(unsigned short int)*4 + sizeof(double)*MAX_GAZCOMP + sizeof(HromatPhysChem),
    Nevernoe_predpolozhenie_o_razmere_MeasureData);

MY_STATIC_CHECK( sizeof(Jurnal_GraduateArchive)==
    sizeof(HromatDateTimeInternal) +
    sizeof(double)*(1 + MAX_GAZCOMP*4 ) + 16*MAX_GAZCOMP,
    Nevernoe_predpolozhenie_o_razmere_Jurnal_GraduateArchive);

MY_STATIC_CHECK( sizeof(Jurnal_Analize) ==
    sizeof(HromatDateTimeInternal) + sizeof(double)*2 + sizeof(MeasureData)*30,
    Nevernoe_predpolozhenie_o_razmere_Jurnal_Analize);

MY_STATIC_CHECK( sizeof(HromatGraduation)==MAX_GAZCOMP*sizeof(double)+
	sizeof(HromatDateTimeInternal), Nevernoe_predpolozhenie_o_razmere_HromatGraduation);

MY_STATIC_CHECK( sizeof(HromatStatistic)==sizeof(double)*( 2*MAX_GAZCOMP + 17 )+
    2 + 6, HromatStatistic_Size_Error);

MY_STATIC_CHECK( sizeof(HromatStatisticRepresentation) == sizeof(HromatValStr) *
    2 * ( MAX_GAZCOMP + 8 ) + 2 + 6, HromatStatisticRepresentation_Size_Error );

MY_STATIC_CHECK(sizeof(HromatogramId)==8+4+2+2, Nevernoe_predpolozhenie_o_razmere_HromatogramId);

MY_STATIC_CHECK(sizeof(HromatGraduateItem)==3*sizeof(HromatGraduation)+4*sizeof(unsigned)+
    sizeof(double),
    Nevernoe_predpolozhenie_o_razmere_HromatGraduateItem);

//------------------------------------------------------------------------------
std::vector<unsigned> GetNumbersList1(const std::string& source )
{
    std::vector<unsigned> ret;
    std::stringstream sstr(source);

    while( !sstr.fail() )
    {
        std::string s;		
        sstr >> s;
        const MyInt v = MyStrToInt( s.c_str() );
        if( v.second && v.first>0 )
            ret.push_back(v.first);
        else
        {
            std::vector<std::string> pairVals;
            my::GetSubStrsDelimitedByChar(pairVals, s, "-");
            if( pairVals.size()!=2 ) continue;
            const MyInt
                v1 = MyStrToInt( pairVals[0].c_str() ),
                v2 = MyStrToInt( pairVals[1].c_str() );
            if( !v1.second || !v2.second ) continue;
            for( unsigned n=v1.first; n<=v2.first; ++n )
                ret.push_back(n);
        }
    }
    return ret;
}
//------------------------------------------------------------------------------

HromatGraduation::HromatGraduation()
{
}
HromatGraduation::HromatGraduation(const HromatGraduation& o)
{
    std::copy( o.v, o.v + MAX_GAZCOMP, v);
    dateTime = o.dateTime;
}
//------------------------------------------------------------------------------
void HromatGraduation::operator=(const HromatGraduation& o)
{
    std::copy( o.v, o.v + MAX_GAZCOMP, v);
    dateTime = o.dateTime;
}
//------------------------------------------------------------------------------
HromatDateTimeInternal ExtractHromatDateTimeInternal( const unsigned char* p)
{
	HromatDateTimeInternal ret;
    ret.day_ 		= p[0];
    ret.month_		= p[1];
    ret.year_		= (p[2] << 8) + (p[3] << 0);
    ret.hour_ 		= p[4];
    ret.minute_		= p[5];
    ret.second_ 	= p[7];
    return ret;
}
//------------------------------------------------------------------------------
unsigned ExtractRGMChanal(unsigned char type)
{
    return (type >> 4);
}
//------------------------------------------------------------------------------
bool ExtractModbusFileType(unsigned char type)
{
    const unsigned char v = type & 0x0F;
    return v;
}
//------------------------------------------------------------------------------
bool TryEncodeDateTime( const HromatDateTimeInternal& hdt, TDateTime& ret)
{
    return TryEncodeDateTime(hdt.year_, hdt.month_, hdt.day_, hdt.hour_,
    	hdt.minute_, hdt.second_, 0, ret);
}
//------------------------------------------------------------------------------
TDateTime EncodeDateTime( const HromatDateTimeInternal& hdt)
{
    TDateTime ret = 0;
    if( !TryEncodeDateTime(hdt.year_, hdt.month_, hdt.day_, hdt.hour_,
    	hdt.minute_, hdt.second_, 0, ret) )
        ret = 0;
    return ret;
}
//------------------------------------------------------------------------------
TDateTime ExtractHromatDateTime( const unsigned char* p)
{
    return EncodeDateTime( ExtractHromatDateTimeInternal(p) ) ;
}
//------------------------------------------------------------------------------
HromatDateTimeInternal DecodeDateTime( TDateTime dt )
{
	Word AYear, AMonth, ADay, AHour, AMinute, ASecond, AMilliSecond;
	DecodeDateTime(dt, AYear, AMonth, ADay, AHour, AMinute, ASecond, AMilliSecond);

	HromatDateTimeInternal ret;
    ret.day_ 		= ADay;
    ret.month_		= AMonth;
    ret.year_		= AYear;
    ret.hour_ 		= AHour;
    ret.minute_		= AMinute;
    ret.second_ 	= ASecond;
    return ret;
}
//------------------------------------------------------------------------------
bool operator==( const HromatDateTimeInternal& lhs, const HromatDateTimeInternal& rhs)
{
	#define COMP_(nn) lhs.##nn##_==rhs.##nn##_
	return
    COMP_(day) && COMP_(month) && COMP_(year) && COMP_(hour) &&
    COMP_( minute ) && COMP_( second);
    #undef COMP_
}
//------------------------------------------------------------------------------
void PhysChemFieldAsStr( const HromatPhysChem& phch, unsigned i, AnsiString& s1, AnsiString& s2)
{
    const double
        v = GetPhysChemVal(phch, i, 0),
        d = GetPhysChemVal(phch, i, 1);
    if( v<0 || d<0 )
    {
    	s1 = s2 = "-";
    	return;
    }
    const unsigned
        countAfterCommaDigitsVal  = CountAfterCommaDigits( v  ),
        countAfterCommaDigitsIndet  = CountAfterCommaDigits( d  ),
        countAfterCommaDigits = std::max( countAfterCommaDigitsVal, countAfterCommaDigitsIndet);

    s1 = MyFormatFloat(v, countAfterCommaDigits);
    s2 = MyFormatFloat(d, countAfterCommaDigits);
}
//------------------------------------------------------------------------------
AnsiString PhysChemFieldAsStr( const HromatPhysChem& phch, unsigned i)
{
	AnsiString s1, s2;
    PhysChemFieldAsStr( phch, i, s1, s2);
    return (s1=="-"||s2=="-") ? AnsiString("-") : s1+" ± "+s2;
}
//------------------------------------------------------------------------------

std::ostream& operator<<(std::ostream& strm, const MeasureData& o )
{
	const unsigned gasCnt = Gas::Count();

    strm << MYSPRINTF_("%s\nЗначения измеренных концентраций.\n",
        MyDateTimeToStr( EncodeDateTime( o.dateTime ) ) ).c_str() ;
    for( unsigned i=0; i<gasCnt; ++i )
        strm << MYSPRINTF_("%s\t%s ± %s\n", Gas::Formula(i),
        MyFloatToStr(o.conc[i]), MyFloatToStr(o.dConc[i]) ).c_str() ;
    strm << "Физико-химические показатели.\n";
    for( unsigned i=0; i<PHY_CHEM::COUNT; ++i )
        strm << MYSPRINTF_("%s\t%s\n", PHY_CHEM::CAPTION[i], PhysChemFieldAsStr(o.physChem, i)  ).c_str() ;

    return strm;
}
//------------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& strm, const Jurnal_Analize& o )
{
    const AnsiString
        strLine = AnsiString().StringOfChar('*', 75)+"\n",
        strLine1 = AnsiString().StringOfChar('-', 75)+"\n";
    const unsigned
        cntLastHourData = std::min( Ceil(o.cntLastHourData), 5 ),
        cntAvgHourData = std::min( Ceil(o.cntAvgHourData), 24 );

    strm << strLine.c_str()   <<
    MYSPRINTF_(
        "Начало контрактных суток: %s\n"
        "Количество измерений за текущий час: %d\n",
        MyDateTimeToStr( EncodeDateTime( o.startDateTime ) ).c_str() ,
        cntLastHourData ).c_str() ;
    for( unsigned i=0; i<cntLastHourData; ++i )
        strm << strLine1.c_str()  << "Измерение " << i+1 << "\n" << strLine1.c_str()  << o.LastHourData[i];

    strm << strLine.c_str()   <<
    MYSPRINTF_( "Количество измерений в текущих сутках: %d\n", cntAvgHourData ).c_str() ;
    for( unsigned i=0; i<cntAvgHourData; ++i )
        strm << strLine1.c_str()  << "Измерение " << i+1 << "\n" << strLine1.c_str()  << o.AvgHourData[i];

    return strm;
}
//------------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& strm, const Jurnal_AnalizeArchive& o )
{
    const AnsiString strLine = AnsiString().StringOfChar('*', 75)+"\n";
    const unsigned
        cntAvgDayData = std::min( Ceil(o.cntAvgDayData), 35 );
    strm << MYSPRINTF_( "Среднесуточные измерения за %d суток.\n", cntAvgDayData ).c_str() ;
    for( unsigned i=0; i<cntAvgDayData; ++i )
        strm << strLine.c_str()  << "Измерение " << i+1 << "\n" << o.AvgDayData[i];
    return strm;
}
//------------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& strm, const Jurnal_GraduateArchive& o )
{
    const AnsiString strLine = AnsiString().StringOfChar('*', 75)+"\n";
    const unsigned gasCnt = Gas::Count();

    for( unsigned i=0; i<gasCnt; ++i )
        const_cast<char*>(o.name[i])[15]='\0';

    strm << MyDateTimeToStr( EncodeDateTime( o.DateTime ) ).c_str()  << "\n" <<
    "Баллон №" << o.pgs_id << "\n" << strLine.c_str()  <<
    "Состав газа\n";
    for( unsigned i=0; i<gasCnt; ++i )
        strm << MYSPRINTF_( "%s\t%s", o.name[i], MyFloatToStr(o.conc[i]) ).c_str()  << "% моль.доли\n";

    strm << strLine.c_str()  << "Средние значения градуировочных коэфициентов\n";
    for( unsigned i=0; i<gasCnt; ++i )
        strm << MYSPRINTF_( "%s\t%s\n", o.name[i], MyFloatToStr(o.avgKg[i]) ).c_str() ;

    strm << strLine.c_str()  << "Относительный размах и предел допускаемого размаха градуировочных коэфициентов\n";
    for( unsigned i=0; i<gasCnt; ++i )
        strm << MYSPRINTF_( "%s\t%s\t%s\n", o.name[i], MyFloatToStr(o.Rk[i]),
            MyFloatToStr(o.Rkmax[i]) ).c_str() ;
    return strm;
}
//------------------------------------------------------------------------------
AnsiString GraduateResult2Str(unsigned code)
{
    const AnsiString sResults[3] = { "приемлем",  "неприемлем",
        "неприемлим (опорная таблица не соответствует градуировочной смеси)" };
    return code<3 ? sResults[code] : MYSPRINTF_("код окончания градуировки %d", code);
}
//------------------------------------------------------------------------------
TIniFile* IniGas()
{
	static TIniFile* ret = NULL;
    if(ret==NULL)
    {
        const AnsiString fn = GetAppDataDir()+"gas.ini";
        MyExtractFileFromResource( fn, "HROMAT_GAS_FILE", false );
    	ret = new TIniFile( fn );
    }
    return ret;
}
#define ERROR_GAS_INI_(ss) MY_THROW_(AnsiString("Ошибка gas.ini: ")+ss);
//------------------------------------------------------------------------------
namespace HromatFileSign
{
    enum { CODES_COUNT = 14};

    const unsigned codes[CODES_COUNT]  =
    {
        PERFORMED,
        USER_BREAKED,
        ERROR_BREAKED,
        PERFORMING,
        CALCULETED_MODE,
        POWER_OFF,

        TEMP_COLS_OUT_RANGE,
        CONS_D1_OUT_RANGE,
        CONS_D2_OUT_RANGE,
        CONS_OUT_RANGE,

        P_SAMPLE_OUT_RANGE,
        P_AUXILIARY_GAS_OUT_RANGE,
        P_CARRIER_GAS_OUT_RANGE,

        HEATING
    };

    const char *s[CODES_COUNT] =
    {
        "выполнена",
        "прервана",
        "прервана с ошибкой",
        "сбор данных",
        "обсчёт пиков",
        "выключение питания",

        "Температура ТК не норма",
        "Расход Г-Н ДТП2 не норма",
        "Расход Г-Н ДТП1 не норма",
        "Расход Г-Н ДТП1, ДТП2 не норма",

        "Давление пробы не норма",
        "Давление вспом. газа не норма",
        "Давление Г-Н не норма",

        "Прогрев"
    };

    unsigned GetIdx(unsigned code)
    {
    	for(unsigned i=0; i<CODES_COUNT; ++i)
        	if( code==codes[i] )
            	return i;
        return -1;

    }

    bool IsInvalide(unsigned code)
    {
        return GetIdx(code)==-1;
    }

    AnsiString FormatCode(unsigned code)
    {
        const unsigned idx = GetIdx(code);
        return idx==-1 ? "..." : s[idx];
    }
};
//------------------------------------------------------------------------------

namespace Gas
{
    typedef std::vector<Item> Items;

    bool CompareItemsPriority(const Item& lhs, const Item& rhs)
    {
        return lhs.reportnum < rhs.reportnum; 
    }

    Item::Item(
    	double h0_,double h1_,					//идеальная молярная теплота сгорания компонента (высшая/низшая)
   		double M_,						  		//молярная масса компонента
   		double Upgs0_,double Upgs1_,	  		//коэффициенты расширенной абсолютной неопределённости аттестации
   		double Ux0_,double Ux1_,		  		//коэффициенты расширенной абсолютной неопределённости результатов измерений
        double sqrtb_,							//коэффициент b
        double Uh0_,
        double min_, double max_,
        const AnsiString& name_,
        const AnsiString& caption_,
        int reportnum_,
        int sensorId_
    ) :
    	h1(h1_),
    	h0(h0_),
   		M(M_),
   		Upgs0(Upgs0_),
    	Upgs1(Upgs1_),
   		Ux0(Ux0_),
    	Ux1(Ux1_),
    	sqrtb(sqrtb_),
        Uh0( Uh0_ ),
        min( min_ ), max( max_ ),
        name( name_ ),
        caption(caption_),
        reportnum(reportnum_),
        sensorId(sensorId_)
	{}

    Item::Item( const Item& obj ) :
    	h1(obj.h1),
    	h0(obj.h0),
   		M(obj.M),
   		Upgs0(obj.Upgs0),
    	Upgs1(obj.Upgs1),
   		Ux0(obj.Ux0),
    	Ux1(obj.Ux1),
    	sqrtb(obj.sqrtb),
        Uh0( obj.Uh0 ),
        min( obj.min ), max( obj.max ),
        name( obj.name ),
        caption(obj.caption),
        reportnum(obj.reportnum),
        sensorId( obj.sensorId)
	{}

    Item Item::operator=( const Item& obj )
	{

    	this->h1 = obj.h1;
    	this->h0 = obj.h0;
   		this->M = obj.M;
   		this->Upgs0 = obj.Upgs0;
    	this->Upgs1 = obj.Upgs1;
   		this->Ux0 = obj.Ux0;
    	this->Ux1 = obj.Ux1;
    	this->sqrtb = obj.sqrtb;
        this->Uh0 = obj.Uh0;
        this->min = obj.min;
        this->max = obj.max;
        this->caption = obj.caption;
        this->reportnum = obj.reportnum;
        this->name =  obj.name;
        this->sensorId =  obj.sensorId;
        return *this;
    }

    int GetIdx(const AnsiString& formula)
    {
    	const unsigned cnt = Count();
    	for(unsigned i=0; i<cnt; ++i)
        	if( formula==Formula(i) )
            	return i;
        return -1;
    }

    class ListT : public boost::noncopyable
    {
    public:
    	unsigned Count() const{ return cnt_; }
        unsigned GrauatorsCount() const { return cntGrad_; }

        const Item* GetItems() const{ return &items_.at(0); }
        const Item* GetSortedItems() const{ return &sortedItems_.at(0); }

        static ListT& Instance()
        {
            static ListT* pInst = NULL;
            if(pInst == NULL)
                pInst = new ListT;
            return *pInst;
        }

    private:
    	const unsigned cnt_, cntGrad_;
        int idxCH4_;
        Items items_;
        Items sortedItems_;
        explicit ListT();
    };

	ListT::ListT() :
        cnt_( 		IniGas()->ReadInteger( "COMMON", "max_comp", 	0 ) ),
        cntGrad_( 	IniGas()->ReadInteger( "COMMON", "calc_comp", 	0 ) ),
        idxCH4_(-1)
    {
        if( cnt_==0 || cnt_>25 ) ERROR_GAS_INI_(
            "Параметр [COMMON] max_comp не задан или задан неверно!");
        if( cntGrad_==0 || cntGrad_>cnt_ ) ERROR_GAS_INI_(
            "Параметр [COMMON] calc_comp не задан или задан неверно!");
        const char decSep = DecimalSeparator;
        DecimalSeparator = '.';
        TIniFile *ini = IniGas();


    	for(  unsigned i=0; i<cnt_; ++i )
        {
            const AnsiString sektN = IntToStr(i);
            const Item gasVal
            (
                ini->ReadFloat( sektN, "h0", 0 ),
                ini->ReadFloat( sektN, "h1", 0 ),
                ini->ReadFloat( sektN, "M", 0 ),
                ini->ReadFloat( sektN, "Upgs0", 0 ),
                ini->ReadFloat( sektN, "Upgs1", 0 ),
                ini->ReadFloat( sektN, "Ux0", 0 ),
                ini->ReadFloat( sektN, "Ux1", 0 ),
                ini->ReadFloat( sektN, "sqrtb", 0 ),
                ini->ReadFloat( sektN, "Uh0", 0 ),
                ini->ReadFloat( sektN, "min", 0 ),
                ini->ReadFloat( sektN, "max", 0 ),
                ini->ReadString( sektN, "name", " " ),
                ini->ReadString( sektN, "caption", "" ),
                ini->ReadInteger( sektN, "reportnum", i ),
                ini->ReadInteger( sektN, "sensorId", -1 )

            );
            items_.push_back( gasVal );
            if( gasVal.name==AnsiString() )
                idxCH4_=i;
        }
        DecimalSeparator = decSep;

        //if( idxCH4_==-1 ) ERROR_GAS_INI_("Не найдена секция метана!");

        sortedItems_ = items_;
        std::sort( sortedItems_.begin(), sortedItems_.end(), CompareItemsPriority );
    }

    // количество компонентов
    unsigned Count()
    {
        return ListT::Instance().Count();
    }

    unsigned GrauatorsCount()
    {
        return ListT::Instance().GrauatorsCount();
    }

    // формулы компонентов
    AnsiString Formula(unsigned n)
    {
        assert( n<Count() );
        return GetItems()[n].name;
    }

    const Item* GetItems()
    {
        return ListT::Instance().GetItems();
    }

    const Item* GetSortedItems()
    {
        return ListT::Instance().GetSortedItems();
    }

}; // namespace Gas
//------------------------------------------------------------------------------
namespace HromatVar
{
//------------------------------------------------------------------------------
Groups CreateGroups(const Items& vars)
{
    const char fileName[] = "varsgroups.ini";
    Groups ret;
    try
    {
        const AnsiString fn = GetAppDataDir()+fileName;
        MyExtractFileFromResource( fn, "HROMAT_GROUPS_FILE", false );
        boost::shared_ptr<TIniFile> ini( new TIniFile( fn ) );
        boost::shared_ptr<TStringList> skts( new TStringList );
        ini->ReadSections( skts.get() );
        
        for( int i=0; i<skts->Count; ++i )
        {
            const AnsiString sekt = skts->Strings[i];
            try
            {
                Group item;
                item.name = sekt;
                item.vars = GetNumbersList1( ini->ReadString( sekt, "vars", "" ).c_str() );
                for( unsigned nGrpVar=0; nGrpVar<item.vars.size(); ++nGrpVar )
                    if( item.vars[nGrpVar]>vars.size() )
                        MY_THROW_(MYSPRINTF_("недопустимый номер праметра %d",
                        item.vars[nGrpVar] ));
                //my::CreateBitset( ini->ReadString( sekt, "vars", "" ).c_str(), vec );
                /*
                if( vec.size()>vars.size() )
                    MY_THROW_(MYSPRINTF_(
                        "недопустимый номер праметра %d", (int)vec.size()-1 ));

                for( unsigned j=0; j<vec.size(); ++j )if( vec[j] )
                {
                    const_cast<Item*>(&vars[j])->group = i+1;
                    item.vars.push_back(j);
                }
                */
                ret.push_back(item);
            }
            catch(...){ MY_RETHROW_NESTED_("Ошибка секции "+sekt); }
        }
        Group& item0 = ret.front();
        for( unsigned i=0; i<vars.size(); ++i )if( vars[i].group==0 )
            item0.vars.push_back(i);       
    }
    catch(...){  MY_RETHROW_NESTED_( AnsiString("Ошибка файла ")+fileName); }
    return ret;
}
//------------------------------------------------------------------------------

bool IsEmptyString( const AnsiString& s ) { return s.IsEmpty(); }
Items CreateVarsList()
{
    const char fileName[] = "vars.ini";
    Items ret;
    try
    {
        const AnsiString fn = GetAppDataDir()+fileName;
        MyExtractFileFromResource( fn, "HROMAT_VARS_FILE", false );
        boost::shared_ptr<TIniFile> ini( new TIniFile( fn ) );
        boost::shared_ptr<TStringList> skts( new TStringList );
        ini->ReadSections( skts.get() );
        for( int i=0; i<skts->Count; ++i )
        {
            const AnsiString sekt = skts->Strings[i];
            try
            {
                const unsigned idx = i;
                Item item;
                item.name = ini->ReadString( sekt, "Name", "..." );
                item.units = ini->ReadString( sekt, "units", "" );
                item.min = ini->ReadString( sekt, "min", "" );
                item.max = ini->ReadString( sekt, "max", "" );
                item.type = ini->ReadString( sekt, "type", "" );
                item.group = -1;
                item.readOnly = ini->ReadBool( sekt, "readOnly", false );
                item.visible = ini->ReadBool( sekt, "visible", true );
                item.values = GetMyStringList( ini->ReadString( sekt, "value",
                    "" ), ";");
                ret.push_back(item);
            }
            catch(...){ MY_RETHROW_NESTED_("Ошибка секции "+sekt); }
        }
    }
    catch(...){ MY_RETHROW_NESTED_( AnsiString("Ошибка файла ")+fileName); }
    return ret;
}

//------------------------------------------------------------------------------
class Helper : public boost::noncopyable
{
public:
	static Helper& Instance()
    {
    	static Helper* pRet = NULL;
        if(pRet==NULL) pRet = new Helper;
        return *pRet;
    }

    const Items& GetList() const { return varsList_; }
    const Groups& GetGroups() const { return groups_; }

    explicit Helper() :
        varsList_( CreateVarsList() ), groups_( CreateGroups(varsList_) )
    {}

private:

    const Items varsList_;
    const Groups groups_;
};
struct HelperTag {};
typedef MyNoDestroySingletonHolder<Helper, HelperTag> Hlpr;
//------------------------------------------------------------------------------
const Items& GetList()
{
    return Hlpr::Instance().GetList();
}
//------------------------------------------------------------------------------
const Groups& GetGroups()
{
    return Hlpr::Instance().GetGroups();
}
//------------------------------------------------------------------------------
unsigned GetVarIndexByName(const AnsiString& name)
{
    const Items& lst = GetList();
    for( unsigned i=0; i<lst.size(); ++i )
        if(lst[i].name.LowerCase().Pos( name.LowerCase() )!=0)
            return i;
    MY_THROW_( MYSPRINTF_("vars.ini: не найдена секция \"%s\"", name));
    //return -1;
}

unsigned GetDiskreteTimeVarIndex()
{
    static unsigned ret = GetVarIndexByName("период дискретизации");
    return ret; 
}
//------------------------------------------------------------------------------
unsigned GetRoundingPerGOSTIndex()
{
    static unsigned ret = GetVarIndexByName("округление по гост");
    return ret;
}
//------------------------------------------------------------------------------
unsigned GetKoefDiskreteTimeIndex()
{
    static unsigned ret = GetVarIndexByName("коэффициент понижения частоты дискретизации");
    return ret;
}
//------------------------------------------------------------------------------
//double FlowRateCfg(bool dtctr){}

double ColumnTemperatureCfg()
{
    static unsigned ret = GetVarIndexByName("Т колонок");
}

}; // namespace HromatVar
//------------------------------------------------------------------------------



namespace HromatChanal
{   
    AnsiString ToStr(unsigned n, const double* dt, bool roundGOST)
    {
        assert(n<COUNT);
        double v = dt[n];

        //if(n==P_VSPOM_GAS)  v = 800*v/102.6;
            
        if( roundGOST )
        {
            if(n==T_KOLONOK || n==T_OBOLO4 || n==D0_T_TERMOSTAT || n==D1_T_TERMOSTAT )
                return FormatFloat("#0.00", v);

            if(n==P_GAS_NOSIT || n==P_VSPOM_GAS || n==P_DOZ_V )
                return FormatFloat("#0", v);

            if(n==D0_RASXOD1 || n==D1_RASXOD1 )
                return FormatFloat("#0.0", v);
        }
        return v;
    }
}

//------------------------------------------------------------------------------
AnsiString FormatModbusFileInfo(const ModbusFileInfo& itm)
{
    AnsiString sDate = "";
    try{ sDate = FormatDateTime("dd.mm.yy h.n.s", itm.date_ ); } catch(...) {}

    return AnsiString().sprintf(
    	"Файл %d детектор %d [%d]%s размер %d  дата создания %s [%d]%s [%d]%s",
        itm.detector_,
        itm.index_,
        itm.fileSpaceNum_,
        HromatFileSpace::ToStr(itm.fileSpaceNum_),
        itm.size_,
        sDate,
        itm.sign_, HromatFileSign::FormatCode(itm.sign_),
        itm.type_, HromatFileType::ToStr(itm.type_)
    );
}

//------------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& strm, const ModbusFileInfo& fI)
{
    strm << FormatModbusFileInfo(fI).c_str();
    return strm;
}
//------------------------------------------------------------------------------
AnsiString PicDataToStr(const HromatPicData& pik)
{
    return AnsiString().sprintf(
    "<%s>конц.=%1.6f:t=%1.6f:t1=%1.6f:t2=%1.6f:A=%1.6f:H=%1.6f",
    pik.name_, pik.t_, pik.t1_, pik.t2_, pik.area_, pik.amplitude_);
}
//------------------------------------------------------------------------------
bool FileDateTimeLess::operator()(const HtgItem* lhs, const HtgItem* rhs) const
    {
        try
        {
            return std::make_pair(lhs->dateTime.Val, lhs->file) <
                std::make_pair(rhs->dateTime.Val, rhs->file);
        }
        catch(...)
        {
            return 1;
        }
    }

//------------------------------------------------------------------------------
bool TryExtractHtgItemFromFile(const AnsiString& fn, HtgItem& ret, bool &detector)
{
    const MyStringList strs = GetMyStringList(fn, "_");
    if( strs.size()!=15 ) return 0;
    MyInt v;
    #define GET_DATETIME_ELEM_(nn,nname)\
    v = MyStrToInt( strs[nn] ); if( !v.second ) return 0; const Word f##nname##=v.first;
    GET_DATETIME_ELEM_(0, Hour);
    GET_DATETIME_ELEM_(1, Minute);
    GET_DATETIME_ELEM_(2, Second);
    GET_DATETIME_ELEM_(3, MilliSecond);
    GET_DATETIME_ELEM_(4, Day);
    GET_DATETIME_ELEM_(5, Month);
    GET_DATETIME_ELEM_(6, Year);
    #undef GET_DATETIME_ELEM_
    if( !TryEncodeDateTime(fYear, fMonth, fDay, fHour, fMinute, fSecond,
        fMilliSecond, ret.dateTime) )
        return 0;
    #define GET_ELEM_(nn, mmin, mmax, nname)\
    v = MyStrToInt( strs[nn] );\
    if( !v.second || v.first>mmax || v.first<mmin) return 0;\
    ret.##nname##=v.first;
    GET_ELEM_(10, 0, 0xFFFFFFFF, file);
    GET_ELEM_(12, 0, HromatFileType::COUNT-1, type);
    GET_ELEM_(14, 0, 0xFFFFFFFF, rgm);
    #undef GET_ELEM_

    v = MyStrToInt( strs[8] );
    if( !v.second || v.first>2 || v.first<1) return 0;
    detector = v.first-1;
    return true;
}
//------------------------------------------------------------------------------
AnsiString FormatHtgItem(const HtgItem& itm)
{
    AnsiString sDate = "";
    try{ sDate = FormatDateTime("dd.mm.yyyy, h:n", itm.dateTime); } catch(...) {}
    const AnsiString sType = itm.type ? "градуировочная" : "измерительная";
    return MYSPRINTF_( "%d от %s, %s.", itm.file, sDate, sType );
}
//------------------------------------------------------------------------------
bool operator==(const HtgItem& lhs, const HtgItem& rhs)
{
    return lhs.file==rhs.file;
}
bool operator<(const HtgItem& lhs, const HtgItem& rhs) 
{
    return lhs.dateTime<rhs.dateTime;
}
//------------------------------------------------------------------------------
bool IsBadHtgItemFileIndex( const HtgItem &o )
{
    return o.file == 0xFFFF;
}

//------------------------------------------------------------------------------






