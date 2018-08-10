#ifndef MY_CONSOLE_MATH_UTILS_HEADER_IMCLUDED___
#define MY_CONSOLE_MATH_UTILS_HEADER_IMCLUDED___

#include <sstream>


typedef std::pair<uInt8, bool> MyByte ;
MyByte GetTheByte(const char* s);

namespace ConMath
{
	void ConvertBCDFloat( const std::string& inS)
    {
        uInt8 dt[4];
        std::stringstream strm( inS );

        for( unsigned i=0; i<4; ++i )
        {
        	std::string s;
        	strm >> s;
        	const MyByte getVal = GetTheByte( s.c_str() );
            dt[i] = getVal.second ? getVal.first : 0xFF;
        }
        MYWCOUT_("(%s,%s,%s,%s)=[%s]\n",
        IntToHex(dt[0],2), IntToHex(dt[1],2), IntToHex(dt[2],2), IntToHex(dt[3],2),
        FloatBCDToStr( BCDToFloat(dt) )
        ));
    }

    void ConvertByteDouble( const std::string& inS)
    {
    	union
        {
        	uInt8 dt[8];
            double val;
        };
        std::stringstream strm( inS );

        for( unsigned i=0; i<8; ++i )
        {
        	std::string s;
        	strm >> s;
        	const MyByte getVal = GetTheByte( s.c_str() );
            dt[i] = getVal.second ? getVal.first : 0;
        }
        MYWCOUT_( "(%s,%s,%s,%s,%s,%s,%s,%s)=%s\n",
        IntToHex(dt[0],2), IntToHex(dt[1],2), IntToHex(dt[2],2), IntToHex(dt[3],2),
        IntToHex(dt[4],2), IntToHex(dt[5],2), IntToHex(dt[6],2), IntToHex(dt[7],2),
        FloatToStr( val )
        ));
    }

    void ConvertDoubleBytes( const std::string& inS)
    {
    	union
        {
        	uInt8 dt[8];
            double val;
        };
        std::stringstream strm( inS );
        val = 0;
        strm >> val;
        MYWCOUT_( "%s = (%s,%s,%s,%s,%s,%s,%s,%s)\n",
        	FloatToStr( val ),
        	IntToHex(dt[0],2), IntToHex(dt[1],2),
            IntToHex(dt[2],2), IntToHex(dt[3],2),
        	IntToHex(dt[4],2), IntToHex(dt[5],2),
            IntToHex(dt[6],2), IntToHex(dt[7],2)
        ));
    }
}; // namespace ConMath


#endif