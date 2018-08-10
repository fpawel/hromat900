//---------------------------------------------------------------------------

#ifndef uFormAboutH
#define uFormAboutH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
//---------------------------------------------------------------------------
class TFormAbout : public TForm
{
__published:	// IDE-managed Components
	TButton *Button1;
	TImage *Image1;
	TRichEdit *RichEdit1;
	TLabel *Label1;
	TLabel *Label2;
	TRichEdit *RichEdit2;
	TRichEdit *RichEdit3;
	TLabel *Label4;
	TLabel *Label5;
	void __fastcall Label2Click(TObject *Sender);
	void __fastcall Button1Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TFormAbout(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormAbout *FormAbout;
//---------------------------------------------------------------------------
#endif
