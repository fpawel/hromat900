//---------------------------------------------------------------------------

#ifndef uFormCorruptedFilesMessageDialogH
#define uFormCorruptedFilesMessageDialogH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------
class TFormCorruptedFilesMessageDialog : public TForm
{
__published:	// IDE-managed Components
    TListView *lv1;
    TLabel *Label1;
    TButton *Button2;
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall Button2Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
    __fastcall TFormCorruptedFilesMessageDialog(TComponent* Owner);
    void AddLine(const AnsiString& s1, const AnsiString& s2, const AnsiString& s3);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormCorruptedFilesMessageDialog *FormCorruptedFilesMessageDialog;
//---------------------------------------------------------------------------
#endif
