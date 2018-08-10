//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

#pragma hdrstop

//my
#include "CommSets.h"

#include "uAppSetsDlg.h"
#include "DrawGridCell.h"
#include "SGrdHlpr.h"
#include "AnsiStringUtils_.h"
#include "bcd_.h"

//#include "Unit1.h"

//vcl
#include <Math.hpp>
#include <FileCtrl.hpp>
#include <dialogs.hpp>
#include "MyVclUtils_.h"
#include "xmlUtils_.h"
#include "MyAppProfile.h"
#include "mywinsock2.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormAppSetsDlg *FormAppSetsDlg;
//---------------------------------------------------------------------------
#define MY_DYNAMIC_CAST_(idnm, clnm, arg ) clnm idnm = dynamic_cast<clnm>(arg)
//---------------------------------------------------------------------------
#define EXECUTE_MY_COLOR_DIALOG_(prop)\
    boost::shared_ptr<TColorDialog> dlg ( new TColorDialog(NULL) );\
    dlg->Color =##prop##;\
    if( !dlg->Execute() ) return;##prop##=dlg->Color;\
    static_cast<TPanel*>(Sender)->Color =dlg->Color;


    #define INT_ORDERED_FOO_( nn, mmin, mmax) \
    bool Is##nn##(const AnsiString& s)\
    {\
	    const MyInt v = MyStrToInt( s );\
        return v.second && v.first>=mmin && v.first<=mmax;\
    }

    INT_ORDERED_FOO_(ModbusAddr,1,127);
    INT_ORDERED_FOO_(Unsigned,0,MAX_BCD_FLOAT);
    INT_ORDERED_FOO_(TimeOut,100,5000000000000);
    INT_ORDERED_FOO_(WriteDelay,0,20000000);

    INT_ORDERED_FOO_(SilentTime,5,10000);
    INT_ORDERED_FOO_(MaxFrameSize,100, 1000000);

//---------------------------------------------------------------------------
bool Is_1_2(const AnsiString& s)
    {
	    const MyInt v = MyStrToInt( s );
        return v.second && (v.first==1 || v.first==2);
    }


//---------------------------------------------------------------------------
__fastcall TFormAppSetsDlg::TFormAppSetsDlg(TComponent* Owner)
	: TForm(Owner),
    xml_( TiXML::GiveNotNulElement( ProfileXmlElem(), "Диалог_настройки_приложения" ) ),
    activated_(false), itfcChngd_(false), drwGrd(grd1)

