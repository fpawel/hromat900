//---------------------------------------------------------------------------
#ifndef DrawGridCellH
#define DrawGridCellH

// std
#include <utility>


// boost
#include "boost\noncopyable.hpp"
#include "boost\shared_ptr.hpp"




//my
#include "..\my_include\gmodlib_macro.h"
#include "DrawGridDefine.hpp"

// ��� ������������� � ����������� ������
DECLARE_MY_MODULE_INSTANCE_(DrawGridCell);



namespace my
{

typedef std::pair<unsigned,unsigned> IntPair;
    //using std::make_pair;

//---------------------------------------------------------------------------
//       ��������� �����
//---------------------------------------------------------------------------


namespace DrawCellFoo
{
	typedef void (__closure *AfterDraw )
   	(
   		TStringGrid*,
        int,
        int,
        MyGridCellState,
        TRect
	);



	typedef void (__closure *AfterSetCanvas )
   	(
    	TStringGrid*,
        int, // col
        int, // row
        MyGridCellState,
        TRect,
        int&, // text X
        int&  // text Y
    );


    typedef bool (__closure *BeforDrawLine )
    	(
    		TStringGrid*,
        	int,
        	int,
            MyGridCellState,
            unsigned, // ��� ����� - top, left
            TRect
        );


    typedef void (__closure *OnCellBoundChange )
    	(
    		TStringGrid*,
        	int, // col ��� row
            int // ������ ������
        );

    typedef void (__closure *OnDeterminateColumnWidth )
    	(
    		TStringGrid*,
        	int, // col
            int, // row
            bool& 
        );
};

// ��������� ��������� �����
class DrawGridCellManager : public boost::noncopyable
{
	struct DrawLineHelper;
	friend struct DrawLineHelper;
private:
	struct Impl;
    friend struct Impl;
    typedef boost::shared_ptr<Impl> PImpl;
    PImpl impl_;
public:
	explicit DrawGridCellManager(TStringGrid* grd, bool mustRegisterOnMouseOccurrence=false);
    ~DrawGridCellManager();

    // �������� (�������, ���� ���) ���������� ������ ���������
    TFont& GetCellCustomDrawFont( unsigned col, unsigned row );
    TBrush& GetCellCustomDrawBrush(unsigned col, unsigned row);

    TBrush& GetRowCustomDrawBrush(unsigned row);

    // ������� ���������� ������ ���������
    void EraseCellCustomDraw( unsigned col, unsigned row );
    void EraseRowCustomDraw( unsigned row );

    // ������ ��� ��������
    std::pair<int,int> GetHotCell() const;

    void SetAfterSetCanvasFoo(DrawCellFoo::AfterSetCanvas foo);
    void SetAfterDrawFoo(DrawCellFoo::AfterDraw foo);
    void SetBeforDrawLineFoo(DrawCellFoo::BeforDrawLine foo);
    void SetOnColSizingFoo(DrawCellFoo::OnCellBoundChange foo);
    void SetOnDeterminateColumnWidthFoo(DrawCellFoo::OnDeterminateColumnWidth foo);
    
    void SetDrawClp(int l, int t, int r, int b );
    void SetRowPanel(int row );

    void SetWordWrapCell(int col, int row);

    // �������� �������� ��������� ��� �������� �������
    static DrawGridCellManager& GetDrawCellManager( TStringGrid* grd );

    //
    static void UnregisterDrawCellManager( TStringGrid* grd );
};


	TRect DecRect( TRect rect, unsigned n  )
    {
    	rect.top += n;
        rect.bottom -= n;
        rect.left += n;
        rect.right -= n;
        return rect;
    }

    

    class AutoRedrawCellHelper
    {
    public:
        AutoRedrawCellHelper( TStringGrid* grd, unsigned col, unsigned row );
        TFont& GetFont() const;
    	TBrush& GetBrush() const;

        ~AutoRedrawCellHelper();
    private:
    	const unsigned col_,row_;
        DrawGridCellManager& dgcMan_;

    };

}; // namespace my

#endif

