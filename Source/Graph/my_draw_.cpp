//---------------------------------------------------------------------------
#include <math>
#pragma hdrstop
//------------------------------------------------------------------------------
#include "my_draw_.h"

#include <Printers.hpp>
//------------------------------------------------------------------------------
#pragma package(smart_init)
//------------------------------------------------------------------------------
double MyAbs( double val )
{
	return val>0 ? val : val*(-1.0);
}
//------------------------------------------------------------------------------
int MyRound( double val )
{
	const double down = std::floor(val), up = std::ceil(val);
    const int ret = static_cast<int>( MyAbs(down-val)>MyAbs(up-val) ? up : down );
    return ret;
}
//------------------------------------------------------------------------------
MyMetrics GetMyMetrics(HDC h_DC)
{
	MyMetrics ret;
    ret.mm_w_ = ::GetDeviceCaps(h_DC, HORZSIZE);
    ret.mm_h_ = ::GetDeviceCaps(h_DC, VERTSIZE);
    ret.w_ = ::GetDeviceCaps(h_DC, HORZRES);
    ret.h_ = ::GetDeviceCaps(h_DC, VERTRES);
    ret.x_offset_ = ::GetDeviceCaps(h_DC, PHYSICALOFFSETX);
    ret.y_offset_ = ::GetDeviceCaps(h_DC, PHYSICALOFFSETY);
    ret.xdpmm_ = ret.w_/ret.mm_w_;
    ret.ydpmm_ = ret.h_/ret.mm_w_;
    return ret;
}
//------------------------------------------------------------------------------
void MyDrawTextRect(TCanvas *cnv, const TRect& Rect, const AnsiString &txt)
{
	int	textW, textH, txtX, txtY;

	//ширина текста
    textW = cnv->TextWidth( txt );
    //высота текста
	textH = cnv->TextHeight( txt );
    // координаты текста
    txtX = Rect.left+( ( Rect.Right - Rect.left - textW ) / 2);
	txtY = Rect.Top+( ( Rect.bottom - Rect.top - textH ) / 2);
    // вывод текста
    cnv->TextRect(Rect, txtX,  txtY,  txt	);
}

unsigned MyDrawTextRect(TCanvas *cnv, unsigned l, unsigned t, unsigned r,
	const AnsiString &txt, unsigned fontSize)
{
	const unsigned fntSz = cnv->Font->Size;
    cnv->Font->Size = fontSize;
    const unsigned h = cnv->TextHeight(txt);

    MyDrawTextRect(cnv, TRect(l, t, r, t + h ), txt );
    t += h;
    cnv->Font->Size = fntSz;
    return t;
}

//------------------------------------------------------------------------------
void DrawFrameRect(TCanvas *cnv, int l, int t, int r, int b)
{
   cnv->MoveTo(l, t );
   cnv->LineTo(r, t);
   cnv->LineTo(r, b);
   cnv->LineTo(l, b);
   cnv->LineTo(l, t);
}


