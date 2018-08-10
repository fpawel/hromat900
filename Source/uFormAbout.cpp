//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "uFormAbout.h"
#include "hromat_sys.h"
#include "cnfdntl_.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormAbout *FormAbout;
//---------------------------------------------------------------------------
__fastcall TFormAbout::TFormAbout(TComponent* Owner)
	: TForm(Owner)
{
	Label5->Caption = HromatSoftIDToString( GetSoftId() );
}
//---------------------------------------------------------------------------
void __fastcall TFormAbout::Label2Click(TObject *Sender)
{
	::ShellExecute(
    	Handle,
        "open",
    	"http://www.analitpribor-smolensk.ru/",
        NULL,
        NULL,
        SW_SHOWDEFAULT  );
}
//---------------------------------------------------------------------------
void __fastcall TFormAbout::Button1Click(TObject *Sender)
{
	ModalResult = mrOk;
}
//---------------------------------------------------------------------------
