//---------------------------------------------------------------------------
// win
#include <windows.h>
#include <wincon.h>
//std
#include <stdio>
#include <fstream>
#include <utility>

#include <dateutils.hpp>

//------------------------------------------------------------------------------
#pragma hdrstop
//------------------------------------------------------------------------------
#include "guicon.h"

// my
//#include "..\my_include\win\MyModFN.hpp"
#include "..\my_include\FooCalledOnce.hpp"
#include "MyFileCtrl.h"
#include "AnsiStringUtils_.h"
#include "..\my_include\win\WinErr.hpp"
#include "MyAppWinPath.hpp"


//------------------------------------------------------------------------------
#pragma package(smart_init)
//------------------------------------------------------------------------------





////////////////////////////////////////////////////////////////////////////////
	std::pair<unsigned,unsigned> GetOrders()
    {
   		CONSOLE_SCREEN_BUFFER_INFO lConsoleScreenBufferInfo;
   		GetConsoleScreenBufferInfo( GetStdHandle(STD_OUTPUT_HANDLE), &lConsoleScreenBufferInfo );
        return std::make_pair( lConsoleScreenBufferInfo.dwSize.X,
        	lConsoleScreenBufferInfo.dwSize.Y);
    }

    //  control signal type
    BOOL WINAPI HandlerRoutine( const DWORD dwCtrlType)
    {
        const bool isCtrlBreakEvent =
            (dwCtrlType==CTRL_BREAK_EVENT	|| dwCtrlType==CTRL_C_EVENT);
        if( !isCtrlBreakEvent )
            return FALSE;
        printf( "\n\t\t---Ctrl+Break pressed!---\n" );
        return TRUE;
    }
	//--------------------------------------------------------------------------
    
	void InitializeGUIConsole()
	{
        std::fclose(stdin);
        std::fclose(stderr);
    	std::fclose(stdout);
        std::freopen("CONIN$", "r", stdin);
        std::freopen("CONOUT$", "w", stderr);
    	std::freopen("CONOUT$", "w", stdout);
        std::system("MODE CON COLS=80");
    	std::system("MODE CON LINES=9999");
    	std::system("MODE CON CP SELECT=866");
        printf( "GUI CON initialized.\n" );
	}
    //--------------------------------------------------------------------------
    void SaveConsoleToFile(const AnsiString& fn)
	{
        MyForceDirectory( ExtractFilePath(fn) );

		struct FindEndNotSpace
    	{
    		char* operator()( char *b, char *e ) const
			{
				while( (*(e-1)==' ' || *(e-1)=='\0' || *(e-1)=='\t') && --e!=b ) ;
    			return e;
			}
    	};
		const HANDLE hStdout = ::GetStdHandle(STD_OUTPUT_HANDLE);
   		DWORD cWritten;

   		COORD coordBufSize, coordBufCoord;
   		SMALL_RECT srctReadRect;
   		CONSOLE_SCREEN_BUFFER_INFO lConsoleScreenBufferInfo;

   		// Get the screen buffer information and set the rectangles
   		if( GetConsoleScreenBufferInfo( hStdout, &lConsoleScreenBufferInfo )==FALSE )
        	return;

   		const int
    		szX = lConsoleScreenBufferInfo.dwSize.X,
    		szY = lConsoleScreenBufferInfo.dwSize.Y;

   		std::vector<char> lineBuff( szX+1 );

   		DWORD numberOfCharsRead;

        ::SetLastError(0);
    	std::ofstream logCout( fn.c_str() );
        if( !logCout.is_open() )
        {
        	ShowLastWindowsError();
            return;
        }

    	COORD rdCrd;
    	for( rdCrd.Y = 0; rdCrd.Y<szY; ++rdCrd.Y )
    	{
        	rdCrd.X = 0;
            char *pBuff = &*lineBuff.begin()
            //, pBuffEnd = &*lineBuff.end()
            ;
        	::ReadConsoleOutputCharacter
        	(
    			hStdout,			// handle of a console screen buffer
    			pBuff,			    // address of buffer to receive characters
    			szX,				// number of character cells to read from
    			rdCrd,				// coordinates of first cell to read from
    			&numberOfCharsRead 	// address of number of cells read from
   			);
            char *lineBuffEnd = FindEndNotSpace()( lineBuff.begin(), lineBuff.end() );
            *lineBuffEnd = '\0';
            const unsigned cchDstLength = lineBuffEnd - &*lineBuff.begin();

            if( cchDstLength==0 ) continue;

            std::vector<char> translatedBuff( cchDstLength+1 );
            char *pTranslatedBuff = &*translatedBuff.begin();

            ::OemToCharBuff
            (
                pBuff,	                    // pointer to string to translate
                pTranslatedBuff,	        // pointer to buffer for translated string
                cchDstLength  	            // size of buffer
            );

        	logCout << pTranslatedBuff << std::endl;
    	}
    	logCout.close();
	}
