
//---------------------------------------------------------------------------
#pragma hdrstop
//---------------------------------------------------------------------------
#include <winsock2.h>
#include "mywinsock2.h"
//---------------------------------------------------------------------------

#include <inifiles.hpp>
//---------------------------------------------------------------------------
#include "MyExcpt.hpp"
#include "MywinExcpt.hpp"
#include "guicon.h"
#include "AnsiStringUtils_.h"
#include "..\my_include\free_types_.h"
#include "myThread.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
#define MY_SAFE_CALL_WINSOCK2(foo,params)\
		if(::##foo##params ) throw MyExcptionFactory::Create<MyWindowsException>\
		(WSAGetLastError(), __FILE_LINE__,"::"#foo);
//------------------------------------------------------------------------------
namespace Private
{
    WSAInitialize::WSAInitialize()
    {
        static bool called = 0;
        if(called) return;
        WSADATA wsaData = {0};
        if( WSAStartup( MAKEWORD(2,2), &wsaData ) )
        {
    	    ::MessageBox(NULL, "Ошибка инициализации ядра \"WINSOCK2\"!",
    	    "Последняя WIN-ошибка.", MB_OK | MB_APPLMODAL | MB_ICONERROR );
            std::exit( EXIT_FAILURE );
        }
        called = 1;
    }
};
//------------------------------------------------------------------------------
bool IsInetAddr(const AnsiString& s)
{
    return ::inet_addr ( s.c_str() )!=INADDR_NONE;
}
//------------------------------------------------------------------------------
AnsiString GetIpByName( const char *szUserName )
{
    hostent * record = ::gethostbyname(szUserName);
    if(record == NULL)
        throw MyExcptionFactory::Create<MyWindowsException>
            (::WSAGetLastError(), __FILE_LINE__,"::gethostbyname");
    in_addr * address = (in_addr * )record->h_addr;
    return  inet_ntoa(* address);
}
//------------------------------------------------------------------------------
AnsiString GetCurrentUserIp()
{
    char szUserName[1001];
    ::gethostname( szUserName, 1000);
    return GetIpByName(szUserName);
}
//------------------------------------------------------------------------------




//---------------------------------------------------------------------------
VCLClientSocketHlpr::VCLClientSocketHlpr() :
    sckt_( INVALID_SOCKET ),
    port_(502), host_("192.168.1.5"), connected_(false), lastError_(0)
{
    WSADATA wsaData = {0};
    if( WSAStartup( MAKEWORD(2,2), &wsaData ) )
    {
    	::MessageBox(NULL, "Ошибка инициализации ядра \"WINSOCK2\"!",
    	"Последняя WIN-ошибка.", MB_OK | MB_APPLMODAL | MB_ICONERROR );
        std::exit( EXIT_FAILURE );
    }
    MakeInetAddrs();
}
//------------------------------------------------------------------------------
void VCLClientSocketHlpr::SetPortNum(int port)
{
    assert(!connected_);
    port_ = port;
}
//------------------------------------------------------------------------------
int VCLClientSocketHlpr::GetPortNum() const
{
    return port_;
}
//------------------------------------------------------------------------------
void VCLClientSocketHlpr::SetHost(const AnsiString& host)
{
    assert(!connected_);
    host_ = host;
}
//------------------------------------------------------------------------------
AnsiString VCLClientSocketHlpr::GetHost() const
{
    return host_;
}
//------------------------------------------------------------------------------

void VCLClientSocketHlpr::SetLocalAdr(const AnsiString& v)
{
    localAddr_ = v;
}
AnsiString VCLClientSocketHlpr::GetLocalAddr() const
{
    return localAddr_;
}

void VCLClientSocketHlpr::MakeInetAddrs()
{
    servAddr_.sin_family = AF_INET;
    servAddr_.sin_port = htons(port_);
    servAddr_.sin_addr.s_addr = ::inet_addr( host_.c_str() );

    cliAddr_.sin_family = AF_INET;
    cliAddr_.sin_port = 0;

    AnsiString currentIP = Trim(localAddr_);
    if (currentIP == "") {
        currentIP = GetCurrentUserIp();
    }

    //const AnsiString currentIP = host_;

    cliAddr_.sin_addr.s_addr = ::inet_addr( currentIP.c_str() );

}
//------------------------------------------------------------------------------
AnsiString VCLClientSocketHlpr::Description() const
{
	return AnsiString().sprintf( "host=%s port=%d", host_, port_);
}
//------------------------------------------------------------------------------
void __fastcall VCLClientSocketHlpr::ConnectThreadProcedure()
{
    connectionWasBreaked_ = 0;
    lastError_ = 0;
    if( ::connect(sckt_, (LPSOCKADDR)&servAddr_, sizeof(servAddr_))==SOCKET_ERROR )
    {
        unsigned lasError = WSAGetLastError();
        lastError_= WSAGetLastError();
        Disconnect();
    }

}
//------------------------------------------------------------------------------
void VCLClientSocketHlpr::Connect()
{
    if(connected_) return;
    BreakConnection();

    if(sckt_!=INVALID_SOCKET)
        MY_SAFE_CALL_WINSOCK2( closesocket, (sckt_) );
    MakeInetAddrs();
    sckt_ = ::socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if( sckt_==INVALID_SOCKET )
    throw MyExcptionFactory::Create<MyWindowsException>(WSAGetLastError(), __FILE_LINE__,
        "Can`t connect TCP!" );
    MY_SAFE_CALL_WINSOCK2( bind,    (sckt_, (LPSOCKADDR) &cliAddr_,     sizeof(cliAddr_) ) );

    //MY_SAFE_CALL_WINSOCK2( connect, (sckt_, (LPSOCKADDR) &servAddr_,    sizeof(servAddr_) ) );

    lastError_=0;
    connectThread_.reset( new my::Thread( &ConnectThreadProcedure ) );
    connectThread_->Run();
    connectThread_->WaitFor();
    if( connectionWasBreaked_ ) return;

    if( lastError_!=0 )
    {
        throw MyExcptionFactory::Create<MyWindowsException>(
            lastError_, __FILE_LINE__,"Ошибка TCP ::connect");
    }


    //MY_SAFE_CALL_WINSOCK2( connect, (sckt_, (LPSOCKADDR)&servAddr_, sizeof(servAddr_) ) );

    enum { MY_INET_BUFF_SIZE = MAXLONG };
    int
        ethernetBuffSize = MY_INET_BUFF_SIZE,
        sizOfEthernetBuffSize  = sizeof(ethernetBuffSize);
    char *pEthernetBuffSize = (char*) &ethernetBuffSize;

    MY_SAFE_CALL_WINSOCK2( setsockopt, ( sckt_, SOL_SOCKET, SO_RCVBUF,
        pEthernetBuffSize, sizOfEthernetBuffSize ) );

    MY_SAFE_CALL_WINSOCK2( setsockopt, ( sckt_, SOL_SOCKET, SO_SNDBUF,
        pEthernetBuffSize, sizOfEthernetBuffSize ) );

    ethernetBuffSize = 0;
    sizOfEthernetBuffSize  = sizeof(ethernetBuffSize);
    MY_SAFE_CALL_WINSOCK2( getsockopt, ( sckt_, SOL_SOCKET, SO_RCVBUF,
        pEthernetBuffSize, &sizOfEthernetBuffSize ) );
    if( ethernetBuffSize!=MY_INET_BUFF_SIZE )
        throw MyExcptionFactory::Create<MyException>( __FILE_LINE__,
        MYSPRINTF_("Не установлен размер буфера SO_RCVBUF - %d ", ethernetBuffSize ));
    MyWCout( MYSPRINTF_("::setsockopt(... SO_RCVBUF, %d, ...)\n", ethernetBuffSize) );

    ethernetBuffSize = 0;
    sizOfEthernetBuffSize  = sizeof(ethernetBuffSize);
    MY_SAFE_CALL_WINSOCK2( getsockopt, ( sckt_, SOL_SOCKET, SO_SNDBUF,
        pEthernetBuffSize, &sizOfEthernetBuffSize ) );
    if( ethernetBuffSize!=MY_INET_BUFF_SIZE )
        throw MyExcptionFactory::Create<MyException>( __FILE_LINE__,
        MYSPRINTF_("Не установлен размер буфера SO_SNDBUF - %d ", ethernetBuffSize ));
    MyWCout( MYSPRINTF_("::setsockopt(... SO_SNDBUF, %d, ...)\n", ethernetBuffSize) );

    connected_ = true;

}
//------------------------------------------------------------------------------
void VCLClientSocketHlpr::BreakConnection()
{
    if( !connectThread_) return;
    connectionWasBreaked_ = 1;
    connectThread_->Terminate();
    connectThread_.reset();
}
void VCLClientSocketHlpr::Disconnect()
{
    
    if(!connected_) return;
    MY_SAFE_CALL_WINSOCK2( closesocket, (sckt_) );
    connected_ = false;
    sckt_ = INVALID_SOCKET;

    //my::Synchronize1( &SetConnection, false);
}
//------------------------------------------------------------------------------
bool VCLClientSocketHlpr::IsConnected() const
{
	return connected_;
}
//------------------------------------------------------------------------------
void VCLClientSocketHlpr::Purge()
{
    if( !connected_ ) return;

    const unsigned sz = this->GetRxDSize();
    if(sz==0) return;
    VInt8 buff( sz );
    this->Read(  (void*) &buff.at(0), sz );
    
    Disconnect();
    Connect();
}
//------------------------------------------------------------------------------
// Получить количество байтов в приёмнике.
int VCLClientSocketHlpr::GetRxDSize()
{
    u_long ret = 0;
    MY_SAFE_CALL_WINSOCK2( ioctlsocket, (sckt_, FIONREAD, &ret ) );
    return ret;
}
//------------------------------------------------------------------------------
int VCLClientSocketHlpr::Write( void* pBuff, int count )
{
    const int ret = ::send(sckt_, (char*)pBuff, count, 0);
    if(ret!=count)
    throw MyExcptionFactory::Create<MyWindowsException>(WSAGetLastError(), __FILE_LINE__,
        AnsiString().sprintf("Can`t write TCP: %d bytes writen from %d!", ret, count).c_str() );
    return ret;
}
//------------------------------------------------------------------------------
int VCLClientSocketHlpr::Read(  void* pBuff, int count )
{
    count = std::min( count, GetRxDSize() );
    return ::recv(sckt_, (char*)pBuff, count, 0 );

}
//------------------------------------------------------------------------------



