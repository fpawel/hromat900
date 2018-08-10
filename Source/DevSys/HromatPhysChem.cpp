//---------------------------------------------------------------------------


#pragma hdrstop

#include "HromatPhysChem.h"
// my
#include "DrawGridCell.h"
#include "myGeomtr.h"
#include "SGrdHlpr.h"
#include "Unit1.h"
#include "AnsiStringUtils_.h"
#include "uDrawGrids.h"
//------------------------------------------------------------------------------
#pragma package(smart_init)
//------------------------------------------------------------------------------
/*
void DrawPhysChemParamTitle( TCanvas &cnv, const TRect& destRect, const unsigned n )
{
	assert(n<PHY_CHEM::COUNT);
    
	enum { indexFntSz = 6 };
    
	TRect rect = my::DecRect(destRect,1);
    cnv.FillRect( rect );

    const int origFntSz = cnv.Font->Size;
    const AnsiString
        names[] =   { "d", "r", "H", "H", "H", "H", "W", "W" },
        his[] =     { "",  "",  "m", "m", "v", "v", "",  "" },
        los[] =     { "",  "",  "i", "s", "i", "s", "i", "s" },
        edizm[] =   { "%",  "кг/м3",  "кДж/моль", "кДж/моль", "МДж/м3", "МДж/м3", "МДж/м3", "МДж/м3" }
    ;

    const AnsiString
        &sName = names[n],
        &sHi = his[n],
        &sLo = los[n],
        sEdIzm = ", "+edizm[n];
    cnv.Font->Size = indexFntSz;
    const int indexTxtW = std::max( cnv.TextWidth(sHi), cnv.TextWidth(sLo) );

    cnv.Font->Size = origFntSz;

    rect.right -= indexTxtW + cnv.TextWidth(sEdIzm);

    // вывод текста
    const int txtW = cnv.TextWidth( sName ), txtH  = cnv.TextHeight( sName );
    TPoint ptLT = GetCenterPosition( rect, txtW, txtH );

    if (n==1 )
    {
        cnv.Font->Name = "Symbol";
        cnv.Font->Size = 10;
        ptLT.y -= 5;
    } else
    {
        cnv.Font->Name = "MS Sans Serif";
        cnv.Font->Size = 8;
    }

	cnv.TextRect(rect, ptLT.x,  ptLT.y,  sName	);
    // верхний индекс
    cnv.Font->Name = "MS Sans Serif";
    cnv.Font->Size = indexFntSz;
    ptLT.x += txtW + 2;
    ptLT.y -= 6;
    if (n==2 || n==3)
        cnv.Font->Name = "Symbol";
    else
        cnv.Font->Name = "MS Sans Serif";
    cnv.TextOut( ptLT.x, ptLT.y, sHi);

    // нижний индекс
    cnv.Font->Name = "MS Sans Serif";
    ptLT.y += txtH -3;
    if (n==2 || n==3)
        ptLT.x += 2;
    cnv.TextOut( ptLT.x, ptLT.y, sLo);

    cnv.Font->Size = 8;
    const TPoint ptLT1 = GetCenterPosition( rect, txtW, txtH );
    ptLT.y = ptLT1.y;
    ptLT.x += indexTxtW+2;
    cnv.TextOut( ptLT.x, ptLT.y, sEdIzm);

}
*/

void DrawPhysChemParamTitle( TCanvas &cnv, const TRect& destRect, const unsigned n )
{
	assert(n<PHY_CHEM::COUNT);

	enum { indexFntSz = 6 };

	TRect rect = my::DecRect(destRect,1);
    cnv.FillRect( rect );

    const int origFntSz = cnv.Font->Size;
    const AnsiString
        names[] =   { "d", "r", "H", "H", "H", "H", "W", "W" },
        his[] =     { "",  "",  "m", "m", "v", "v", "",  "" },
        los[] =     { "",  "",  "s", "l", "s", "l", "s", "l" }
    ;

    const AnsiString
        &sName = names[n],
        &sHi = his[n],
        &sLo = los[n];
    cnv.Font->Size = indexFntSz;
    const int indexTxtW = std::max( cnv.TextWidth(sHi), cnv.TextWidth(sLo) );

    cnv.Font->Size = origFntSz;

    rect.right -= indexTxtW;

    // вывод текста
    const int txtW = cnv.TextWidth( sName ), txtH  = cnv.TextHeight( sName );
    TPoint ptLT = GetCenterPosition( rect, txtW, txtH );

    if (n==1 )
    {
        cnv.Font->Name = "Symbol";
        cnv.Font->Size = 10;
        ptLT.y -= 5;
    } else
    {
        cnv.Font->Name = "MS Sans Serif";
        cnv.Font->Size = 8;
    }

	cnv.TextRect(rect, ptLT.x,  ptLT.y,  sName	);
    // верхний индекс
    cnv.Font->Name = "MS Sans Serif";
    cnv.Font->Size = indexFntSz;
    ptLT.x += txtW + 2;
    ptLT.y -= 6;
    if (n==2 || n==3)
        cnv.Font->Name = "Symbol";
    else
        cnv.Font->Name = "MS Sans Serif";
    cnv.TextOut( ptLT.x, ptLT.y, sHi);

    // нижний индекс
    cnv.Font->Name = "MS Sans Serif";
    ptLT.y += txtH -3;
    if (n==2 || n==3)
        ptLT.x += 2;
    cnv.TextOut( ptLT.x, ptLT.y, sLo);

    cnv.Font->Size = 8;
    const TPoint ptLT1 = GetCenterPosition( rect, txtW, txtH );
    ptLT.y = ptLT1.y;
    ptLT.x += indexTxtW+2;
}


