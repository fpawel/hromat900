//---------------------------------------------------------------------------
#pragma hdrstop

#include "uPikTblHlpr.h"
#include "SGrdHlpr.h"

#include "uDrawGrids.h"

#include "uFormPikDlg.h"
#include "HromatPic.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
PikTableHlpr::PikTableHlpr( unsigned dtctrNum, TStringGrid* grdBsPk,
	TStringGrid* grdPk, TStringGrid* grdHtgsList ) :
	dtctrNum_(dtctrNum),
    // подписка на сигналы Hromat900
	grdBasePik_(grdBsPk), grdPik_(grdPk),
    drawGrdBasePik_( grdBasePik_ ),
    drawGrdPik_( grdPik_ ),
    drawGrdHtgsList_( grdHtgsList ),
    menuAddNewPik_( new TMenuItem(grdPik_->Owner) ),
    menuEditPik_( new TMenuItem(grdPik_->Owner) ),
    menuDelPik_( new TMenuItem(grdPik_->Owner) ),
    menuAddBasePik_( new TMenuItem(grdPik_->Owner) )
    //connectHromatInitializatonDone_( Hromat900::Instance(). )
{
	InitializeBasePicTableGrd( grdBasePik_ );
    InitializePicTableGrd( grdPik_ );

    //my::Grd::TieWithColsVisiblePopup( grdBasePik_, &OnPopupGrdBasePik );
    //my::Grd::TieWithColsVisiblePopup( grdPik_, &OnPopupGrdPik );

    TPopupMenu *pm = new TPopupMenu(grdPik_->Owner);
    pm->OnPopup = OnPopupGrdBasePik;
    grdBasePik_->PopupMenu = pm;
    TMenuItem *menuSeparate = new TMenuItem(grdPik_->Owner);
    menuSeparate->Caption = "-";
    pm->Items->Add(menuSeparate);

    menuAddNewPik_->Caption = "Добавить новый пик";
    pm->Items->Add(menuAddNewPik_);
    menuAddNewPik_->OnClick =  OnAddPik;

    menuEditPik_->Caption = "Изменить пик";
    menuEditPik_->Visible = 0;
    pm->Items->Add(menuEditPik_);
    menuEditPik_->OnClick =  OnEditPik;

    menuDelPik_->Caption = "Удалить строки";
    menuDelPik_->Visible = 0;
    pm->Items->Add(menuDelPik_);
    menuDelPik_->OnClick =  OnDelPik;

    pm = new TPopupMenu(grdPik_->Owner);
    pm->OnPopup = OnPopupGrdPik;
    grdPik_->PopupMenu = pm;
    menuSeparate = new TMenuItem(grdPik_->Owner);
    menuSeparate->Caption = "-";
    pm->Items->Add(menuSeparate);

    menuAddBasePik_->Caption = "Внести в опорную таблицу";
    menuAddBasePik_->Visible = 0;
    menuAddBasePik_->OnClick = OnAddBasePic;
    pm->Items->Add(menuAddBasePik_);

    const DrawMyGrid& dg = DrawMyGrid::Instance();
    drawGrdBasePik_.SetAfterSetCanvasFoo( &dg.AfterSetBasePikCellCanvas);
    drawGrdPik_.SetAfterSetCanvasFoo(&dg.AfterSetBasePikCellCanvas);
    drawGrdHtgsList_.SetAfterSetCanvasFoo(&dg.AfterSetBasePikCellCanvas);
}
//------------------------------------------------------------------------------
PikTableHlpr::~PikTableHlpr()
{
}
//------------------------------------------------------------------------------
void PikTableHlpr::AfterSetCanvasGrdPhysChem( TStringGrid* grd, int col, int row,
	    MyGridCellState drawState, TRect rect, int& txtX, int& txtY )
{
	TCanvas &cnv = *grd->Canvas;
    if( row>0 && col==0 ) // ВЫРАВНИВАНИЕ по правому краю
    {
    	const int txtW = cnv.TextWidth( grd->Cells[col][row] );
        txtX = rect.right - 3 - txtW;
        cnv.Brush->Color = cl3DLight;
    }

    if ( drawState & mygdSelected )
    {
        grd->Canvas->Brush->Color = clInfoBk;
        grd->Canvas->Font->Color = clNavy;
    }
}
//------------------------------------------------------------------------------
void PikTableHlpr::ShowPikNum(){ for(int i=1; i<grdBasePik_->RowCount; ++i) grdBasePik_->Cells[0][i] = i; }
//------------------------------------------------------------------------------
void __fastcall PikTableHlpr::OnPopupGrdBasePik(TObject *Sender)
{
	const int ro = grdBasePik_->Row;
    if( ro==0 )
    {
    	menuEditPik_->Visible = 0;
        menuDelPik_->Visible = 0;
    	return;
    }
    TGridRect myRect = grdBasePik_->Selection;
    myRect.Left = 0;
    myRect.Right = grdBasePik_->ColCount - 1;
    grdBasePik_->Selection = myRect;

    const AnsiString s = AnsiString().sprintf("%d. \"%s\"", ro, grdBasePik_->Cells[1][ro] );
    menuEditPik_->Visible = 1;
    menuDelPik_->Visible = 1;
    menuEditPik_->Caption = AnsiString().sprintf("Изменить %s", s);
}
//------------------------------------------------------------------------------
void __fastcall PikTableHlpr::OnPopupGrdPik(TObject *Sender)
{
    menuAddBasePik_->Visible = (grdPik_->Row>0);
}
//---------------------------------------------------------------------------
void __fastcall PikTableHlpr::OnEditPik(TObject *Sender)
{
	boost::shared_ptr<TFormPikDlg> dlg_( new TFormPikDlg(NULL,
    grdBasePik_->Rows[grdBasePik_->Row], grdBasePik_->Cols[1]
    ) );
    dlg_->Caption = AnsiString().sprintf("Изменить пик №%d",grdBasePik_->Row);
    dlg_->ShowModal();
    ShowPikNum();
    const HromatBasePicTable basePics = MakeBasePicTableFromGrd(grdBasePik_);
    my::Grd::ClearTStringGrid( grdBasePik_, 1, 1 );
    ShowHromatBasePicTable( grdBasePik_, basePics.begin(), basePics.end() );
}
//---------------------------------------------------------------------------
void __fastcall PikTableHlpr::OnAddPik(TObject *Sender)
{
	grdBasePik_->RowCount = grdBasePik_->RowCount+1;
    grdBasePik_->Row = grdBasePik_->RowCount-1;
    boost::shared_ptr<TFormPikDlg> dlg_( new TFormPikDlg(NULL,
    grdBasePik_->Rows[grdBasePik_->RowCount-1], grdBasePik_->Cols[1]
    ) );
    dlg_->Caption = "Добавить пик";
    dlg_->ShowModal();
    if( dlg_->ModalResult!=mrOk )
        grdBasePik_->RowCount = grdBasePik_->RowCount-1;
    ShowPikNum();
    const HromatBasePicTable basePics = MakeBasePicTableFromGrd(grdBasePik_);
    my::Grd::ClearTStringGrid( grdBasePik_, 1, 1 );
    ShowHromatBasePicTable( grdBasePik_, basePics.begin(), basePics.end() );
}
//---------------------------------------------------------------------------
void __fastcall PikTableHlpr::OnDelPik(TObject *Sender)
{

    if( ::MessageBox( Screen->ActiveForm->Handle,"Подтвердите удаление пиков",
        "Удалить строки?", MB_YESNO ) != IDYES	)
        return;
        
    TGridRect rr = grdBasePik_->Selection;
    my::Grd::DeleteRow( grdBasePik_, rr.Top, 1 + rr.Bottom - rr.Top );
    ShowPikNum();
    const HromatBasePicTable basePics = MakeBasePicTableFromGrd(grdBasePik_);
    my::Grd::ClearTStringGrid( grdBasePik_, 1, 1 );
    ShowHromatBasePicTable( grdBasePik_, basePics.begin(), basePics.end() );
}
//---------------------------------------------------------------------------
void __fastcall PikTableHlpr::OnAddBasePic(TObject *Sender)
{
    grdBasePik_->RowCount = grdBasePik_->RowCount+1;
    grdBasePik_->Row = grdBasePik_->RowCount-1;
    boost::shared_ptr<TFormPikDlg> dlg_( new TFormPikDlg(NULL,
    grdBasePik_->Rows[grdBasePik_->RowCount-1], grdBasePik_->Cols[1]
    ) );
    dlg_->Caption = "Добавить пик";

    //"№", "Наименование", "Конц.", "±конц.", "t,c", "t1,с", "t2,с", "A, В*с", "H, В"

    dlg_->cbGas->ItemIndex = dlg_->cbGas->Items->IndexOf( grdPik_->Cells[1][grdPik_->Row] );
    
    dlg_->LabeledEdit2->Text = grdPik_->Cells[2][grdPik_->Row];
    dlg_->LabeledEdit3->Text = grdPik_->Cells[6][grdPik_->Row];


    dlg_->ShowModal();
    if( dlg_->ModalResult!=mrOk )
        grdBasePik_->RowCount = grdBasePik_->RowCount-1;
        
    ShowPikNum();
    const HromatBasePicTable basePics = MakeBasePicTableFromGrd(grdBasePik_);
    my::Grd::ClearTStringGrid( grdBasePik_, 1, 1 );
    ShowHromatBasePicTable( grdBasePik_, basePics.begin(), basePics.end() );
}


