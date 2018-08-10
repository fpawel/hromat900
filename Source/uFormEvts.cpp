//---------------------------------------------------------------------------
#include <values>
//---------------------------------------------------------------------------
#pragma hdrstop
//---------------------------------------------------------------------------
#include "uFormEvts.h"
#include "my_array_size.h"
#include "SGrdHlpr.h"
#include "myGeomtr.h"
#include "AppEvts.h"
#include "iniUtils_.h"
#include "xmlUtils_.h"
#include "MyAppProfile.h"

//vcl
#include <Dialogs.hpp>

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
//---------------------------------------------------------------------------
AnsiString EvtFldToStr( const MyEvt& evt, const unsigned col )
{
    switch(col)
    {
        case 1: return DateToStr(evt.date_);
        case 2: return TimeToStr(evt.date_);
        case 3: return evt.id_;
        case 4: return evt.content_;
        case 5: return evt.note_;
        default:return "";
    };
}


unsigned DrawCellText( TCanvas& cnv, const TRect& rect, AnsiString Sentence )
{
    const int txtH = cnv.TextHeight(Sentence);
	//{ Рисуем подложку цветом ячейки }
	//cnv.Rectangle(rect.Left-1, rect.Top-1, rect.Right+1, rect.Bottom+1);
    cnv.FillRect(rect);
	//{ Начинаем рисование с верхнего левого угла ячейки }
	int CurX = rect.Left, CurY = rect.Top;

	//{ для каждого слова ячейки }
	bool EndOfSentence = false;
    unsigned lineNo = 0;
	while (!EndOfSentence)
	{
		AnsiString CurWord;
		// { для получения следующего слова ищем пробел }
		const int SpacePos = Sentence.Pos(' ');
		if (SpacePos > 0)
		{
			//{ получаем текущее слово плюс пробел }
			CurWord = Sentence.SubString(0, SpacePos);
			//{ получаем остальную часть предложения }
			Sentence  = Sentence.SubString(SpacePos + 1, Sentence.Length() - SpacePos);
		} else
		{// { это - последнее слово в предложении }
			EndOfSentence = true;
			CurWord = Sentence;
		}
		const int txtW = cnv.TextWidth(CurWord);
		//{ если текст выходит за границы СТОЛБЦА }
		if ( (txtW + CurX+3) > rect.Right && lineNo>0 )
		{// { переносим на следующую строку }
			CurY += txtH;
			CurX = rect.Left;
		}
		//{ выводим слово }
		cnv.TextOut(CurX+3, CurY+3, CurWord);
		//{ увеличиваем X-координату курсора }
		CurX += txtW;
        ++lineNo;
	}
    
    return CurY;
}



//---------------------------------------------------------------------------
TFormEvts *FormEvts;
//---------------------------------------------------------------------------
__fastcall TFormEvts::TFormEvts(TComponent* Owner)
    : TForm(Owner),
    xml_( TiXML::GiveNotNullElementByTag(ProfileXmlElem(), EVENTS_JOURNAL_NODE ) ),

    connectOnUpdateEvtsJournal_
    ( ConnectOnUpdateEvtsJournal( &OnUpdateEvtsJournal ) )
{
    const char *ttls[] = {
        "№",
        "Дата",
        "Время",
        "Тип",
        "Содержание",
        "Примечание"
    };
    const unsigned cc = array_size(ttls);
    grdEvts->ColCount = cc;
    my::Grd::SetRow( grdEvts, 0, ttls, ttls+cc);



    for( int i=1; i<6; ++i )
        READ_XML_INT_VAL_( EVTS_JRNL_FORM_COLWIDTH+IntToStr(i),
        grdEvts->ColWidths[i], 200, 800 );


}
//---------------------------------------------------------------------------
__fastcall TFormEvts::~TFormEvts()
{
    for( int i=1; i<6; ++i )
        WRITE_XML_INT_( EVTS_JRNL_FORM_COLWIDTH+IntToStr(i), grdEvts->ColWidths[i] );

    if(isActivated_)
    	TiXML::SaveWindowPlacement( xml_, Handle );
}
//---------------------------------------------------------------------------
void __fastcall TFormEvts::FormActivate(TObject *Sender)
{
    SetWindowPos(Handle, HWND_TOPMOST, Left, Top, Width, Height, 0	);
    if( !isActivated_ )
    {
        TiXML::LoadWindowPlacement( xml_, Handle );
        isActivated_ = true;
    }
    OnUpdateEvtsJournal();
}
//---------------------------------------------------------------------------
void TFormEvts::OnUpdateEvtsJournal()
{
    const MyEvts& evts = GetMyEvtsJournal();
    const unsigned sz = evts.size();
    grdEvts->RowHeights[0] = 18;
    grdEvts->RowCount = sz+1;
    if(sz==0) return;
    grdEvts->Row = grdEvts->RowCount-1;
    grdEvts->FixedRows = 1;
    if(grdEvts->VisibleRowCount< (int)sz-1 )
        grdEvts->TopRow = sz+1 - grdEvts->VisibleRowCount;

}
//---------------------------------------------------------------------------

