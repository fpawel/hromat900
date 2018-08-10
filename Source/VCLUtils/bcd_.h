//---------------------------------------------------------------------------
#ifndef bcd_H
#define bcd_H
//---------------------------------------------------------------------------

//vcl
#include <SysUtils.hpp>

//std
#include <vector>

#define MAX_BCD_FLOAT 999999

//Преобразование двоично-десятичного упакованного числа в вещественное
double BCDToFloat(const unsigned char* bcd);
//double BCDToFloat_Throw(const unsigned char* bcd);

void TransformBCDDataToFloat( const unsigned char* bcd,	const unsigned char* bcdEnd, double *out );

// Упаковать число в BCD
void FloatToBCD(double val, unsigned char* res);
std::vector<unsigned char> FloatToBCD(double val);

// Преобразовать строку в число, если невозможно, вернуть MAX_BCD_FLOAT
double StrToFloatBCD( const AnsiString& s );

// преобразуема ли строка в двоично-десятичное
bool IsBCDFloat( const AnsiString& s );

// Преобразовать число в строку, если число по модулю >= MAX_BCD_FLOAT, вернуть ""
AnsiString FloatBCDToStr( double val );






#endif
