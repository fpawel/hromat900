//------------------------------------------------------------------------------
#ifndef TheadWindowH
#define TheadWindowH
//------------------------------------------------------------------------------
//std
#include <vector>
// boost
#include "boost\noncopyable.hpp"
#include "boost\shared_ptr.hpp"
// windows
#include <basetsd.h>
// my
#include "..\my_include\free_types_.h"

class TransferThreadWindow : public boost::noncopyable
{
    // функтор обработки событий оконной продцедуры
    typedef LRESULT (__closure *MyWindowMessageHandler )();
public:
	explicit TransferThreadWindow();
    void Cout( const AnsiString& s );
    LRESULT Call( MyWindowMessageHandler mthd );
    LRESULT Call( TVoidMthd mthd );
    LRESULT Call( TVoid_IntStrMthd mthd, unsigned context, const AnsiString& s);

private:
	
    const DWORD threadId_;
    boost::shared_ptr<void> hWnd_;	// оконный объект
    // оконная продцедура
    LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, bool& isHandled);
    LRESULT HandleCallMethod(WPARAM pMethod, LPARAM param) const;

    enum{ CALL_MTHD_MSG_CODE_		= WM_USER + 1000 };
    union Closure_
    {
        MyWindowMessageHandler mthd_;
        struct
        {
            WPARAM w_;
            LPARAM l_;
        } as_WPARAM_LPARAM_;
    };
    Closure_ MakeClosure_As_Mthd(MyWindowMessageHandler mthd)
    {
        Closure_ ret;
        ret.mthd_ = mthd;
        return ret;
    }
    Closure_ MakeClosure_As_WPARAM_LPARAM_(WPARAM w, LPARAM l)
    {
        Closure_ ret;
        ret.as_WPARAM_LPARAM_.w_ = w;
        ret.as_WPARAM_LPARAM_.l_ = l;
        return ret;
    }
};



//------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------
