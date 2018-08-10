//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "uFormHtgFilter.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormHtgFilter *FormHtgFilter;
//---------------------------------------------------------------------------
__fastcall TFormHtgFilter::TFormHtgFilter(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFormHtgFilter::CheckBox1Click(TObject *Sender)
{
    if( !CheckBox1->Checked && !CheckBox2->Checked ) CheckBox2->Checked = 1;
}
//---------------------------------------------------------------------------
void __fastcall TFormHtgFilter::CheckBox2Click(TObject *Sender)
{
    if( !CheckBox1->Checked && !CheckBox2->Checked ) CheckBox1->Checked = 1;    
}
//---------------------------------------------------------------------------
void __fastcall TFormHtgFilter::N71Click(TObject *Sender)
{
    Label2->Caption = static_cast<TMenuItem*>(Sender)->Caption;
}
//---------------------------------------------------------------------------
void __fastcall TFormHtgFilter::Label2Click(TObject *Sender)
{
    TPoint pt;
    ::GetCursorPos( &pt );
    PopupMenu2->Popup(pt.x, pt.y);    
}
//---------------------------------------------------------------------------

void __fastcall TFormHtgFilter::Label3Click(TObject *Sender)
{
    ModalResult = mrOk;
}
//---------------------------------------------------------------------------

void __fastcall TFormHtgFilter::Label4Click(TObject *Sender)
{
    ModalResult = mrCancel;    
}
//---------------------------------------------------------------------------

