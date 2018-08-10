/*
Мышка входит и выходит
*/

//---------------------------------------------------------------------------
#ifndef MouseEvtH
#define MouseEvtH
//---------------------------------------------------------------------------



// loki
#include "Loki\functor.h"

// vcl
#include <Controls.hpp>

namespace my
{
    // мышь вошла - true мышь ушла - false
	enum { iMouseLeaved, iMouseEntered };
    enum { replaceOldHandler = true };

	typedef Loki::Functor<void, TYPELIST_2( TWinControl*, bool ) > MouseOccurrenceHandler;


	void RegisterOnMouseOccurrence( TWinControl* ctlr, MouseOccurrenceHandler cmd,
    	bool haveToRplcOldHndlr = replaceOldHandler);


};

//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
