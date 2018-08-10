#include <iostream>
//---------------------------------------------------------------------------
#pragma hdrstop

#include "MyShllFldr.h"

//---------------------------------------------------------------------------

#include "MyIostream.h"

using std::cout;
void OutLine()
{ cout << '\n' << myIO::LongLine(75, '-') << '\n'; }

void MyWriteConsole(const wchar_t* str, DWORD nNumberOfCharsToWrite)
{
    DWORD lpNumberOfCharsWritten  = 0;
    ::WriteConsoleW( ::GetStdHandle(STD_OUTPUT_HANDLE), (const void*) str,
    	nNumberOfCharsToWrite, &lpNumberOfCharsWritten, NULL);
}

void MyWCout(const AnsiString& str)
{
    const WideString s = str;
    const DWORD nNumberOfCharsToWrite = s.Length();
    MyWriteConsole(s.c_bstr(), nNumberOfCharsToWrite);
}

#pragma argsused
int main(int argc, char* argv[])
{
	typedef struct ItemT
    {
    	explicit ItemT( unsigned nn, const AnsiString ss1, const AnsiString ss2 ) :
        	n(nn), s1(ss1), s2(ss2)
        {}
    	const unsigned n;
    	const AnsiString s1, s2;
    } Item;

    #define DECL_(cOde, s) Item( ShllFldr::##cOde##,#cOde, s  )

    const Item items[56] =
    {
    	DECL_(DESKTOP, 		"<desktop>"),
		DECL_(INTERNET, 	"Internet Explorer (icon on desktop)"),
		DECL_(PROGRAMS,        	"Start Menu\\Programs"),
		DECL_(CONTROLS,        	"My Computer\\Control Panel"),
		DECL_(PRINTERS,        	"My Computer\\Printers"),
		DECL_(PERSONAL,        	"My Documents"),
		DECL_(FAVORITES,        	"<user name>\\Favorites"),
		DECL_(STARTUP,        	"Start Menu\\Programs\\Startup"),
		DECL_(RECENT,        		"<user name>\\Recent"),
		DECL_(SENDTO,        		"<user name>\\SendTo"),
		DECL_(BITBUCKET,        	"<desktop>\\Recycle Bin"),
		DECL_(STARTMENU,        	"<user name>\\Start Menu"),
		DECL_(MYDOCUMENTS,        "logical \"My Documents\" desktop icon"),
		DECL_(MYMUSIC,        	"\"My Music\" folder"),
		DECL_(MYVIDEO,        	"\"My Videos\" folder"),
		DECL_(DESKTOPDIRECTORY,   "<user name>\\Desktop"),
		DECL_(DRIVES,        		"My Computer"),
		DECL_(NETWORK,        	"Network Neighborhood (My Network Places)"),
		DECL_(NETHOOD,        	"<user name>\\nethood"),
		DECL_(FONTS,        		"windows\\fonts"),
		DECL_(TEMPLATES, " "),
		DECL_(COMMON_STARTMENU,   "All Users\\Start Menu"),
		DECL_(COMMON_PROGRAMS,    "All Users\\Start Menu\\Programs"),
		DECL_(COMMON_STARTUP,     "All Users\\Startup"),
		DECL_(COMMON_DESKTOPDIRECTORY,        "All Users\\Desktop"),
		DECL_(APPDATA,        		"<user name>\\Application Data"),
		DECL_(PRINTHOOD,        		"<user name>\\PrintHood"),


		DECL_(LOCAL_APPDATA,        	"<user name>\\Local Settings\\Applicaiton Data (non roaming)"),


		DECL_(ALTSTARTUP,        		"non localized startup"),
		DECL_(COMMON_ALTSTARTUP,      "non localized common startup"),
		DECL_(COMMON_FAVORITES, ""),


		DECL_(INTERNET_CACHE, " "),
		DECL_( COOKIES, " "),
		DECL_(HISTORY, " "),
		DECL_(COMMON_APPDATA,        	"All Users\\Application Data"),
		DECL_(WINDOWS,        		"GetWindowsDirectory()"),
		DECL_(SYSTEM,        			"GetSystemDirectory()"),
		DECL_(PROGRAM_FILES,        	"C:\\Program Files"),
		DECL_(MYPICTURES,        		"C:\\Program Files\\My Pictures"),


		DECL_(PROFILE,        		"USERPROFILE"),
		DECL_(SYSTEMX86,        		"x86 system directory on RISC"),
		DECL_(PROGRAM_FILESX86,       "x86 C:\\Program Files on RISC"),


		DECL_(PROGRAM_FILES_COMMON,   "C:\\Program Files\\Common"),


		DECL_(PROGRAM_FILES_COMMONX86,	"x86 Program Files\\Common on RISC"),
		DECL_(COMMON_TEMPLATES,        	"All Users\\Templates"),


		DECL_(COMMON_DOCUMENTS,        	"All Users\\Documents"),
		DECL_(COMMON_ADMINTOOLS,        	"All Users\\Start Menu\\Programs\\Administrative Tools"),
		DECL_(ADMINTOOLS,        			"<user name>\\Start Menu\\Programs\\Administrative Tools"),


		DECL_(CONNECTIONS,        	"Network and Dial-up Connections"),
		DECL_(COMMON_MUSIC,        	"All Users\\My Music"),
		DECL_(COMMON_PICTURES,        "All Users\\My Pictures"),
		DECL_(COMMON_VIDEO,        	"All Users\\My Video"),
		DECL_(RESOURCES,        		"Resource Direcotry"),


		DECL_(RESOURCES_LOCALIZED,    "Localized Resource Direcotry"),


		DECL_(COMMON_OEM_LINKS,       "Links to All Users OEM specific apps"),
		DECL_(CDBURN_AREA,        	   "USERPROFILE\\Local Settings\\Application Data\\Microsoft\\CD Burning")
        #undef DECL_
    };

    OutLine();


    
    for( unsigned i=0; i<56; ++i )
    {
    	const Item& itm = items[i];
    	cout << i+1 << ".\t" << itm.s1.c_str() << " - " << itm.s2.c_str() << "\n";
        MyWCout( "\""+ShllFldr::Path(itm.n)+"\"" );
        OutLine();
    }

	std::system("PAUSE");
	return 0;
}
//---------------------------------------------------------------------------
 