//---------------------------------------------------------------------------

#pragma hdrstop

//---------------------------------------------------------------------------

#include <boost/crc.hpp>      // for boost::crc_basic, boost::crc_optimal
#include <boost/cstdint.hpp>  // for boost::uint16_t

#include <algorithm>  // for std::for_each
#include <cassert>    // for assert
#include <cstddef>    // for std::size_t
#include <iostream>   // for std::cout
#include <ostream>    // for std::endl

#include <system.hpp>    // for AnsiString
#include <SysUtils.hpp>    // 



#include "..\my_include\CRC16.h"

using std::endl;
using std::cout;


#pragma argsused
int main(int argc, char* argv[])
{
	/// This is "123456789" in ASCII
    unsigned char const  data[] = { 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
     0x38, 0x39 };
    std::size_t const    data_len = sizeof( data ) / sizeof( data[0] );

    // The expected CRC for the given data
    boost::uint16_t const  expected = 0x29B1;

    // Simulate CRC-CCITT
    boost::crc_basic<16>  crc_ccitt1( 0x1022, 0xFFFF, 0, false, false );
    crc_ccitt1.process_bytes( data, data_len );
    //assert( crc_ccitt1.checksum() == expected );

    // Repeat with the optimal version (assuming a 16-bit type exists)
    boost::crc_optimal<16, 0x1022, 0xFFFF, 0, false, false>  crc_ccitt2;
    crc_ccitt2 = std::for_each( data, data + data_len, crc_ccitt2 );
    //assert( crc_ccitt2() == expected );

    cout << AnsiString().sprintf(
    	"crc=0x%s\nmycrc=0x%s\n",
    	IntToHex( crc_ccitt2(), 4 ), IntToHex( GetCRC16 ( data, data+data_len), 4)
    );

    std::system("PAUSE");

	return 0;
}
//---------------------------------------------------------------------------
 