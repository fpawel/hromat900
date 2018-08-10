#include "express_bar.h"
#include <assert>

const unsigned int MARGIN_SIZE = 6;
const unsigned int SEPORATOR_HEIGHT = 2;

const TColor SELECT_COLOR = (TColor)RGB( 0xbb, 0xe4, 0xe8 );
const TColor MOUSE_FOCUS_COLOR = (TColor)RGB( 0xb9, 0xe9, 0xff );

const int crWindowsHandCursor = 99;

ExpressBarItem::ExpressBarItem()
:   m_paintBox( NULL ),
    m_selected( false ),
    m_bitmap( NULL ),
    m_name( "noname" ),
    m_installed( false ),
    m_iconAdjustment( IconAdjustment_Top ),
    m_textAdjustment( TextAdjustment_Center ),
    onSelect( NULL ),
    onUnselect( NULL ),
    onSelectForParent( NULL ),
    onContentChanged( NULL ),
    m_index( -1 )
{
    m_paintBox = new TPaintBox( (TComponent*)NULL );
    m_paintBox->OnPaint = OnPaint;
    m_paintBox->OnMouseDown = OnMouseDown;
    m_bitmap = new Graphics::TBitmap();

    hMsimg32Library=LoadLibrary("msimg32.dll");
    if(hMsimg32Library == NULL) {ShowMessage("Не найдена библиотека msimg32.dll."); return;}
    TranspBlt=(TTranspBlt)GetProcAddress(hMsimg32Library, "TransparentBlt");
    if(TranspBlt == NULL) {ShowMessage("Не найдена функция 'TransparentBlt'."); return;}
}

ExpressBarItem::~ExpressBarItem()
{
    if ( m_paintBox )
    {
        m_paintBox->OnPaint = NULL;
        m_paintBox->OnMouseDown = NULL;
        m_paintBox->Parent = NULL;
        delete m_paintBox;
    }
    if ( m_bitmap )
        delete m_bitmap;

    FreeLibrary( hMsimg32Library );
}

void ExpressBarItem::setName( AnsiString name )
{
    m_name = name;
    contentUpdated();
    redraw();
}

AnsiString ExpressBarItem::name()
{
    return m_name;
}

void __fastcall ExpressBarItem::OnPaint( TObject* )
{
    TCanvas *canvas = m_paintBox->Canvas;
    canvas->Pen->Color = clWhite;
    canvas->Brush->Color = clWhite;
    canvas->Font->Color = clBlack;
    if ( m_selected )
    {
        canvas->Pen->Color = clBlue;
        canvas->Brush->Color = SELECT_COLOR;
    }

    canvas->Rectangle( 0, 1, m_paintBox->Width, m_paintBox->Height - 1 );

    TRect textRect;
    m_bitmap->Transparent = true;
    switch( m_iconAdjustment )
    {
        case IconAdjustment_Top:
            /*canvas->Draw( 
                m_paintBox->Width / 2 - m_bitmap->Width / 2,
                MARGIN_SIZE,
                m_bitmap
            );*/
            TranspBlt(
                canvas->Handle,
                m_paintBox->Width / 2 - m_bitmap->Width / 2,
                MARGIN_SIZE,
                m_bitmap->Width,
                m_bitmap->Height,
                m_bitmap->Canvas->Handle,
                0,
                0,
                m_bitmap->Width,
                m_bitmap->Height,
                m_bitmap->TransparentColor
            );
            textRect = TRect( 
                MARGIN_SIZE, 
                MARGIN_SIZE + m_bitmap->Height + MARGIN_SIZE,
                m_paintBox->Width - MARGIN_SIZE,
                m_paintBox->Height - MARGIN_SIZE
            );
            break;

        case IconAdjustment_Bottom:
            /*canvas->Draw(
                m_paintBox->Width / 2 - m_bitmap->Width / 2,
                m_paintBox->Height - MARGIN_SIZE - m_bitmap->Height,
                m_bitmap
            );*/

            TranspBlt(
                canvas->Handle,
                m_paintBox->Width / 2 - m_bitmap->Width / 2,
                m_paintBox->Height - MARGIN_SIZE - m_bitmap->Height,
                m_bitmap->Width,
                m_bitmap->Height,
                m_bitmap->Canvas->Handle,
                0,
                0,
                m_bitmap->Width,
                m_bitmap->Height,
                m_bitmap->TransparentColor
            );

            textRect = TRect(
                MARGIN_SIZE,
                MARGIN_SIZE,
                m_paintBox->Width - MARGIN_SIZE,
                m_paintBox->Height - m_bitmap->Height - MARGIN_SIZE * 2
            );
            break;

        case IconAdjustment_Left:
            /*canvas->Draw(
                MARGIN_SIZE,
                m_paintBox->Height / 2 - m_bitmap->Height / 2,
                m_bitmap
            );*/

            TranspBlt(
                canvas->Handle,
                MARGIN_SIZE,
                m_paintBox->Height / 2 - m_bitmap->Height / 2,
                m_bitmap->Width,
                m_bitmap->Height,
                m_bitmap->Canvas->Handle,
                0,
                0,
                m_bitmap->Width,
                m_bitmap->Height,
                m_bitmap->TransparentColor
            );

            textRect = TRect(
                MARGIN_SIZE * 2 + m_bitmap->Width,
                MARGIN_SIZE,
                m_paintBox->Width - MARGIN_SIZE,
                m_paintBox->Height - MARGIN_SIZE
            );
            break;

        case IconAdjustment_Right:
            /*canvas->Draw(
                m_paintBox->Width - MARGIN_SIZE - m_bitmap->Width,
                m_paintBox->Height / 2 - m_bitmap->Height / 2,
                m_bitmap
            );*/
            TranspBlt(
                canvas->Handle,
                m_paintBox->Width - MARGIN_SIZE - m_bitmap->Width,
                m_paintBox->Height / 2 - m_bitmap->Height / 2,
                m_bitmap->Width,
                m_bitmap->Height,
                m_bitmap->Canvas->Handle,
                0,
                0,
                m_bitmap->Width,
                m_bitmap->Height,
                m_bitmap->TransparentColor
            );

            textRect = TRect(
                MARGIN_SIZE,
                MARGIN_SIZE,
                m_paintBox->Width - m_bitmap->Width - MARGIN_SIZE * 2,
                m_paintBox->Height - MARGIN_SIZE
            );
            break;
    }
    drawName( textRect );
}

