//---------------------------------------------------------------------------
#ifndef HromatPhysChemH
#define HromatPhysChemH
//---------------------------------------------------------------------------
// vcl
#include <grids.hpp>

#include "Hromat900.hpp"
#include "DrawGridDefine.hpp"
//---------------------------------------------------------------------------
void MakeGrdPhysChem(TStringGrid* grd);
void ShowGrdPhysChem(TStringGrid* grd, const HromatPhysChem&);
void DrawPhysChemParamTitle( TCanvas &cnv, const TRect& destRect, const unsigned n );

struct DrawPhysChemParamTitles
{
	explicit DrawPhysChemParamTitles(int col, int row);
    const int col_, row_;


	void AfterDraw( TStringGrid* stringGrid, int col, int row,
    	MyGridCellState drawState, TRect rect ) const;

};

//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
