#ifndef MY_WINDOWS_ERROR_HEADER_INCLUDED__________
#define MY_WINDOWS_ERROR_HEADER_INCLUDED__________

#include <windows.h>
#include <system.hpp>
#include <string>

/*
AnsiString WindowsInetErrorCodeToStr( )
{
    DWORD len = 0, errorCode =0;
    ::InternetGetLastResponseInfo(&errorCode, NULL, &len);
    std::vector<char> vBuff(len+1, '\0');
    char *p = &vBuff.at(0);
    ::InternetGetLastResponseInfo(&errorCode, p, &len);

    // строка результата
    std::string ret( p );
    ret.erase( std::remove_if(ret.begin(), ret.end(), std::iscntrl ), ret.end() );
    return ret.c_str();
}
*/
AnsiString WindowsErrorCodeToStr( unsigned long errorCode )
{
	if( errorCode==0 ) return AnsiString();
    // if( errorCode==ERROR_INTERNET_EXTENDED_ERROR) return WindowsInetErrorCodeToStr();
    try
    {
        LPVOID lpMsgBuf;
        ::FormatMessage
        (
    	    FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
            NULL, errorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPTSTR) &lpMsgBuf, 0, NULL
        );
        // строка результата
        if(lpMsgBuf==NULL) return "";
        std::string ret( (char*) lpMsgBuf );
        ret.erase( std::remove_if(ret.begin(), ret.end(), std::iscntrl ), ret.end() );
        LocalFree(lpMsgBuf);
        return ret.c_str();
    }
    catch(...)
    {
        return "";
    }

}




#endif