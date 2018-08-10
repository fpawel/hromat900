#ifndef MY_MODBUS_DEFINITIONS_HEADER_INCLUDED_____
#define MY_MODBUS_DEFINITIONS_HEADER_INCLUDED_____

#include "..\my_include\free_types_.h"

/**
*		�������� MODBUS-���������� ������������
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

    // ���� �������������� ��������
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

    // �������� �������������� ��������
    const ConstCString DSCR[COUNT + 1] =
    {
    	//01h	ILLEGAL FUNCTION
		"������� � �������� ��������� �� �������������� �� ������ ������������",

		//02h	ILLEGAL DATA ADDRESS
        "�����, ��������� � ���� ������, �������� �� ���������� ��� ������� ������������",

		//03h	ILLEGAL DATA VALUE
        "�������� � ���� ������ ����������� ��� ������� ������������",

		//04h	FAILURE IN ASSOCIATED DEVICE
        "����������� �� ����� �������� �� ������ ��� ��������� ������",

		//06h	SLAVE DEVICE BUSY
        "������ ��������� ���������� ��������",

		//11h	BUSY
        "������ ��������� ���������� ��������",

        //12h
        "���� �� ������",

        "����������� �������������� �������� ������������"
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

}; // namespace ModbusException  �������� MODBUS-���������� ������������

#endif