{
	// имена СОМ-портов системы
    const std::vector<AnsiString>& ports = my::RS232::SystemPorts();


    for( unsigned i = 0; i<ports.size(); ++i )
    	cbPortName->Items->Add( ports[i] );

    // скорость обмена
    const unsigned myBRs[12] = { CBR_1200, CBR_2400, CBR_4800, CBR_9600, CBR_14400, CBR_19200,
            CBR_38400, CBR_56000, CBR_57600, CBR_115200, CBR_128000, CBR_256000 };
    for( unsigned i=0; i<12; ++i ) cbCommBR->Items->Add(myBRs[i]);

    drwGrd.SetAfterSetCanvasFoo( &AfterSetCanvasGrd );
    drwGrd.SetOnDeterminateColumnWidthFoo( &OnDeterminateColumnWidth );


    using namespace AppSetsGrdItems;

    grd1->RowCount = rowCount;
    grd1->Cells[1][0] = "Примечание";
    grd1->Cells[2][0] = "Значение";
    my::Grd::SetColumn( grd1, 0, ttls, ttls+rowCount);

    Height = rowCount*grd1->DefaultRowHeight + 100;

    using namespace Row;
    StringgridControl::RegisterRowControl( cbChanal, grd1, ConnectChanal );
    StringgridControl::RegisterRowControl( cbPortName, grd1, COM );
    StringgridControl::RegisterRowControl( cbCommBR, grd1, COMBoudRate );
    StringgridControl::RegisterRowControl( cbDetectorType1, grd1, Detector1 );
    StringgridControl::RegisterRowControl( cbDetectorType1, grd1, Detector2 );
    StringgridControl::RegisterRowControl( edPath, grd1, Path );
    StringgridControl::RegisterOneEditControl( edGrd, grd1  );
    StringgridControl::RegisterRowControl( ComboBox1, grd1, EnableReadDetectorIndication );
    StringgridControl::RegisterRowControl( ComboBox1, grd1, UseProxi );
    StringgridControl::RegisterRowControl( ComboBox1, grd1, UseFTP );

    StringgridControl::RegisterRowControl( ComboBox1, grd1, EnableWarningSound );


    grd1->Cells[1][COMBoudRate] = "бод";
    grd1->Cells[1][ModbusAddy] = "1..127";
    grd1->Cells[1][Timeout] = "мс";
    grd1->Cells[1][SilentTime] = "мс";
    grd1->Cells[1][WaitDelay] = "мс";
    grd1->Cells[1][RXDFrameSize] = "байт";
    grd1->Cells[1][DiskreteTime] = "мс";

    grd1->Cells[1][DurHtg1] = "мин";




    correctFooMap_[ModbusAddy] = IsModbusAddr;
    correctFooMap_[Timeout] = IsTimeOut;
    correctFooMap_[WaitDelay] = IsWriteDelay;
    correctFooMap_[TCPPort] = IsUnsigned;
    correctFooMap_[IP] =IsInetAddr;

    correctFooMap_[SilentTime] = IsSilentTime;
    correctFooMap_[RXDFrameSize] = IsMaxFrameSize;

    correctFooMap_[DiskreteTime] = IsUnsigned;
    correctFooMap_[DetectorsCount] = Is_1_2;

    ;
}
//---------------------------------------------------------------------------
void __fastcall TFormAppSetsDlg::FormActivate(TObject *Sender)
{
	if( activated_ ) return;
    activated_ = true;
    itfcChngd_ = false;
    TiXML::LoadWindowPlacement( TiXML::GiveNotNulElement( xml_, "Placement" ), Handle );
    my::Grd::SetColWidthsAsStr(grd1, 	READ_XML_STR_( "ширина_колонок", "") );
}
//------------------------------------------------------------------------------
__fastcall TFormAppSetsDlg::~TFormAppSetsDlg()
{
    StringgridControl::UnregisterAll( this );
    if( activated_ ) TiXML::SaveWindowPlacement( TiXML::GiveNotNulElement( xml_, "Placement" ), Handle );
    WRITE_XML_STR_( "ширина_колонок", my::Grd::ColWidthsToStr(grd1) );
    ProfileXmlDoc()->SaveFile();

}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void TFormAppSetsDlg::AfterSetCanvasGrd( TStringGrid* grd, int col, int row,
	MyGridCellState drawState, TRect rect, int& txtX, int& txtY )
{
    if( IsLeftAligmentAndNoAutoColWidthCell(grd, col, row) ) txtX = rect.left + 3;

	TCanvas &cnv = *grd->Canvas;
    if ( col<2 )
        cnv.Brush->Color  = grd->FixedColor;

    if ( row==0 )
    {
        cnv.Brush->Color  = 0x00DDDDDD;
        cnv.Font->Size  = grd->Font->Size+1;
    }

    if ( drawState & mygdHot )
        cnv.Font->Color  = clNavy;

    if ( col>1 && drawState & mygdSelected )
    {
        cnv.Font->Color  = clWhite;
        cnv.Brush->Color = clMenuHighlight;
    }
}
//------------------------------------------------------------------------------
void TFormAppSetsDlg::OnDeterminateColumnWidth( TStringGrid* grd, int col, int row, bool& ff)
{
    ff = !IsLeftAligmentAndNoAutoColWidthCell( grd, col, row );
}
//------------------------------------------------------------------------------
bool TFormAppSetsDlg::IsLeftAligmentAndNoAutoColWidthCell( TStringGrid* grd, int col, int row )
{
    return true;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void __fastcall TFormAppSetsDlg::Button1Click(TObject *Sender)
{
	ModalResult = mrOk;	
}
//------------------------------------------------------------------------------
void __fastcall TFormAppSetsDlg::Button2Click(TObject *Sender)
{
	//std::fill( prmToCtrl_, prmToCtrl_+HROMAT_NUMBER_PARAM_COUNT, (TWinControl*)NULL );
	ModalResult = mrCancel;
}
//------------------------------------------------------------------------------
void __fastcall TFormAppSetsDlg::rgInterfaceClick(TObject *Sender)
{
	itfcChngd_ = true;
}
//------------------------------------------------------------------------------
void __fastcall TFormAppSetsDlg::CheckListBox1ClickCheck(TObject *Sender)
{
	TCheckListBox& clb = *static_cast<TCheckListBox*>(Sender);
    if( clb.Checked[ clb.ItemIndex ] ) return;
	for( int i=0; i<clb.Items->Count; ++i )
    	if( clb.Checked[i] ) return;
    clb.OnClickCheck = NULL;
    clb.Checked[ clb.ItemIndex ] = true;
    clb.OnClickCheck = CheckListBox1ClickCheck;
}
//------------------------------------------------------------------------------

void __fastcall TFormAppSetsDlg::grd1SelectCell(TObject *Sender, int ACol,
      int ARow, bool &CanSelect)
{
    my::Grd::SetEditing(grd1, ARow>0 && ACol>1);
}
//---------------------------------------------------------------------------

void __fastcall TFormAppSetsDlg::grd1SetEditText(TObject *Sender, int ACol,
      int ARow, const AnsiString Value)
{
    int a = 0;
}
//---------------------------------------------------------------------------


void __fastcall TFormAppSetsDlg::edGrdExit(TObject *Sender)
{   
    TEdit* ed = static_cast<TEdit*>(Sender);
    const AnsiString s = ed->Text;
    CorrectFooMap::iterator i = correctFooMap_.find(grd1->Row);
    if( i==correctFooMap_.end() ) return;
    IsStrCorrectFoo checker = i->second;
    if( !checker( ed->Text ) )
    {
        ShowUserInputError( grd1->Cells[0][grd1->Row], s);
        ed->Visible = 1;
        ActiveControl = ed;
    }
}
//---------------------------------------------------------------------------

void __fastcall TFormAppSetsDlg::edPathEnter(TObject *Sender)
{
    AnsiString dir = edPath->Text;
    if( SelectDirectory("Путь к файлам хроматограмм", "", dir) )
        edPath->Text = dir;
}
//---------------------------------------------------------------------------
void __fastcall TFormAppSetsDlg::cbChanalKeyDown(TObject *Sender,
      WORD &Key, TShiftState Shift)
{
    TWinControl *ctrl = dynamic_cast<TWinControl*>(Sender);
    TStringGrid* grd = StringgridControl::GetActiveGrid();
    const bool
        escapeKey = Key==VK_ESCAPE,
        enterKey  = Key==VK_RETURN
    ;

    if( escapeKey )
    {
        const TNotifyEvent onExit = GetCtrlOnExit( ctrl );
        SetCtrlOnExit( ctrl, NULL );
        ctrl->Visible = 0;
        SetCtrlOnExit( ctrl, onExit );
        grd->Cells[grd->Col][grd->Row] = StringgridControl::GetStoredValue(ctrl);

    }
    if( escapeKey || enterKey )
        ActiveControl = grd;
}
//---------------------------------------------------------------------------



