//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

enum
{

    CAPTION_HEIGHT = 20
};

#include "uFrameGrid.h"
#include "MouseEvt.h"
#include "SGrdHlpr.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"



//------------------------------------------------------------------------------
//TFrameDropDownPanel *FrameDropDownPanel;
//------------------------------------------------------------------------------
__fastcall TFrameGrid::TFrameGrid(TComponent* Owner,
	TWinControl* par, const AnsiString& cpt)
	: TFrame(Owner),
    hotTrackCaptionBrushColor_( 0x0000E9E4E6 ),
    captionBrushColor_( Panel2->Color ),
    expandedHeight_(300),
    isExpanded_(false), drwGrd(grd1)
{
	Parent = par;
    Label5->Caption = cpt;
    Initialize();
    Panel1->Height = CAPTION_HEIGHT;
    Colapse();

    drwGrd.SetAfterSetCanvasFoo( &AfterSetCanvasGrd );
    drwGrd.SetOnDeterminateColumnWidthFoo( &OnDeterminateColumnWidth );
}
//------------------------------------------------------------------------------
void TFrameGrid::SetRowCount(unsigned n)
    {
        grd1->RowCount = n;
        SetExpandedHeight( grd1->DefaultRowHeight*n+25 );
    }
void TFrameGrid::OnMouseOcurence(TWinControl* ctrl, bool isEntered)
{
    if( isEntered )
    {
    	Panel2->Color = GetHotTrackCaptionBrushColor();
        Label5->Font->Style = TFontStyles() << fsUnderline;
        Label5->Font->Color = clBlue;
    }
    else
    {
    	Panel2->Color = GetCaptionBrushColor();
        Label5->Font->Style = TFontStyles();
        Label5->Font->Color = clBlack;
    }
}
//------------------------------------------------------------------------------
void TFrameGrid::Initialize()
{
	my::RegisterOnMouseOccurrence( Panel2, &OnMouseOcurence );
    //my::RegisterOnMouseOccurrence( Panel3, &OnMouseOcurence );
}
//------------------------------------------------------------------------------
void TFrameGrid::Expand()
{
    const int
        grdHeight = my::Grd::GetActualRowsTotalHeight(grd1)+5,
        maxHeight = static_cast<TForm*>(Owner)->Height-Top-50,
        expHeight = std::min(grdHeight, maxHeight)        
        ;
    SetExpandedHeight( expHeight>50 ?  expHeight : 50);
	Image1->Visible = 1;
    Image2->Visible = 0;
    isExpanded_ = 1;
    const int newH = Panel2->Height + expandedHeight_ ;
    if(!alClientWhenExpanded_)
        Height = newH;
    else
        Align = alClient;
    Repaint();

}
//------------------------------------------------------------------------------
void TFrameGrid::Colapse()
{

    Image1->Visible = 0;
    Image2->Visible = 1;
    isExpanded_ = 0;
    Align = alTop;

	const int newH = CAPTION_HEIGHT;
    /*
    while( Height>newH )
    {
    	Height = Height - 3;
        Repaint();
    }
    */
	Height = newH;
    Repaint();

}
//------------------------------------------------------------------------------
void TFrameGrid::AddTopPanel( TWinControl* pn )
{
	SetExpandedHeight( GetExpandedHeight() + pn->Height);
    pn->Parent = Panel4;
    pn->Align = alTop;
    pn->Top = 1000000;
}
//------------------------------------------------------------------------------
void TFrameGrid::AddAllClienPanel( TWinControl* pn )
{
	Name = "FrameDropDownPanel_"+pn->Name;
	pn->Parent = Panel4;
    SetExpandedHeight( pn->Height );
    pn->Align = alClient;
    Top = 100000;
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TFrameGrid::AfterSetCanvasGrd( TStringGrid* grd, int col, int row,
	MyGridCellState drawState, TRect rect, int& txtX, int& txtY )
{
    if( IsLeftAligmentAndNoAutoColWidthCell(grd, col, row) ) txtX = rect.left + 3;
	TCanvas &cnv = *grd->Canvas;
    if ( col==0 || row==0 )
        cnv.Brush->Color  = grd->FixedColor;
    if ( drawState & mygdHot )
        cnv.Font->Color  = clNavy;

    if ( col>1 && drawState & mygdSelected )
    {
        cnv.Font->Color  = clWhite;
        cnv.Brush->Color = clMenuHighlight;
    }
}
//------------------------------------------------------------------------------
void TFrameGrid::OnDeterminateColumnWidth( TStringGrid* grd, int col, int row, bool& ff)
{
    ff = !IsLeftAligmentAndNoAutoColWidthCell( grd, col, row );
}
//------------------------------------------------------------------------------
bool TFrameGrid::IsLeftAligmentAndNoAutoColWidthCell( TStringGrid* grd, int col, int row )
{
    return true;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void __fastcall TFrameGrid::Label5Click(TObject *Sender)
{
	if( isExpanded_ )
    	Colapse();
    else
    	Expand();	
}
//---------------------------------------------------------------------------

