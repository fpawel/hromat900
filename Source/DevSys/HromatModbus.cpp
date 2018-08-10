//#include <map>
//---------------------------------------------------------------------------
#pragma hdrstop
//---------------------------------------------------------------------------
#include "HromatModbus.h"
#include "bcd_.h"
#include "my_array_size.h"
#include "Loki\static_check.h"
#include "MyExcpt.hpp"
#include "MyWinExcpt.hpp"
// ��� ������ ������� PerformModbusTransfer
#include "PerformTransfer.h"

#include "MySynchronize.h"
#include "AnsiStringUtils_.h"
#include "MyModbusException.h"
#include "MyModbusDefine.hpp"
#include "AppEvts.h"

#include <DateUtils.hpp>
#include "MyFTP.h"
#include "HromatCrypt.h"
#include "guicon.h"


// ��������������� ���������� �������-���������� ��� ������
// #include "HromatExceptLst.hpp"
//------------------------------------------------------------------------------
#pragma package(smart_init)
//------------------------------------------------------------------------------
enum{ READ_MODBUS_FILE_CMD_CODE = 0x41 };
enum{ WRITE_MODBUS_FILE_CMD_CODE = 0x51 };

//  ���������� "MODBUS"
DECLARATE_AND_DEFINE_MY_EXCEPTION_CLASS_(HromatModbus, ExternalException );
//  ���������� �������� 16
DECLARATE_AND_DEFINE_MY_EXCEPTION_CLASS_(HromatModbusCmd10h, HromatModbusException );
//  ���������� �������� 3
DECLARATE_AND_DEFINE_MY_EXCEPTION_CLASS_(HromatModbusCmd3h, HromatModbusException );
//  ���������� �������� 0x41
DECLARATE_AND_DEFINE_MY_EXCEPTION_CLASS_(HromatModbusCmd41h, HromatModbusException );

//  ���������� ��� ���������� � ������������ ������������ ������ ���������� � ����������
DECLARATE_AND_DEFINE_MY_EXCEPTION_CLASS_(HromatModbusGotAndRequestedLengthMismatch,
	HromatModbusException );

//  ���������� ��� ���������� �� ������ ������ �����
DECLARATE_AND_DEFINE_MY_EXCEPTION_CLASS_(HromatModbusFileWrite, HromatModbusException );

//  ���������� ��� ���������� �� ������ �������������� BCD
DECLARATE_AND_DEFINE_MY_EXCEPTION_CLASS_(HromatModbusBCDToUnsignedIntConversion,
	HromatModbusException );

DECLARATE_AND_DEFINE_MY_EXCEPTION_CLASS_(HromatModbusBCDToFloatConversion,
	HromatModbusException );

#undef MY_THROW_
#undef MY_THROW_CLASS_
#define THROW_MODBUS_EXCEPTION_(THECLASS, MSG)\
	throw MyExcptionFactory::Create<MyHromatModbus##THECLASS##Exception>(__FILE_LINE__, MSG)

