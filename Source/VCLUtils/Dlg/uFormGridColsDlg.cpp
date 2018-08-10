//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "uFormGridColsDlg.h"
#include "SGrdHlpr.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormGridColsDlg *FormGridColsDlg;
//---------------------------------------------------------------------------
void TFormGridColsDlg::RowsHelper::Set(TStringGrid *grd, unsigned n, bool v) const
{
	my::Grd::SetRowVis(grd, n, v) ;
}
void TFormGridColsDlg::ColumnsHelper::Set(TStringGrid *grd, unsigned n, bool v) const
{
	my::Grd::SetColVis(grd, n, v) ;
}

__fastcall TFormGridColsDlg::TFormGridColsDlg(TComponent* Owner, TStringGrid* grd,
	bool isCol, unsigned titleNum )
	: TForm(Owner), columnsHelper_( ColumnsHelper() ), rowsHelper_( RowsHelper() ), grd_(grd)
{
    
    if(isCol)
    {
        Count_ =  &columnsHelper_.Count;
        Get_ = &columnsHelper_.Get;
        Set_ = &columnsHelper_.Set;
    }
    else
    {
        Count_ =  &rowsHelper_.Count;
        Get_ = &rowsHelper_.Get;
        Set_ = &rowsHelper_.Set;
    }

    
	//int height = 100, width = 100;
	clb->Items->Assign( isCol ? grd->Rows[titleNum] : grd->Cols[titleNum] );
    clb->Items->Add("Показать все");
    clb->ItemEnabled[0] = 0;
    Height = 100 + clb->ItemHeight*clb->Count;
    Width = 100 + Canvas->TextWidth(Caption);

	for( unsigned cl=0; cl<Count_(grd); ++cl )
    	clb->Checked[cl] = Get_(grd,cl);

    Caption = AnsiString().sprintf("Выбирите видимые %s таблицы",  isCol ? "столбцы" : "строки" );
    Set_(grd, 0, 1);

}
//---------------------------------------------------------------------------
__fastcall TFormGridColsDlg::~TFormGridColsDlg()
{
	//int a = 0;
}

void __fastcall TFormGridColsDlg::clbClickCheck(TObject *Sender)
{
	const int idx = clb->ItemIndex, cnt = clb->Count - 1;
    if( idx>-1 && idx<cnt )
    	Set_( grd_, idx, clb->Checked[idx] );
    if( idx==cnt ) for( int i=0; i<cnt; ++i )
    {
        if( clb->ItemEnabled[i] )
        {
    	    clb->Checked[i] = clb->Checked[idx];
    	    Set_( grd_, i, clb->Checked[idx] );
        }
    }


}
//---------------------------------------------------------------------------


void __fastcall TFormGridColsDlg::clbKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
	if( Key==VK_ESCAPE || Key==VK_RETURN)
    	Close();	
}
//---------------------------------------------------------------------------


