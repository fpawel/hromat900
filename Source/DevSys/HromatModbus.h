//---------------------------------------------------------------------------
#ifndef HromatModbusH
#define HromatModbusH
//---------------------------------------------------------------------------
#include "..\my_include\free_types_.h"
#include "Hromat900.hpp"
#include "SoftID.hpp"

AnsiString FormatHromatCommand(uInt16 cmd, const uInt8* dt);
AnsiString FormatHromatCommand(uInt16 cmd, double val);

/**
*       ������ ����� ������ � ����������� � ����� MODBUS
*/
enum{ MODBUS_FILE_INFO_SIZE = 17 };

/**
*		������� ����� ��� ���������� ������
*/
class MyHromatModbusException;


/**
*       �������� ��� ��������� ������������
*/
unsigned char GetHromatStateCode();


/**
*		�������� 4 �����
*		throw PMyHromatModbusException
*/
void WriteModbus4Bytes(  uInt16 cmd, const uInt8* dt );
/**
*		�������� �����
*/
void WriteModbusFloat(  uInt16 cmd, double val );
/**
*		������� ������
*		throw PMyHromatModbusException
*/
void ReadModbusRegistersData(  uInt16 regAddr,
	unsigned regCount,	ByteBuffPtr& rxd, ByteBuffPtr& rxdEnd );
/**
*		������� ������ ����������� �����
*		throw PMyHromatModbusException
*/
void ReadModbusFloat(  uInt16 regAddr, unsigned count,	double *pResult);
double GetModbusFloat( uInt16 regAddr );
unsigned ReadModbusUnsigned(  uInt16 regAddr );
int ReadModbusSigned(  uInt16 regAddr );



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
    uInt8* pFile,                   // �������� ������
    LRESULT (__closure *OnGet )(ByteBuffPtr, ByteBuffPtr) = NULL,
    const AnsiString &fileCaption = "" 
);

bool GetHromatFile(
	unsigned nFile,			        // ������ �����
    unsigned count,                   // ����������� ����
    unsigned fileSpaceNum,             // ����� �������� ������� �� 0
    bool detector,                  // ����� ���������
    uInt8* pFile,             // �������� ������
    const AnsiString &fileCaption = ""
    );

/**
*		������� ���������� � ������, ��������� ��������� � ModbusFileInfo *fileInfo
*		�������������� ��������������� �������������� ������� �������
*		throw PMyHromatModbusException
*
*       �����������: ������ ������� fileInfo �� ����� count*sizeof(ModbusFileInfo)
*/
unsigned ReadHromatFilesInfo(
	unsigned firstFileIndex,          // ������ ������� �����
    unsigned count,                   // ����������� ������
    unsigned fileSpaceNum,             // ����� �������� ������� �� 0
    bool detector,                  // ����� ���������
    ModbusFileInfo *fileInfo		// �������� �����
    );

bool ReadHromatFileInfo(
	unsigned fileIndex,          // ������ ������� �����
    unsigned fileSpaceNum,             // ����� �������� ������� �� 0
    bool detector,                  // ����� ���������
    ModbusFileInfo *fileInfo		// �������� �����
    );

/**
*		�������� ����
*		�������������� ��������������� �������������� ������� �������
*		throw PMyHromatModbusException
*/
void WriteModbusFile( uInt16 nFile,
    ByteBuffPtr pFile,
    ByteBuffPtr pFileEnd,           // ������
    unsigned offset,                  // �������� �� ������ �����
    unsigned fileSpaceNum,             // ����� �������� ������� �� 0
    bool detector                   // ����� ���������
);

/**
*           ��������� ��������� ���������
*/
void SetHromatActiveDetector(bool dtctr);

/**
*           �������� ��������� �����������
*/
bool ReadLastHromatGraduation( HromatGraduation& ki, HromatGraduation& rk, HromatGraduation& rkmax  );

/**
*           �������� ������������� ����������� ��
*/
HromatSoftID GetHromatInlineSoftId();

bool WriteHromatCRC16Key(unsigned short new_, unsigned short old_);




//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