//------------------------------------------------------------------------------
AnsiString FormatFileInfoHead(bool detector, unsigned idx, unsigned fileSpaceNum )
{
    return MYSPRINTF_("�%d �%d %s", idx, detector, HromatFileSpace::ToStr(fileSpaceNum) );
}
//------------------------------------------------------------------------------
typedef struct KostikFileNameTag
{
    KostikFileNameTag()
    {}
    KostikFileNameTag( unsigned nFil, unsigned nSpac, bool de ) :
        nFile( nFil ), nSpace(nSpac), det( de )
    {}
    unsigned nFile, nSpace;
    bool det;
} KostikFileName;
//------------------------------------------------------------------------------
typedef struct FTPPathTag
{
    FTPPathTag()
    {
    }
    FTPPathTag( const AnsiString& di, const AnsiString& nam) : dir(di), name(nam)
    {
    }
    AnsiString dir, name;
} FTPPath;
//------------------------------------------------------------------------------
FTPPath KostikFileNameToFTPPath( const KostikFileName& fn )
{
    FTPPath ret("","");
    switch( fn.nSpace )
    {
        case HromatFileSpace::CONFIG_VALUES:
            ret.dir = "/B:/SERVICE";
            ret.name = "hromat.cfg";
            break;
        case HromatFileSpace::BASE_PIK_DATA:
            ret.dir = MYSPRINTF_("/B:/LIB/%d", fn.det);
            ret.name = "00000.h90";
            break;
        case HromatFileSpace::PIK_DATA:
            ret.dir = MYSPRINTF_("/B:/PEAK/%d/%d", fn.det, fn.nFile/100 );
            ret.name = MYSPRINTF_("%05d", fn.nFile%100 )+".h90";
            break;

        case HromatFileSpace::HTG_DATA:
            ret.dir = MYSPRINTF_("/B:/DATA/%d/%d", fn.det, fn.nFile/100 );
            ret.name = MYSPRINTF_("%05d", fn.nFile%100 )+".h90";
            break;

        case HromatFileSpace::HTG_NAME:
            ret.dir = "/B:/SERVICE";
            ret.name = MYSPRINTF_("filetable%d.h90", fn.det );
            break;

        case HromatFileSpace::PIK_NAME:
            ret.dir = "/B:/SERVICE";
            ret.name = MYSPRINTF_("peaktable%d.h90", fn.det );
            break;

        case HromatFileSpace::GRADUATION: {
                const char *names[6] =
                {
                    "FILE_KG.H90", "FILE_RK.H90", "file_Rkmax.h90",
                    "JANALIZE.H90", "jarhanalize.h90", "JARHGRAD.H90"
                };
                ret.dir = "/B:/";
                ret.name = fn.nFile<6 ? names[fn.nFile] : "";
            }
            break;
        case HromatFileSpace::GAS_INI:
            {
                ret.dir = "/B:/";
                ret.name = "reference.ini";
            }
            break;

        default:
            break;
    }
    return ret;
}
//------------------------------------------------------------------------------
// ����������������� char[MODBUS_FILE_INFO_SIZE] � ModbusFileInfo
void ExtractModbusFilesInfo(ModbusFileInfo *fileInfo, const uInt8* rxd)
{
    fileInfo->index_	= (rxd[0] << 0) + (rxd[1] << 8 );
    fileInfo->size_     = (rxd[2] << 0) + (rxd[3] << 8) + (rxd[4] << 16) + (rxd[5] << 16);
 	fileInfo->date_ = ExtractHromatDateTime(rxd+6);
    //fileInfo->mode_     = rxd[14]; - �� ������������
    fileInfo->sign_   	= rxd[15];
    fileInfo->type_ = rxd[16];

}
//------------------------------------------------------------------------------
AnsiString BCDBuffToStr(const uInt8* dt)
{
	return MYSPRINTF_( "%s, %s, %s, %s", IntToHex(dt[0],2),	IntToHex(dt[1],2),
    	IntToHex(dt[2],2),	IntToHex(dt[3],2) );
}
//------------------------------------------------------------------------------
AnsiString TryFormatBCDFloat( const uInt8* dt )
{
	const double val = BCDToFloat(dt);
    return val==MAX_BCD_FLOAT ? BCDBuffToStr(dt) : FloatBCDToStr( val );
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
namespace Private
{
//------------------------------------------------------------------------------
/**
*		������� ����, ��������� ��������� � uInt8* pOut
*		�������������� ��������������� �������������� ������� �������
*		throw PMyHromatModbusException
*		�����������: count<MODBUS_READ_FILE_BLOCK_SIZE
*/
unsigned ReadModbusFileBlock(
	const unsigned nFile,			        // ������ �����
    const unsigned count,                   // ����������� ����
    const unsigned offset,                // �������� �� ������ �����
    const unsigned fileSpaceNum,             // ����� �������� ������� �� 0
    const bool detector,                  // ����� ���������
    ByteBuffPtr& rxd                // �������� ������
)
{
    assert( fileSpaceNum<HromatFileSpace::COUNT );

    const AnsiString sFile = FormatFileInfoHead(detector, nFile, fileSpaceNum);
    MYWCOUT_("ReadModbusFileBlock - ������ %d ���� (���.=%d) ����� %s\n", count, offset, sFile  ));

	enum{ TXD_LEN = 8 };

    uInt8 txd[TXD_LEN];
    txd[0] = nFile >> 8;
    txd[1] = nFile;
    txd[2] = count >> 8;
    txd[3] = count;

    txd[4] = ( detector << 4 ) + fileSpaceNum;

    txd[5] = offset >> 16;
    txd[6] = offset >> 8;
    txd[7] = offset;

    ByteBuffPtr rxdEnd;

    try
    {
    	PerformModbusTransfer(READ_MODBUS_FILE_CMD_CODE, txd, txd + TXD_LEN, rxd, rxdEnd );
    }
    catch(...)
    {
    	// ������������� ����������
    	const PMyExcpt excpt = RethrowMyException(__FILE_LINE__);
        if( const DeviceModbusException *modbusException = excpt->As<DeviceModbusException>() )
        {
        	using namespace ModbusException;
            const unsigned i = modbusException->idx_;
            if( i==ILLEGAL_DATA_ADDRESS || i==ILLEGAL_DATA_VALUE  )
        		return 0;
        }
        throw excpt;
    }

    const unsigned rxdLen = rxdEnd - rxd;
    if( (unsigned)(count+2)!=rxdLen )
        THROW_MODBUS_EXCEPTION_(Cmd41h, MYSPRINTF_("���������� ������ ���������� �����:"
        "<����� ������� ������>(%d) �� ����� <����������� �����>(%d)+2!", rxdLen, count) );

    if( *rxd++!=txd[2] || *rxd++!=txd[3] )
        THROW_MODBUS_EXCEPTION_(Cmd41h,
        "���������� ������ ���������� �����: �������������� ����� ������� ������!" );

    return count;
}
//------------------------------------------------------------------------------
unsigned ReadModbusFile(
    unsigned nFile,			        // ������ �����
    unsigned count,                   // ����������� ����
    unsigned offset,                // �������� �� ������ �����
    unsigned fileSpaceNum,             // ����� �������� ������� �� 0
    bool detector,                  // ����� ���������
    uInt8* pFile             // �������� ������
)
{
    DWORD totallReaded = 0;
    ByteBuffPtr rxd;
    try
    {
        while(totallReaded<count)
        {
            const unsigned
            requestedLen = std::min( count - totallReaded, GetHromatMaxRxDFrameSize() ),
            actualReaded = ReadModbusFileBlock(
	            nFile,              // ������ �����
                requestedLen,       // ����������� ����
                offset,             // �������� �� ������ �����
                fileSpaceNum,       // ����� �������� ������� �� 0
                detector,           // ����� ���������
                rxd                 // �������� ������
            );
            totallReaded +=  actualReaded;
            offset  +=  actualReaded;

            std::copy(rxd, rxd + actualReaded, pFile);
            pFile += actualReaded;
        }
        return totallReaded;
    }
    catch(...)
    {
        // ������������� ����������
    	const PMyExcpt excpt = RethrowMyException(__FILE_LINE__);
        if( const DeviceModbusException *modbusException = excpt->As<DeviceModbusException>() )
        {
        	using namespace ModbusException;
            const unsigned i = modbusException->idx_;
            if( i==FILE_NOT_FOUND  ) return 0;

        }
        throw excpt;
    }
}
//------------------------------------------------------------------------------
/*
unsigned ReadFTPFile(const AnsiString &dir, const AnsiString &name,
    uInt8* pFile, unsigned count )
{
    FTPHlpr* ftp = GetFTP();
    DWORD totallReaded = 0;
    const AnsiString fullFileName = MYSPRINTF_("%s/%s", dir, name);
    try
    {
        if( ftp->GetCurrentDirectory()!=dir )
            ftp->SetCurrentDirectory(dir);


        WIN32_FIND_DATA findFileData = {0};

        try
        {
            ftp->FindFirstFile( name, &findFileData, 0 );
        }
        catch(...)
        {
    	    // ������������� ����������
    	    const PMyExcpt excpt = RethrowMyException(__FILE_LINE__);
            if( const MyWindowsException *winExcpt = excpt->As<MyWindowsException>() )
            {
                if( winExcpt->Code()!=12003  ) throw;
        	    findFileData.nFileSizeLow = 0;
            }
        }
        if( findFileData.nFileSizeLow==0 ) return 0;
        ftp->ReadFile( name, pFile,  std::min(count, findFileData.nFileSizeLow), &totallReaded );
    }
    catch(...)
    {
        const PMyExcpt excpt = RethrowMyException(__FILE_LINE__);
        const AnsiString fn = excpt->Location().second;
        if( fn=="MyFTP.cpp" ) // ������ ����� ����� � ������������ ���!
        {
            LogMyAppEvt("���� ����������.", fullFileName );
            //throw PMyExcpt( new MyException( __FILE_LINE__, MYSPRINTF_(
            //    "���� %s �� ������!", fullFileName), excpt));
        }
        else
            throw;
    }
    return totallReaded;
}
*/
unsigned ReadFTPFile(const AnsiString &dir, const AnsiString &name,
    uInt8* pFile, unsigned count )
{
    FTPHlpr* ftp = GetFTP();
    DWORD totallReaded = 0;
    if( ftp->GetCurrentDirectory()!=dir )
        ftp->SetCurrentDirectory(dir);

    
    WIN32_FIND_DATA findFileData = {0};

    try
    {
        ftp->FindFirstFile( name, &findFileData, 0 );
    }
    catch(...)
    {
    	// ������������� ����������
    	const PMyExcpt excpt = RethrowMyException(__FILE_LINE__);
        if( const MyWindowsException *winExcpt = excpt->As<MyWindowsException>() )
        {
            if( winExcpt->Code()!=12003  ) throw excpt;
        	findFileData.nFileSizeLow = 0;
        }
    }
    if( findFileData.nFileSizeLow==0 ) return 0;
    ftp->ReadFile( name, pFile,  std::min(count, findFileData.nFileSizeLow), &totallReaded );
    return totallReaded;
}
//------------------------------------------------------------------------------
};// namespace Private
//------------------------------------------------------------------------------

