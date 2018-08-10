//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "uFormSoftIDDlg.h"
#include "AnsiStringUtils_.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormSoftIDDlg *FormSoftIDDlg;



//---------------------------------------------------------------------------
__fastcall TFormSoftIDDlg::TFormSoftIDDlg(TComponent* Owner, HromatSoftID id)
	: TForm(Owner) , id_(id)
{
	Edit1->Text = HromatSoftIDItemToString(id_.hromat900);
    Edit2->Text = HromatSoftIDItemToString(id_.hrmtlchk);
    Edit3->Text = HromatSoftIDItemToString(id_.h9001);
    Edit4->Text = HromatSoftIDItemToString(id_.ucint);
    Edit5->Text = HromatSoftIDItemToString(id_.calculator);
}
//---------------------------------------------------------------------------


void __fastcall TFormSoftIDDlg::Edit1Exit(TObject *Sender)
{
	TEdit* ed = static_cast<TEdit*>(Sender);
	const int num = ed->Tag;
    const AnsiString s = ed->Text;

    HromatSoftID::Item itm;

    if( !StringToHromatSoftIDItem(s,itm) )
    {
    	const AnsiString msg = AnsiString().sprintf(
        	"Не корректное значение \"%s\" в поле ввода №%d.\n"
        	"Введите щестнадцатиричное число из шести символов.",
            s, num
        );
        ::MessageBox(Handle, msg.c_str(), "Некорректный ввод!", MB_OK | MB_APPLMODAL | MB_ICONERROR );
        ActiveControl = ed;
    }
}
//---------------------------------------------------------------------------

void __fastcall TFormSoftIDDlg::Button1Click(TObject *Sender)
{

	const AnsiString sId =
    Edit1->Text+"-"+Edit2->Text+"-"+Edit3->Text+"-"+Edit4->Text+"-"+Edit5->Text;
    if( StringToHromatSoftID(sId, id_) )
    {
    	ModalResult = mrOk;
        return;
    }

    const AnsiString msg = AnsiString().sprintf(
        	"Не корректное значение идентификатора ПО\n"
            "\"%s\"",sId);
    ::MessageBox(Handle, msg.c_str(), "Некорректный ввод!", MB_OK | MB_APPLMODAL | MB_ICONERROR );
    ActiveControl = Edit1;
}
//---------------------------------------------------------------------------

void __fastcall TFormSoftIDDlg::Button2Click(TObject *Sender)
{
	ModalResult = mrNo;
}
//---------------------------------------------------------------------------

void __fastcall TFormSoftIDDlg::FormActivate(TObject *Sender)
{
	::SetWindowPos(Handle, HWND_TOPMOST, Left, Top, Width, Height, 0	);	
}
//---------------------------------------------------------------------------


void __fastcall TFormSoftIDDlg::Edit1KeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
    if( Key==VK_RETURN )
    {
        Button1->Click();
        return;
    }
    if( Key==VK_ESCAPE )
    {
        Button2->Click();
        return;
    }
}
//---------------------------------------------------------------------------

