//---------------------------------------------------------------------------

#ifndef uFormPikDlgH
#define uFormPikDlgH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>

//std
//#include <map>

#include "Hromat900.hpp"

//---------------------------------------------------------------------------
class TFormPikDlg : public TForm
{
__published:	// IDE-managed Components
    TLabeledEdit *LabeledEdit3;
	TButton *Button1;
	TButton *Button2;
    TLabeledEdit *LabeledEdit2;
	TComboBox *cbGas;
	TLabel *Label1;
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall LabeledEdit3Exit(TObject *Sender);
	void __fastcall Button1Click(TObject *Sender);
	void __fastcall Button2Click(TObject *Sender);
private:	// User declarations

    // для поиска функции проверки строки для заданного TLabeledEdit
    TStrings *outDt_;

public:		// User declarations
	__fastcall TFormPikDlg(TComponent* Owner, TStrings* strs, TStrings* names);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormPikDlg *FormPikDlg;
//---------------------------------------------------------------------------
#endif