//------------------------------------------------------------------------------
void DoDrawCustomCell(TCanvas *cnv,	const TRect& Rect, const AnsiString &txt)
{
	MyDrawTextRect(cnv, Rect, txt);
    // обрамление
    cnv->MoveTo(Rect.Left,Rect.Bottom);
    cnv->LineTo(Rect.Left,Rect.Top);
	cnv->LineTo(Rect.Right,Rect.Top);
}
//------------------------------------------------------------------------------
void MyDrawTextLine(TCanvas *cnv, TRect& rect, const AnsiString& s)
{
	const int h = cnv->TextHeight(s);
    cnv->TextOut( rect.Left, rect.top, s );
    rect.top = rect.top+h+5;
}
//------------------------------------------------------------------------------
bool AnsiStringLess(const AnsiString& lhs, const AnsiString& rhs)
{
	return lhs.Length()<rhs.Length();
}
unsigned GetTextWidthOfColumn(const MyColumn& column, TCanvas* cnv)
{
	MyColumn::const_iterator e = column.end(), b = column.begin(),
    iMax = std::max_element(b,e, AnsiStringLess );
    const unsigned ret = (iMax==e) ? 0 : cnv->TextWidth(*iMax);
    return ret;
}
//------------------------------------------------------------------------------
void GetWidthsOfTableCols(TCanvas *cnv, unsigned x0, unsigned x1, const MyColumns& tbl, std::vector<unsigned>& ret)
{
	const unsigned sz = tbl.size();
	ret.resize( sz );
    std::vector<double> otn_coords(sz, 0);
    double sm = 0;   
    for( unsigned i=0; i<sz; ++i )
    {
    	sm += GetTextWidthOfColumn(tbl[i], cnv );
        otn_coords[i] = sm;
    }
    for( unsigned i=0; i<sz; ++i ) otn_coords[i]/=sm;
    for( unsigned i=0; i<sz; ++i )
    	ret[i] = x0 + MyRound(otn_coords[i]* (x1-x0) );
}
//------------------------------------------------------------------------------
unsigned GetMyTableRowCount(const MyColumns& tbl)
{
	unsigned ret = 0;
	for( unsigned i=0; i<tbl.size(); ++i )
    	ret = std::max(ret, tbl[i].size() );
    return ret;

}
//------------------------------------------------------------------------------
unsigned MyDrawTable(TCanvas *cnv,	const TRect& rect, const MyColumns& tbl,
	const unsigned rowHeight)
{
    const unsigned colCount = tbl.size(), rowCount = GetMyTableRowCount(tbl);
    std::vector<unsigned> colwidths(colCount,0);

    // вычисляем длины колонок
    GetWidthsOfTableCols(cnv, rect.left, rect.right, tbl, colwidths);
    //const unsigned grdHeight = rowCount * rowHeight, grdWidth = colwidths[colCount-1]-rect.left ;
    unsigned l, t, r, b;
    t = rect.top;
    for ( unsigned ro=0; ro<rowCount; ++ro )
    {
    	b = t + rowHeight;
     	l = rect.left;
        for ( unsigned cl=0; cl<colCount; ++cl )
        {
        	r = colwidths[cl];
            MyDrawTextRect(cnv, TRect(l,t,r,b), tbl[cl][ro] );
            DrawFrameRect(cnv, l, t, r, b);
            l = r;
        }
        t = b;
    }
    return b;
}
//------------------------------------------------------------------------------
unsigned MyPrintTable(const unsigned left, unsigned top, const unsigned right,
	const unsigned paperTop, const MyColumns& tbl, const unsigned rowHeight)
{
	TPrinter& prntr = *Printer();
    TCanvas *cnv = prntr.Canvas;
    const TRect
    	cnvRect = cnv->ClipRect;
    const unsigned
    	cnvRectWidth = cnvRect.Width(),
    	xWidth = right - left;
    //assert( left<cnvRectWidth && right<=cnvRectWidth && (right-left)<=cnvRectWidth );

	const unsigned
    	colCount = tbl.size(),
        rowCount = GetMyTableRowCount(tbl);

    std::vector<unsigned> colwidths(colCount,0);

    // вычисляем длины колонок
    GetWidthsOfTableCols(cnv, left, left+xWidth, tbl, colwidths);

    //const unsigned grdHeight = rowCount * rowHeight, grdWidth = colwidths[colCount-1]-left;

    int l,t,r,b;
    t = top;
    for ( unsigned ro=0; ro<rowCount; ++ro )
    {
    	b = t + rowHeight;

        if( b>cnvRect.bottom )
        {
            top = paperTop;
            t = top;
            b = top + rowHeight;
            prntr.NewPage();
        }
    	l = left;
        for ( unsigned cl=0; cl<colCount; ++cl )
        {
        	r = colwidths[cl];
            MyDrawTextRect(cnv, TRect(l,t,r,b), tbl[cl][ro] );
            DrawFrameRect(cnv, l, t, r, b);
            l = r;
        }
        t = b;
    }
    return b;

}

void DrawIcon_16_16_OnControl( HICON hIcon, TControl* ctrl )
{
    const TPoint P = ctrl->ClientToScreen( TPoint(10, ctrl->Height/2 - 8 ) );
    ::DrawIconEx( ::GetDC(0), P.x, P.y, hIcon, 16, 16, 0, NULL, DI_NORMAL );
}
