//---------------------------------------------------------------------------

#ifndef uFormSetupTuneTaskH
#define uFormSetupTuneTaskH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <CheckLst.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TFormRele : public TForm
{
__published:	// IDE-managed Components
    TPanel *Panel4;
    TCheckListBox *CheckListBox1;
    TButton *Button2;
    TButton *Button1;
private:	// User declarations
public:		// User declarations
    __fastcall TFormRele(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormRele *FormRele;
//---------------------------------------------------------------------------
#endif
