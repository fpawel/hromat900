//---------------------------------------------------------------------------
#ifndef uPikTblHlprH
#define uPikTblHlprH
//---------------------------------------------------------------------------

// boost
#include "boost\noncopyable.hpp"
#include "boost\shared_ptr.hpp"

//vcl
#include <StdCtrls.hpp>
#include <grids.hpp>

// my

#include "DrawGridCell.h"
#include "Hromat900.hpp"

class PikTableHlpr : public boost::noncopyable
{
public:
	explicit PikTableHlpr( unsigned dtctrNum, TStringGrid* grdBasePik,
    	TStringGrid* grdPik, TStringGrid* grdHtgsList );
    ~PikTableHlpr();

private:
	
	const unsigned dtctrNum_;
	TStringGrid *grdBasePik_, *grdPik_;
	my::DrawGridCellManager drawGrdBasePik_, drawGrdPik_, drawGrdHtgsList_;
    TMenuItem *menuAddNewPik_, *menuEditPik_, *menuDelPik_, *menuAddBasePik_;

    void AfterSetCanvasGrdPhysChem( TStringGrid* grd, int col, int row,
	    MyGridCellState drawState, TRect rect, int& txtX, int& txtY );
    void __fastcall OnPopupGrdBasePik(TObject *Sender);
    void __fastcall OnPopupGrdPik(TObject *Sender);

    void __fastcall OnEditPik(TObject *Sender);
    void __fastcall OnAddPik(TObject *Sender);
    void __fastcall OnDelPik(TObject *Sender);
    void __fastcall OnAddBasePic(TObject *Sender);
    void ShowPikNum();

};



//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
