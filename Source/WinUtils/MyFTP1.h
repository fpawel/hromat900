//---------------------------------------------------------------------------
#ifndef MyFTPH
#define MyFTPH
//---------------------------------------------------------------------------
#include <windows.h>
#include <wininet.h>
#include <system.hpp>

#include "boost\noncopyable.hpp"
#include "boost\shared_ptr.hpp"

#define HANDLE_MY_WININET_EXCEPTION \
const PMyExcpt excpt = RethrowMyException(__FILE_LINE__);\
const AnsiString\
    className = excpt->ClassName(),\
    fullMsg = FormatMyException( excpt.get(), "%eLine:%eFile %eMsg; " );\
MyWCout( "Ошибка FTP! "+ className+" "+ fullMsg);

typedef void (__closure *ReportFTPState )( unsigned, const AnsiString&);

namespace my
{
    class Thread;
};

class FTPHlpr : boost::noncopyable
{
public:
    explicit FTPHlpr();
    void Connect( const AnsiString& serverName, const AnsiString& userName,
        const AnsiString& userPassword );


    void Disconnect();

    bool IsConnected() const { return connected_; }

    void ReadFile( const AnsiString& fileName,
	    LPVOID lpBuffer,  DWORD dwNumberOfBytesToRead,
        LPDWORD lpdwNumberOfBytesRead );

    void WriteFile( const AnsiString& fileName,  DWORD dwFlags,
	    LPCVOID lpBuffer, DWORD dwNumberOfBytesToWrite,
        LPDWORD lpdwNumberOfBytesWritten );

    AnsiString GetCurrentDirectory();
    void SetCurrentDirectory(const AnsiString& dirName);

    void GetFile( const AnsiString& remoteFile, const AnsiString& newFile );
    void PutFile( const AnsiString& remoteFile, const AnsiString& localFile );
    DWORD GetFileSize( const AnsiString& remoteFile);
    void Command( const AnsiString &cmd );


    void FindFirstFile( const AnsiString& fileName,
        LPWIN32_FIND_DATAA lpFindFileData, DWORD dwFlags );

    void SetReportFTPStateFunctor( ReportFTPState reportFTPState )
    {
        reportFTPState_ = reportFTPState;
    }

private:
    HINTERNET hInet_, hConnect_;
    bool connected_;
    DWORD_PTR cntxt_;
    AnsiString curDir_;
    ReportFTPState reportFTPState_;

    boost::shared_ptr<my::Thread> connectThread_;
    DWORD connectThreadLastError_;
    bool connectionWasBreaked_;

    AnsiString serverName_, userName_, userPassword_;

    void __fastcall ConnectThreadProcedure();

    static void __stdcall StatusCallBack( HINTERNET hInet, DWORD_PTR cntxt,
    DWORD status, LPVOID pStatusNfo, DWORD statusNfoLen);
};

//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
