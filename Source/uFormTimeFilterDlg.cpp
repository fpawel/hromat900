//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "uFormTimeFilterDlg.h"
#include "MyVclUtils_.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormTimeFilterDlg *FormTimeFilterDlg;
//---------------------------------------------------------------------------
__fastcall TFormTimeFilterDlg::TFormTimeFilterDlg(TComponent* Owner)
    : TForm(Owner)
{
    MakeLabelAsButton( Label3 );
    MakeLabelAsButton( Label4 );
    MakeLabelAsButton( Label5 );
}
//---------------------------------------------------------------------------
void __fastcall TFormTimeFilterDlg::Label3Click(TObject *Sender)
{
    ModalResult = mrOk;
}
//---------------------------------------------------------------------------

void __fastcall TFormTimeFilterDlg::Label4Click(TObject *Sender)
{
    ModalResult = mrCancel;
}
//---------------------------------------------------------------------------

void __fastcall TFormTimeFilterDlg::FormClose(TObject *Sender,
      TCloseAction &Action)
{
    UnregisterAllLabelsAsButton( this );    
}
//---------------------------------------------------------------------------

void __fastcall TFormTimeFilterDlg::Label5Click(TObject *Sender)
{
    ModalResult = mrAbort;
}
//---------------------------------------------------------------------------

