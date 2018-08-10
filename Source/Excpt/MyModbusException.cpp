//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#pragma hdrstop
//---------------------------------------------------------------------------
#include "MyModbusException.h"
#include "..\DevSys\MyModbusDefine.hpp"
#include "AnsiStringUtils_.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
AnsiString DeviceModbusException::ClassName() const
{
	return "DeviceModbusException";
}
//---------------------------------------------------------------------------
AnsiString DeviceModbusException::MakeMsg(const AnsiString& msg, Tag tag )
{
    const AnsiString s = AnsiString().sprintf("Код ошибки=%d слэйв=%d команда=%d %s: %s",
    tag.code, tag.slave, tag.cmd, msg, ModbusException::GetDescrOfCode(tag.code) );
    return s;
}
//------------------------------------------------------------------------------
DeviceModbusException::DeviceModbusException(Tag tag, const FileLine& loc,
	const AnsiString& msg, PMyExcpt nest) :
    MyExternalException(loc, MakeMsg(msg, tag ), nest),
    tag_(tag), idx_(ModbusException::GetIndexOfCode(tag.code) ),
    descr_( ModbusException::GetDescrOfCode(tag.code) )
{
}
//---------------------------------------------------------------------------
