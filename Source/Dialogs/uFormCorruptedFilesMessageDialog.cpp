//------------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "uFormCorruptedFilesMessageDialog.h"
//------------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormCorruptedFilesMessageDialog *FormCorruptedFilesMessageDialog;
//------------------------------------------------------------------------------
__fastcall TFormCorruptedFilesMessageDialog::TFormCorruptedFilesMessageDialog(TComponent* Owner)
    : TForm(Owner)
{
}
//------------------------------------------------------------------------------
void __fastcall TFormCorruptedFilesMessageDialog::FormCreate(
      TObject *Sender)
{
    // подготовить таблицу
    lv1->Clear();
}
//------------------------------------------------------------------------------
void TFormCorruptedFilesMessageDialog::AddLine(const AnsiString& s1,
    const AnsiString& s2, const AnsiString& s3)
{
    TListItem& itm = *lv1->Items->Add();
    itm.Caption = lv1->Items->Count;
    itm.SubItems->Add(s1);
    itm.SubItems->Add(s2);
    itm.SubItems->Add(s3);
    itm.Checked = 1;
}
//------------------------------------------------------------------------------
void __fastcall TFormCorruptedFilesMessageDialog::Button2Click(
      TObject *Sender)
{
    for( int i=0; i<lv1->Items->Count; ++i )
    {
        TListItem *itm = lv1->Items->Item[i];
        if(!itm->Checked) continue;
        const AnsiString fn = itm->SubItems->Strings[0] + "\\"+
            itm->SubItems->Strings[1];
        DeleteFile( fn );
        lv1->Items->Delete(i);
    }
    ModalResult = mrOk;
}
//---------------------------------------------------------------------------

