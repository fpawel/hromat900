//---------------------------------------------------------------------------
#include <sstream>
#include <map>

//---------------------------------------------------------------------------
#pragma hdrstop
//---------------------------------------------------------------------------

// boost
#include "boost\tuple\tuple.hpp"

// loki
#include "loki\singleton.h"
//---------------------------------------------------------------------------

#include "MouseEvt.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

namespace my
{

    struct Dt
    {
    	TWinControl* ctrl_;			// VCL компонент
        MouseOccurrenceHandler onOccurrence_; // обработчик
        bool entered_;              // находится ли мышь в компоненте
        WNDPROC oldWndProc_;		// замещённая оконная продцедура
        TRACKMOUSEEVENT tme_;       //
    };

    typedef std::map<HWND,Dt> Assoc;
	struct MouseInfoT
    {
        Assoc assoc_;
    };

    typedef Loki::SingletonHolder
    	<
    		MouseInfoT,
        	Loki::CreateUsingNew,
    		Loki::NoDestroy
    	> MouseInfo;

LRESULT CALLBACK WindowProcWithMouseOccurrenceHandler(HWND hWnd, UINT msg,
	WPARAM w, LPARAM l)
{   
	Assoc& assoc = MouseInfo::Instance().assoc_;
    Assoc::iterator itr = assoc.find(hWnd);
    assert( itr!=assoc.end() );
    Dt &dt = itr->second;
    if (msg == WM_MOUSELEAVE  )
  	{
        dt.entered_ = false;
        dt.onOccurrence_(dt.ctrl_, iMouseLeaved);
  	} else
    if( msg == WM_MOUSEMOVE )
    {
    	if(!dt.entered_)
        {
    		dt.tme_.dwFlags = TME_LEAVE;
    		TrackMouseEvent(&dt.tme_);
            dt.entered_ = true;
            dt.onOccurrence_(dt.ctrl_, iMouseEntered);
        }
  	};
  	return CallWindowProc((FARPROC)dt.oldWndProc_, hWnd, msg, w, l);
}

struct ChainMouseOccurrenceHandlers
{
	const MouseOccurrenceHandler cmd1_, cmd2_;
	explicit ChainMouseOccurrenceHandlers(MouseOccurrenceHandler cmd1,
    MouseOccurrenceHandler cmd2) : cmd1_(cmd1), cmd2_(cmd2) {}
	void operator()(TWinControl* ctrl, bool isMouseEntered ) const
    {
    	#pragma warn -8037
    	cmd1_(ctrl,isMouseEntered);
        cmd2_(ctrl,isMouseEntered);
        #pragma warn +8037
    }
};

void RegisterOnMouseOccurrence( TWinControl* ctrl, MouseOccurrenceHandler cmd,
	bool haveToRplcOldHndlr)
{
	Assoc& assoc = MouseInfo::Instance().assoc_;
    Assoc::iterator itr = assoc.find(ctrl->Handle);
    const bool findAssoc = (itr!=assoc.end());
    Dt& dt = !findAssoc ? assoc[ctrl->Handle] : itr->second;

    if( findAssoc )
    {
    	if( haveToRplcOldHndlr )
            dt.onOccurrence_ = cmd;
        else
        	dt.onOccurrence_ = ChainMouseOccurrenceHandlers(dt.onOccurrence_,cmd);
    	return;
    }
    else
    	dt.onOccurrence_ = cmd;        
    dt.ctrl_ = ctrl;
    dt.entered_ = false;
    dt.oldWndProc_ = (WNDPROC)SetWindowLong(ctrl->Handle,
    	GWL_WNDPROC, (long)WindowProcWithMouseOccurrenceHandler);
    dt.tme_.cbSize = sizeof(TRACKMOUSEEVENT);
    dt.tme_.hwndTrack = ctrl->Handle;
    dt.tme_.dwHoverTime = HOVER_DEFAULT;
}


}; // namespace my
