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
*       Размер блока данных с информацией о файле MODBUS
*/
enum{ MODBUS_FILE_INFO_SIZE = 17 };

/**
*		Базовый класс для исключений модуля
*/
class MyHromatModbusException;


/**
*       Получить код состояния хроматографа
*/
unsigned char GetHromatStateCode();


/**
*		Записать 4 байта
*		throw PMyHromatModbusException
*/
void WriteModbus4Bytes(  uInt16 cmd, const uInt8* dt );
/**
*		Записать число
*/
void WriteModbusFloat(  uInt16 cmd, double val );
/**
*		Считать массив
*		throw PMyHromatModbusException
*/
void ReadModbusRegistersData(  uInt16 regAddr,
	unsigned regCount,	ByteBuffPtr& rxd, ByteBuffPtr& rxdEnd );
/**
*		Считать массив вщественных чисел
*		throw PMyHromatModbusException
*/
void ReadModbusFloat(  uInt16 regAddr, unsigned count,	double *pResult);
double GetModbusFloat( uInt16 regAddr );
unsigned ReadModbusUnsigned(  uInt16 regAddr );
int ReadModbusSigned(  uInt16 regAddr );



/**
*		Считать файл, поместить результат в uInt8* pOut
*		Предваоительно устанавливается соответсвующая фаловая область
*		throw PMyHromatModbusException
*/
unsigned ReadHromatFile(
	unsigned nFile,			        // индекс файла
    unsigned count,                   // колличество байт
    unsigned offset,                // смещение от начала файла
    unsigned fileSpaceNum,             // номер файловой области от 0
    bool detector,                  // номер детектора
    uInt8* pFile,                   // выходной буффер
    LRESULT (__closure *OnGet )(ByteBuffPtr, ByteBuffPtr) = NULL,
    const AnsiString &fileCaption = "" 
);

bool GetHromatFile(
	unsigned nFile,			        // индекс файла
    unsigned count,                   // колличество байт
    unsigned fileSpaceNum,             // номер файловой области от 0
    bool detector,                  // номер детектора
    uInt8* pFile,             // выходной буффер
    const AnsiString &fileCaption = ""
    );

/**
*		Считать информацию о файлах, поместить результат в ModbusFileInfo *fileInfo
*		Предваоительно устанавливается соответсвующая фаловая область
*		throw PMyHromatModbusException
*
*       Предусловие: размер буффера fileInfo не менее count*sizeof(ModbusFileInfo)
*/
unsigned ReadHromatFilesInfo(
	unsigned firstFileIndex,          // индекс первого файла
    unsigned count,                   // колличество файлов
    unsigned fileSpaceNum,             // номер файловой области от 0
    bool detector,                  // номер детектора
    ModbusFileInfo *fileInfo		// выходной буфер
    );

bool ReadHromatFileInfo(
	unsigned fileIndex,          // индекс первого файла
    unsigned fileSpaceNum,             // номер файловой области от 0
    bool detector,                  // номер детектора
    ModbusFileInfo *fileInfo		// выходной буфер
    );

/**
*		Записать файл
*		Предваоительно устанавливается соответсвующая фаловая область
*		throw PMyHromatModbusException
*/
void WriteModbusFile( uInt16 nFile,
    ByteBuffPtr pFile,
    ByteBuffPtr pFileEnd,           // данные
    unsigned offset,                  // смещение от начала файла
    unsigned fileSpaceNum,             // номер файловой области от 0
    bool detector                   // номер детектора
);

/**
*           Установка активного детектора
*/
void SetHromatActiveDetector(bool dtctr);

/**
*           Получить последнюю градуировку
*/
bool ReadLastHromatGraduation( HromatGraduation& ki, HromatGraduation& rk, HromatGraduation& rkmax  );

/**
*           Получить идентификатор встроенного ПО
*/
HromatSoftID GetHromatInlineSoftId();

bool WriteHromatCRC16Key(unsigned short new_, unsigned short old_);




//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
