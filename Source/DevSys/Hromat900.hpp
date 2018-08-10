//------------------------------------------------------------------------------
#ifndef HROMAT_900_MAIN_HEADER_INCLUDED______
#define HROMAT_900_MAIN_HEADER_INCLUDED______
//------------------------------------------------------------------------------
#define VCL_IOSTREAM
//------------------------------------------------------------------------------
#include <iostream>
#include <vector>
#include <map>
//------------------------------------------------------------------------------
#include <system.hpp>
//------------------------------------------------------------------------------
//Промежуток времени между двумя точками хроматоргаммы, мсек
unsigned GetDiskretTimeMSec();


enum
{

    //HROMAT_HTG_DISKRET_TIME_MSEC = 400,
    
    // максимальное количество компонентов
    MAX_GAZCOMP = 25,
    HROMAT_DTCTR_0 = false,
    HROMAT_DTCTR_1 = true,
    HROMAT_MAX_ARCHIVE_SIZE=10000,
    HROMAT_PIC_NAME_LEN = 32,
	HROMAT_IDENTIFIED_PIC_STRUCT_SIZE = 72,
    HROMAT_BASE_PIC_STRUCT_SIZE = 4+HROMAT_PIC_NAME_LEN+4*4,
    HROMAT_PHIS_CHEM_INFO_SIZE = 8*3*sizeof(double),

    // Коды комманд
    HROMAT_CICLE_TYPE_WRT	 = 0x20,
    HROMAT_INTERNAL_DATE_WRT = 0x15,
    HROMAT_INTERNAL_TIME_WRT = 0x16,

    // номера регистров Modbus
    H9005_DOT_EXE_VERSION_REG                   = 55,
    H9005_DOT_EXE_CRC16_REG                     = 56,
    UCINT_DOT_EXE_VERSION_REG                   = 57,
    UCINT_DOT_EXE_CRC16_REG                     = 58,
    CALCR_DOT_EXE_VERSION_REG                   = 59,
    CALCR_DOT_EXE_CRC16_REG                     = 60,
    TEMPERATURA_TERMOSTATA_KOLONOK_REG 			= 232,
    TEMPERATURA_TERMOSTATA_D0_REG 				= 234,
    TEMPERATURA_TERMOSTATA_D1_REG 				= 236,
    RASXOD_D0_REG 								= 238,
    RASXOD_D1_REG 								= 240,
    TOK_D0_REG 									= 242,
    TOK_D1_REG 									= 244,
    HROMAT_CICLE_TYPE_REG						= 250,
    HROMAT_SAVED_FILES_COUNT_REG				= 274,
    CURRENT_HTG_INDEX_D0_REG  					= 292, // Индекс файла хроматограммы детектора 1
    CURRENT_HTG_INDEX_D1_REG  					= 294, // Индекс файла хроматограммы детектора 2
    CURRENT_HTG_SIZE_D0_REG                     = 296, // Размер текущего файла хроматограммы детектора 1
    CURRENT_HTG_SIGN_D0_REG               		= 298, // Признак обработки текущего файла хроматограммы детектора 1
    CURRENT_HTG_SIZE_D1_REG                     = 300, // Размер текущего файла хроматограммы детектора 2
    CURRENT_HTG_SIGN_D1_REG                     = 302, // Признак обработки текущего файла хроматограммы детектора 2
    HROMAT_GAS_INI_SIZE                     	= 304  // Размер файла gas.ini


};
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
struct ModbusFileInfo
{
    unsigned index_, size_, sign_;
    unsigned char type_;
    TDateTime date_;
    bool detector_;
    unsigned fileSpaceNum_;
};
typedef std::map<unsigned, unsigned> FileIndex2SizeMap;
typedef std::vector<ModbusFileInfo> ModbusFileInfoList;
//------------------------------------------------------------------------------
struct HtgItem
{
    TDateTime dateTime;
    unsigned file;
    unsigned rgm, type, sign, size[2];
    AnsiString fileName[2];
};
AnsiString FormatHtgItem(const HtgItem& itm);
typedef HtgItem* PHtgItem;
typedef const HtgItem * const PHtgItemConst;
typedef std::vector<HtgItem> HtgItemsArray;

