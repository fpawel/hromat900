//---------------------------------------------------------------------------

#ifndef uFormSoftIDDlgH
#define uFormSoftIDDlgH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
#include "..\DevSys\SoftID.hpp"
//---------------------------------------------------------------------------
class TFormSoftIDDlg : public TForm
{
__published:	// IDE-managed Components
	TPanel *Panel1;
	TButton *Button1;
	TButton *Button2;
	TEdit *Edit1;
	TLabel *Label1;
	TLabel *Label2;
	TEdit *Edit2;
	TLabel *Label3;
	TEdit *Edit3;
	TEdit *Edit4;
	TLabel *Label4;
	TLabel *Label5;
	TEdit *Edit5;
	void __fastcall Edit1Exit(TObject *Sender);
	void __fastcall Button1Click(TObject *Sender);
	void __fastcall Button2Click(TObject *Sender);
	void __fastcall FormActivate(TObject *Sender);
    void __fastcall Edit1KeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
private:	// User declarations
	HromatSoftID id_;
public:		// User declarations
	__fastcall TFormSoftIDDlg(TComponent* Owner, HromatSoftID id);
    HromatSoftID Result() const { return id_; } 
};
//---------------------------------------------------------------------------
extern PACKAGE TFormSoftIDDlg *FormSoftIDDlg;
//---------------------------------------------------------------------------
#endif
