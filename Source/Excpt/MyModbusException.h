//---------------------------------------------------------------------------
#ifndef MyModbusExceptionH
#define MyModbusExceptionH
//---------------------------------------------------------------------------
#include "..\my_include\free_types_.h"
#include "MyExcpt.hpp"

	/**
	*  	Исключение для информирования о исключительной ситуации MODBUS в хроматографе
	*/

class DeviceModbusException : public MyExternalException
{
friend struct MyExcptionFactory;
public:
	typedef struct { uInt8 code, slave, cmd; } Tag;
protected:
	DeviceModbusException(Tag tag, const FileLine& loc, const AnsiString& msg, PMyExcpt nest);
public:
	virtual AnsiString ClassName() const;
    const Tag tag_;
    const unsigned idx_;
    const AnsiString descr_;
private:
    static AnsiString MakeMsg(const AnsiString& msg, Tag tag );
};

//---------------------------------------------------------------------------
#endif
