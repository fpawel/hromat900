//---------------------------------------------------------------------------

#ifndef uFormReportAnalizeArchiveH
#define uFormReportAnalizeArchiveH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include "SHDocVw_OCX.h"
#include <OleCtrls.hpp>
//---------------------------------------------------------------------------
class TFormReportAnalizeArchive : public TForm
{
__published:	// IDE-managed Components
    TPageControl *PageControl1;
    TTabSheet *TabSheet1;
    TTabSheet *TabSheet2;
    TCppWebBrowser *CppWebBrowser1;
    TCppWebBrowser *CppWebBrowser2;
private:	// User declarations
public:		// User declarations
    __fastcall TFormReportAnalizeArchive(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormReportAnalizeArchive *FormReportAnalizeArchive;
//---------------------------------------------------------------------------
#endif