AnsiString FormatHromatCommand(uInt16 cmd, const uInt8* dt)
{
    const double val = BCDToFloat(dt);
    const unsigned asInt = Ceil(val);
    const HromatDateTimeAsBuffer& dateTime = *(reinterpret_cast<const HromatDateTimeAsBuffer*>(dt));



    if(cmd==HROMAT_CICLE_TYPE_WRT)
    {
        return MYSPRINTF_("���  ����� %d:%s", asInt, HROMAT_CICLE::ToStr(asInt) );
    }

    if(cmd==HROMAT_INTERNAL_DATE_WRT)
    {
        return MYSPRINTF_("���������� ���� %d.%d.%d", dateTime.date.day,
            dateTime.date.month, dateTime.date.year );
    }

    if(cmd==HROMAT_INTERNAL_TIME_WRT)
    {
        return MYSPRINTF_("���������� ����� %d:%d:%d", dateTime.time.hour,
            dateTime.time.minute, dateTime.time.second );
    }

    if(cmd==2)
    {
        return MYSPRINTF_("�������� �������� %d", asInt+1 );
    }
    const AnsiString asFloat = val==MAX_BCD_FLOAT ? AnsiString("...") : FloatBCDToStr( val );
    return MYSPRINTF_( "%s %s(%s)", IntToHex(cmd,2), BCDBuffToStr(dt), asFloat );
}

