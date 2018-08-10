//---------------------------------------------------------------------------
#ifndef uFrameGridH
#define uFrameGridH
//---------------------------------------------------------------------------
#include "DrawGridCell.h"
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
#include <Grids.hpp>
//---------------------------------------------------------------------------
class TFrameGrid : public TFrame
{
__published:	// IDE-managed Components
	TPanel *Panel13;
	TPanel *Panel8;
	TPanel *Panel6;
	TPanel *Panel1;
	TPanel *Panel2;
	TPanel *Panel4;
	TLabel *Label5;
	TImage *Image2;
	TImage *Image1;
    TStringGrid *grd1;
	void __fastcall Label5Click(TObject *Sender);
private:	// User declarations
	TColor hotTrackCaptionBrushColor_, captionBrushColor_;
    int expandedHeight_;
    bool isExpanded_;
    bool alClientWhenExpanded_;

    void OnMouseOcurence(TWinControl* ctrl, bool isEntered);

    void AfterSetCanvasGrd( TStringGrid* grd, int col, int row,
		MyGridCellState drawState, TRect rect, int& txtX, int& txtY );
    void OnDeterminateColumnWidth( TStringGrid* grd, int col, int row, bool& ff);
    bool IsLeftAligmentAndNoAutoColWidthCell( TStringGrid* grd, int col, int row );

public:		// User declarations
	__fastcall TFrameGrid(TComponent* Owner, TWinControl* par, const AnsiString&);

    my::DrawGridCellManager drwGrd;

    void SetHotTrackCaptionBrushColor(TColor c){ hotTrackCaptionBrushColor_ = c; }
    TColor GetHotTrackCaptionBrushColor() const { return hotTrackCaptionBrushColor_; }

    void SetAlClientWhenExpanded() { alClientWhenExpanded_=1; }

    void SetCaptionBrushColor(TColor c)
    {
    	captionBrushColor_ = c;
        Panel2->Color = c;
    }
    TColor GetCaptionBrushColor() const { return captionBrushColor_; }

    void SetRowCount(unsigned n);

    void SetExpandedHeight(int h)
    {
    	expandedHeight_ = h;
        if(isExpanded_) Expand();
    }
    int GetExpandedHeight() const    {    	return expandedHeight_;    }
    void Initialize();
    void Colapse();
    void Expand();
    void AddTopPanel( TWinControl* pn );
    void AddAllClienPanel( TWinControl* pn );
    bool IsExpanded () const { return isExpanded_; }
};
//---------------------------------------------------------------------------
extern PACKAGE TFrameGrid *FrameGrid;
//---------------------------------------------------------------------------
#endif
