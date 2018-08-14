//------------------------------------------------------------------------------
#include <vcl.h>
#include <fstream>
#include <math>
//------------------------------------------------------------------------------
#pragma hdrstop
//------------------------------------------------------------------------------
#include "Unit1.h"

#include "MyIostream.h"
#include "hromatgas.hpp"

//vcl
#include <FileCtrl.hpp>
#include <DateUtils.hpp>

#include "MyAppProfile.h"
#include "MyAppWinPath.hpp"

#include "DevSys\ReportContext.hpp"

#include "uFormHromatogramm.h"
#include "MyVclUtils_.h"

#include "iniUtils_.h"
#include "my_array_size.h"
#include "SGrdHlpr.h"
#include "bin_nums.h"

#include "AnsiStringUtils_.h"
#include "uAppSetsDlg.h"
#include "uFormEvts.h"
#include "MyProcEntry.h"
#include "uDrawGrids.h"

#include "xmlUtils_.h"
#include "..\my_include\FooCalledOnce.hpp"
#include "..\MY_INCLUDE\win\MyModFN.hpp"

#include "myconv.h"
#include "myformat.hpp"


// Хромат 900
#include "hromat_sys.h"
#include "HromatPhysChem.h"
#include "TransferManage.h"
#include "htg_.h"
#include "guicon.h"
#include "uFrmWait.h"
#include "AppEvts.h"
#include "grdtns_.h"
#include "uFormAbout.h"
#include "cnfdntl_.h"
#include "uTestCalcHromatSatat.h"
#include "winrc.h"
#include "MyFileCtrl.h"
#include "my_draw_.h"
#include "TaskList.h"
#include "uFrameDropDownPanel.h"
#include "HromatConsoleInput.h"
#include "HromatTask.h"
#include "MyModFN.hpp"
#include "uFormArchiveView.h"
#include "uFormGraduate.h"
#include "HromatHtmlReport.h"
#include "uFormIzobutChart.h"
#include "CurHtg.h"



//------------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "SHDocVw_OCX"
#pragma resource "*.dfm"
//------------------------------------------------------------------------------
TForm1 *Form1;
//------------------------------------------------------------------------------
#define EXECUTE_MY_COLOR_DIALOG_(prop)\
    boost::shared_ptr<TColorDialog> dlg ( new TColorDialog(NULL) );\
    dlg->Color = FrmHTG[0]->##prop##;\
    if( !dlg->Execute() ) return;\
    FrmHTG[0]->##prop##=dlg->Color;\
    FrmHTG[1]->##prop##=dlg->Color;\

