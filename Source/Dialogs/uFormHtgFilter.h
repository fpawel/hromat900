//---------------------------------------------------------------------------

#ifndef uFormHtgFilterH
#define uFormHtgFilterH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Menus.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TFormHtgFilter : public TForm
{
__published:	// IDE-managed Components
    TPopupMenu *PopupMenu2;
    TMenuItem *N01;
    TMenuItem *N11;
    TMenuItem *N21;
    TMenuItem *N31;
    TMenuItem *N41;
    TMenuItem *N51;
    TMenuItem *N61;
    TMenuItem *N71;
    TMenuItem *N1;
    TPanel *Panel1;
    TLabel *Label1;
    TLabel *Label2;
    TCheckBox *CheckBox1;
    TCheckBox *CheckBox2;
    TLabel *Label3;
    TLabel *Label4;
    void __fastcall CheckBox1Click(TObject *Sender);
    void __fastcall CheckBox2Click(TObject *Sender);
    void __fastcall N71Click(TObject *Sender);
    void __fastcall Label2Click(TObject *Sender);
    void __fastcall Label3Click(TObject *Sender);
    void __fastcall Label4Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
    __fastcall TFormHtgFilter(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormHtgFilter *FormHtgFilter;
//---------------------------------------------------------------------------
#endif