struct IsHtgItemFileIndex
{
    explicit IsHtgItemFileIndex(unsigned file_) : file(file_) {}
    bool operator()(const HtgItem& itm) const
    { return file == itm.file; }
    const unsigned file;
};

//------------------------------------------------------------------------------
struct FileDateTimeLess
{
    bool operator()(const HtgItem* lhs, const HtgItem* rhs) const;
};
//------------------------------------------------------------------------------
bool TryExtractHtgItemFromFile(const AnsiString& fn, HtgItem& ret, bool &detector);
//------------------------------------------------------------------------------
bool operator==(const HtgItem& lhs, const HtgItem& rhs);
bool operator<(const HtgItem& lhs, const HtgItem& rhs);
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
unsigned ExtractRGMChanal(unsigned char type);
bool ExtractModbusFileType(unsigned char type);
AnsiString FormatModbusFileInfo(const ModbusFileInfo&);
//------------------------------------------------------------------------------
// данные пика
typedef struct
{
    char name_[HROMAT_PIC_NAME_LEN];
    float
        t_,
        hourConc_,
        amplitude_,
        area_,
        dHourConc_,
        conc_,
        t1_,
        t2_,
        nouse4_,
        dConc_
        ;
} HromatPicData;
typedef std::vector<HromatPicData> HromatPicTable;
//------------------------------------------------------------------------------
typedef struct
{
    float conc_;
    char name_[HROMAT_PIC_NAME_LEN];
    float t_;
    float enable;
    float nouse2_;
    float nouse3_;
} HromatBasePicData;
typedef std::vector<HromatBasePicData> HromatBasePicTable;
//------------------------------------------------------------------------------
//       Физико-химические показатели
typedef struct
{
    //относительная плотность идеального Компонента
    double relTghtns[3];

    //плотность идеального Компонента
    double absTghtns[3];

    //молярная теплота сгорания идеального Компонента (высшая)
    double hiHm[3];

    //молярная теплота сгорания идеального Компонента (низшая)
    double loHm[3];

    //теплота сгорания идеального Компонента (высшая)
    double hiH[3];

    //теплота сгорания идеального Компонента (низшая)
    double loH[3];

    //число Воббе (высшее)
    double hiW[3];

    //число Воббе (низшее)
    double loW[3];

} HromatPhysChem;
//------------------------------------------------------------------------------
double GetPhysChemVal(const HromatPhysChem& physChem, unsigned fld, unsigned n);
void SetPhysChemVal(double val, HromatPhysChem& physChem, unsigned fld, unsigned n);
HromatPhysChem NullPhysChem() {    HromatPhysChem ret = {0}; return ret; }
AnsiString PhysChemFieldAsStr( const HromatPhysChem&, unsigned i);
void PhysChemFieldAsStr( const HromatPhysChem& phch, unsigned i, AnsiString& s1, AnsiString& s2);
//------------------------------------------------------------------------------
union HromatDateTimeAsBuffer
{
    unsigned char dt[4];
    struct
    {
        unsigned char day;
        unsigned char month;
        unsigned short int year;
    } date;
    struct
    {
        unsigned char hour;
        unsigned char minute;
        unsigned char second;
        unsigned char hsec;
    } time;
};
//------------------------------------------------------------------------------
//	формат внутреннего представления даты/времени
typedef struct HromatDateTimeInternalRepresentationTag
{
	unsigned char day_;
    unsigned char month_;
    unsigned short int year_;
    unsigned char hour_;
    unsigned char minute_;
    unsigned char nouse1_;
    unsigned char second_;
} HromatDateTimeInternal;
//------------------------------------------------------------------------------
HromatDateTimeInternal ExtractHromatDateTimeInternal( const unsigned char* p);
TDateTime EncodeDateTime( const HromatDateTimeInternal& hdt);
bool TryEncodeDateTime( const HromatDateTimeInternal& hdt, TDateTime& ret);
TDateTime ExtractHromatDateTime( const unsigned char* p);
HromatDateTimeInternal DecodeDateTime( TDateTime dt );
bool operator==( const HromatDateTimeInternal&, const HromatDateTimeInternal&);
//------------------------------------------------------------------------------
// данные градуировки
struct HromatGraduation
{
	double v[MAX_GAZCOMP];
    HromatDateTimeInternal dateTime;
    explicit HromatGraduation();
    HromatGraduation(const HromatGraduation&);
    void operator=(const HromatGraduation&);
} ;
typedef std::vector<HromatGraduation> HromatGraduations;
//------------------------------------------------------------------------------
struct HromatGraduateItem
{
    double dateTime;
    unsigned fileIndex, result, ballon, reserved1 ;
    HromatGraduation Ki, Rk, Rkmax;
};
typedef std::vector<HromatGraduateItem> HromatGraduateArchive;
AnsiString GraduateResult2Str(unsigned code);
//------------------------------------------------------------------------------
//       Коэффициенты хроматографа
namespace HromatVar
{
    typedef struct ItemTag
    {
        AnsiString name, units, min, max, type;
        std::vector<AnsiString> values;
        bool readOnly, visible;
        unsigned group;
    } Item;
    typedef std::vector<Item> Items;
    const Items& GetList();
    typedef struct GroupTag
    {
        AnsiString name;
        std::vector<unsigned> vars;
    } Group;
    typedef std::vector<Group> Groups;
    const Groups& GetGroups();