AnsiString FormatHromatCommand(uInt16 cmd, double val)
{
    uInt8 bcd[4];
    FloatToBCD(val, bcd );
    return FormatHromatCommand(cmd, bcd);
}


void WriteModbus4Bytes(  uInt16 cmd, const uInt8* dt )
{
	const AnsiString msg = MYSPRINTF_( "�������� ������� %s\n", FormatHromatCommand(cmd, dt) );
    MyWCout(msg);
    
	enum
    {
    	MODUS_WRITE_REGISTER_COMMAND_CODE = 16,
        MODUS_WRITE_REGISTER_ADDR = 32,
        MODUS_WRITE_REGISTERS_COUNT = 3,
        MODUS_WRITE_DATA_LEN = 6,
        MODUS_WRITE_STR_LEN = 11
    };

    ByteBuffPtr rxd=0, rxdEnd=0;

	uInt8 txd[MODUS_WRITE_STR_LEN];
    txd[0] = MODUS_WRITE_REGISTER_ADDR >> 8;
    txd[1] = MODUS_WRITE_REGISTER_ADDR;
    txd[2] = MODUS_WRITE_REGISTERS_COUNT>>8;
    txd[3] = MODUS_WRITE_REGISTERS_COUNT;
    txd[4] = MODUS_WRITE_DATA_LEN;
    // ������
    txd[5] = cmd >> 8;
    txd[6] = cmd;
    std::copy( dt, dt+4, txd+7 );

    PerformModbusTransfer(MODUS_WRITE_REGISTER_COMMAND_CODE, txd,
    	txd+MODUS_WRITE_STR_LEN, rxd, rxdEnd);

    const unsigned rxdLen = rxdEnd - rxd;
    if( rxdLen!=4 )
        THROW_MODBUS_EXCEPTION_(Cmd10h,
        MYSPRINTF_("���������� ������ ������� 16: <����� ������� ������>(=%d) �� ����� 4!", rxdLen ));
    if( !std::equal(txd, txd+4, rxd) )
        THROW_MODBUS_EXCEPTION_(Cmd10h,
        "���������� ������ ������� 16: �� ������ ������ ������� ������!");
}
//------------------------------------------------------------------------------
void WriteModbusFloat( uInt16 cmd, double val )
{
	uInt8 bcd[4];
    FloatToBCD(val, bcd );
    WriteModbus4Bytes( cmd, bcd);
}
//------------------------------------------------------------------------------
void ReadModbusRegistersData( uInt16 regAddr,
	unsigned regCount,	ByteBuffPtr& rxd, ByteBuffPtr& rxdEnd )
{
	MYWCOUT_( "������ %d ��������� c %d �� %d...\n", regCount, regAddr, regAddr+regCount-1 ));

	enum { MODUS_READ_REGISTER_COMMAND_CODE = 0x03 };

    const unsigned waitStrLen = regCount*2;
    assert( regCount<0xFF );

	uInt8 txd[4];
    txd[0] = regAddr >> 8;
    txd[1] = regAddr;
    txd[2] = regCount>>8;
    txd[3] = regCount;

    PerformModbusTransfer(MODUS_READ_REGISTER_COMMAND_CODE, txd, txd+4, rxd, rxdEnd);

    const unsigned rxdLen = rxdEnd - rxd;

    if( rxdLen!=waitStrLen + 1)
        THROW_MODBUS_EXCEPTION_(Cmd3h, MYSPRINTF_(
            "���������� ������ ������� 3: <����� ������� ������>(=%d) �� ����� "
            "2*<���-�� ���������>(=%d)+1",
            rxdLen, waitStrLen));

    if( waitStrLen != static_cast<unsigned>(*rxd) )
        THROW_MODBUS_EXCEPTION_(Cmd3h, MYSPRINTF_(
            "���������� ������ ������� 3: <����� ������� ������>(=%d) �� ����� "
            "�������� ����� � �������(=%d)",
            waitStrLen, rxd[0] ));
    ++rxd;
}
//------------------------------------------------------------------------------
void ReadModbusFloat( uInt16 regAddr, unsigned count, double *pResult )
{
	const unsigned reg1 = regAddr+count*2-1;
	MYWCOUT_( "������ %d ����� ���%d...���%d\n", count, regAddr, reg1 ));
	ByteBuffPtr rxd, rxdEnd;
    ReadModbusRegistersData( regAddr, count*2, rxd, rxdEnd );
	TransformBCDDataToFloat( rxd, rxdEnd, pResult );

    MYWCOUT_( "���%d...���%d==(", regAddr, reg1 ));
    std::cout << myIO::OutDoubleVec(pResult, pResult+count, ", ") << ")\n";
}
//------------------------------------------------------------------------------
double GetModbusFloat( uInt16 regAddr )
{
	MYWCOUT_( "������ ����� �� ���%d,���%d\n", regAddr, regAddr+1 ));
	ByteBuffPtr rxd, rxdEnd;
    ReadModbusRegistersData( regAddr, 2, rxd, rxdEnd );
    const double res = BCDToFloat(rxd);
    MYWCOUT_( "���%d,���%d=%s\n", regAddr, regAddr+1, FloatBCDToStr(res) ));
	return res;
}
//------------------------------------------------------------------------------
unsigned ReadModbusUnsigned( uInt16 regAddr )
{
	MYWCOUT_( "������ ����� �� ���%d,���%d\n", regAddr, regAddr+1 ));

	ByteBuffPtr rxd, rxdEnd;
    ReadModbusRegistersData( regAddr, 2, rxd, rxdEnd );
    // 10000111 = 0x87
    if( (rxd[0] & 0x78)!=0  )
    	MYWCOUT_( "��������������! ����������� �������� BCD ������ ������������ (%s) � �������� %d!\n",
        BCDBuffToStr(rxd), regAddr ));

    const unsigned res = Ceil( BCDToFloat(rxd) );
    MYWCOUT_( "���%d,���%d=%d\n", regAddr, regAddr+1, res ));

    return res;
}
//------------------------------------------------------------------------------
int ReadModbusSigned(  uInt16 regAddr )
{
	MYWCOUT_( "������ ����� �� ���%d,���%d\n", regAddr, regAddr+1 ));
    
	ByteBuffPtr rxd, rxdEnd;
    ReadModbusRegistersData( regAddr, 2, rxd, rxdEnd );
    // 10000111 = 0x87
    if( (rxd[0] & 0xF8)!=0  )
        MYWCOUT_( "��������������! ����������� �������� BCD ������ (%s) � �������� %d!\n",
        BCDBuffToStr(rxd), regAddr ));
        
    const int res = Ceil( BCDToFloat(rxd) );
    MYWCOUT_( "���%d,���%d=%d\n", regAddr, regAddr+1, res ));
    return res;
}
//------------------------------------------------------------------------------





