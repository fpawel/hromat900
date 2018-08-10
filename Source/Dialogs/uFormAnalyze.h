//---------------------------------------------------------------------------

#ifndef uFormAnalyzeH
#define uFormAnalyzeH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Grids.hpp>
#include <Graphics.hpp>
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// my
#include "VCLUtils\DrawGridCell.h"
//---------------------------------------------------------------------------
class TFormAnalyze : public TForm
{
__published:	// IDE-managed Components
    TPanel *Panel1;
    TStringGrid *grdPhyChe;
    TStringGrid *grdConc;
    TSplitter *Splitter1;
    TLabel *Label2;
    void __fastcall Label6Click(TObject *Sender);
private:	// User declarations
    my::DrawGridCellManager drwGrdConc_;
public:		// User declarations
    __fastcall TFormAnalyze(TComponent* Owner);
    __fastcall ~TFormAnalyze();
};
//---------------------------------------------------------------------------
extern PACKAGE TFormAnalyze *FormAnalyze;
//---------------------------------------------------------------------------
#endif
