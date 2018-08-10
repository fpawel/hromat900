//---------------------------------------------------------------------------

#ifndef uTestCalcHromatSatatH
#define uTestCalcHromatSatatH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Grids.hpp>
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
#include "DrawGridCell.h"
//---------------------------------------------------------------------------
class TFormTestCalcStatDlg : public TForm
{
__published:	// IDE-managed Components
    TStringGrid *grdPhysChem;
    TButton *Button1;
    TStringGrid *grd1;
    TButton *Button2;
    TCheckBox *CheckBox1;
    TLabel *Label1;
    void __fastcall Button2Click(TObject *Sender);
    void __fastcall Button1Click(TObject *Sender);
private:	// User declarations
    my::DrawGridCellManager drwGrd1_;
public:		// User declarations
    __fastcall TFormTestCalcStatDlg(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormTestCalcStatDlg *FormTestCalcStatDlg;
//---------------------------------------------------------------------------
#endif