using std::hex;
using std::uppercase;
using std::showbase;
using boost::io::ios_all_saver;
//------------------------------------------------------------------------------
AnsiString GetWindowsTempPath()
{
    const unsigned len = ::GetTempPath( 0, NULL );
    if(len==0) return ExtractFilePath( ParamStr(0) );
    std::vector<char> ret(len+1);
    ::GetTempPath( len, &ret.at(0) );
    ret.at(len) = '\0';
    return AnsiString( &ret.at(0) );
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
AnsiString NormalizeStateReport(const AnsiString& inS)
{
	AnsiString ret = "";
    const int len = inS.Length() + 1;
    for( int i=1; i<len; ++i )
    {
    	if( iscntrl(inS[i]) )
        {
        	ret = ret + "\n";
        	continue;
        }
        if( inS[i]=='<' )
        {
        	while( i<len && inS[++i]!='>' );
            ret = ret + "\n";
            continue;
        }
        if( inS[i]=='[' )       
        {
        	while( i<len && inS[i++]!=']' );
            ret = ret + "\n";
            continue;
        }
        ret = ret + AnsiString( inS[i] );
    }
    return ret;
}
//------------------------------------------------------------------------------
template<class T> void FreeAndNULL(T*& p)
{
    delete p;
    p = NULL;
}

//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void* InitializeApplication()
{
	return PVOID();
}


void FinalizeApplication(void*)
{
	ASSERT_FUNCTION_CALLED_ONCE__;
    LOG_FOO_LIFETIME_QUOTES;
}

//------------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
	:
    logLifeTime_( MakeMyLogQuote("Form1.[members]") ),
    firstMemeber_( InitializeApplication(), FinalizeApplication  ),

    TForm(Owner),

    xml_( TiXML::GiveNotNulElement( ProfileXmlElem(), "FORM1") ),

    connectOnTransferManagerReport_
    ( Hromat900::Instance().GetTransferManager().ConnectOnReportState(OnTransferManagerReport) ),

    currentHromatCicleType_(HROMAT_CICLE::COUNT) ,

    MeasureUnits( IniGas()->ReadString("COMMON","units", "моль,%") )
{
	ASSERT_FUNCTION_CALLED_ONCE__;
	LOG_FOO_LIFETIME_QUOTES;
    //DecimalSeparator = '.';

    //testjournals();

    IniGas()->WriteString("COMMON","units", MeasureUnits) ;

    MediaPlayer1->Open();

    // устанавливаем интервал таймера - 1 час
    enum
    {
        defaultBackUpInterval = 60,
    	minimumBackUpInterval = 1,
        maximumBackUpInterval = 120
    };

    TimerBackup->Interval = 30*60000;

    // окно с журналом событий
    FormEvts = new TFormEvts(NULL );
    FormEvts->Visible = 0;

    // окно для просмотра архива хроматографа
    //FormHromatArchiveDlg = new TFormHromatArchiveDlg(NULL );

    // окно "О программе"
    FormAbout = new TFormAbout(NULL);

    MakeGrdPhysChem( grdPhisChem );

    const bool isAdmin = IsAdminAutorized();
    menuUsersControlDialog->Enabled = isAdmin;
    menuSoftIDDialog->Enabled = isAdmin;
    menuSetHromatKey->Enabled = isAdmin;

    AnsiString s;
    s.sprintf("ХРОМАТ-900. Профиль: \"%s\". Пользователь: \"%s\". Тип пользователя: %s",
        GetApplicationProfileId(), AutorizedUser().name.c_str(),
        User::AccessToStr( AutorizedUser().accessLevel) );
    Caption = s;

    const bool myParamStrHasDebug = MyParamStrHas( "-debug" );

    menuDebug->Visible = myParamStrHasDebug;
    menuHromatParams->Enabled = myParamStrHasDebug;


    ::SetParent( GetConsoleWindow(), Handle );
    ::ShowWindow(::GetConsoleWindow(), SW_HIDE );


    // grdToolInfo
    {
    const char *ttls[] = {
        "Адрес",
        "Статус",
        "Цикл",
        "Режим"
    };
    const unsigned rc = array_size(ttls);
    grdToolInfo->RowCount = rc;
    my::Grd::SetColumn( grdToolInfo, 0, ttls, ttls+rc);
        
    }
    grdToolInfo->Cells[1][0] = Hromat900::Instance().GetTransferManager().ModusAddr();

    {
    const char *ttls[] = {
        "Т колонок,\"С",
        "Т оболочки,\"С",
        "Р газа-носителя,кПа",
        "Р пробы, кПа.",
        "Р вспом. газа,кПа"
        };
    const unsigned rc = array_size(ttls);
    grdToolPrms->RowCount = rc;
    my::Grd::SetColumn( grdToolPrms, 0, ttls, ttls+rc);
    }


    {
    const char *ttls[] = {
    	"Uc,B",
        "T,\"C",
        "I,мА",
        "Q г.н., мл/мин",
    	"Хроматограмма",
        "Время создания",
        "Считано от начала",
        "Продолжительность",
        "Время обновления"
    };
    const unsigned rc = array_size(ttls);
    grdDtctr1->RowCount = rc;
    grdDtctr2->RowCount = rc;
    my::Grd::SetColumn( grdDtctr1, 0, ttls, ttls+rc);
    my::Grd::SetColumn( grdDtctr2, 0, ttls, ttls+rc);
    }

    // инициализировать правые панели
    InitializeRightPanels();
    grdPhisChem->FixedCols = 0;
    my::Grd::AddCopyItemToPopupMenu(grdPhisChem);

    // настройка панелей статуса
    pnStatus_[0] = Panel9;
    pnStatus_[1] = Panel10;
    pnStatus_[2] = Panel11;
    pnStatus_[3] = Panel12;
    pnStatus_[4] = Panel2;

    // ширина панелей статуса
    for ( unsigned i=0; i<STATUS_PNLS_COUNT; ++i )
    	READ_XML_INT_VAL_( AnsiString().sprintf("%s_%d",
        STATUS_PANEL_WIDTH,i), pnStatus_[i]->Width, 50, 200  );
    READ_XML_INT_VAL_(RIGHT_PANEL_WIDTH, ScrollBox1->Width, 100, 800);

    // файл с описанием костиковых кодов режима/состояния
    const AnsiString hromatModeFN = GetAppDataDir()+"\\hrmtmd.ini";
    MyExtractFileFromResource( hromatModeFN, "HRMTMD_INI_FILE", false );
    // формировать карту отображения кода режима хроматографа в описание modes_
    // по данным файла hromatModeFN
    boost::shared_ptr<TIniFile> ini( new TIniFile( hromatModeFN ) );
    boost::shared_ptr<TStringList> skts( new TStringList );
    ini->ReadSections( skts.get() );

    for( int i=0; i<skts->Count; ++i )
    {
        const unsigned short code = ini->ReadInteger( skts->Strings[i], "Код", 0xFFFF );
        if( code==0xFFFF ) continue;
        HromatModeMessage msg;
        msg.s = ini->ReadString( skts->Strings[i], "Текст", "" );
        msg.isNorm = ini->ReadBool( skts->Strings[i], "Норма", 1 );
        modes_[code] = msg;
    }

    // кнопки установки цикла
    #define SETT__(nn,ss) btnCicle[nn] = btnCicle##ss##; imgCicle[nn] = imgCicle##ss##;
    SETT__(0, Run);
    SETT__(1, Single);
    SETT__(2, Grad);
    SETT__(3, Stop);
    #undef SETT__



    MyExtractFileFromResource( "template.css", "CSS_REPORT_FILE", true );

    MyForceDirectory( GetPersonalDir()+"\\Отчёты\\" );

}
//---------------------------------------------------------------------------
void __fastcall TForm1::FormActivate(TObject *Sender)
{
	static bool isDone = false;
    if( isDone ) return;
    isDone = true;
    LOG_FOO_LIFETIME_QUOTES;
    ASSERT_FUNCTION_CALLED_ONCE__;

    TiXML::LoadWindowPlacement(  TiXML::GiveNotNulElement( xml_, "Placement" ), Handle );

    FrmHTG[1] = new TFormHromatogramm(NULL, 1);
    FrmHTG[0] = new TFormHromatogramm(NULL, 0);
    this->Tile();
    SetDetectorType( 0, READ_XML_STR_( "D0_TYPE", "" ) );
    SetDetectorType( 1, READ_XML_STR_( "D1_TYPE", "" ) );

    #pragma warn -8018
    FrmHTG[0]->Graph()->SeriesColor = 8421376;

    READ_XML_INT_VAL_( DETECTOR_SERIES_COLOR,    FrmHTG[0]->Graph()->SeriesColor, -INT_MAX, INT_MAX );
    READ_XML_INT_VAL_( DETECTOR_CHART_COLOR,     FrmHTG[0]->Chart()->Color, -INT_MAX, INT_MAX );
    READ_XML_INT_VAL_( DETECTOR_CHART_BACKCOLOR, FrmHTG[0]->Chart()->BackColor, -INT_MAX, INT_MAX );
    READ_XML_INT_VAL_( DETECTOR_CHART_BACKCOLOR, FrmHTG[0]->Chart()->BackColor, -INT_MAX, INT_MAX );
    READ_XML_INT_VAL_( EVTS_JRNL_FORM_VISIBLE,   FormEvts->Visible, 0, 1  );

    #pragma warn +8018
    READ_XML_INT_VAL_( DETECTOR_SERIES_LINE_WIDTH, FrmHTG[0]->Graph()->LinePen->Width, 1, 3);

    OpenDialog1->InitialDir = READ_XML_STR_( "OPENDIALOG1_INITIALDIR", "" );

    FrmHTG[1]->Graph()->SeriesColor 	= FrmHTG[0]->Graph()->SeriesColor;
    FrmHTG[1]->Chart()->Color 			= FrmHTG[0]->Chart()->Color;
    FrmHTG[1]->Chart()->BackColor 		= FrmHTG[0]->Chart()->BackColor;
    FrmHTG[1]->Graph()->LinePen->Width 	= FrmHTG[0]->Graph()->LinePen->Width;

    // если в ПКСах есть -debug и -suspended, незапущать поток
    if( !( MyParamStrHas("-debug") && MyParamStrHas("-suspended") ) )
    	Hromat900::Instance().GetTransferManager().StartThread();                                                                                             
    rpn_[RIGHT_PNLS_COUNT-1]->Visible = IniGas()->ReadInteger("COMMON","calcfxp_enable",0)!=0;
    IniGas()->WriteInteger("COMMON","calcfxp_enable",rpn_[RIGHT_PNLS_COUNT-1]->Visible);

    FrmHTG[1]->Repaint();
    FrmHTG[1]->grdBasePik->Repaint();
    FrmHTG[0]->Repaint();
    FrmHTG[0]->grdBasePik->Repaint();

    UpdateDetector2Info( Hromat900::Instance().DetectorsCount()==2 );

    if(IsIzobut())
    {
        FormIzobutChart = new TFormIzobutChart(NULL);
    }
}
//---------------------------------------------------------------------------
__fastcall TForm1::~TForm1()
{
    //WinHelp(0, "", HELP_QUIT, 0);
    LOG_FOO_LIFETIME_QUOTES;
    ASSERT_FUNCTION_CALLED_ONCE__;
    Hromat900::Instance().GetTransferManager().StopThread();

    // удаляем окна детекторов
    FreeAndNULL( FrmHTG[0] );
    FreeAndNULL( FrmHTG[1] );

    // удаляем окно для просмотра архива хроматографа
    //FreeAndNULL( FormHromatArchiveDlg );

    // удаляем окно с журналом событий
    FreeAndNULL( FormEvts );

    // окно "О программе"
    FreeAndNULL( FormAbout );

    Form1 = NULL;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
void __fastcall TForm1::FormCreate(TObject *Sender)
{
	ASSERT_FUNCTION_CALLED_ONCE__;
    //

}
//---------------------------------------------------------------------------
void __fastcall TForm1::FormClose(TObject *Sender, TCloseAction &Action)
{
	if( !MyParamStrHas( "-debug" ) && ::MessageBox(
        Screen->ActiveForm->Handle, "Закрыть приложение?",
        "Хромат-900", MB_YESNO | MB_ICONQUESTION )==IDNO )
        Action = caNone;
    TiXML::SaveWindowPlacement( TiXML::GiveNotNulElement( xml_, "Placement" ), Handle );
    // выбранность правых панелей
    unsigned rpnSlktd = 0;
    for( unsigned i=0; i<RIGHT_PNLS_COUNT; ++i )
    {
    	Setbit( rpnSlktd, ( rpn_[i]->IsExpanded() ), i );
        if( !rpnGrd_[i] ) continue;
		TiXmlElement* elem = TiXML::GiveNotNulElement( xml_, rpnGrd_[i]->Name );
        TiXML::SetAtr( elem, "rows", my::Grd::GetVisRowsInfo(rpnGrd_[i]) );
        TiXML::SetIntAtr( elem, "col1", rpnGrd_[i]->ColWidths[0] );
        TiXML::SetIntAtr( elem, "col2", rpnGrd_[i]->ColWidths[1] );
    }

    WRITE_XML_INT_(RIGHT_PANELS_SELECTION, rpnSlktd);
    WRITE_XML_INT_(RIGHT_PANEL_WIDTH, ScrollBox1->Width);

    // ширина панелей статуса
    for ( unsigned i=0; i<STATUS_PNLS_COUNT; ++i )
    {
    	WRITE_XML_INT_( AnsiString().sprintf("%s_%d",STATUS_PANEL_WIDTH,i), pnStatus_[i]->Width );
    }

    WRITE_XML_INT_( DETECTOR_SERIES_COLOR, FrmHTG[0]->Graph()->SeriesColor );
    WRITE_XML_INT_( DETECTOR_CHART_COLOR, FrmHTG[0]->Chart()->Color );
    WRITE_XML_INT_( DETECTOR_CHART_BACKCOLOR, FrmHTG[0]->Chart()->BackColor );
    WRITE_XML_INT_( DETECTOR_SERIES_LINE_WIDTH, FrmHTG[0]->Graph()->LinePen->Width);
    WRITE_XML_INT_( EVTS_JRNL_FORM_VISIBLE, FormEvts->Visible );

    WRITE_XML_STR_( "D0_TYPE", typeOfDtctr_[0] );
    WRITE_XML_STR_( "D1_TYPE", typeOfDtctr_[1] );

    WRITE_XML_STR_( "OPENDIALOG1_INITIALDIR", OpenDialog1->InitialDir );

    WRITE_XML_STR_( "UNITS", MeasureUnits );

    MyWCout("Cохранить файл настроек...\n");
    SaveAppConfigFiles();
    MyWCout("Сохранить текущий журнал событий...\n");
    std::cout << "</WinMain>\n";
    BackupMyAppEvtJournal();
    MyWCout("Cохранить конфиденциальную инфу...\n");
    SaveConfidential();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::FormShow(TObject *Sender)
{
	LOG_FOO_LIFETIME_QUOTES;
    //
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
int TForm1::GetIndexOfBtnCicle(TObject * control)
{
    TObject ** ctrls = dynamic_cast<TImage*>(control) ? imgCicle : btnCicle;
    for(int i=0; i<4; ++i)
        if( ctrls[i]==control )
            return i;
    return -1;
}
//------------------------------------------------------------------------------
void TForm1::SetLeftPanelHeight(unsigned n)
{
    assert(n<RIGHT_PNLS_COUNT);
    TStringGrid *grd = rpnGrd_[n];
    grd->Top = 1;
    int ht=1;
    for( int i=0; i<grd->RowCount; ++i )
        if( grd->RowHeights[i]>0 )
            ht = ht + (grd->DefaultRowHeight+1);
    rpn_[n]->SetExpandedHeight( ht );

}
//---------------------------------------------------------------------------
void TForm1::InitializeRightPanels()
{
    rpn_[0] =  new TFrameDropDownPanel(this, ScrollBox1, "Хроматограф");
    rpn_[0]->Name = "rpn0";
    rpn_[0]->Top = 0;
    rpn_[0]->AddAllClienPanel( grdToolInfo );
    rpn_[0]->Panel5->Height = 0;
    rpn_[0]->Colapse();

    rpn_[1] =  new TFrameDropDownPanel(this, ScrollBox1, "Параметры хроматографа");
    rpn_[1]->Name = "rpn1";
    rpn_[1]->Top = 1000000;
    rpn_[1]->AddAllClienPanel( grdToolPrms );

    rpn_[2] =  new TFrameDropDownPanel(this, ScrollBox1, "Детектор 1");
    rpn_[2]->Name = "rpn2";
    rpn_[2]->Top = 1000000;
    rpn_[2]->AddAllClienPanel( grdDtctr1 );

    rpn_[3] =  new TFrameDropDownPanel(this, ScrollBox1, "Детектор 2");
    rpn_[3]->Name = "rpn3";
    rpn_[3]->Top = 1000000;
    rpn_[3]->AddAllClienPanel( grdDtctr2 );

    rpn_[4] =  new TFrameDropDownPanel(this, ScrollBox1, "Физико-химические показатели");
    rpn_[4]->Name = "rpn3";
    rpn_[4]->Top = 1000000;
    rpn_[4]->AddAllClienPanel( grdPhisChem );

    rpnGrd_[0] = grdToolInfo;
    rpnGrd_[1] = grdToolPrms;
    rpnGrd_[2] = grdDtctr1;
    rpnGrd_[3] = grdDtctr2;
    rpnGrd_[4] = grdPhisChem;

    // выбранность правых панелей
    const unsigned rpnSlktd = TiXML::GetIntAtrDef( xml_, RIGHT_PANELS_SELECTION, 0xFF);

    for ( unsigned n=0; n<RIGHT_PNLS_COUNT; ++n )
    {
        rpnGrd_[n]->FixedCols = 0;
        rpnGrd_[n]->FixedRows = 1;
        rpnGrd_[n]->Align = alClient;
        if(n!=4)
        {
            drwRpnGrd_[n].reset( new my::DrawGridCellManager( rpnGrd_[n] ) );
            drwRpnGrd_[n]->SetAfterSetCanvasFoo( &DrawMyGrid::Instance().AfterSetCanvasGrdChns );
            drwRpnGrd_[n]->SetOnDeterminateColumnWidthFoo( &DrawMyGrid::Instance().OnDeterminateColumnWidthForm1RightGrd );
        } else
        {
            my::DrawGridCellManager& dgcm = my::DrawGridCellManager::GetDrawCellManager(grdPhisChem);
            dgcm.SetAfterSetCanvasFoo( &DrawMyGrid::Instance().AfterSetCanvasGrdChns );
            dgcm.SetOnDeterminateColumnWidthFoo( &DrawMyGrid::Instance().OnDeterminateColumnWidthForm1RightGrd );
        }

        if( Getbit(rpnSlktd,n) ) rpn_[n]->Expand(); else rpn_[n]->Colapse();


        TiXmlElement* elem = TiXML::GiveNotNulElement( xml_, rpnGrd_[n]->Name );

        rpnGrd_[n]->ColWidths[0] = TiXML::GetXMLInteger( elem, "col1", 20, 400, 150);
        rpnGrd_[n]->ColWidths[1] = TiXML::GetXMLInteger( elem, "col2", 20, 500, 45);
        my::Grd::SetVisRowsInfo(rpnGrd_[n], TiXML::GetAtr(elem, "rows" ) );
        my::Grd::SetRowVis(rpnGrd_[n], 0, true );

        SetLeftPanelHeight(n);
    }
    grdPhisChem->ColWidths[0] = 50;
    grdPhisChem->ColWidths[1] = 85;
}
//---------------------------------------------------------------------------
void TForm1::SetDetectorType(bool dtctr, const AnsiString& strType)
{
    TLabel* pns[2] = { rpn_[2]->Label5, rpn_[3]->Label5};
    TLabel* pn = pns[dtctr];
    const AnsiString
    	s2  = MYSPRINTF_( "Детектор %d", dtctr+1 ),
        s3 = strType.IsEmpty() ? s2 : s2 + MYSPRINTF_(" (%s)",strType);
    pn->Caption = s3;
    typeOfDtctr_[dtctr] = strType;
    FrmHTG[dtctr]->Caption = s3;
}
//---------------------------------------------------------------------------
AnsiString TForm1::GetDetectorType(bool dtctr) const
{
    return typeOfDtctr_[dtctr];
}
//---------------------------------------------------------------------------
void TForm1::OnTransferManagerReport(unsigned context, const AnsiString& state)
{
    TransferManager& trmn = Hromat900::Instance().GetTransferManager();
	if ( !(context<RPT_TRNSFR_CNTXT::COUNT) )
        return;
    if( context==RPT_TRNSFR_CNTXT::THREAD_OK )
    {
        SetStatus( state, 1 );
        return;
    }
    if( context==RPT_TRNSFR_CNTXT::ITFC )
    {
        SetStatus( state, 1 );
        SetStatus( trmn.ActiveInterfaceDescription(), 2 );
        return;
    }
    if( context==RPT_TRNSFR_CNTXT::HROMAT_BUSY )
    {
        SetStatus( "Хроматограф занят. Запрос будет повторен.", 1 );
        return;
    }
    const bool
        isStartThreadMsg    = context==RPT_TRNSFR_CNTXT::START_THREAD,
        isStopThread        = context==RPT_TRNSFR_CNTXT::STOP_THREAD;
    if( isStartThreadMsg || isStopThread )
    {   
        SetStatus(state,0);
        for( unsigned i=1; i<STATUS_PNLS_COUNT; ++i ) SetStatus("",i);
        return;
    }
    AnsiString s = state;
    if( context==RPT_TRNSFR_CNTXT::END )
        s = "Ок";
    if( context==RPT_TRNSFR_CNTXT::ERR )
        context=RPT_TRNSFR_CNTXT::END;
    SetStatus( s, context );
}
//---------------------------------------------------------------------------
void TForm1::SetStatus( const AnsiString s, unsigned idx )
{
    assert( idx<STATUS_PNLS_COUNT );
    pnStatus_[idx]->Caption = NormalizeStateReport(s);
    pnStatus_[idx]->Hint = pnStatus_[idx]->Caption;
}
//---------------------------------------------------------------------------


void TForm1::HandleChnlsReaded(const double* dt, const double*)
{
    Hromat900T& ctrlSys =  Hromat900::Instance();
    const bool isRoundGOST = ctrlSys.IsRoundingGOST();
    using namespace HromatChanal;

    enum
    {
        T_KOLONOK_ROW = 0,      // "Т колонок,\"С",
        T_OBOLO4_ROW = 1,       // "Т оболочки,\"С",
        P_GAS_NOSIT_ROW = 2,    // "Р газа-носителя,кПа",
        P_PROB_ROW = 3,         // "Р пробы, мм.рт.ст.",
        P_VSPOM_GAS_ROW = 4,    // "Р вспом. газа,кПа"

        U_C_ROW = 0,            // "Uc,B",
        T_ROW = 1,              // "T,\"C",
        I_ROW = 2,              // "I,мА",
        Q_ROW = 3,              // "Q г.н., мл/мин",
    	HTG_ROW = 4,            // "Хроматограмма",
        TIME_ROW = 5,           // "Время создания",
        READED_ROW = 6,         // "Считано от начала",
        DUR_ROW = 7,            // "Продолжительность",
        MOMENT_ROW = 8          // "Время обновления"
    };


    grdToolPrms->Cells[1][T_KOLONOK_ROW]    = ToStr( T_KOLONOK, dt, isRoundGOST);
    grdToolPrms->Cells[1][T_OBOLO4_ROW]     = ToStr( T_OBOLO4, dt, isRoundGOST);
    grdToolPrms->Cells[1][P_GAS_NOSIT_ROW]  = ToStr( P_GAS_NOSIT, dt, isRoundGOST);
    grdToolPrms->Cells[1][P_VSPOM_GAS_ROW]  = ToStr( P_DOZ_V, dt, isRoundGOST);
    grdToolPrms->Cells[1][P_PROB_ROW]      = ToStr( P_VSPOM_GAS, dt, isRoundGOST);

    grdDtctr1->Cells[1][U_C_ROW] = ToStr( D0_UC, dt, isRoundGOST);
    grdDtctr1->Cells[1][T_ROW] = ToStr( D0_T_TERMOSTAT, dt, isRoundGOST);
    grdDtctr1->Cells[1][I_ROW] = ToStr( D0_TOK, dt, isRoundGOST);
    grdDtctr1->Cells[1][Q_ROW] = ToStr( D0_RASXOD1, dt, isRoundGOST);

    grdDtctr2->Cells[1][U_C_ROW] = ToStr( D1_UC, dt, isRoundGOST);
    grdDtctr2->Cells[1][T_ROW] = ToStr( D1_T_TERMOSTAT, dt, isRoundGOST);
    grdDtctr2->Cells[1][I_ROW] = ToStr( D1_TOK, dt, isRoundGOST);
    grdDtctr2->Cells[1][Q_ROW] = ToStr( D1_RASXOD1, dt, isRoundGOST);

    const double
        t_colon = dt[T_KOLONOK],  
        q1 = StrToFloatDef(grdDtctr1->Cells[1][Q_ROW], 0),
        q2 = StrToFloatDef(grdDtctr2->Cells[1][Q_ROW], 0),
        pGasNosit = StrToFloatDef(grdToolPrms->Cells[1][P_GAS_NOSIT_ROW], 0),
        q1_set = ctrlSys.FlowRateCfg(0), q2_set = ctrlSys.FlowRateCfg(1),
        p_vsp_gas = StrToFloatDef(grdToolPrms->Cells[1][P_VSPOM_GAS_ROW], 0),
        p_prob = StrToFloatDef(grdToolPrms->Cells[1][P_PROB_ROW], 0)
        ;

    struct {
        void operator()(boost::shared_ptr<my::DrawGridCellManager> dgcm, int row )
        {
            TFont &fnt = dgcm->GetCellCustomDrawFont( 1, row );
            fnt.Color = clRed;
            fnt.Style = TFontStyles()<< fsBold;

            TBrush &brush = dgcm->GetCellCustomDrawBrush( 1, row );
            brush.Color = clYellow;
            tmr->Enabled = 1;
            fixed = true;
        }
        TTimer *tmr;
        bool fixed;
    } h;
    h.tmr = this->TimerWarn;


    const double
        pGasNositNom = ctrlSys.GetKoefficient(231),
        deltaPGasNositMax = ctrlSys.GetKoefficient(232),
        deltaPGasNosit = std::fabs(pGasNosit-pGasNositNom);

    if(deltaPGasNosit>deltaPGasNositMax)
    {
        LogMyAppEvt(MYSPRINTF_(
        "Отклонение давления газа носителя %g кПа от нормы %g кПа "
        "составляет %g кПа и превышает допустимый предел %g кПа",
            pGasNosit, pGasNositNom, deltaPGasNosit, deltaPGasNositMax));
        h(drwRpnGrd_[1], P_GAS_NOSIT_ROW);
    }
    else
        drwRpnGrd_[1]->EraseCellCustomDraw(1, P_GAS_NOSIT_ROW);

    const double
        q1Nom = ctrlSys.GetKoefficient(6),
        deltaQ1max = ctrlSys.GetKoefficient(233),
        deltaQ1 = std::fabs(q1Nom-q1);
    if(deltaQ1>deltaQ1max)
    {
        LogMyAppEvt( MYSPRINTF_(
        "Отклонение расхода газа носителя детектора 1 %g мл от нормы %g мл "
        "составляет %g мл и превышает допустимый предел %g мл",
        q1, q1Nom, deltaQ1, deltaQ1max) );
        h(drwRpnGrd_[2], Q_ROW);
    }
    else
        drwRpnGrd_[2]->EraseCellCustomDraw(1, Q_ROW);


    const double
        q2Nom = ctrlSys.GetKoefficient(7),
        deltaQ2max = ctrlSys.GetKoefficient(234),
        deltaQ2 = std::fabs(q2Nom-q2);
    if(deltaQ2>deltaQ2max)
    {
        LogMyAppEvt( MYSPRINTF_(
        "Отклонение расхода газа носителя детектора 2 %g мл от нормы %g мл "
        "составляет %g мл и превышает допустимый предел %g мл",
        q2, q2Nom, deltaQ2, deltaQ2max) );
        h(drwRpnGrd_[3], Q_ROW);
    }
    else
        drwRpnGrd_[3]->EraseCellCustomDraw(1, Q_ROW);

    const double
        p_vsp_gas_nom = ctrlSys.GetKoefficient(235),
        delta_p_vsp_gas_max = ctrlSys.GetKoefficient(236),
        delta_p_vsp_gas = std::fabs(p_vsp_gas_nom-p_vsp_gas);

    if(delta_p_vsp_gas>delta_p_vsp_gas_max)
    {
        LogMyAppEvt( MYSPRINTF_(
        "Отклонение давления вспомогательного газа %g кПа от нормы "
        "%g кПа составляет %g кПа и превышает допустимый предел %g кПа",
        p_vsp_gas, p_vsp_gas_nom, delta_p_vsp_gas, delta_p_vsp_gas_max));
        h(drwRpnGrd_[1], P_VSPOM_GAS_ROW);
    }
    else
        drwRpnGrd_[1]->EraseCellCustomDraw(1, P_VSPOM_GAS_ROW);

    const double
        p_prob_nom = ctrlSys.GetKoefficient(237),
        delta_p_prob_max = ctrlSys.GetKoefficient(238),
        delta_p_prob = std::fabs(p_prob_nom-p_prob);

    if(delta_p_prob>delta_p_prob_max)
    {
        LogMyAppEvt( MYSPRINTF_(
        "Отклонение давления пробы %g мм.рт.ст. от номинального значения %g мм.рт.ст. "
        "составляет %g мм.рт.ст. и превышает допустимый предел %g мм.рт.ст.",
        p_prob, p_prob_nom, delta_p_prob, delta_p_prob_max
        ));
        h(drwRpnGrd_[1], P_PROB_ROW);
    }
    else
        drwRpnGrd_[1]->EraseCellCustomDraw(1, P_PROB_ROW);

    const double
        t_colon_nom = ctrlSys.GetKoefficient(8),
        delta_t_colon_max = ctrlSys.GetKoefficient(239),
        delta_t_colon = std::fabs(t_colon_nom - t_colon);

    if(delta_t_colon>delta_t_colon_max)
    {
        LogMyAppEvt( MYSPRINTF_(
        "Отклонение температуры термостата колонок %g \"С от номинального значения %g \"С "
        "составляет %g \"С и превышает допустимый предел %g \"С",
        t_colon, t_colon_nom, delta_t_colon, delta_t_colon_max
        ));
        h(drwRpnGrd_[1], T_KOLONOK_ROW);
    }
    else
        drwRpnGrd_[1]->EraseCellCustomDraw(1, T_KOLONOK_ROW);

    if( !h.fixed && TimerWarn->Enabled ) {
        TimerWarn->Enabled = 0;
    }
}
//---------------------------------------------------------------------------
void TForm1::HandleDeviceStateReaded( unsigned deviceStatecode, unsigned cicleType)
{
    currentHromatCicleType_ = cicleType;
    MyWCout("Получены тип цикла и код состояния.\n");

    const HromatModeMessage mdMsg = modes_[deviceStatecode];
    const AnsiString
        strMode = mdMsg.isNorm ? "норма" : "отказ",
        strSatatus = mdMsg.s
    ;

    grdToolInfo->Cells[1][1] =  strMode;
    grdToolInfo->Cells[1][2] =  HROMAT_CICLE::ToStr(currentHromatCicleType_);

    if( !mdMsg.isNorm )
    {
        SetStatus( strSatatus, 3 );
        grdToolInfo->Cells[1][3] = "...";
    } else
    {
        SetStatus( "", 3 );
        grdToolInfo->Cells[1][3] = strSatatus;
    }
}

//---------------------------------------------------------------------------
void TForm1::HandleProcessCurentHTG(const ModbusFileInfo& fi, unsigned offset)
{
	const unsigned
        ptsCount = fi.size_ / 4,
        readedPtsCount = offset / 4
        //,sign = fi.sign_
        ;
    const TDateTime
        totalDuration = IncMilliSecond( 0, ptsCount*GetDiskretTimeMSec() ),
        readedDuration = IncMilliSecond( 0, readedPtsCount*GetDiskretTimeMSec())
        ;

    TStringGrid *grd = fi.detector_==0 ? grdDtctr1 : grdDtctr2;
    grd->Cells[1][4] = fi.index_;
    grd->Cells[1][5] = FormatDateTime( "hh:nn", fi.date_ );
    grd->Cells[1][6] = FormatDateTime( "nn:ss", readedDuration );
    grd->Cells[1][7] = FormatDateTime( "nn:ss", totalDuration );
    grd->Cells[1][8] = FormatDateTime( "hh:nn:ss", ::Time() );
}
//---------------------------------------------------------------------------

//------------------------------------------------------------------------------
void TForm1::ShowArchivedialog(bool hardDisk)
{
    const HWND hCon = ::GetConsoleWindow();
	const bool isConVis = ::IsWindowVisible(hCon) && !::IsIconic(hCon);


    boost::shared_ptr<TFormArchiveView> dlg( new TFormArchiveView(NULL, hardDisk) );
    const bool isd2 = Hromat900::Instance().DetectorsCount()==2;

    dlg->Panel6->Visible=isd2;
    dlg->grdPic2->Visible=isd2;
    dlg->TabSheet1->TabVisible=isd2;
    dlg->CheckBox5->Checked=isd2;


    panelStatus->Parent = dlg.get();

    if(isConVis)
    	::SetParent( hCon, dlg->Handle );

    Hide();
    dlg->ShowModal();
    Show();
    panelStatus->Parent = this;

    if(isConVis)
    	::SetParent( hCon, Handle );
}
//------------------------------------------------------------------------------
void TForm1::UpdateDetector2Info(bool visible)
{
    FrmHTG[1]->Visible = visible;
    rpn_[3]->Visible = visible;
}

//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
void __fastcall TForm1::stdin1Click(TObject *Sender)
{
    const Hromat900T::StopThreadInThisFoo stopThread;
	GUIConsole::Instance().Create(GetAppProfileDir()+"\\");
    ::SetParent( ::GetConsoleWindow(), Handle );
    ::ShowWindow(::GetConsoleWindow(), SW_SHOWNORMAL );
	SetConsoleInput();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Panel1CanResize(TObject *Sender, int &NewWidth,
      int &NewHeight, bool &Resize)
{
    NewWidth = (NewWidth >50) ? NewWidth : 50;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ActionShowDevSetsDlgExecute(TObject *Sender)
{   
    Hromat900::Instance().SetupSysConfigDialog();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::N1Click(TObject *Sender)
{
    GUIConsole::Instance().Create(GetAppProfileDir()+"\\");
    ::SetParent( ::GetConsoleWindow(), Handle );
    ::ShowWindow(::GetConsoleWindow(), SW_SHOWNORMAL );
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Panel11DblClick(TObject *Sender)
{
	ShowMessage( static_cast<TPanel*>(Sender)->Caption );
}
//---------------------------------------------------------------------------
void __fastcall TForm1::MenuItem22Click(TObject *Sender)
{
	const Hromat900T::StopThreadInThisFoo stopThread;
	Hromat900::Instance().GetTransferManager().GetTaskList().DebugEdit_GUI();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::grdToolPrmsSelectCell(TObject *Sender, int ACol,
      int ARow, bool &CanSelect)
{
    CanSelect = 0;
    /*
	TStringGrid* grd = static_cast<TStringGrid*>(Sender);
	grd->OnSelectCell = NULL;
    TGridRect r;
    r.Left = 0;
    r.Top = 0;
    r.Bottom = 0;
    r.Right = 0;
    grd->Selection = r;
    CanSelect = 0;
    grd->OnSelectCell = &grdToolPrmsSelectCell;
    */
}
//---------------------------------------------------------------------------
void __fastcall TForm1::WindowCascade1Execute(TObject *Sender)
{
	::ShowWindow( FrmHTG[0]->Handle, SW_SHOWNORMAL | SW_RESTORE );
    ::ShowWindow( FrmHTG[1]->Handle, SW_SHOWNORMAL | SW_RESTORE );
}
//---------------------------------------------------------------------------
void __fastcall TForm1::MenuItem2Click(TObject *Sender)
{
    if( !OpenDialog1->Execute() ) return;
    OpenDialog1->InitialDir = ExtractFilePath( OpenDialog1->FileName );

    std::vector<HromatogrammPtr> htgs[2];
    for( int i=0; i<OpenDialog1->Files->Count; ++i )
    {
        const AnsiString fn = OpenDialog1->Files->Strings[i];

        try
        {
        	HromatogrammPtr htg( new Hromatogramm(ChangeFileExt(fn, ".htg")) );
        	htgs[ htg->GetId().dtctr_ ].push_back( htg );
        }

        catch(...)
        {
            const PMyExcpt e = MY_RETHROW_;
            const AnsiString msg = e->ToStringAll().c_str();
        	ShowMessage( MYSPRINTF_("Неверный формат файла \"%s\"!\n%s", fn, msg) );
        }
    }
    FrmHTG[0]->AddHtg( htgs[0].begin(), htgs[0].end() );
    FrmHTG[1]->AddHtg( htgs[1].begin(), htgs[1].end() );
}
//---------------------------------------------------------------------------
void __fastcall TForm1::menuThreadClick(TObject *Sender)
{
    TransferManager& man = Hromat900::Instance().GetTransferManager();
    if( !man.WasStopedOrTerminated() )
        man.KillThread();
    else
        man.StartThread();
}
//------------------------------------------------------------------------------
void __fastcall TForm1::grdToolPrmsMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
    my::Grd::SetupVisibleRowsOnMouseDownCol0(Sender, Button, Shift, X, Y, 0 );
    const unsigned n = (std::find( rpnGrd_, rpnGrd_+RIGHT_PNLS_COUNT, Sender ) - rpnGrd_);
    assert(n<RIGHT_PNLS_COUNT);
    SetLeftPanelHeight( n );

}
//------------------------------------------------------------------------------
void __fastcall TForm1::N8Click(TObject *Sender)
{
    const bool isStarted = !Hromat900::Instance().GetTransferManager().WasStopedOrTerminated();
    if(isStarted)
    {
    	menuThread->Caption = "Разорвать соединение";
    	menuThread->ImageIndex = 9;
    }
    else
    {
    	menuThread->Caption = "Установить соединение" ;
    	menuThread->ImageIndex = 8;
    }


}
//------------------------------------------------------------------------------
void __fastcall TForm1::menuViewDiskArchiveClick(TObject *Sender)
{
    ShowArchivedialog(1);
}
//------------------------------------------------------------------------------
void __fastcall TForm1::N6Click(TObject *Sender)
{
    Hromat900::Instance().SetupApplicationDialog();
}
//--------------------------------------------------------------------------- 
void __fastcall TForm1::menuUsersControlDialogClick(TObject *Sender)
{
    if( !IsAdminAutorized() )
    {
        ShowMessage("Только администратор имеет право совершать это действие!");
        return;
    }
    // вызвать диалог управления правами пользователей
    ExecuteUserControlDialog();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::N4Click(TObject *Sender)
{
    FormEvts->Show();
}
//------------------------------------------------------------------------------
void __fastcall TForm1::TimerBackupTimer(TObject *Sender)
{
	LOG_FOO_LIFETIME_QUOTES;
 	//GUIConsole::Instance().Backup();
    BackupMyAppEvtJournal();
    ProfileXmlDoc()->SaveFile();
}
//------------------------------------------------------------------------------
void __fastcall TForm1::MenuItem7Click(TObject *Sender)
{
	Close();
}
//------------------------------------------------------------------------------
void __fastcall TForm1::FormMouseWheelDown(TObject *Sender,
      TShiftState Shift, const TPoint &MousePos, bool &Handled)
{
	if( !IsChildControlOf( GetVCLControlAtPos( this, MousePos ), ScrollBox1 ) )
    	return;
    ScrollBox1->VertScrollBar->Position = ScrollBox1->VertScrollBar->Position - 50;
}
//------------------------------------------------------------------------------

void __fastcall TForm1::FormMouseWheelUp(TObject *Sender,
      TShiftState Shift, const TPoint &MousePos, bool &Handled)
{
	if( !IsChildControlOf( GetVCLControlAtPos( this, MousePos ), ScrollBox1 ) )
    	return;
    ScrollBox1->VertScrollBar->Position = ScrollBox1->VertScrollBar->Position + 50;
}
//------------------------------------------------------------------------------

void __fastcall TForm1::N12Click(TObject *Sender)
{
	AnsiString fn;
    if( !PromptForFileName(fn, "Журнал событий (*.evj)|*.evj", "evj", "Сохранить журнал событий",
        ExtractFilePath( READ_XML_STR_( EVTS_JRNL_LAST_FILE, GetAppDataDir()+"\\Архив_журнала_событий\\1.1") ),
        true) ) return;

    // сохранить текущий журнал
    SaveMyAppEvtJournal( fn );	
}
//------------------------------------------------------------------------------

void __fastcall TForm1::N13Click(TObject *Sender)
{
	AnsiString fn;
    const AnsiString initialDir =  ExtractFilePath(
        READ_XML_STR_( EVTS_JRNL_LAST_FILE, GetAppDataDir()+"\\Архив_журнала_событий\\1.1") );
    if( !PromptForFileName(fn, "Журнал событий (*.evj)|*.evj", "evj", "Открыть журнал событий",
        initialDir,
        false) ) return;
    LoadMyAppEvtJournalFromFile( fn );
}
//------------------------------------------------------------------------------

void __fastcall TForm1::menuSoftIDDialogClick(TObject *Sender)
{
    if( !IsAdminAutorized() )
    {
        ShowMessage("Только администратор имеет право совершать это действие!");
        return;
    }

    const Hromat900T::StopThreadInThisFoo stopThreadInThisFoo;
	SoftIDSetupDialog();
}
//------------------------------------------------------------------------------

void __fastcall TForm1::N16Click(TObject *Sender)
{
	FormAbout->ShowModal();	
}
//------------------------------------------------------------------------------
void __fastcall TForm1::menuSetHromatKeyClick(TObject *Sender)
{
    if( !IsAdminAutorized() )
    {
        ShowMessage("Только администратор имеет право совершать это действие!");
        return;
    }
    const Hromat900T::StopThreadInThisFoo stopThreadInThisFoo; 
    SetupHromatCRC16Key();
}
//------------------------------------------------------------------------------

void __fastcall TForm1::N19Click(TObject *Sender)
{
    Grdtn::Report();
}
//------------------------------------------------------------------------------

void __fastcall TForm1::menuGetHromatJournalsClick(TObject *Sender)
{
    menuGetHromatJournals->Enabled = 0;
    Hromat900::Instance().RequestMeasurementArchives();
}
//------------------------------------------------------------------------------
void __fastcall TForm1::N18Click(TObject *Sender)
{
    boost::shared_ptr<TFormTestCalcStatDlg> dlg( new TFormTestCalcStatDlg(NULL) );
    dlg->ShowModal();
}
//------------------------------------------------------------------------------

void __fastcall TForm1::btnCicleRunClick(TObject *Sender)
{   
    const int idx = GetIndexOfBtnCicle(Sender);
    if( idx==-1 ) return;

    const AnsiString sCurrCicleType = HROMAT_CICLE::ToStr(currentHromatCicleType_);

    if(currentHromatCicleType_!=HROMAT_CICLE::OSTANOV && idx!=3 )
    {
        if( ::MessageBox( Screen->ActiveForm->Handle,
            MYSPRINTF_("Для запуска данного вида цикла необходимо\n"
            "перевести хроматограф в режим \"останов\".\n"
            "В настоящий момент хроматограф находится в режиме %s.\n"
            "Для запуска выбранного типа цикла нажмите \"Да\".", sCurrCicleType).c_str(),
            "Хромат-900", MB_YESNO	 | MB_ICONEXCLAMATION)!=IDYES)
        return;
    }

    for( int i=0; i<4; ++i )
    {
        TControl *ctrl = static_cast<TControl*>(btnCicle[i]);
        TImage *img = static_cast<TImage*>(imgCicle[i]);
        if(i==idx)
        {
            ctrl->SendToBack();
            Hromat900::Instance().SetCicle( ctrl->Tag );
        }
        else
        {
            ctrl->BringToFront();
        }
    }
}
//------------------------------------------------------------------------------


void __fastcall TForm1::imgCicleStopClick(TObject *Sender)
{
    const int idx = GetIndexOfBtnCicle(Sender);
    if( idx==-1 ) return;
    for( int i=0; i<4; ++i )
    {
        TImage *img = static_cast<TImage*>(imgCicle[i]);
        TSpeedButton *btn = static_cast<TSpeedButton*>(btnCicle[i]);
        if(i==idx)
        {
            btn->Click();
        }
        else
        {
        }
    }
}
//------------------------------------------------------------------------------

void __fastcall TForm1::N20Click(TObject *Sender)
{
    const AnsiString
        path1 = GetAppDataDir(),
        path2 = GetPersonalDir(),
        path3 = HtgArchivePath();

    #define EXPLOR_DIR_(nn) ::ShellExecute( Handle, "open", path##nn##.c_str(), "", path##nn##.c_str(), SW_SHOW );
    EXPLOR_DIR_(1);
    EXPLOR_DIR_(2);
    EXPLOR_DIR_(3);
    #undef EXPLOR_DIR_
}
//------------------------------------------------------------------------------
void __fastcall TForm1::N9Click(TObject *Sender)
{
    std::vector<AnsiString> dirs;
    MySearchDirectories( GetPersonalDir(), dirs );
    MySearchDirectories( GetAppDataDir(), dirs );

    for( unsigned i=0; i<dirs.size(); ++i )
    {
        const AnsiString path = dirs[i];
        TSearchRec sr;
        std::vector<AnsiString> toDel;
        for( bool wasFound = !FindFirst(path+"\\*.*", faAnyFile, sr);
            wasFound; wasFound = !FindNext(sr) )
        {
            if(  sr.Attr & faDirectory ) continue;
            const AnsiString ext = ExtractFileExt(sr.Name);

            if( sr.Name=="log.rar" || ext==".log" || ext==".xls" )
                toDel.push_back(sr.Name);
        }
        FindClose(sr);
        if( !toDel.empty() )
            MYWCOUT_( "\"%s\"\n", path ));
        for( unsigned i2del=0; i2del<toDel.size(); ++i2del )
        {
            MYWCOUT_( "\t\tУдалён файл \"%s\".\n", sr.Name ));
            const AnsiString fn = ExcludeTrailingPathDelimiter(path) + "\\" + toDel[i2del];
            ::DeleteFile(fn.c_str());
        }
        if( ::RemoveDirectory( path.c_str() )==TRUE )
            MyWCout("\t\tУдалён пустой каталог.\n");
    }
}
//------------------------------------------------------------------------------
void __fastcall TForm1::N21Click(TObject *Sender)
{
    Grdtn::ShowLast();    
}
//------------------------------------------------------------------------------

void __fastcall TForm1::Timer1Timer(TObject *Sender)
{
    static int nn=0;
    if(++nn>5) Timer1->Enabled = 0;
    Grdtn::ShowLast();
}
//------------------------------------------------------------------------------

void __fastcall TForm1::N22Click(TObject *Sender)
{
    boost::shared_ptr<TFrmWait> frmWait( new TFrmWait(NULL, false) );
    frmWait->Panel2->Caption = "Переименование файлов хроматограмм...";
    frmWait->FixPosition();
    frmWait->Show();
    frmWait->Repaint();

    AnsiString mainPath = HtgArchivePath();
    if( !SelectDirectory("Путь к файлам хроматограмм", "", mainPath) )
        return;
    std::vector<AnsiString> files;
    MySearchAllFiles( mainPath, "\\*.htg", files );
    frmWait->pb1->Max = files.size();
    frmWait->pb1->Visible = 1;
    frmWait->Repaint();
    for( unsigned i=0; i<files.size(); ++i )
    {
        const AnsiString fn = files[i];
        Hromatogramm htg(fn);
        const AnsiString newFn = htg.NormalFileName();
        if(fn!=newFn)
            RenameFile( fn, newFn );
        frmWait->pb1->Position = frmWait->pb1->Position+1;
        frmWait->Repaint();
    }
}
//------------------------------------------------------------------------------

void __fastcall TForm1::Button2Click(TObject *Sender)
{
    TimerWarn->Enabled = false;    
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TimerWarnTimer(TObject *Sender)
{
    if( Hromat900::Instance().IsWarningSoundEnabled() ) {
        waveOutSetVolume((void*)WAVE_MAPPER,0xFFFF);
        MediaPlayer1->Play();
    }
    
}
//---------------------------------------------------------------------------


void __fastcall TForm1::Button1Click(TObject *Sender)
{
    static bool v = true;
    UpdateDetector2Info(v);
    v = !v;
}
//---------------------------------------------------------------------------

