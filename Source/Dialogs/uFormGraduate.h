//------------------------------------------------------------------------------
#ifndef uFormGraduateH
#define uFormGraduateH
//------------------------------------------------------------------------------
#define VCL_IOSTREAM

#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <FileCtrl.hpp>
#include <ExtCtrls.hpp>
#include <Grids.hpp>
#include <Menus.hpp>
#include <ComCtrls.hpp>
#include <Buttons.hpp>
#include <Graphics.hpp>
//------------------------------------------------------------------------------
// my
//#include "Hromat900.hpp"
#include "Hromatogram.hpp"
#include "hromat900.hpp"
#include "VCLUtils\DrawGridCell.h"
//------------------------------------------------------------------------------
//std
#include <map>
//------------------------------------------------------------------------------
class TFormGraduate : public TForm
{
__published:	// IDE-managed Components
    TStringGrid *grd;
    TPanel *Panel1;
    TLabel *Label1;
    TLabel *Label2;
    TLabel *Label4;
    TLabel *Label6;
    TLabel *Label7;
    TLabel *Label9;
    TLabel *Label10;
    TMemo *Memo1;
	void __fastcall FormActivate(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall FormHide(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall FormCreate(TObject *Sender);
private:	// User declarations
    my::DrawGridCellManager drwGrd;
    HromatGraduateItem graduate_;
    void AfterSetCanvasGrd( TStringGrid* grd, int col, int row,
	    MyGridCellState drawState, TRect rect, int& txtX, int& txtY ) const;
    void OnDeterminateColumnWidth(TStringGrid*, int col, int row, bool& );
public:		// User declarations
	__fastcall TFormGraduate(TComponent* Owner, const HromatGraduateItem& graduate);
    __fastcall virtual ~TFormGraduate();
};
//------------------------------------------------------------------------------
extern PACKAGE TFormGraduate *FormGraduate;
//------------------------------------------------------------------------------
#endif
