#ifndef EXPRESS_BAR_INCLUDED
#define EXPRESS_BAR_INCLUDED

#include <vcl.h>
#include <vector>

class ExpressBarItem
{
public:
    friend class ExpressBar;
    enum IconAdjustment
    {
        IconAdjustment_Right,
        IconAdjustment_Left,
        IconAdjustment_Top,
        IconAdjustment_Bottom
    };

    enum TextAdjustment
    {
        TextAdjustment_Left,
        TextAdjustment_Right,
        TextAdjustment_Center
    };

    ExpressBarItem();
    ~ExpressBarItem();
    void setName( AnsiString name );
    AnsiString name();
    void setIcon( Graphics::TBitmap* bitmap );
    bool selected();
    void select();
    void unselect();
    int index();

    void setIconAdjustment( IconAdjustment adjustment );
    void setNameAdjustment( TextAdjustment adjustment );

    void (__closure *onSelect)( ExpressBarItem *item );
    void (__closure *onUnselect)( ExpressBarItem *item );
    
private:
    void (__closure *onSelectForParent)( ExpressBarItem *item );
    void (__closure *onContentChanged)( ExpressBarItem *item );
    void setState( bool select );
    void setParent( TScrollBox *parent );
    void setTop( int top );
    void setIndex( int index );
    int likedWidth();
    int likedHeight();
    void setWidth( int width );
    void setHeight( int height );
    bool hasIcon();
    void setCursor( TCursor cursor );
    
    void redraw();
    void contentUpdated();
    void drawName( TRect rect );

    void __fastcall OnPaint( TObject* );
    void __fastcall OnMouseDown(
        System::TObject* Sender,
        TMouseButton Button,
        Classes::TShiftState Shift,
        int X,
        int Y
    );

    typedef BOOL __stdcall (* TTranspBlt)
      (HDC hdcDest,        // handle to destination DC
      int nXOriginDest,   // x-coord of destination upper-left corner
      int nYOriginDest,   // y-coord of destination upper-left corner
      int nWidthDest,     // width of destination rectangle
      int hHeightDest,    // height of destination rectangle
      HDC hdcSrc,         // handle to source DC
      int nXOriginSrc,    // x-coord of source upper-left corner
      int nYOriginSrc,    // y-coord of source upper-left corner
      int nWidthSrc,      // width of source rectangle
      int nHeightSrc,     // height of source rectangle
      UINT crTransparent  // color to make transparent
    );

    TTranspBlt TranspBlt;

private:
    AnsiString m_name;
    Graphics::TBitmap* m_bitmap;
    bool m_selected;
    int m_index;
    TPaintBox *m_paintBox;
    IconAdjustment m_iconAdjustment;
    TextAdjustment m_textAdjustment;
    bool m_installed;

    HANDLE hMsimg32Library;
};

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

class ExpressBar
{
public:
    ExpressBar( TPanel *parent );
    ~ExpressBar();

    unsigned int count();
    ExpressBarItem* item( unsigned int index );

    void add( ExpressBarItem *item );
    void addSeporator();
    void remove( unsigned int index );

    ExpressBarItem* selected();
    void setDefaultIcon( Graphics::TBitmap *bitmap );

    void update();
private:
    void refresh();
    void onSelect( ExpressBarItem *item );
    
    void setIndexes();

private:
    typedef std::vector<ExpressBarItem*> Items;
    typedef std::vector<std::pair< TPanel*, int > > Seporators;
    Items m_items;
    Seporators m_seporators;
    Graphics::TBitmap *m_defaultIcon;
    TScrollBox *m_scrollBox;
    ExpressBarItem *m_selected;
    
private:
    int startSeporator();
    int nextSeporator();
    TPanel* seporator();
    Seporators::iterator m_currentSeporator;

};

#endif