//---------------------------------------------------------------------------

#ifndef uFormHromatArchiveDlgH
#define uFormHromatArchiveDlgH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Grids.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
#include <Menus.hpp>
#include <ImgList.hpp>
//---------------------------------------------------------------------------


#include <map>
//---------------------------------------------------------------------------
// my

#include "DrawGridCell.h"
#include "Hromat900.hpp"
#include "hromat900_ini.h"



namespace MFI
{   
    typedef std::vector<PHtgItemConst> PNfoLst;
    typedef PNfoLst::const_iterator PPNfo;

    void MakePNfoLst(const HtgItemsArray& lst, PNfoLst& ret);
};

class TFormArchiveView;
class TiXmlElement;
class TFormHromatArchiveDlg : public TForm
{
__published:	// IDE-managed Components
    TPanel *Panel5;
    TLabel *Label3;
    TLabel *Label4;
    TLabel *Label1;
    void __fastcall Button2Click(TObject *Sender);
    void __fastcall FormActivate(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall grd1MouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
    void __fastcall Label3Click(TObject *Sender);
    void __fastcall Label4Click(TObject *Sender);
    void __fastcall Label1Click(TObject *Sender);
private:	// User declarations
    
    TiXmlElement* xml_;

    TPanel *statusPanel_;

    bool isActivated_;

    HtgItemsArray origItems_, items_;

    TFormArchiveView * formArchiveView;


    void AcceptTimeFilter();

public:		// User declarations
    __fastcall TFormHromatArchiveDlg(TComponent* Owner);
    __fastcall ~TFormHromatArchiveDlg();
    void SetItems(const HtgItemsArray& items);
    void ShowArchive();
};
//---------------------------------------------------------------------------
//extern PACKAGE TFormHromatArchiveDlg *FormHromatArchiveDlg;
//---------------------------------------------------------------------------
#endif