void ExpressBarItem::redraw()
{
    if ( !m_installed )
        return;
    m_paintBox->Refresh();
}

bool ExpressBarItem::hasIcon()
{
    return 0 != m_bitmap->Width && 0 != m_bitmap->Height; 
}

void ExpressBarItem::drawName( TRect textRect )
{
    TCanvas *canvas = m_paintBox->Canvas;
    switch( m_textAdjustment )
    {
        case TextAdjustment_Left:
            canvas->TextRect( 
                textRect, 
                textRect.Left, 
                textRect.Top + textRect.Height() / 2 - canvas->TextHeight( m_name ) / 2,
                m_name
            );
            break;

        case TextAdjustment_Right:
            canvas->TextRect(
                textRect,
                textRect.Left + textRect.Width() - canvas->TextWidth( m_name ),
                textRect.Top + textRect.Height() / 2 - canvas->TextHeight( m_name ) / 2,
                m_name
            );
            break;

        case TextAdjustment_Center:
            canvas->TextRect(
                textRect,
                textRect.Left + textRect.Width() / 2 - canvas->TextWidth( m_name ) / 2,
                textRect.Top + textRect.Height() / 2 - canvas->TextHeight( m_name ) / 2,
                m_name
            );
            break;
    }
}

void ExpressBarItem::setIcon( Graphics::TBitmap* bitmap )
{
    if ( bitmap )
    {
        m_bitmap->Assign( bitmap );
        contentUpdated();
        redraw();
    }
}

bool ExpressBarItem::selected()
{
    return m_selected;
}

void ExpressBarItem::select()
{
    setState( true );
}

void ExpressBarItem::unselect()
{
    setState( false );
}

void ExpressBarItem::setState( bool select )
{
    m_selected = select;
    if ( m_selected )
    {
        if ( onSelect )
            onSelect( this );
        if ( onSelectForParent )
            onSelectForParent( this );
    }
    else
    {
        if ( onUnselect )
            onUnselect( this );
    }
    redraw();
}

void ExpressBarItem::setIconAdjustment( IconAdjustment adjustment )
{
    m_iconAdjustment = adjustment;
}

void ExpressBarItem::setNameAdjustment( TextAdjustment adjustment )
{
    m_textAdjustment = adjustment;
}

void ExpressBarItem::setParent( TScrollBox *parent )
{
    m_paintBox->Parent = parent;
    m_paintBox->Align = alTop;
    m_paintBox->Canvas->Font->Assign( parent->Font );
    m_installed = true;
}

void ExpressBarItem::setTop( int top )
{
    m_paintBox->Top = top;
}

void ExpressBarItem::contentUpdated()
{
    if ( onContentChanged )
        onContentChanged( this );
}

int ExpressBarItem::likedWidth()
{
    int width = 0;
    switch( m_iconAdjustment )
    {
        case IconAdjustment_Left:
        case IconAdjustment_Right:
            width = m_bitmap->Width + m_paintBox->Canvas->TextWidth( m_name ) + MARGIN_SIZE * 3;
            break;

        case IconAdjustment_Top:
        case IconAdjustment_Bottom:
            width = std::max( m_bitmap->Width, m_paintBox->Canvas->TextWidth( m_name ) ) + MARGIN_SIZE * 3;
    }
    return width;
}

int ExpressBarItem::likedHeight()
{
    int height = 0;
    switch( m_iconAdjustment )
    {
        case IconAdjustment_Left:
        case IconAdjustment_Right:
            height = std::max( m_bitmap->Height, m_paintBox->Canvas->TextHeight( m_name ) ) + MARGIN_SIZE * 3;
            break;

        case IconAdjustment_Top:
        case IconAdjustment_Bottom:
            height = m_bitmap->Height + m_paintBox->Canvas->TextHeight( m_name ) + MARGIN_SIZE * 3;
            break;
    }
    return height;
}