void __fastcall TFormEvts::grdEvtsDrawCell(TObject *Sender, const int col,
      const int row, TRect &rect, TGridDrawState State)
{
    TStringGrid *grd = grdEvts;
    const my::Grd::Cell cell(grd, col, row);
    const bool isFixed = my::Grd::IsCellFixed(cell);

    #define CNV grd->Canvas

    CNV->Brush->Assign( grd->Brush );
    CNV->Font->Assign( grd->Font );
    CNV->Brush->Color = isFixed ? grd->FixedColor : grd->Color;
    #pragma warn -8018
    CNV->Pen->Color   =    0x00BCBCBC;
    #pragma warn +8018
    CNV->Pen->Width = -1;
    CNV->Font->Color = isFixed ? clBlack : clNavy;


    const unsigned evtN = row-1;

    AnsiString cellTxt = grd->Cells[col][row];
    if( col==0 && row!=0)
        cellTxt = row;

    const MyEvts& evts = GetMyEvtsJournal();

    if( col>0 && evtN<evts.size() )
    {
        cellTxt = EvtFldToStr( evts[evtN], col );
        if(evts[evtN].id_.Pos("Ошибка")>0)
        {
            CNV->Font->Color = clRed;
            CNV->Brush->Color = clWhite;
        } else
        if( evts[evtN].id_.Pos("Запуск приложения")>0 || evts[evtN].id_.Pos("Выход из приложения")>0 )
        {
            CNV->Font->Color = clBlue;
            //CNV->Font->Style = TFontStyles() << fsBold;
            CNV->Brush->Color = clInfoBk;
        }
    }

    // параметры
    const int
		textW = CNV->TextWidth( cellTxt ),   	//ширина текста
		textH  = CNV->TextHeight( cellTxt ); 	//высота текста

    // позиция левого верхнего угла прямоугольника с текстом
    const TPoint txtLT = GetCenterPosition( rect, textW, textH );
    int txtX = txtLT.x, txtY = txtLT.y;
    if( col>3 ) txtX = rect.Left + 3;

    //автоподбор ширины колонки
    if( grd->ColWidths[col]>0 && (col<3 || row==0 ) && textW+5>grd->ColWidths[col] )
    {
	    grd->ColWidths[col] = textW+5;
        return;
    }

    // вывод текста
	if( col<3 || row==0 )
    {
        CNV->TextRect( rect, txtX,  txtY,  cellTxt	);
    }
    else
    {
        const int allHeight = DrawCellText( *CNV, rect, cellTxt ) - rect.Top + grd->DefaultRowHeight;
        if( allHeight>grd->RowHeights[row] )
			grd->RowHeights[row] = allHeight;
    }

    // обрамление
    CNV->MoveTo( rect.Left, rect.Bottom);
    CNV->LineTo(rect.Left, rect.Top);
    CNV->LineTo(rect.Right-1, rect.Top);
    if( col==my::Grd::GetLastVisCol(grd) )
        CNV->LineTo(rect.Right-1, rect.Bottom);
    if(row==my::Grd::GetLastVisRow(grd) )
    {
        CNV->MoveTo(rect.Right, rect.Bottom-1);
        CNV->LineTo(rect.Left, rect.Bottom-1);
    }
	#undef CNV
    grd->DefaultDrawing  = !isFixed;
}

//---------------------------------------------------------------------------


void __fastcall TFormEvts::grdEvtsMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
    my::Grd::SetupVisibleColsOnMouseDownRow0(Sender, Button, Shift, X, Y,0 );    
}
//---------------------------------------------------------------------------

