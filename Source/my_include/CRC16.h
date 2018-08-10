//---------------------------------------------------------------------------
#ifndef MY_CALC_CRC16_H_INCLUDED___
#define MY_CALC_CRC16_H_INCLUDED___
//---------------------------------------------------------------------------
#include "HromatCrypt.h"

typedef unsigned short int  uInt16;
typedef unsigned char  uInt8;
typedef const uInt8* PuInt8;

//----------------------------------------------------------------
// Функция подсчёта контрольной суммы CRC блока данных
//----------------------------------------------------------------
uInt16 CRC16Byte(unsigned char val, uInt16 crc16)
{

	crc16 ^=val;
	for (int i=0;i<8;i++)
    {
		const bool bitik = ( (crc16&0x0001)==0) ? 0 : 1;
		crc16 = (crc16>>1) & 0x7FFF;
		if ( bitik )
			crc16 ^= 0xA001;

	}
	return crc16;
}

uInt16 CalculateCRC16(PuInt8 buf, unsigned count, uInt16 crc16)
{
	for (unsigned i=0; i<count; i++)
		crc16 = CRC16Byte( buf[i], crc16 );
	return crc16;
}

uInt16 CalculateCRC16(PuInt8 bufBeg, PuInt8 bufEnd, uInt16 crc16)
{	
	return CalculateCRC16(bufBeg, bufEnd-bufBeg, crc16);
}


unsigned short int GetCRC16 ( const unsigned char* i, const unsigned char* end,
	const unsigned short crc = 0xFFFF)
{
	return CalculateHromatCRC16( i, end-i, crc);
}

class CalcCRC16Hlpr
{
public:
    explicit CalcCRC16Hlpr(unsigned short crc16 = 0xFFFF) : crc16_(crc16)
    {
    }
    
    CalcCRC16Hlpr(const CalcCRC16Hlpr& aCopy) : crc16_( aCopy.crc16_ )
    {
    }

    void operator=(const CalcCRC16Hlpr& aCopy)
    {
       crc16_ = aCopy.crc16_;
    }

    void UpdateCRC16 ( unsigned char *p, unsigned len )
    {
        crc16_ = CalculateHromatCRC16( p, len, crc16_);
    }

    unsigned short GetCRC16() const { return crc16_; }
private:
    unsigned short int crc16_;
};  



template<typename T> unsigned short CalcCRC16OfValue ( const T& t, unsigned short init = 0xFFFFul )
{
    const unsigned char
        *i = reinterpret_cast<const unsigned char*>(&t),
        *e = i + sizeof(t);
    return CalcCRC16 ( i, e, init );

}

bool IsModbusStr(const unsigned char* b, const unsigned char* e,
	unsigned char devAddr, unsigned char commandCode)
{
	return !(e-b<4) && b[0]==devAddr &&
    ( b[1]==commandCode || b[1]==( commandCode | 0x80 ) ) &&
    GetCRC16( b, e )==0;
}


//---------------------------------------------------------------------------
#endif
//-----------------------------------------------------------------