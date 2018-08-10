//---------------------------------------------------------------------------
#include <math>
//---------------------------------------------------------------------------
#pragma hdrstop

#include "bcd_.h"
#include "AnsiStringUtils_.h"
#include "MyExcpt.hpp"

//---------------------------------------------------------------------------

#pragma package(smart_init)
//---------------------------------------------------------------------------
void TransformBCDDataToFloat( const unsigned char* bcd,	const unsigned char* bcdEnd,
    double *out )
{
	for( ; bcd<bcdEnd; bcd+=4, ++out )
        *out = BCDToFloat( bcd );
}
//---------------------------------------------------------------------------
//Преобразование двоично-десятичного упакованного числа в вещественное
double BCDToFloat_Throw(const unsigned char* bcd)
{
	for(int i=1;i<4;++i)
    	if ( (bcd[i] & 0xF0)>0x90 || (bcd[i] & 0x0F)>0x09 )
        MY_THROW_( AnsiString().sprintf("Error convert bcd[%d,%d,%d,%d] "
        "to float: bad number!", bcd[0], bcd[1], bcd[2], bcd[3]).c_str() );

	AnsiString s = IntToHex(bcd[1],2)+IntToHex(bcd[2],2)+IntToHex(bcd[3],2);
    int zap = bcd[0] & 0x07;
    try
    {
    	if (zap>0) s.Insert(',', 6-zap+1 );
    	if ( (bcd[0] & 0x80)!=0 ) s.Insert('-',1);
     	return StrToFloat( s );
    }
    catch(...)
    {
    	assert(!"Ошибка при преобразовании BCD в double!");
        return 0;
    }
}
//---------------------------------------------------------------------------
// преобразуема ли строка в двоично-десятичное
bool IsBCDFloat( const AnsiString& s )
{
	const MyDouble val = MyStrToD( s );
    return val.second && fabs(val.first)< MAX_BCD_FLOAT;
}
//---------------------------------------------------------------------------
//Преобразование двоично-десятичного упакованного числа в вещественное
double BCDToFloat(const unsigned char* bcd)
{
	for(int i=1;i<4;++i)
    	if ( (bcd[i] & 0xF0)>0x90 || (bcd[i] & 0x0F)>0x09 )
        	return MAX_BCD_FLOAT;
	AnsiString s = IntToHex(bcd[1],2)+IntToHex(bcd[2],2)+IntToHex(bcd[3],2);

    int zap = bcd[0] & 0x0F, sign = bcd[0] & 0xF0;
    if (zap)
    	s.Insert(DecimalSeparator, 6-zap+1 );
    if (sign == 0x80)
    	s.Insert('-',1);
    return StrToFloat( s );
}
//---------------------------------------------------------------------------
// Преобразовать строку в число, если невозможно, вернуть MAX_BCD_FLOAT
double StrToFloatBCD( const AnsiString& s )
{
	const MyDouble val = MyStrToD( s );
    return val.second ? val.first : MAX_BCD_FLOAT;
}
//---------------------------------------------------------------------------
// Преобразовать число в строку, если число по модулю >= MAX_BCD_FLOAT, вернуть ""
AnsiString FloatBCDToStr( double val )
{
    return fabs(val)>=MAX_BCD_FLOAT ? AnsiString(" ") : FormatFloat("#0.######",val);
}
//---------------------------------------------------------------------------
// Упаковать число в BCD
void FloatToBCD(double val, unsigned char* res)
{
    if (fabs(val)>=MAX_BCD_FLOAT)
    {
    	res[0] = res[1] = res[2] = res[3] = 0xFF;
        return;
    }


	double absVal = val;
    res[0] = 0;
    if (val<0)
    {
    	res[0] = 0x80;
        absVal *= -1;
    };


    int zap = 0;
    if (val!=0)
    {
    	int n = FormatFloat(".000000",absVal).Pos(DecimalSeparator);
    	zap = 7-n;   //позиция запятой
    	if (zap<0) zap = 0;
        else
        	if (zap>6) zap = 6;
    }
    absVal *= pow(10,zap);
    const AnsiString s = FormatFloat("000000",absVal);
    res[0] += zap;
    res[1] = StrToInt(s[1])*16+StrToInt(s[2]);
    res[2] = StrToInt(s[3])*16+StrToInt(s[4]);
    res[3] = StrToInt(s[5])*16+StrToInt(s[6]);
}
//---------------------------------------------------------------------------
std::vector<unsigned char> FloatToBCD(double val)
{
    std::vector<unsigned char> ret(4,0);
    FloatToBCD(val, &ret.at(0) );
    return ret;
}