    unsigned GetDiskreteTimeVarIndex();
    unsigned GetKoefDiskreteTimeIndex();

    unsigned GetRoundingPerGOSTIndex();
};
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
#define DECLARE_HROMAT_INFO_TO_STR_ AnsiString ToStr(unsigned n)\
{ if(n<COUNT) return CAPTION[n]; return "...";  }
//------------------------------------------------------------------------------
namespace HromatFileSign
{
    enum
    {
	    PERFORMED               = 0,
        USER_BREAKED            = 1,
        ERROR_BREAKED           = 2,
        PERFORMING              = 3,
        CALCULETED_MODE         = 4,
        POWER_OFF               = 5,

        TEMP_COLS_OUT_RANGE     = 0x5B,
        CONS_D1_OUT_RANGE       = 0x59,
        CONS_D2_OUT_RANGE       = 0x58,
        CONS_OUT_RANGE          = 0x5A,

        P_SAMPLE_OUT_RANGE              = 0x57,
        P_AUXILIARY_GAS_OUT_RANGE       = 0x56,
        P_CARRIER_GAS_OUT_RANGE         = 0x55,

        HEATING = 0x5C,

        INVALID_CODE = -1
    };
    
    bool IsInvalide(unsigned);
    AnsiString FormatCode(unsigned);
};
//------------------------------------------------------------------------------

namespace HromatFileSpace
{

