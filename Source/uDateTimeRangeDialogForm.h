//---------------------------------------------------------------------------

#ifndef uDateTimeRangeDialogFormH
#define uDateTimeRangeDialogFormH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------
class TDateTimeRangeDialog : public TForm
{
__published:	// IDE-managed Components
    TDateTimePicker *DateTimePicker1;
    TDateTimePicker *DateTimePicker2;
    TLabel *Label1;
    TLabel *Label2;
    TButton *Button1;
    TLabel *Label3;
private:	// User declarations
public:		// User declarations
    __fastcall TDateTimeRangeDialog(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TDateTimeRangeDialog *DateTimeRangeDialog;
//---------------------------------------------------------------------------
#endif
