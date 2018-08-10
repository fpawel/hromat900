//---------------------------------------------------------------------------

#ifndef AdminFormUnitH
#define AdminFormUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
class TadminForm : public TForm
{
__published:	// IDE-managed Components
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
private:	// User declarations
public:		// User declarations
	__fastcall TadminForm(TComponent* Owner);
    __fastcall ~TadminForm();
};
//---------------------------------------------------------------------------
extern PACKAGE TadminForm *adminForm;
//---------------------------------------------------------------------------
#endif
