//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
#pragma hdrstop
//------------------------------------------------------------------------------
#include "uDrawGrids.h"
#include "myGeomtr.h"
#include "SGrdHlpr.h"
//------------------------------------------------------------------------------
#pragma package(smart_init)
//------------------------------------------------------------------------------
void SetSelectedCellCanvas(TCanvas *cnv)
{
    //cnv->Brush->Color = clYellow;
    cnv->Font->Color  = clWhite;
    cnv->Brush->Color = clMenuHighlight;
    //cnv->Font->Color  = clNavy;
    //cnv->Font->Style = TFontStyles()<< fsBold;
}






const DrawMyGrid& DrawMyGrid::Instance()
{
	static const DrawMyGrid ret;
    return ret;
}

void DrawMyGrid::OnDeterminateColumnWidthForm1RightGrd( TStringGrid* grd, int col,
	int row, bool &autoColWidth) const
{
    autoColWidth = 0;
}

void DrawMyGrid::AfterSetCanvasGrdChns( TStringGrid* grd, int col, int row,
	    MyGridCellState drawState, TRect rect, int& txtX, int& txtY ) const
{
	TCanvas &cnv = *grd->Canvas;
    const AnsiString s = grd->Name;
    const bool isGrdPhyschem = grd->Name=="grdPhisChem";

    const bool isform1leftGrd = s=="grdDtctr1"||s=="grdDtctr2"||s=="grdDtctr1"||
        s=="grdPhisChem"||s=="grdToolPrms"||s=="grdToolInfo";


    if( col==0 || (row==0 && isGrdPhyschem ) )  {
        cnv.Brush->Color = grd->FixedColor;
        cnv.Font->Color = clNavy;
    } else {
        if (isform1leftGrd && row==0 && col==1&& cnv.Brush->Color==grd->FixedColor ) {
            cnv.Brush->Color = grd->Color;

    	    //cnv.Brush->Color = grd->Color;
            //cnv.Font->Color = clBlack;
        }
    }
    //if( col == 0 || (row==0 && isGrdPhyschem ) )
    txtX = rect.left + 5;
}

void DrawMyGrid::AfterSetCanvasForm1GrdPhysChem( TStringGrid* grd, int col, int row,
	    MyGridCellState drawState, TRect rect, int& txtX, int& txtY ) const
{
	TCanvas &cnv = *grd->Canvas;

    if( col==0 ) // ÂÛÐÀÂÍÈÂÀÍÈÅ ïî ïðàâîìó êðàþ
    {
    	const int txtW = cnv.TextWidth( grd->Cells[col][row] );
        txtX = rect.right - 3 - txtW;

    };

    if ( row==0 )
    {
        cnv.Brush->Color  = 0x00DDDDDD;
        cnv.Font->Size  = grd->Font->Size+1;
    } else
    if( col==0  )
    	cnv.Brush->Color = grd->FixedColor;
    else
    {
    	cnv.Brush->Color = grd->Color;
        cnv.Font->Color = clBlack;
    }
    if ( (drawState & mygdSelected) && !(drawState & mygdFixed) && row>0 )
        SetSelectedCellCanvas(grd->Canvas);

    if ( grd->ColWidths[col]<50 ) grd->ColWidths[col] = 50;
}
void DrawMyGrid::AfterSetCanvasFormArchiveViewGrdPhysChem( TStringGrid* grd, int col, int row,
	    MyGridCellState drawState, TRect rect, int& txtX, int& txtY ) const
{
	TCanvas &cnv = *grd->Canvas;

    if ( row==0 )
    {
        cnv.Brush->Color  = 0x00DDDDDD;
        cnv.Font->Size  = grd->Font->Size+1;
    } else
    if( col==0  )
    	cnv.Brush->Color = cl3DLight;
    else
    {
    	//cnv.Brush->Color = 0x00DDDDDD;
        cnv.Brush->Color = grd->Color;
        grd->Canvas->Font->Color  = clBlack;
    }



    if ( drawState & mygdSelected && row>0 )
        SetSelectedCellCanvas(grd->Canvas);

    if ( grd->ColWidths[col]<50 ) grd->ColWidths[col] = 50;
}
//------------------------------------------------------------------------------
void DrawMyGrid::AfterSetBasePikCellCanvas( TStringGrid* grd, int col, int row,
    	MyGridCellState drawState, TRect rect, int& txtX, int& txtY ) const
{
    const bool isGrdPic = grd->Name.Pos("grdPi") || grd->Name.Pos("grdBasePi");

    if( col==0 || row==0)
    {
        grd->Canvas->Brush->Color = cl3DLight;
    }
    else if( !my::Grd::IsRowSelected(grd,row)  ) {
        grd->Canvas->Brush->Color = 0x00DDDDDD;
    }
    else
    {
    	//grd->Canvas->Brush->Color = 0x00DDDDDD;
        grd->Canvas->Brush->Color = grd->Color;
        grd->Canvas->Font->Color  = clBlack;
    }

    if ( isGrdPic && (drawState & mygdSelected) && row>0 ) {
        SetSelectedCellCanvas(grd->Canvas);
    }




}
//------------------------------------------------------------------------------


