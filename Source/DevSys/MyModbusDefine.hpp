#ifndef MY_MODBUS_DEFINITIONS_HEADER_INCLUDED_____
#define MY_MODBUS_DEFINITIONS_HEADER_INCLUDED_____

#include "..\my_include\free_types_.h"

/**
*		Описание MODBUS-исключений хроматографа
*/
namespace ModbusException
{
	enum
	{
		ILLEGAL_FUNCTION,				// 0x01
		ILLEGAL_DATA_ADDRESS,           // 0x02
		ILLEGAL_DATA_VALUE,             // 0x03
		FAILURE_IN_ASSOCIATED_DEVICE,   // 0x04
		SLAVE_DEVICE_BUSY,              // 0x06
		BUSY,                           // 0x11
        FILE_NOT_FOUND,                 // 0x12
        COUNT
	};

    // Коды исключительных ситуаций
    const unsigned CODE[COUNT] =
    {
		0x01, 	//	ILLEGAL_FUNCTION
		0x02, 	//	ILLEGAL_DATA_ADDRESS
		0x03, 	//	ILLEGAL_DATA_VALUE
		0x04, 	//	FAILURE_IN_ASSOCIATED_DEVICE
		0x06, 	//	SLAVE_DEVICE_BUSY
		0x11, 	//	BUSY
        0x12    //  FILE_NOT_FOUND,                 // 0x11
    };

    // Описвния исключительных ситуаций
    const ConstCString DSCR[COUNT + 1] =
    {
    	//01h	ILLEGAL FUNCTION
		"Команда в принятом сообщении не поддерживается на данном хроматографе",

		//02h	ILLEGAL DATA ADDRESS
        "Адрес, указанный в поле данных, является не допустимым для данного хроматографа",

		//03h	ILLEGAL DATA VALUE
        "Значение в поле данных недопустимо для данного хроматографа",

		//04h	FAILURE IN ASSOCIATED DEVICE
        "хроматограф не может ответить на запрос или произошла авария",

		//06h	SLAVE DEVICE BUSY
        "Прибор выполняет длительную операцию",

		//11h	BUSY
        "Прибор выполняет длительную операцию",

        //12h
        "Файл не найден",

        "Неизвестная исключительная ситуация хроматографа"
    };

    unsigned GetIndexOfCode(uInt8 code)
    {
        return std::find(CODE, CODE + COUNT, code) - CODE;
    }

    ConstCString GetDescrOfCode(uInt8 code)
    {
    	return DSCR[ GetIndexOfCode(code) ];
    }

    ConstCString GetDescr(unsigned n)
    {   
    	return DSCR[ n<COUNT ? n : COUNT ];
    }

}; // namespace ModbusException  Описание MODBUS-исключений хроматографа

#endif