//------------------------------------------------------------------------------
/**
*		������� ����, ��������� ��������� � uInt8* pOut
*		�������������� ��������������� �������������� ������� �������
*		throw PMyHromatModbusException
*/
unsigned ReadHromatFile(
	unsigned nFile,			        // ������ �����
    unsigned count,                   // ����������� ����
    unsigned offset,                // �������� �� ������ �����
    unsigned fileSpaceNum,             // ����� �������� ������� �� 0
    bool detector,                  // ����� ���������
    uInt8* pFile,             // �������� ������
    LRESULT (__closure *OnGet )(ByteBuffPtr, ByteBuffPtr),
    const AnsiString &fileCaption
)
{
    const bool mustLog = !fileCaption.IsEmpty();
    const AnsiString sFile = fileCaption + " " +FormatFileInfoHead(detector, nFile, fileSpaceNum);
    const KostikFileName fn( nFile, fileSpaceNum, detector);
    const FTPPath ftpPath = KostikFileNameToFTPPath( KostikFileNameTag(fn) );
    const bool correctFTPPath = !ftpPath.dir.IsEmpty() && !ftpPath.name.IsEmpty();

    DWORD totallReaded = 0;

    totallReaded = ( offset==0 && FTPEnable() && correctFTPPath ) ?
        Private::ReadFTPFile( ftpPath.dir, ftpPath.name, pFile, count ) :
        Private::ReadModbusFile( nFile, count, offset, fileSpaceNum, detector, pFile ) ;

    if( totallReaded == count )
    {
        if(OnGet!=0) my::Synchronize2( OnGet, pFile, pFile + count );
        if(mustLog)
            LogMyAppEvt("������� ����.", sFile, MYSPRINTF_("%d ����",  count) );
    }
    else
    {
        if( totallReaded == 0 )
            LogMyAppEvt("��������! ���� ����������.", sFile );
        else
            if( totallReaded < count )
                LogMyAppEvt("��������! ����� ����� ����� �����������!", sFile,
                    MYSPRINTF_("%d ���� �� %d",  totallReaded, count ) );
    }   
    return totallReaded;
}
//------------------------------------------------------------------------------
bool GetHromatFile(
	unsigned nFile,			        // ������ �����
    unsigned count,                   // ����������� ����
    unsigned fileSpaceNum,             // ����� �������� ������� �� 0
    bool detector,                  // ����� ���������
    uInt8* pFile,             // �������� ������
    const AnsiString &fileCaption
    )
{
    return ReadHromatFile(
	nFile,			        // ������ �����
    count,                   // ����������� ����
    0,                // �������� �� ������ �����
    fileSpaceNum,             // ����� �������� ������� �� 0
    detector,                  // ����� ���������
    pFile,             // �������� ������
    NULL,
    fileCaption)==count;
}