//------------------------------------------------------------------------------
GUIConsole::GUIConsole() :
	isCreated_(false), isRedirected_(false)
{
    // проверка уникальности вызова функции
    ASSERT_FUNCTION_CALLED_ONCE__;
}
//------------------------------------------------------------------------------
void GUIConsole::InitializePaths(const AnsiString& logDir)
{
	static bool isInitialized = 0;
    if(isInitialized) return;
    //logDir_ =  GetAppDataDir()+"\\Log" ;
    logDir_ =  logDir;
    logFn_ = logDir_+"\\STDOUT.LOG";
    logArchiveFileName_ = "log.rar";
    MyForceDirectory(logDir_);
    isInitialized = 1;
}
//------------------------------------------------------------------------------
void GUIConsole::Create(const AnsiString& logDir)
{
    if( isCreated_ ) return;

    InitializePaths(logDir);

    ::AllocConsole();
    const HWND
        hWnd_ = ::GetConsoleWindow(),
        hMenu_ = ::GetSystemMenu(hWnd_, FALSE);
    // удалить меню "закрыть"
    ::DeleteMenu(hMenu_, SC_CLOSE, MF_BYCOMMAND);
    // скрыть окно консоли
    ::ShowWindow(hWnd_, SW_HIDE);
    ::SetConsoleCtrlHandler( HandlerRoutine, TRUE);

    InitializeGUIConsole();
    isCreated_ = true;
    isRedirected_ = false;

}
//------------------------------------------------------------------------------
void GUIConsole::Redirect(const AnsiString& logDir)
{
	InitializePaths(logDir);
    
    std::fclose(stdout);
    std::freopen(logFn_.c_str(), "w+", stdout);

    if(isCreated_)
    {
    	::FreeConsole();
        isCreated_ = false;
    }
    isRedirected_ = true;
    printf( "CON redirected to file.\n" );
}
//------------------------------------------------------------------------------
void GUIConsole::Backup()
{
	if( !(IsCreated() || IsRedirected() ) ) return;

    Word year, month, day, hour, minute, second, milliSecond;
    DecodeDateTime( Now(), year, month, day, hour, minute, second, milliSecond);
    const AnsiString
        logDir = MYSPRINTF_( "%s\\Log\\%d\\%s\\", logDir_, year, MyFormatMonth(month) ),
        newFn = MYSPRINTF_( "%s%d_%d_%d_%d_%d.log", logDir,  day, hour, minute, second, milliSecond );
    ForceDirectories(logDir);

    // сохранить содержимое консоли
    if( IsRedirected() )
    {
        std::fclose(stdout);
        RenameFile(logFn_, newFn );
        Redirect(logDir_);
    }
}
//------------------------------------------------------------------------------
void SaveGUIConsoleToFile()
{
    GUIConsole::Instance().Backup();
}
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
void MyWriteConsole(const wchar_t* str, DWORD nNumberOfCharsToWrite)
{
    DWORD lpNumberOfCharsWritten  = 0;
    ::WriteConsoleW( ::GetStdHandle(STD_OUTPUT_HANDLE), (const void*) str,
    	nNumberOfCharsToWrite, &lpNumberOfCharsWritten, NULL);
}
//------------------------------------------------------------------------------
void MyWCout(const AnsiString& str)
{
	GUIConsole& inst =  GUIConsole::Instance();
    if( inst.IsRedirected() )
        std::cout << str.c_str();
    else if( inst.IsCreated() )
    {
        WideString s = str;
        const DWORD nNumberOfCharsToWrite = s.Length();
        MyWriteConsole( s.c_bstr(), nNumberOfCharsToWrite );
    }
}
//------------------------------------------------------------------------------
void MyWCoutColored(const AnsiString& str, IncExcAttr inc_exc )
{
    const ConsoleScreenAtrributesHolder obj(inc_exc);
    MyWCout( str );
}
//------------------------------------------------------------------------------
void MyWCoutTime(const AnsiString& str, IncExcAttr inc_exc  )
{
    const ConsoleScreenAtrributesHolder obj(inc_exc);
    MYWCOUT_( "<%s> %s\n", TimeToStr( Time() ), str ));
}
//------------------------------------------------------------------------------
TemporaryRedirectConsole::TemporaryRedirectConsole( const AnsiString& fn) //: fn_(fn)
{
    std::freopen(fn.c_str(), "w+", stdout);
}
TemporaryRedirectConsole::~TemporaryRedirectConsole()
{
    std::freopen("CONOUT$", "w", stdout);

    /*
    // извлечь содержимое входного файла в буффер buff
	std::ifstream ifstrm( fn_.c_str() );
    AutoCloseFileStreamHelper<std::ifstream> acf(ifstrm);
    std::string s;
    while( std::getline(ifstrm, s) )
        MyWCout( AnsiString(s.c_str())+"\n"  );
    ifstrm.close();
    */
}
//------------------------------------------------------------------------------
ConsoleScreenAtrributesHolder::ConsoleScreenAtrributesHolder(IncExcAttr inc_exc)
{
    HANDLE hConsoleOutput = ::GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi = {0};
    ::GetConsoleScreenBufferInfo( hConsoleOutput,  &csbi );
    attrs_ = csbi.wAttributes;
    ::SetConsoleTextAttribute(hConsoleOutput, (attrs_ | inc_exc.first ) & ~inc_exc.second );
}
//------------------------------------------------------------------------------
ConsoleScreenAtrributesHolder::~ConsoleScreenAtrributesHolder()
{
    ::SetConsoleTextAttribute( ::GetStdHandle(STD_OUTPUT_HANDLE), attrs_ );
}
//------------------------------------------------------------------------------
ConsoleMultioutputHelper::ConsoleMultioutputHelper()
{
    CONSOLE_SCREEN_BUFFER_INFO lConsoleScreenBufferInfo_ = {0};
    hCon_ = ::GetStdHandle(STD_OUTPUT_HANDLE);
    ::GetConsoleScreenBufferInfo( hCon_, &lConsoleScreenBufferInfo_ );
    pos0_ = lConsoleScreenBufferInfo_.dwCursorPosition;
    curWriteConsoleCount_ = 0;
}
//------------------------------------------------------------------------------
void ConsoleMultioutputHelper::Set(const AnsiString& s)
{
    CONSOLE_SCREEN_BUFFER_INFO lConsoleScreenBufferInfo_ = {0};
    ::GetConsoleScreenBufferInfo( hCon_, &lConsoleScreenBufferInfo_ );
    const COORD pos = lConsoleScreenBufferInfo_.dwCursorPosition;

    if( curWriteConsoleCount_!=0 )
    {
        ::SetConsoleCursorPosition( hCon_, pos0_);
        MyWCout( AnsiString::StringOfChar(' ', curWriteConsoleCount_) );
        ::SetConsoleCursorPosition( hCon_, pos0_);
    }
    MyWCout(s);
    curWriteConsoleCount_ = s.Length();
    ::SetConsoleCursorPosition( hCon_, pos);
}
//------------------------------------------------------------------------------
    MyConQuote::MyConQuote( const AnsiString& s ) :  s_(s)
    {
    	MyWCout( "<" + s_ + ">\n");

    }
    MyConQuote::~MyConQuote()
    {
    	MyWCout( "<\\" + s_ + ">\n");
    }

    #include <forms.hpp>
    GUIStdInputHelper::GUIStdInputHelper() :
    	hWndMainForm_( Application->MainForm->Handle ),
        hWndConsole_(GetConsoleWindow()),
        hWndConsoleParent_( ::SetParent( hWndConsole_, NULL ) )
    {
    	std::freopen("CONIN$", "r", stdin);
    	ShowWindow(hWndMainForm_, SW_HIDE );
    	ShowWindow(hWndConsole_, SW_SHOWNORMAL | SW_MAXIMIZE );
    }
    GUIStdInputHelper::~GUIStdInputHelper()
    {
        ::SetParent( hWndConsole_, hWndConsoleParent_ );
    	ShowWindow(hWndMainForm_, SW_SHOW );
    	ShowWindow(hWndConsole_, SW_HIDE );
    }
