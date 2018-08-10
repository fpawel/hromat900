//---------------------------------------------------------------------------

#ifndef uFormTimeFilterDlgH
#define uFormTimeFilterDlgH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TFormTimeFilterDlg : public TForm
{
__published:	// IDE-managed Components
    TPanel *Panel1;
    TLabel *Label2;
    TLabel *Label1;
    TDateTimePicker *edTime2;
    TDateTimePicker *edDate2;
    TDateTimePicker *edTime1;
    TDateTimePicker *edDate1;
    TLabel *Label3;
    TLabel *Label4;
    TLabel *Label5;
    TLabel *Label6;
    void __fastcall Label3Click(TObject *Sender);
    void __fastcall Label4Click(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall Label5Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
    __fastcall TFormTimeFilterDlg(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormTimeFilterDlg *FormTimeFilterDlg;
//---------------------------------------------------------------------------
#endif