DrawPhysChemParamTitles::DrawPhysChemParamTitles(int col, int row ) :
	col_(col), row_(row)
{}

void DrawPhysChemParamTitles::AfterDraw( TStringGrid* stringGrid, int col, int row,
	MyGridCellState drawState, TRect rect ) const
{
	TStringGrid& grd = *stringGrid;
    TCanvas& cnv = *( grd.Canvas );
    const TRect originalRect = rect;
    const unsigned idxPhysChem = row - row_;
    if( idxPhysChem>=PHY_CHEM::COUNT || col!=col_ || grd.ColWidths[col]==0 || grd.RowHeights[row]==0) return;
    #pragma warn -8037
    DrawPhysChemParamTitle( cnv, rect, idxPhysChem );
    #pragma warn +8037
}
//------------------------------------------------------------------------------

void MakeGrdPhysChem(TStringGrid* grdPhysChem)
{
	const DrawMyGrid& dg = DrawMyGrid::Instance();
    static const DrawPhysChemParamTitles drawGrdPhChm(0,1);
    my::DrawGridCellManager* drawGCM = new my::DrawGridCellManager(grdPhysChem);
    drawGCM->SetAfterDrawFoo( &drawGrdPhChm.AfterDraw );
    drawGCM->SetAfterSetCanvasFoo( &dg.AfterSetCanvasFormArchiveViewGrdPhysChem );

    grdPhysChem->ColCount = 4;
    grdPhysChem->DefaultRowHeight = 30;
    grdPhysChem->RowHeights[0] = 20;
    grdPhysChem->ColWidths[0] = 50;
    grdPhysChem->ColWidths[1] = 85;

    grdPhysChem->RowCount = PHY_CHEM::COUNT+1;
    grdPhysChem->Cells[0][0] = "ФХП";
    grdPhysChem->Cells[1][0] = "Ед.изм.";
    grdPhysChem->Cells[2][0] = "Значение";
    grdPhysChem->Cells[3][0] = " ± ";

    grdPhysChem->FixedCols = 0;
    grdPhysChem->FixedRows = 1;
    grdPhysChem->Height = grdPhysChem->DefaultRowHeight*(grdPhysChem->RowCount+1);

    for( int i=0; i<PHY_CHEM::COUNT; ++i )
    {
        grdPhysChem->Cells[0][i+1] = PHY_CHEM::CAPTION[i];
        grdPhysChem->Cells[1][i+1] = PHY_CHEM::EDIZM[i];
    }
}
//------------------------------------------------------------------------------
double GetPhysChemVal(const HromatPhysChem& physChem, unsigned fld, unsigned n)
{
	assert(fld<PHY_CHEM::COUNT && n<3);
    switch(fld)
    {
    	#define HROMAT_PHYS_CHEM_FIELD_(nn,ss)\
    	case nn: return physChem.##ss##[n];
        HROMAT_PHYS_CHEM_FIELD_(0, relTghtns);
    	HROMAT_PHYS_CHEM_FIELD_(1, absTghtns);
    	HROMAT_PHYS_CHEM_FIELD_(2, hiHm);
    	HROMAT_PHYS_CHEM_FIELD_(3, loHm);
    	HROMAT_PHYS_CHEM_FIELD_(4, hiH);
    	HROMAT_PHYS_CHEM_FIELD_(5, loH);
    	HROMAT_PHYS_CHEM_FIELD_(6, hiW);
    	HROMAT_PHYS_CHEM_FIELD_(7, loW);
        default:
        {
        	assert(false);
            return 0;
        }
        #undef HROMAT_PHYS_CHEM_FIELD_(nn,ss)
    }
}

void SetPhysChemVal(double val, HromatPhysChem& physChem, unsigned fld, unsigned n)
{
	assert(fld<PHY_CHEM::COUNT && n<3);
    switch(fld)
    {
    	#define HROMAT_PHYS_CHEM_FIELD_(nn,ss)\
    	case nn: physChem.##ss##[n]=val;break;
        HROMAT_PHYS_CHEM_FIELD_(0, relTghtns);
    	HROMAT_PHYS_CHEM_FIELD_(1, absTghtns);
    	HROMAT_PHYS_CHEM_FIELD_(2, hiHm);
    	HROMAT_PHYS_CHEM_FIELD_(3, loHm);
    	HROMAT_PHYS_CHEM_FIELD_(4, hiH);
    	HROMAT_PHYS_CHEM_FIELD_(5, loH);
    	HROMAT_PHYS_CHEM_FIELD_(6, hiW);
    	HROMAT_PHYS_CHEM_FIELD_(7, loW);
        default:
        {
        	assert(false);
        }
        #undef HROMAT_PHYS_CHEM_FIELD_(nn,ss)
    }
}
//------------------------------------------------------------------------------
// отобразить ФХП
void ShowGrdPhysChem(TStringGrid* grdPhysChem, const HromatPhysChem& phch)
{
    for( int i=0; i<PHY_CHEM::COUNT; ++i )
    {
    	AnsiString s1, s2;
    	PhysChemFieldAsStr( phch, i, s1, s2);
        grdPhysChem->Cells[2][i+1] = s1;
        grdPhysChem->Cells[3][i+1] = s2;
    }

}
//------------------------------------------------------------------------------