void __fastcall ExpressBarItem::OnMouseDown(
    System::TObject* ,
    TMouseButton Button,
    Classes::TShiftState /*Shift*/,
    int /*X*/,
    int /*Y*/
)
{
    if ( Button == mbLeft )
    {
        setState( !selected() );
    }
}

void ExpressBarItem::setWidth( int width )
{
    m_paintBox->Width = width;
}

void ExpressBarItem::setHeight( int height )
{
    m_paintBox->Height = height;
}

void ExpressBarItem::setIndex( int index )
{
    m_index = index;
}

int ExpressBarItem::index()
{
    return m_index;
}

void ExpressBarItem::setCursor( TCursor cursor )
{
    m_paintBox->Cursor = cursor;
}
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
ExpressBar::ExpressBar( TPanel *parent )
:   m_scrollBox( NULL ),
    m_selected( NULL )
{
    assert( parent );

    m_scrollBox = new TScrollBox( (TComponent*)NULL );
    m_scrollBox->BorderStyle = bsNone;
    m_scrollBox->Color = clWhite;
    m_scrollBox->Parent = parent;
    m_scrollBox->Align = alClient;

    HCURSOR handCursor = LoadCursor( NULL, IDC_HAND );
    Screen->Cursors[ crWindowsHandCursor ] = handCursor;

    m_defaultIcon = new Graphics::TBitmap();
}

ExpressBar::~ExpressBar()
{
    for ( Items::iterator it = m_items.begin();
          it != m_items.end();
          ++it )
    {
        ExpressBarItem *item = *it;
        delete item;
    }
    m_items.clear();

    if ( m_defaultIcon )
        delete m_defaultIcon;
}

ExpressBarItem* ExpressBar::item( unsigned int index )
{
    ExpressBarItem *result = NULL;
    if ( index < m_items.size() )
        result = m_items.at( index );
    return result;
}

void ExpressBar::add( ExpressBarItem *item )
{
    if ( item )
    {
        if ( !item->hasIcon() )
            item->setIcon( m_defaultIcon );
        item->setCursor( (TCursor)crWindowsHandCursor );
        item->onSelectForParent = onSelect;
        m_items.push_back( item );
        setIndexes();
        item->setParent( m_scrollBox );
        refresh();
    }
}

void ExpressBar::setIndexes()
{
    for ( size_t index = 0; index < m_items.size(); ++index )
    {
        m_items.at( index )->setIndex( index );
    }
}

void ExpressBar::remove( unsigned int index )
{
    ExpressBarItem *mustDie = item( index );
    if ( mustDie )
    {
        delete mustDie;
        m_items.erase( m_items.begin() + index );
        setIndexes();
    }
}

void ExpressBar::refresh()
{
    int height = 0;

    for ( size_t i = 0; i < m_items.size(); ++i )
    {
        ExpressBarItem *current = m_items.at( i );
        current->setParent( m_scrollBox );
        int currentHeight = current->likedHeight();
        if ( height < currentHeight )
            height = currentHeight;
    }

    size_t seporatorIdx = startSeporator();

    int top = 0;
    for ( size_t i = 0; i < m_items.size(); ++i )
    {
        ExpressBarItem *current = m_items.at( i );
        current->setHeight( height );
        current->setTop( top );
        top += height;

        if ( i == seporatorIdx )
        {
            TPanel *sep = seporator();
            sep->Top = top;
            top += sep->Height;
            seporatorIdx = nextSeporator();
        }
    }
}

ExpressBarItem* ExpressBar::selected()
{
    return m_selected;
}

void ExpressBar::setDefaultIcon( Graphics::TBitmap *bitmap )
{
    m_defaultIcon->Assign( bitmap );
}

void ExpressBar::update()
{
    for ( Items::iterator it = m_items.begin();
          it != m_items.end();
          ++it )
    {
        (*it)->redraw();
    }
}

void ExpressBar::onSelect( ExpressBarItem *item )
{
    if ( m_selected == item )
        return;
    /*if ( m_selected )
    {
        m_selected->setState( false );
    }*/
    m_selected = item;
}

unsigned int ExpressBar::count()
{
    return m_items.size();
}

void ExpressBar::addSeporator()
{
    TPanel *sepor = new TPanel( m_scrollBox );
    sepor->Parent = m_scrollBox;
    sepor->Align = alTop;
    sepor->Height = SEPORATOR_HEIGHT;

    m_seporators.push_back( Seporators::value_type( sepor, m_items.size() - 1 ) );
}

int ExpressBar::startSeporator()
{
    m_currentSeporator = m_seporators.begin();
    int index = -1;
    if ( m_currentSeporator != m_seporators.end() )
    {
        index = (*m_currentSeporator).second;
    }
    return index;
}

int ExpressBar::nextSeporator()
{
    int index = -1;
    if ( m_currentSeporator != m_seporators.end() )
        ++m_currentSeporator;
    if ( m_currentSeporator != m_seporators.end() )
    {
        index = (*m_currentSeporator).second;
    }
    return index;
}

TPanel* ExpressBar::seporator()
{
    return (*m_currentSeporator).first;
}