    enum
    {
        HTG_NAME,       HTG_DATA,
        PIK_NAME,       PIK_DATA,
        BASE_PIK_NAME,  BASE_PIK_DATA,
        TREND_NAME,     TREND_DATA,
        GRADUATION,
        CONFIG_VALUES,
        GAS_INI,
        COUNT
    };
    const char * const CAPTION[COUNT] =
    {
        "имена х-мм",
        "данные х-мм",
        "имена пиков",
        "данные пиков",
        "имена опорных пиков",
        "данные опорных пиков",
        "имена трендов",
        "данные трендов",
        "журналы",
        "праметры конфигурации",
        "компоненты"
    };
    DECLARE_HROMAT_INFO_TO_STR_
};// namespace HromatFileSpace
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//Индексы показаний каналов( значения измеряемой величины по каждому из 16-и каналов )	регистры 0..31
namespace HromatChanal
{
    enum
    {
	    T_KOLONOK,
        T_OBOLO4,
        P_GAS_NOSIT,
        P_VSPOM_GAS,
        P_DOZ_V,
        KLAPANY,
        D0_RASXOD1,
        D1_RASXOD1,
        D0_UC,
        D0_T_TERMOSTAT,
        D0_TOK,
        D1_UC,
        D1_T_TERMOSTAT,
        D1_TOK,
        COUNT
    };
    const char * const CAPTION[COUNT] =
    {
	"Т колонок, \"С",
    "Т оболочки, \"С",
    "P Компонента-носителя, МПа",
    "P вспом.Компонента, МПа",
    "P пробы, мм.рт.ст.",
    "Дискретные каналы",
    "Расход детектора 1, мл/мин",
    "Расход детектора 2, мл/мин",
    "Uc, B",
    "T, \"C",
    "I, мА",
    "Uc, B",
    "T, \"C",
    "I, мА"
    };
    DECLARE_HROMAT_INFO_TO_STR_
    AnsiString ToStr(unsigned n, const double* dt, bool roundGOST);
};
//------------------------------------------------------------------------------
/**
* 		Наименования каналов( значения измеряемой величины по
*		каждому из 16-и каналов )	регистры 0..31
*/

//------------------------------------------------------------------------------
//		индексы полей структуры данных пика
namespace PIK_FIELD
{
    enum
    {
        NAME,
        T,
        NO_USE_1_,
        SQUARE,
        AMPLITUDE,
        NO_USE_2_,
        CONC,
        T1,
        T2,
        dCONC,
        NO_USE_4_,
        COUNT
    };
    // Наименования полей пика
    const char * const CAPTION[COUNT] =
    { "№", "Наименование", "С", "±С", "t,c", "t1,с", "t2,с", "A, В*с", "H, В" };
    DECLARE_HROMAT_INFO_TO_STR_
};

//------------------------------------------------------------------------------
namespace PHY_CHEM
{
	enum
    {
		OTN_PO_IDEAL_GAS,
    	ABS_PO_IDEAL_GAS,
    	HI_HM,
    	LO_HM,
    	HI_H,
    	LO_H,
    	HI_W,
    	LO_W,
    	COUNT
	};
    const char * const CAPTION[COUNT] =
    {
    	"d",
    	"ro",
        "Hms",
    	"Hml",
        "Hvs",
    	"Hvl",
        "Ws",
    	"Wl"
    };
    const char * const EDIZM[COUNT] =
    { "%",  "кг/м3",  "кДж/моль", "кДж/моль", "МДж/м3", "МДж/м3", "МДж/м3", "МДж/м3" };
};
//------------------------------------------------------------------------------

// Наменование типа файла
namespace HromatFileType
{
    enum
    {
        MEASURE,
        KALIBR,
        COUNT
    };
    const char * const CAPTION[COUNT] = { "Измерение", "Градуировка" };
    DECLARE_HROMAT_INFO_TO_STR_
};
//------------------------------------------------------------------------------
namespace HROMAT_CICLE
{
    enum
    {
        OSTANOV, EDIZM, IZM, GRAD, COUNT
    };
    const char * const CAPTION[COUNT] = { "останов.", "ед.изм.",  "цикл.изм.", "град." };
    DECLARE_HROMAT_INFO_TO_STR_
};
//------------------------------------------------------------------------------
#undef DECLARE_HROMAT_INFO_TO_STR_
//------------------------------------------------------------------------------


 template<class T> void NormalizePicName(const T& pic)
{
	char *pPicName = const_cast<char*>(pic.name_);
    pPicName[HROMAT_PIC_NAME_LEN-1] = '\0';
}
template<class P> void NormalizePicNameInTable1(P p, P e)
{ for( ; p!=e; ++p ) NormalizePicName( *p );}

template<class V> void NormalizePicNameInTable2(V& v)
{ NormalizePicNameInTable1( v.begin(), v.end() ); }





//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------