//------------------------------------------------------------------------------
/**
*		������� ���������� � ������, ��������� ��������� � ModbusFileInfo *fileInfo
*		�������������� ��������������� �������������� ������� �������
*		throw PMyHromatModbusException
*
*       �����������: ������ ������� fileInfo �� ����� count*(MODBUS_FILE_INFO_SIZE+1)
*/
unsigned ReadHromatFilesInfo(
	unsigned firstFileIndex,          // ������ ������� �����
    unsigned count,                   // ����������� ������
    unsigned fileSpaceNum,             // ����� �������� ������� �� 0
    bool detector,                  // ����� ���������
    ModbusFileInfo *fileInfo		// �������� �����
)
{   

    const unsigned rxdSz = count*MODBUS_FILE_INFO_SIZE;

    // �������� ����� fileInfo
    for( unsigned i=0; i<count; ++i )
    {
        fileInfo[i].index_= 0;
        fileInfo[i].size_ = 0;
        fileInfo[i].sign_ = 0;
        fileInfo[i].type_ = 0;
        fileInfo[i].date_ = 0;
        fileInfo[i].detector_ = 0;
        fileInfo[i].fileSpaceNum_ = 0;
    }
        
    // ������ ��������� ����� ��� ���������� �����
    VInt8 rxd(rxdSz);

    const unsigned actualReaded = ReadHromatFile(
	    firstFileIndex,			// ������ ����� - �� ����� ��������
        rxdSz,                  // ����������� ����
                                // �������� �� ������ �����
        firstFileIndex*MODBUS_FILE_INFO_SIZE,
        fileSpaceNum,     		// ����� �������� ������� �� 0
        detector,          		// ����� ���������
        rxd.begin()             // �������� ������

    );
    rxd.resize( actualReaded );

    for( ByteBuffPtr i = rxd.begin(); i<rxd.end(); i+=MODBUS_FILE_INFO_SIZE, ++fileInfo )
    {
        ExtractModbusFilesInfo(fileInfo, i);
        fileInfo->detector_ = detector;
        fileInfo->fileSpaceNum_ = fileSpaceNum;
        //MyWCout( ModbusFileInfotoStr(*fileInfo) + AnsiString("\n") );
    }
    const unsigned ret = actualReaded / MODBUS_FILE_INFO_SIZE;
    return ret;
}
//------------------------------------------------------------------------------
bool ReadHromatFileInfo
(
	const unsigned fileIndex,          		// ������ ������� �����
    const unsigned fileSpaceNum,            // ����� �������� ������� �� 0
    const bool detector,                  	// ����� ���������
    ModbusFileInfo *fileInfo				// �������� �����
)
{
    const bool ret = ReadHromatFilesInfo
    (
		fileIndex,          // ������ ������� �����
    	1,                   // ����������� ������
    	fileSpaceNum,             // ����� �������� ������� �� 0
    	detector,                  // ����� ���������
    	fileInfo		// �������� �����
    ) == 1;
    return ret;
}
//------------------------------------------------------------------------------
void WriteModbusFileBlock(
	unsigned nFile,			// ������ �����
    unsigned count,           // �����������
    unsigned offset,                  // �������� �� ������ �����
    unsigned fileSpaceNum,             // ����� �������� ������� �� 0
    bool detector,                   // ����� ���������
    ByteBuffPtr pFile
)
{
    assert( fileSpaceNum<HromatFileSpace::COUNT );

    const AnsiString sFile = FormatFileInfoHead(detector, nFile, fileSpaceNum );

    MYWCOUT_("������ ����� ����� %s:��������=%d:������=%d\n",
        sFile, offset, count ) );

	enum{ TXD_LEN = 8};

    VInt8 txd(TXD_LEN + count);
    txd[0] = nFile >> 8;
    txd[1] = nFile;
    txd[2] = count >> 8;
    txd[3] = count;

    txd[4] = ( detector << 4 ) + fileSpaceNum;

    txd[5] = offset >> 16;
    txd[6] = offset >> 8;
    txd[7] = offset;
    std::copy( pFile, pFile+count, txd.begin()+TXD_LEN );
    ByteBuffPtr rxd = NULL, rxdEnd = NULL;
    PerformModbusTransfer(WRITE_MODBUS_FILE_CMD_CODE, txd.begin(), txd.end(), rxd, rxdEnd );
}
//------------------------------------------------------------------------------
/**
*		�������� ����
*		�������������� ��������������� �������������� ������� �������
*		throw PMyHromatModbusException
*/
void WriteModbusFile(	uInt16 idx,			    // ������ �����
    ByteBuffPtr pFile,
    ByteBuffPtr pFileEnd,           // ������
    unsigned offset,                  // �������� �� ������ �����
    unsigned fileSpaceNum,             // ����� �������� ������� �� 0
    bool detector                   // ����� ���������
)
{
    //const unsigned maxRxDFrameSize = GetHromatMaxRxDFrameSize();
    const unsigned count = std::distance(pFile, pFileEnd);

    const AnsiString sFile = FormatFileInfoHead(detector, idx, fileSpaceNum);

    LogMyAppEvt("������ �����", sFile, MYSPRINTF_("%d ���� � ������� %d",  count, offset) );

    unsigned totallWriten = 0;

    while(totallWriten<count)
    {
        const unsigned
        //requestedLen = std::min( count - totallWriten, maxRxDFrameSize),
        requestedLen = count - totallWriten
        ;

        WriteModbusFileBlock(
	        idx,			        // ������ �����
            requestedLen,           // �����������
            offset,                 // �������� �� ������ �����
            fileSpaceNum,             // ����� �������� ������� �� 0
            detector,                   // ����� ���������
            pFile
        );

        pFile += requestedLen;
        offset  +=  requestedLen;
        totallWriten += requestedLen;
    }
    if(totallWriten==count)
        LogMyAppEvt("������� ����", sFile, MYSPRINTF_("%d ���� � ������� %d",  count, offset) );
    else
        LogMyAppEvt("������ ������ �����", sFile, MYSPRINTF_(
        "�������� %d ���� �� %d � ������� %d", totallWriten, count, offset) );


}
//------------------------------------------------------------------------------
bool ReadHromatGraduation(HromatGraduation& grad, unsigned idx, const AnsiString &ss )
{
    return ReadHromatFile
    (
	    idx,			                        	// ������ �����
        sizeof(HromatGraduation),            		// ����������� ����
        0,                                      // �������� �� ������ �����
        HromatFileSpace::GRADUATION, 			// ����� �������� ������� �� 0
        0,                                  	// ����� ���������
        reinterpret_cast<uInt8*>( &grad ), 		// �������� ������
        NULL,
        ss
    );
}
bool ReadLastHromatGraduation( HromatGraduation& ki, HromatGraduation& rk, HromatGraduation& rkmax  )
{
    return
        ReadHromatGraduation(ki, 0, "Ki") &&
        ReadHromatGraduation(rk, 1, "Rk") &&
        ReadHromatGraduation(rkmax, 2, "Rkmaxi");
}

