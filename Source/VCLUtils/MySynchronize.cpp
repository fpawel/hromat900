//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
#pragma hdrstop
//------------------------------------------------------------------------------
// boost
#include "boost\noncopyable.hpp"
#include "boost\shared_ptr.hpp"

// my
#include "MySynchronize.h"
#include "MyWindow.h"
#include "MyAppDef.hpp"
#include "..\my_include\MyNoDestroySingleton.hpp"
#include "guicon.h"
#include "MyExcpt.hpp"

//------------------------------------------------------------------------------

#pragma package(smart_init)

#define MY_SYNCRONIZE_THREAD_WINDOW_CLASS_NAME "My synchronize thread window class name FB448FFB-619B-4A8F-BB25-DF803E874B0B"
#define MY_SYNCRONIZE_THREAD_WINDOW_NAME "My synchronize thread window name 5C47AA51-219B-4F54-AA56-8AD8DF2D258A"

//------------------------------------------------------------------------------
union Closure_
{
	MyWindowMessageMethod mthd_;
    struct
    {
    	WPARAM w_;
        LPARAM l_;
    } as_WPARAM_LPARAM_;
};
//------------------------------------------------------------------------------
Closure_ MakeClosure_As_Mthd(MyWindowMessageMethod mthd)
{
        Closure_ ret;
        ret.mthd_ = mthd;
        return ret;
}
//------------------------------------------------------------------------------
Closure_ MakeClosure_As_WPARAM_LPARAM_(WPARAM w, LPARAM l)
{
        Closure_ ret;
        ret.as_WPARAM_LPARAM_.w_ = w;
        ret.as_WPARAM_LPARAM_.l_ = l;
        return ret;
}
//------------------------------------------------------------------------------

class ModuleImpl : public boost::noncopyable
{
public:
	explicit ModuleImpl();
    void Cout( const AnsiString& s );
    LRESULT CallMthd( MyWindowMessageMethod ) const;
    void PostCallClosure( MyWindowMessageMethod ) const;

    bool IsMainThread() const{ return threadId_== GetCurrentThreadId(); }
private:

    const DWORD threadId_;
    boost::shared_ptr<void> hWnd_;	// оконный объект
    // оконная продцедура
    LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, bool& isHandled);

    enum{ CALL_MTHD_MSG_CODE_		= WM_USER + 1000 };
};
//------------------------------------------------------------------------------
ModuleImpl::ModuleImpl():
    threadId_( GetCurrentThreadId() ),
    hWnd_( MyAllocateWindow( MY_SYNCRONIZE_THREAD_WINDOW_CLASS_NAME,
    	MY_SYNCRONIZE_THREAD_WINDOW_NAME, WndProc ), ::CloseWindow )
{
}
//------------------------------------------------------------------------------
LRESULT ModuleImpl::CallMthd( MyWindowMessageMethod mthd ) const
{
    if( threadId_ == GetCurrentThreadId() )
        return mthd();
    const Closure_ closure_ = MakeClosure_As_Mthd(mthd);
    return ::SendMessage(hWnd_.get() , CALL_MTHD_MSG_CODE_,
        closure_.as_WPARAM_LPARAM_.w_ ,
        closure_.as_WPARAM_LPARAM_.l_ );
}
//------------------------------------------------------------------------------
void ModuleImpl::PostCallClosure( MyWindowMessageMethod mthd ) const
{
    const Closure_ closure_ = MakeClosure_As_Mthd(mthd);
    ::PostMessage(hWnd_.get() , CALL_MTHD_MSG_CODE_,
        closure_.as_WPARAM_LPARAM_.w_ ,
        closure_.as_WPARAM_LPARAM_.l_ );

}
//------------------------------------------------------------------------------
LRESULT ModuleImpl::WndProc(HWND hWnd, UINT msg, WPARAM wParam,
	LPARAM lParam, bool& isHandled)
{
    try
    {
		assert(hWnd==hWnd_.get() );
    	if( msg==CALL_MTHD_MSG_CODE_ )
    	{
        	isHandled = true;
        	return MakeClosure_As_WPARAM_LPARAM_(wParam, lParam).mthd_();
    	}
    }
    catch(...)
    {
    	const AnsiString msg = AnsiString().sprintf( "Необработанное исключение в %s!\n%s",
        __FUNC__,
            FormatMyException( MY_RETHROW_.get(), "%eLine:%eFile %eMsg\n\t" )
            );
    	::MessageBox(NULL, msg.c_str(), __FUNC__, MB_OK | MB_APPLMODAL | MB_ICONERROR );
        std::exit(EXIT_FAILURE);
    }
    return 0;
}
//------------------------------------------------------------------------------
struct ModuleTagT {};
typedef MyNoDestroySingletonHolder<ModuleImpl, ModuleTagT> Module;
//------------------------------------------------------------------------------


namespace my
{
//------------------------------------------------------------------------------
LRESULT SynchronizeMthd( MyWindowMessageMethod mthd)
{
    return Module::Instance().CallMthd( mthd );
}
//------------------------------------------------------------------------------
void FixMainThread()
{
    Module::Instance();
}

void PostCallClosure( MyWindowMessageMethod mthd )
{
    return Module::Instance().PostCallClosure( mthd );
}

bool IsMainThread() { return Module::Instance().IsMainThread(); }

};//namespace my
