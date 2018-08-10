//---------------------------------------------------------------------------

#ifndef uFormAppProfileDlgH
#define uFormAppProfileDlgH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// my
#include "tinyxml.h"

//STD
#include <vector>

class TFormAppProfileDlg : public TForm
{
__published:	// IDE-managed Components
	TButton *Button1;
	TListBox *ListBox1;
	TButton *Button2;
	TButton *Button3;
	TButton *Button4;
	TButton *Button5;
	void __fastcall ListBox1Click(TObject *Sender);
	void __fastcall ListBox1DblClick(TObject *Sender);
	void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
	void __fastcall ListBox1KeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
	void __fastcall Button2Click(TObject *Sender);
	void __fastcall Button4Click(TObject *Sender);
	void __fastcall Button5Click(TObject *Sender);
private:	// User declarations
	TiXmlElement *profilesElem_;
    TiXmlDocument* doc_;
    TStrings& itms;
    int Idx() { return ListBox1->ItemIndex; }
public:		// User declarations
	__fastcall TFormAppProfileDlg(TComponent* Owner, TiXmlElement *profilesElem, TiXmlDocument* doc);

    AnsiString Result()
    {
    	AnsiString ret = "";
        const int idx = Idx();
    	if(ModalResult==mrOk && idx!=-1 )
        	ret =  itms[ idx ];
        return ret;

    }
};
//---------------------------------------------------------------------------
extern PACKAGE TFormAppProfileDlg *FormAppProfileDlg;
//---------------------------------------------------------------------------
#endif
