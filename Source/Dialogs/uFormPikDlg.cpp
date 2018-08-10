//---------------------------------------------------------------------------
//std
#include <math>
#include <fstream>


//---------------------------------------------------------------------------
#pragma hdrstop

#include "uFormPikDlg.h"
#include "AnsiStringUtils_.h"
#include "bcd_.h"
#include "HromatGas.hpp"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
//TFormPikDlg *FormPikDlg;

struct IsBDCFloat
{
    bool operator()(const AnsiString& s) const
    {
        const MyDouble v = MyStrToD( s );
        return v.second && std::fabs(v.first)>=0;
    }
};



//---------------------------------------------------------------------------
__fastcall TFormPikDlg::TFormPikDlg(TComponent* Owner, TStrings* strs, TStrings* names)
	: TForm(Owner), outDt_(strs)
{

    //"№", , "Конц.", "t,c", "A, В*с", "H, В"
    //eds_[0]->EditLabel->Caption = "Компонент";
    LabeledEdit2->EditLabel->Caption = "Конц.";
    LabeledEdit3->EditLabel->Caption = "t, c";

    LabeledEdit2->Text = strs->Strings[2] ;
    LabeledEdit3->Text = strs->Strings[3] ;


    const unsigned gasCnt = Gas::Count();
    const Gas::Item* gases = Gas::GetSortedItems();

    for( unsigned i=0; i<gasCnt; ++i )
        cbGas->Items->Add( gases[i].name );
    lx:
    for( int i=0; i<names->Count; ++i )
    {
    	const int idx = cbGas->Items->IndexOf( names->Strings[i] );
        if( idx!=-1 )
        {
        	cbGas->Items->Delete(idx);
            goto lx;
        }
    }
    
    if( !strs->Strings[1].IsEmpty() )
    	cbGas->Items->Add( strs->Strings[1] );
    cbGas->ItemIndex = cbGas->Items->IndexOf( strs->Strings[1] );



}
//---------------------------------------------------------------------------
void __fastcall TFormPikDlg::FormClose(TObject *Sender,
      TCloseAction &Action)
{
	outDt_->Strings[1] = cbGas->Text;
	outDt_->Strings[2] = LabeledEdit2->Text;
    outDt_->Strings[3] = LabeledEdit3->Text;
}
//---------------------------------------------------------------------------
void __fastcall TFormPikDlg::LabeledEdit3Exit(TObject *Sender)
{
	if( ActiveControl==Button2 ) return;
    const IsBDCFloat isBDCFloat;
	TLabeledEdit* ed = dynamic_cast<TLabeledEdit*>(Sender);
    if( ed ) if( !isBDCFloat( ed->Text ) )
    {
    	ShowUserInputError(ed->EditLabel->Caption, ed->Text);
        ActiveControl = ed;
        return;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormPikDlg::Button1Click(TObject *Sender)
{

    const IsBDCFloat isBDCFloat;
	if( ModalResult==mrNo ) return;

    if( !isBDCFloat( LabeledEdit2->Text ) )
    {
    	ShowUserInputError(LabeledEdit2->EditLabel->Caption, LabeledEdit2->Text);
        return;
    }
    
    if( !isBDCFloat( LabeledEdit3->Text ) )
    {
    	ShowUserInputError(LabeledEdit3->EditLabel->Caption, LabeledEdit3->Text );
        return;
    }

    if( cbGas->Text.IsEmpty() )
    {
    	ShowUserInputError( "Наименование", ""  );
        return;
    }
    
	ModalResult=mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TFormPikDlg::Button2Click(TObject *Sender)
{
	ModalResult=mrNo;
}
//---------------------------------------------------------------------------



