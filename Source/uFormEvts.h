//---------------------------------------------------------------------------

#ifndef uFormEvtsH
#define uFormEvtsH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Grids.hpp>
#include <ComCtrls.hpp>
#include <Menus.hpp>
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// boost
#include "boost\noncopyable.hpp"
#include "boost\shared_ptr.hpp"

#include "AppEvtsDef.hpp"
#include "hromat900_ini.h"

//---------------------------------------------------------------------------
class TiXmlElement;
class TiXmlDocument;
class TFormEvts : public TForm
{
__published:	// IDE-managed Components
    TStringGrid *grdEvts;
    void __fastcall FormActivate(TObject *Sender);
    void __fastcall grdEvtsDrawCell(TObject *Sender, int ACol, int ARow,
          TRect &Rect, TGridDrawState State);
    void __fastcall grdEvtsMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
private:	// User declarations
	// XML
    TiXmlElement* xml_;
    
    boost::shared_ptr<void> connectOnUpdateEvtsJournal_;
    bool isActivated_;
    void OnUpdateEvtsJournal();
public:		// User declarations
    __fastcall TFormEvts(TComponent* Owner);
    __fastcall ~TFormEvts();
};
//---------------------------------------------------------------------------
extern PACKAGE TFormEvts *FormEvts;
//---------------------------------------------------------------------------
#endif
