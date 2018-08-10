//------------------------------------------------------------------------------
#include "hromat900_pch.h"
//------------------------------------------------------------------------------
#pragma hdrstop
//------------------------------------------------------------------------------
// my
#include "TheadWindow.h"
#include "MyWindow.h"
#include "MyIostream.h"
#include "console_.h"
#include "..\VCLUtils\VCLSignal.h"
//------------------------------------------------------------------------------

#pragma package(smart_init)

TransferThreadWindow::TransferThreadWindow():
    threadId_( GetCurrentThreadId() ),
    hWnd_( MyAllocateWindow( "My transfer manager window class name",
    	"My transfer manager window name", WndProc ), ::CloseWindow )
{}
//------------------------------------------------------------------------------
LRESULT TransferThreadWindow::Call( MyWindowMessageHandler mthd )
{
    if( threadId_ == GetCurrentThreadId() )
        return mthd();

    const Closure_ closure_ = MakeClosure_As_Mthd(mthd);
    return SendMessage(hWnd_.get() , CALL_MTHD_MSG_CODE_,
        closure_.as_WPARAM_LPARAM_.w_ ,
        closure_.as_WPARAM_LPARAM_.l_ );
}
//------------------------------------------------------------------------------
LRESULT TransferThreadWindow::Call( TVoidMthd mthd )
{   
	struct Foo
    {
        Foo(TVoidMthd mthd) : mthd_(mthd) {}
        const TVoidMthd mthd_;
        LRESULT Call() { mthd_(); return 0; }
    };
    const Foo foo(mthd);
    return Call( &foo.Call );
}
//------------------------------------------------------------------------------
LRESULT TransferThreadWindow::Call( TVoid_IntStrMthd mthd, unsigned context,
	const AnsiString& s )
{
	struct Foo
    {
        Foo(TVoid_IntStrMthd mthd, unsigned context, const AnsiString& s  ) :
            mthd_(mthd), context_(context), s_(s)
        {
        }
    	TVoid_IntStrMthd mthd_;
        unsigned context_;
        AnsiString s_;
        LRESULT Call()
        {
        	mthd_(context_,s_);
            return 0;
        }
    };
    const  Foo foo( mthd, context, s );
	return Call( &foo.Call );
}
//------------------------------------------------------------------------------
void TransferThreadWindow::Cout( const AnsiString& s )
{
    struct SyncCout
    {
        const AnsiString& s_;
        SyncCout(const AnsiString& s) : s_(s) {}
        LRESULT Do() const
        {
		    std::cout << s_;
            std::cout.flush();
    	    return 0;
        }
    };
    const SyncCout syncCout(s);
    Call( &syncCout.Do );
}
//------------------------------------------------------------------------------
// оконная продцедура
LRESULT TransferThreadWindow::WndProc(HWND hWnd, UINT msg, WPARAM wParam,
	LPARAM lParam, bool& isHandled)
{
	assert(hWnd==hWnd_.get() );
    if( msg==CALL_MTHD_MSG_CODE_ )
    {
        isHandled = true;
        return MakeClosure_As_WPARAM_LPARAM_(wParam, lParam).mthd_();
    }
  	return 0;
}
//------------------------------------------------------------------------------
