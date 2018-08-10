//---------------------------------------------------------------------------
#ifndef uDrawGridsH
#define uDrawGridsH
//---------------------------------------------------------------------------
// my
#include "DrawGridCell.h"

// boost
#include "boost\noncopyable.hpp"

class DrawMyGrid : public boost::noncopyable
{
public:
	 void AfterSetBasePikCellCanvas( TStringGrid* grd, int col, int row,
    	MyGridCellState drawState, TRect rect, int& txtX, int& txtY ) const;

     void OnDeterminateColumnWidthForm1RightGrd( TStringGrid* grd, int col,
		int row, bool &autoColWidth) const;

     void AfterSetCanvasGrdChns( TStringGrid* grd, int col, int row,
	    MyGridCellState drawState, TRect rect, int& txtX, int& txtY ) const;

      void AfterSetCanvasGrdHtgDetail( TStringGrid* grd, int col, int row,
	    MyGridCellState drawState, TRect rect, int& txtX, int& txtY ) const;

      void AfterSetCanvasForm1GrdPhysChem( TStringGrid* grd, int col, int row,
	    MyGridCellState drawState, TRect rect, int& txtX, int& txtY ) const;

      void AfterSetCanvasFormArchiveViewGrdPhysChem( TStringGrid* grd, int col, int row,
	    MyGridCellState drawState, TRect rect, int& txtX, int& txtY ) const;

	static const DrawMyGrid& Instance();
private:
	explicit DrawMyGrid() {}

};





//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
