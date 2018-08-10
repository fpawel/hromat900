//---------------------------------------------------------------------------
#ifndef my_draw_H
#define my_draw_H
//---------------------------------------------------------------------------
#include <vector>
#include "boost\shared_ptr.hpp"

#include <system.hpp>
#include <Graphics.hpp>
#include <Controls.hpp>

typedef std::vector<AnsiString> MyStrs;
typedef MyStrs MyColumn;
typedef std::vector<MyColumn> MyColumns;

typedef struct
{
    int mm_w_, mm_h_, w_, h_, x_offset_, y_offset_;
    double xdpmm_,ydpmm_;
} MyMetrics;

int MyRound( double val );

MyMetrics GetMyMetrics(HDC h_DC);

void MyDrawTextRect(TCanvas *cnv, const TRect& Rect, const AnsiString &txt);
unsigned MyDrawTextRect(TCanvas *cnv, unsigned l, unsigned t, unsigned r, 
	const AnsiString &txt, unsigned fontSize);

void DoDrawCustomCell(TCanvas *cnv,	const TRect& Rect, const AnsiString &txt);



unsigned GetTextWidthOfColumn(const MyColumn& column, TCanvas* cnv);

unsigned MyDrawTable(TCanvas *cnv,	const TRect& rect, const MyColumns& tbl, unsigned rowHeight);

unsigned MyPrintTable(unsigned left, unsigned top, unsigned right,
	unsigned paperTop, const MyColumns& tbl, unsigned rowHeight);

void DrawIcon_16_16_OnControl( HICON hIcon, TControl* ctrl );



//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
