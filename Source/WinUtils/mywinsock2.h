//---------------------------------------------------------------------------
#ifndef mywinsock2H
#define mywinsock2H
//---------------------------------------------------------------------------
//#include <ScktComp.hpp>
#include <basetsd.h>

//std

// boost
#include "boost\noncopyable.hpp"
#include "boost\shared_ptr.hpp"



// my
#include "..\my_include\myport.hpp"

namespace my
{
    class Thread;
};

namespace Private
{
    class WSAInitialize : public boost::noncopyable
    {
    public:
        explicit WSAInitialize();
    };
	// для инициализации ядра std::ios_base
	const WSAInitialize wsaInitialize;
};


class VCLClientSocketHlpr : public MyPort
{

public:
    explicit VCLClientSocketHlpr();
    virtual ~VCLClientSocketHlpr() {}

    virtual void Connect();
    virtual void Disconnect();
    virtual bool IsConnected() const;
    virtual bool ConnectionWasBreaked() const { return connectionWasBreaked_; }


    virtual AnsiString Description() const;
    virtual int Write( void* pBuff, int count );
    virtual int Read(  void* pBuff, int count );
    virtual int GetRxDSize();
    virtual void Purge();

    void SetPortNum(int port);
    int GetPortNum() const;
    
    void SetHost(const AnsiString&);
    AnsiString GetHost() const;

    void SetLocalAdr(const AnsiString&);
    AnsiString GetLocalAddr() const;

    void BreakConnection();

private:
    int port_;
    AnsiString host_, localAddr_;
    sockaddr_in servAddr_, cliAddr_;
    bool connected_;

    UINT_PTR sckt_;

    boost::shared_ptr<my::Thread> connectThread_;
    mutable unsigned lastError_;
    bool connectionWasBreaked_;

    void __fastcall ConnectThreadProcedure();

    void MakeInetAddrs();
    void LogInfo();
};

bool IsInetAddr(const AnsiString& s);


//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
