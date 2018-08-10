//---------------------------------------------------------------------------
#include <numeric>
//---------------------------------------------------------------------------
#pragma hdrstop
//---------------------------------------------------------------------------
#include "uFormGraduate.h"
//---------------------------------------------------------------------------

#include "SGrdHlpr.h"
#include "MyFileCtrl.h"
#include "AnsiStringUtils_.h"
#include "MyAppProfile.h"
#include "MyAppWinPath.hpp"
#include "myconv.h"
//---------------------------------------------------------------------------
#include "HromatGas.hpp"
#include "iniUtils_.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
//------------------------------------------------------------------------------
TFormGraduate *FormGraduate;
//------------------------------------------------------------------------------
__fastcall TFormGraduate::TFormGraduate(TComponent* Owner,
    const HromatGraduateItem& graduate)
	: TForm(Owner), drwGrd(grd), graduate_(graduate)
{
    Caption = "Результат последней градуировки";
    my::Grd::AddCopyItemToPopupMenu(grd);
    drwGrd.SetAfterSetCanvasFoo(&AfterSetCanvasGrd);
    drwGrd.SetOnDeterminateColumnWidthFoo( &OnDeterminateColumnWidth );
    const unsigned sz = Gas::GrauatorsCount();
    const Gas::Item *gas = Gas::GetItems();
    grd->RowCount = sz+1;
    grd->ColCount = 4;
    grd->FixedCols = 0;
    grd->FixedRows = 1;
    for( unsigned i=0; i<sz; ++i )
    {
        grd->Cells[0][i+1] = gas[i].name;
        grd->Cells[1][i+1] = MyFloatToStr( MyDoubleToFloat( graduate_.Ki.v[i] ) );
        grd->Cells[2][i+1] = MyFloatToStr( MyDoubleToFloat( graduate_.Rk.v[i]) );
        grd->Cells[3][i+1] = MyFloatToStr( MyDoubleToFloat( graduate_.Rkmax.v[i] ));
    }
    grd->Cells[0][0] = "Компонент";
    grd->Cells[1][0] = "Ki";
    grd->Cells[2][0] = "Rk";
    grd->Cells[3][0] = "Rk_max";
    Label4->Caption = graduate.fileIndex;
    Memo1->Lines->Text = GraduateResult2Str(graduate.result);
    if( graduate.result==0 )
        Memo1->Font->Color = clBlue;
    
    Label7->Caption = MyFormatDateTime1( EncodeDateTime(graduate_.Ki.dateTime));
    Label10->Caption = graduate_.ballon;

    ClientHeight = grd->DefaultRowHeight*(sz+1)+20 + Panel1->Height;

}
//---------------------------------------------------------------------------
__fastcall TFormGraduate::~TFormGraduate()
{
}
//---------------------------------------------------------------------------
void __fastcall TFormGraduate::FormActivate(TObject *Sender)
{
    OnActivate = NULL;
}
//---------------------------------------------------------------------------
void __fastcall TFormGraduate::FormCreate(TObject *Sender)
{
	//
}
//---------------------------------------------------------------------------

void __fastcall TFormGraduate::FormClose(TObject *Sender,
      TCloseAction &Action)
{   
    Action = caFree;
}
//---------------------------------------------------------------------------
void __fastcall TFormGraduate::FormShow(TObject *Sender)
{
    //
}
//---------------------------------------------------------------------------
void __fastcall TFormGraduate::FormHide(TObject *Sender)
{
    //
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void TFormGraduate::AfterSetCanvasGrd( TStringGrid* grd, int col, int row,
	    MyGridCellState drawState, TRect rect, int& txtX, int& txtY ) const
{
	TCanvas &cnv = *grd->Canvas;
    if( col==0 )
    {
        cnv.Brush->Color = grd->FixedColor;
    }
    else
    {
    	cnv.Brush->Color = grd->Color;
        cnv.Font->Color = clBlack;
    }

    if ( row==0 )
    {
        cnv.Brush->Color  = 0x00DDDDDD;
        cnv.Font->Size  = grd->Font->Size+1;
    }


    if(row!=0)
        txtX = rect.left + 3;
        
    if ( drawState & mygdSelected )
    {
        cnv.Font->Color  = clWhite;
        cnv.Brush->Color = clMenuHighlight;
    }
}
//------------------------------------------------------------------------------
void TFormGraduate::OnDeterminateColumnWidth(TStringGrid*, int col, int row, bool& f)
{
    f = 0;
    
}
//------------------------------------------------------------------------------