/**
*           �������� ������������� ����������� ��
*/
HromatSoftID GetHromatInlineSoftId()
{
	HromatSoftID ret = {0};

    ByteBuffPtr dt, rxdEnd;
    ReadModbusRegistersData( H9005_DOT_EXE_VERSION_REG, 6, dt, rxdEnd);


    ret.h9001.version 		= ( dt[0]  << 8 ) + dt[1];
    ret.h9001.crc16 		= ( dt[3]  << 8 ) + dt[2];
    ret.ucint.version 		= ( dt[4]  << 8 ) + dt[5];
    ret.ucint.crc16 		= ( dt[7]  << 8 ) + dt[6];
    ret.calculator.version 	= ( dt[8]  << 8 ) + dt[9];
    ret.calculator.crc16 	= ( dt[10] << 8 ) + dt[11];

    return ret;
}

bool WriteHromatCRC16Key(unsigned short new_, unsigned short old_)
{
    uInt8 dt[4];
    dt[0] = new_ >> 8;
    dt[1] = new_;
    dt[2] = old_ >> 8;
    dt[3] = old_;

    HromatCryptData( dt, 4, dt );

    try
    {
    	WriteModbus4Bytes(  0x30, dt );
    }
    catch(...)
    {
    	// ������������� ����������
    	const PMyExcpt excpt = RethrowMyException(__FILE_LINE__);
        if( const DeviceModbusException *modbusException = excpt->As<DeviceModbusException>() )
        {
        	using namespace ModbusException;
            const unsigned i = modbusException->idx_;
            if( i != ILLEGAL_DATA_VALUE )
                throw excpt;
            return false;
        }
    }
    return true;
}

