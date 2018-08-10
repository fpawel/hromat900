//------------------------------------------------------------------------------

#include <MAP>
//------------------------------------------------------------------------------
#pragma hdrstop
//------------------------------------------------------------------------------
#include "MyFTP.h"

#include "..\DevSys\ReportContext.hpp"

// boost
#include "boost\noncopyable.hpp"
#include "boost\shared_ptr.hpp"
#include "guicon.h"
#include "MyExcpt.hpp"
#include "MyWinExcpt.hpp"
#include "MyWindow.h"
#include "AnsiStringUtils_.h"
#include "MySynchronize.h"

//------------------------------------------------------------------------------
#pragma package(smart_init)
//------------------------------------------------------------------------------
#define MY_SAFE_CALL_WININET1(dest,foo,params)::SetLastError(0);\
dest##=##::##foo##params;\
if(##dest##==##0) throw MyExcptionFactory::Create<MyWindowsException>\
(::GetLastError(), __FILE_LINE__,"Ошибка FTP::"#foo);

#define MY_SAFE_CALL_WININET2(foo,params) ::SetLastError(0);\
if(::##foo##params==FALSE) throw MyExcptionFactory::Create<MyWindowsException>\
(::GetLastError(), __FILE_LINE__,"Ошибка FTP::"#foo);
//------------------------------------------------------------------------------
struct TimeCounter
{
    TimeCounter() : tm_( ::GetTickCount() )
    {
    }
    ~TimeCounter()
    {
        MyWCout( IntToStr(::GetTickCount()-tm_)+" мсек.\n" );
    }
    const unsigned tm_;
};
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
class CloseHINTERNET : public boost::noncopyable
{
public:
	explicit CloseHINTERNET(HINTERNET& hFile) : hFile_(hFile)
    {
    }
    ~CloseHINTERNET()
    {
    	try
        {
    		MY_SAFE_CALL_WININET2( InternetCloseHandle, (hFile_) );
        }
        catch(...)
        {
        	HANDLE_MY_WININET_EXCEPTION
        }
    }
private:
	HINTERNET& hFile_;
};
//------------------------------------------------------------------------------
void __stdcall FTPHlpr::StatusCallBack( HINTERNET hInet, DWORD_PTR cntxt,
    DWORD status, LPVOID pStatusNfo, DWORD statusNfoLen)
{
    FTPHlpr* this_ = reinterpret_cast<FTPHlpr*>(cntxt);

    if( status==INTERNET_STATUS_CONNECTION_CLOSED )
        this_->connected_ = false;
    AnsiString stsNfo = 0;
    if(statusNfoLen==4)
    stsNfo = IntToStr( (int)*( (int*)pStatusNfo ) );

    typedef std::map<unsigned,AnsiString> MapT;
    typedef MapT::iterator IterT;
    MapT codeToCpt;

    #define SET_(nn) codeToCpt[##nn##]=#nn
    SET_( INTERNET_STATUS_CONNECTION_CLOSED     );
    SET_( INTERNET_STATUS_CONNECTED_TO_SERVER   );
    SET_( INTERNET_STATUS_CONNECTING_TO_SERVER  );
    SET_( INTERNET_STATUS_CLOSING_CONNECTION    );
    #undef SET_(nn)

    IterT i = codeToCpt.find(status);
    if( i==codeToCpt.end() ) return;
    MyWCout( i->second );
}
//------------------------------------------------------------------------------
FTPHlpr::FTPHlpr() : hInet_(NULL), hConnect_(NULL), connected_(false), curDir_(""),
    reportFTPState_(NULL)
{
    cntxt_ = reinterpret_cast<DWORD_PTR>( const_cast<FTPHlpr*>(this) );

}
//------------------------------------------------------------------------------
void FTPHlpr::Connect(const AnsiString& serverName, const AnsiString& userName,
        const AnsiString& userPassword)
{
	if(connected_) return;

    serverName_ = serverName;
    //serverName_ = "85.26.215.212";

    userName_ = userName;
    userPassword_ = userPassword;

    if( reportFTPState_ ) reportFTPState_(RPT_TRNSFR_CNTXT::ITFC, "Установка соединения с FTP-сервером...");


    MY_SAFE_CALL_WININET1( hInet_, InternetOpen, ("ХРОМАТ-900", INTERNET_OPEN_TYPE_DIRECT,
        NULL, NULL, 0) );

    MY_SAFE_CALL_WININET1( hConnect_, InternetConnect, (hInet_, serverName_.c_str(),
    	21, userName_.c_str(), userPassword_.c_str(), INTERNET_SERVICE_FTP,
        0, cntxt_) );

    connected_ = true;

    MyWCout( "Установлено соединение FTP-сервером хроматографа.\n" );
    curDir_ = GetCurrentDirectory();
    MYWCOUT_("Текущая директория %s\n", curDir_));
    if( reportFTPState_ ) reportFTPState_(RPT_TRNSFR_CNTXT::ITFC, "Установлено соединения с FTP-сервером.");
}
//------------------------------------------------------------------------------
void FTPHlpr::Disconnect()
{
	if( !connected_ ) return;
    ::InternetCloseHandle( hConnect_ );
    ::InternetCloseHandle( hInet_ );
    connected_ = false;
    MyWCout( "Cоединение FTP-сервером хроматографа разорвано.\n" );
    curDir_ = "";
    if( reportFTPState_ ) reportFTPState_(RPT_TRNSFR_CNTXT::THREAD_OK, "Ок");
}
//------------------------------------------------------------------------------
void FTPHlpr::ReadFile( const AnsiString& fileName,  LPVOID lpBuffer,
    DWORD dwNumberOfBytesToRead, LPDWORD lpdwNumberOfBytesRead )
{   
	HINTERNET hFile = NULL;
    const CloseHINTERNET closeHINTERNET(hFile);

    MY_SAFE_CALL_WININET1( hFile, FtpOpenFile, (hConnect_, fileName.c_str(),
    	GENERIC_READ, FTP_TRANSFER_TYPE_BINARY, cntxt_) );


    MY_SAFE_CALL_WININET2( InternetReadFile, (hFile, lpBuffer, dwNumberOfBytesToRead,
  		lpdwNumberOfBytesRead ) );
    if( reportFTPState_ ) reportFTPState_(RPT_TRNSFR_CNTXT::THREAD_OK, "Ок");
}
//------------------------------------------------------------------------------
void FTPHlpr::WriteFile( const AnsiString& fileName,  DWORD dwFlags,
	LPCVOID lpBuffer, DWORD dwNumberOfBytesToWrite, LPDWORD lpdwNumberOfBytesWritten )
{
	HINTERNET hFile = NULL;
    const CloseHINTERNET closeHINTERNET(hFile);
    MY_SAFE_CALL_WININET1( hFile, FtpOpenFile, (hConnect_, fileName.c_str(),
    	GENERIC_WRITE, FTP_TRANSFER_TYPE_BINARY, cntxt_) );


    MY_SAFE_CALL_WININET2( InternetWriteFile, (hFile, lpBuffer,
    	dwNumberOfBytesToWrite, lpdwNumberOfBytesWritten ) );
    if( reportFTPState_ ) reportFTPState_(RPT_TRNSFR_CNTXT::THREAD_OK, "Ок");
}
//------------------------------------------------------------------------------
AnsiString FTPHlpr::GetCurrentDirectory()
{
    if( !curDir_.IsEmpty() ) return curDir_;

    const TimeCounter tm;
    MyWCout( "FTP: Запрос текущей удалённой директории...");

    DWORD dwCurrentDirectory = MAX_PATH ;
    char lpszCurrentDirectory[MAX_PATH+1] = {'0'};
	MY_SAFE_CALL_WININET2( FtpGetCurrentDirectory, (hConnect_,
    	lpszCurrentDirectory, &dwCurrentDirectory ) );
    curDir_ = AnsiString(lpszCurrentDirectory);
    if( reportFTPState_ ) reportFTPState_(RPT_TRNSFR_CNTXT::THREAD_OK, "Ок");
    return curDir_;
}
//------------------------------------------------------------------------------
void FTPHlpr::SetCurrentDirectory(const AnsiString& dirName)
{
    if( curDir_==dirName ) return;

    const TimeCounter tm;
    MyWCout( MYSPRINTF_("FTP: Смена удалённой директории на %s...", dirName) );

	MY_SAFE_CALL_WININET2( FtpSetCurrentDirectory, (hConnect_,
    	dirName.c_str()) );

    curDir_=dirName;
    if( reportFTPState_ ) reportFTPState_(RPT_TRNSFR_CNTXT::THREAD_OK, "Ок");

}
//------------------------------------------------------------------------------
void FTPHlpr::GetFile( const AnsiString& remoteFile, const AnsiString& newFile )
{
    const TimeCounter tm;
    MyWCout( MYSPRINTF_("FTP: Копирование удалённого файла %s в %s...", remoteFile, newFile) );

	const DWORD
    	dwFlagsAndAttributes = FILE_ATTRIBUTE_NORMAL,
    	//dwFlags = FTP_TRANSFER_TYPE_BINARY | INTERNET_FLAG_RELOAD;
        dwFlags = FTP_TRANSFER_TYPE_BINARY | INTERNET_FLAG_RELOAD;

    const BOOL fFailIfExists = FALSE;

	MY_SAFE_CALL_WININET2( FtpGetFile, (hConnect_, remoteFile.c_str(),
  		newFile.c_str(), fFailIfExists, dwFlagsAndAttributes, dwFlags, cntxt_ ));
    if( reportFTPState_ ) reportFTPState_(RPT_TRNSFR_CNTXT::THREAD_OK, "Ок");
}
//------------------------------------------------------------------------------
void FTPHlpr::PutFile( const AnsiString& remoteFile, const AnsiString& localFile )
{
    MyWCout( MYSPRINTF_("FTP: Запись удалённого файла %s из %s...", remoteFile, localFile) );
	const DWORD
    	dwFlags = FTP_TRANSFER_TYPE_BINARY;
	MY_SAFE_CALL_WININET2( FtpPutFile, ( hConnect_, localFile.c_str(),
  		remoteFile.c_str(), dwFlags, cntxt_ ) );
    if( reportFTPState_ ) reportFTPState_(RPT_TRNSFR_CNTXT::THREAD_OK, "Ок");
}
//------------------------------------------------------------------------------
DWORD FTPHlpr::GetFileSize( const AnsiString& remoteFile)
{
    MyWCout( MYSPRINTF_("FTP: Запрос размера файла %s...", remoteFile) );

	HINTERNET hFile = NULL;
    const CloseHINTERNET closeHINTERNET(hFile);
    
    MY_SAFE_CALL_WININET1( hFile, FtpOpenFile, (hConnect_, remoteFile.c_str(),
    	GENERIC_READ, FTP_TRANSFER_TYPE_BINARY, cntxt_ ) );

    DWORD hi = 0;
    const DWORD lo = ::FtpGetFileSize( hFile,  &hi );
    if( reportFTPState_ ) reportFTPState_(RPT_TRNSFR_CNTXT::THREAD_OK, "Ок");

    return lo;
}
//------------------------------------------------------------------------------
void FTPHlpr::Command( const AnsiString &cmd )
{
    MyWCout( MYSPRINTF_("FTP: отправка команды %s...", cmd) );
	const DWORD dwFlags = FTP_TRANSFER_TYPE_BINARY;
    MY_SAFE_CALL_WININET2( FtpCommand, ( hConnect_, FALSE, dwFlags,
  		cmd.c_str(), cntxt_, NULL ));
    if( reportFTPState_ ) reportFTPState_(RPT_TRNSFR_CNTXT::THREAD_OK, "Ок");
}
//------------------------------------------------------------------------------
void FTPHlpr::FindFirstFile( const AnsiString& fileName,
        LPWIN32_FIND_DATAA lpFindFileData, DWORD dwFlags )
{
    HINTERNET hFind = NULL;
    const CloseHINTERNET closeHINTERNET(hFind);
    
    MY_SAFE_CALL_WININET1(  hFind, FtpFindFirstFile, (hConnect_,
        fileName.c_str(), lpFindFileData, dwFlags, cntxt_ ) );
    if( reportFTPState_ ) reportFTPState_(RPT_TRNSFR_CNTXT::THREAD_OK, "Ок");

}






