//---------------------------------------------------------------------------
#include <numeric>
#include <set>
//---------------------------------------------------------------------------
#pragma hdrstop
//---------------------------------------------------------------------------
#include "uFormArchiveView.h"


#include <DateUtils.hpp>
#include <dirent.h>
#include <dialogs.hpp>

#include "SGrdHlpr.h"
#include "uFrameGraph.h"
#include "uFrmWait.h"
#include "uFormCorruptedFilesMessageDialog.h"
#include "listViewHelpr_.h"
#include "MyFileCtrl.h"
#include "AnsiStringUtils_.h"
#include "my_draw_.h"
#include "MyAppProfile.h"
#include "MyAppWinPath.hpp"
#include "uDrawGrids.h"
#include "MyVclUtils_.h"
#include "guicon.h"
// Хромат 900
#include "HromatPhysChem.h"
#include "TransferManage.h"
#include "htg_.h"
#include "HromatPic.h"
#include "kostik_fround.h"
#include "hrmtstat.h"

//---------------------------------------------------------------------------
#include "HromatGas.hpp"
#include "iniUtils_.h"
#include "uFormAnalyze.h"
#include "uFormHtgFilter.h"
#include "hromat_sys.h"
#include "MyProcEntry.h"
#include "HromatTask.h"
#include "TaskList.h"
#include "MySynchronize.h"

#include "MyModFN.hpp"
#include "HromatHtmlReport.h"
#include "uFormIzobutChart.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "SHDocVw_OCX"
#pragma resource "*.dfm"
TFormArchiveView *FormArchiveView;
HtgItemsArray TFormArchiveView::deviceArchive_;
//------------------------------------------------------------------------------
AnsiString HtgFilter2Str( const HtgFilter& fltr )
{
    if(fltr.rgm==-1 && fltr.izm && fltr.grad ) return "";
    AnsiString ret;
    if(fltr.rgm>-1) ret = "Канал "+IntToStr(fltr.rgm)+" ";
    if(!fltr.izm || !fltr.grad)
    {
        if(fltr.izm) ret += "изм. ";
        if(fltr.grad) ret += "град. ";
    }
    return ret;
}
//------------------------------------------------------------------------------

void ShowAnalysedConc( TStringGrid *grdConc, int col, int row, double val, int n, bool okrug )
{
    const AnsiString s = okrug ? MyFormatFloat(  val, n ) : MYSPRINTF_("%g",val);
	grdConc->Cells[col][row] = StrReplaceSymbols(s, '.', ',' );
}
//------------------------------------------------------------------------------
double GetVDoubleAverage(const VDouble& v)
{
	const unsigned sz = v.size();
    return sz==0 ? 0 : std::accumulate( v.begin(), v.end(), 0.0 )/( sz*1.0 );
}
//------------------------------------------------------------------------------
void ShowAnalyzeHtgResult(const  AnalyzeHromatogram::Tag& analyze,
    TStringGrid *grdConc, TStringGrid *grdPhyChem)
{
	const unsigned gasCnt = Gas::Count();

    HromatStatistic stat;
    for( unsigned i=0; i<gasCnt; ++i )
    	stat.conc[i] = GetVDoubleAverage( analyze.conc[i] );

    CalculateHromatStatistic( stat );
    if( analyze.okrug ) OkruglitHromatStatistic( stat );

    for( unsigned i=0; i<gasCnt; ++i )
	{
    	const double val = stat.conc[i], dVal = stat.dConc[i];
        int n;
        KostikGOSTRound( dVal, &n );

        const int row = Gas::GetItems()[i].reportnum+1;
        ShowAnalysedConc( grdConc, 1, row, val, n, analyze.okrug);
        ShowAnalysedConc( grdConc, 2, row, dVal, n, analyze.okrug);
    }
	ShowGrdPhysChem( grdPhyChem, HromatStatisticToHromatPhysChem(stat) );
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void CreateHtgItemsTreeView(TTreeNodes* nodes, const HtgItemList::MapT4& map4,
    HtgItemList::MapT5 &map5, HtgItemList::List &items, bool skipIfNotExists=true )
{
    using namespace HtgItemList;
    nodes->Clear();
    for( MapT4::const_iterator iYear=map4.begin(); iYear!=map4.end(); ++iYear )
    {
        TTreeNode* ndYear = nodes->Add(NULL, IntToStr(iYear->first) );
        ndYear->Data = (void*)iYear->first;
        ndYear->ImageIndex = 3;
        const MapT3& map3 = iYear->second;
        for( MapT3::const_iterator iMonth=map3.begin(); iMonth!=map3.end(); ++iMonth )
        {
            TTreeNode* ndMonth = nodes->AddChild(ndYear, MyFormatMonth(iMonth->first) );
            ndMonth->Data = (void*)iMonth->first;
            ndMonth->ImageIndex = 3;
            const MapT2& map2 = iMonth->second;
            for( MapT2::const_iterator iDay=map2.begin(); iDay!=map2.end(); ++iDay )
            {
                TTreeNode* ndDay = nodes->AddChild(ndMonth, IntToStr(iDay->first) );
                ndDay->Data = (void*)iDay->first;
                ndDay->ImageIndex = 3;
                const MapT1& map1 = iDay->second;
                for( MapT1::const_iterator iItem=map1.begin(); iItem!=map1.end(); ++iItem )
                {
                    const HtgItem& itm = iItem->second;
                    if( skipIfNotExists && ( !MyFileExists( itm.fileName[0] )  ) )
                        continue;
                    Word AYear, AMonth, ADay, AHour, AMinute, ASecond, AMilliSecond;
                    DecodeDateTime(itm.dateTime, AYear, AMonth, ADay, AHour, AMinute, ASecond, AMilliSecond);
                    const AnsiString cation = MYSPRINTF_("%d     %s     Канал %d     %s",
                        itm.file, FormatDateTime( "hh:nn", itm.dateTime), itm.rgm,
                        HromatFileType::ToStr(itm.type) );
                    TTreeNode* ndItem = nodes->AddChild(ndDay, cation );
                    ndItem->Data = (void*)( AHour*60*60+AMinute*60*+ASecond );
                    ndItem->ImageIndex = 3;
                    map5[ndItem] = &itm;
                    items.push_back(&itm);
                }
            }
        }
    }
    std::sort(items.begin(), items.end(), FileDateTimeLess() );
}
//------------------------------------------------------------------------------
void MakeHromatogramsTreeFromHtgItemsArray(const HtgItemsArray& itemsList, TTreeView* trv,
    HtgItemList::MapT4& map4, HtgItemList::MapT5 &map5, HtgItemList::List &items,
        unsigned rgm, bool izm, bool grad )
{
    using namespace HtgItemList;
    trv->Items->Clear();
    map4.clear();
    map5.clear();
    items.clear();


    for( unsigned iItem=0; iItem<itemsList.size(); ++iItem )
    {
        const HtgItem &itm = itemsList[iItem];

        if( rgm!=-1 && itm.rgm!=rgm ||
            itm.type==HromatFileType::MEASURE && !izm ||
            itm.type==HromatFileType::KALIBR && !grad )
            continue;

        Word AYear, AMonth, ADay, AHour, AMinute, ASecond, AMilliSecond;
        DecodeDateTime(itm.dateTime, AYear, AMonth, ADay, AHour, AMinute, ASecond, AMilliSecond);
        const HourMinuteSecond hms = std::make_pair( AHour, std::make_pair(AMinute,ASecond) );
        map4[AYear][AMonth][ADay][hms] = itm;
    }
    CreateHtgItemsTreeView(trv->Items, map4, map5, items, false);
}
//------------------------------------------------------------------------------
void MakeHromatogramsTree(const AnsiString& mainPath, TTreeView* trv,
    HtgItemList::MapT4& map4, HtgItemList::MapT5 &map5, HtgItemList::List &items,
    unsigned rgm, bool izm, bool grad )
{
    using namespace HtgItemList;
    trv->Items->Clear();
    map4.clear();
    map5.clear();
    items.clear();

    boost::shared_ptr<TFrmWait> frmWait( new TFrmWait(NULL, false) );
    frmWait->Panel2->Caption = "Формирование структуры каталога...";
    frmWait->FixPosition();
    frmWait->Show();
    frmWait->Repaint();

    // найти все файлы с расширеним .htg
    std::vector<AnsiString> files;
    MySearchAllFiles( mainPath, "\\*.htg", files );
    const unsigned itemsCount = files.size();

    frmWait->pb1->Max = itemsCount;
    frmWait->pb1->Visible = 1;
    frmWait->Repaint();

    for( unsigned iFile=0; iFile<itemsCount; ++iFile )
    {
        frmWait->pb1->Position = frmWait->pb1->Position+1;
        const AnsiString fn = ChangeFileExt( ExtractFileName( files[iFile] ), "" );
        HtgItem itm;
        bool detector;
        if(
            !TryExtractHtgItemFromFile(fn, itm, detector) ||
            rgm!=-1 && itm.rgm!=rgm ||
            itm.type==HromatFileType::MEASURE && !izm ||
            itm.type==HromatFileType::KALIBR && !grad
        ) continue;



        Word AYear, AMonth, ADay, AHour, AMinute, ASecond, AMilliSecond;
        DecodeDateTime(itm.dateTime, AYear, AMonth, ADay, AHour, AMinute, ASecond, AMilliSecond);
        const HourMinuteSecond hms = std::make_pair( AHour, std::make_pair(AMinute,ASecond) );


        MapT1 &map1 = map4[AYear][AMonth][ADay];
        MapT1::iterator iItem = map1.find(hms);
        if( iItem==map1.end() )
            iItem = map1.insert( std::make_pair(hms, itm) ).first;
        HtgItem &mapItm = iItem->second;
        mapItm.dateTime = itm.dateTime;
        mapItm.file = itm.file;
        mapItm.rgm = itm.rgm;
        mapItm.type = itm.type;
        mapItm.fileName[detector] = files[iFile];
    }
    CreateHtgItemsTreeView(trv->Items, map4, map5, items);
    //trv->HideSelection = 0;
}
//------------------------------------------------------------------------------




namespace IniId
{
	ConstCString
        MAIN = "MAIN",
		GRD_HTG_LST_D1_VIS_COLS = "GRD_HTG_D1_VIS_COLS",
        GRD_HTG_LST_D2_VIS_COLS = "GRD_HTG_D2_VIS_COLS",
        GRD_PIC_VIS_COLS = "GRD_PIC_VIS_COLS1",
        LEFT_PANEL_WIDTH = "LEFT_PANEL_WIDTH",
        LEFT_TOP_PANEL_HEIGHT = "LEFT_TOP_PANEL_HEIGHT",
        SELECTED_NODE = "SELECTED_NODE",
        LEFT_BOTTTOM_PANEL_HEIGHT = "LEFT_BOTTTOM_PANEL_HEIGHT"
    ;
}; // namespace XML_ID
//------------------------------------------------------------------------------
TIniFile* AppIni()
{
	static TIniFile* ret = NULL;
    if(ret==NULL)
    	ret = new TIniFile( GetAppProfileDir()+"\\h900arch.ini" );
    return ret;
}
//------------------------------------------------------------------------------
struct BadFileInfo
{
    unsigned id;
    AnsiString fn, reason;
};

//------------------------------------------------------------------------------
AnsiString FormatValues1( double v, double u, unsigned n )
{
	return MYSPRINTF_("%s ± %s", MyFormatFloat( v, n ), MyFormatFloat( u, n ) );
}
//------------------------------------------------------------------------------
bool IsYear(const AnsiString& s)
{
	const MyInt year = MyStrToInt( s );
    return year.second && year.first>2000;
}
//------------------------------------------------------------------------------
int MonthStrToNum(const AnsiString& s)
{
	const AnsiString
    	*beg = LongMonthNames,
        *end = LongMonthNames+12,
        *fnd = std::find( LongMonthNames, LongMonthNames+12, s);
    return ( fnd==end ) ? 0 : fnd-beg+1;
}
//------------------------------------------------------------------------------
bool IsDay(const AnsiString& s)
{
	const MyInt day = MyStrToInt( s );
    return day.second && day.first>0 && day.first<32;
}
//------------------------------------------------------------------------------
void FindHTGFiles(const AnsiString& path, std::vector<AnsiString>& files)
{
    struct Hlpr : public boost::noncopyable
    {
        explicit Hlpr( std::vector<AnsiString>& files) : files_(files)
        {
        }
        void OnFind(const AnsiString& path, const  TSearchRec& sr)
        {
            files_.push_back(path+"\\"+sr.Name);
        }
        std::vector<AnsiString>& files_;
    };
    Hlpr hlpr(files);
    if( path.IsEmpty() ) return;
    const bool noUse = false;
    MySearchFiles( path, ".htg", &hlpr.OnFind, noUse);
}
//------------------------------------------------------------------------------
TDateTime GetDateTimeOf(TDateTime date, TDateTime time)
{
    Word Year1, Month1, Day1, Hour1, Min1, Sec1, MSec1;
    DecodeDate(date, Year1, Month1, Day1);
    DecodeTime(time, Hour1, Min1, Sec1, MSec1);
    return EncodeDateTime(Year1, Month1, Day1, Hour1, Min1, Sec1, MSec1);
}
//------------------------------------------------------------------------------
AnsiString NodePathToStr( TTreeNode *Node )
{
	AnsiString ret = "";
    while( Node )
    {
    	ret = "\\"+Node->Text+ret;
		Node = Node->Parent;
    }
    return ret;
}
//------------------------------------------------------------------------------
void SelectNodeByPath(TTreeView *trv, const AnsiString& path )
{   
	trv->FullCollapse();
    TTreeNodes& nds = *trv->Items;

    const MyStringList sItems = GetMyStringList(path, "\\");

    TTreeNode *node = NULL;
    for( unsigned nItem=0; nItem<sItems.size(); ++nItem  )
    {
        const AnsiString &nodeName = sItems[nItem];
        if( nItem==0 && nodeName.IsEmpty() ) continue;
        if(node==NULL)
        {
        	for( int nNode=0; nNode<nds.Count; ++nNode)
            {
            	if( nds[nNode]->Level==0 && nds[nNode]->Text==nodeName )
                {
                	node = nds[nNode];
                    node->Expand(false);
                    trv->Select(node);
                    break;
                }
            }
            if(node==NULL) return;
        }
        else
        {
        	bool found = false;
        	for( int nNode=0; nNode<node->Count; ++nNode)
            {
            	const AnsiString thisNodeText = node->Item[nNode]->Text;
            	if( thisNodeText==nodeName )
                {
                	node = node->Item[nNode];
                    node->Expand(false);
                    trv->Select(node);
                    found = true;
                    break;
                }
            }
            if( !found ) return;
        }
    }
}
//------------------------------------------------------------------------------







//------------------------------------------------------------------------------
namespace AnalyzeHromatogram
{
	//--------------------------------------------------------------------------
	// анализировать файл хроматограммы VDouble conc_[Gas::COUNT], physChem[3];
	void ProcesArchiveFile( const AnsiString& fn, Tag& tag)
	{
		const unsigned cnt = Gas::Count();
    	try
    	{
	    	const Hromatogramm htg( fn );
            const TDateTime date = htg.GetId().date_;

            if(tag.count==0)
            {
                tag.dateMin = date;
                tag.dateMax = date;
            } else
            {
                if( date<tag.dateMin ) tag.dateMin = date;
                if( date>tag.dateMax ) tag.dateMax = date;
            }

        	const HromatPicTable& pics = htg.GetPicTable();
        	for( unsigned i=0; i<pics.size(); ++i )
        	{
    	    	const HromatPicData& pic = pics[i];
            	const unsigned gasIdx = Gas::GetIdx(pic.name_);
            	if( gasIdx<cnt )
        	    	tag.conc[gasIdx].push_back(pic.conc_);
                if(std::strcmp(pic.name_, "i-C4H8")==0)
                {
                    tag.IzobutConc.push_back( std::make_pair(date,pic.conc_) );
                }
                    
        	}
        	++tag.count;
    	}
    	catch(...)
    	{
    	}
	}
    void ProcesItem( const HtgItem& htg, Tag& tag)
    {
        AnalyzeHromatogram::ProcesArchiveFile( htg.fileName[0], tag );
        AnalyzeHromatogram::ProcesArchiveFile( htg.fileName[1], tag );
    }
}; // namespace AnalyzeHromatogram
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void SetComboBoxTextItemIndex(TComboBox* cb, const AnsiString& s)
{
    if( s.IsEmpty() )
    {
        cb->ItemIndex = -1;
        return;
    }
    if( cb->Items->IndexOf(s)==-1 )
    {
        cb->Items->Add(s);
        cb->ItemIndex = cb->Items->IndexOf(s);
        ComboboxList::FixItem(cb);
    } else
        cb->ItemIndex = cb->Items->IndexOf(s);
}
//------------------------------------------------------------------------------


void DeleteHtgFile(TTreeNode *nd, HtgItemList::MapT5& map5)
{
    if( nd->Level==3 )
    {
        HtgItemList::MapT5::iterator fnd = map5.find(nd);
        if( fnd!=map5.end() )
        {
            const HtgItem *itm = fnd->second;
            ::DeleteFile( itm->fileName[0].c_str() );
            ::DeleteFile( itm->fileName[1].c_str() );
            //map5.erase(fnd);
            nd->Selected = 1;
        }
    }
    for( int i=0; i<nd->Count; ++i )
        DeleteHtgFile(nd->Item[i], map5);
}

bool HtgItemFilesExists(const HtgItem& itm)
{
    return MyFileExists(itm.fileName[0]) ;
}



LRESULT HandleEndReadArchiveFile1_(unsigned file, TDateTime dateTime)
{
    if( FormArchiveView )
        FormArchiveView->HandleEndReadArchiveFile(file, dateTime);
    return 0;
}
//------------------------------------------------------------------------------
void OnAfterEndReadArchiveFile(unsigned file, TDateTime dateTime)
{
    my::Synchronize2( &HandleEndReadArchiveFile1_, file, dateTime );
}

//------------------------------------------------------------------------------
__fastcall TFormArchiveView::TFormArchiveView(TComponent* Owner, bool mustMakeTree)
	: TForm(Owner),
    drawGrdPik1_(grdPic1), drawGrdPik2_(grdPic2), mustMakeTree_(mustMakeTree)
{
    const AnsiString mainPath = HtgArchivePath();
    Caption = mainPath;

    filter_.rgm = -1;
    filter_.izm = 1;
    filter_.grad = 1;

    frameGraph_[0] = new TFrameGraph(this, TabSheet5);
    frameGraph_[1] = new TFrameGraph(this, TabSheet1);

    
    grdPic[0] = grdPic1; grdPic[1] = grdPic2;
    grdPhysChemHtg[0] = grdPhysChem; grdPhysChemHtg[1] = NULL;

	//const unsigned gasCnt = Gas::Count();
	PageControl1->ActivePageIndex = 0;
    my::Grd::AddCopyItemToPopupMenu(grdPhysChem);
    MakeGrdPhysChem(grdPhysChem);
    my::Grd::SetColumn( grdPhysChem, 1, PHY_CHEM::EDIZM, PHY_CHEM::EDIZM + PHY_CHEM::COUNT, 1);

    const DrawMyGrid& dg = DrawMyGrid::Instance();
    drawGrdPik1_.SetAfterSetCanvasFoo(dg.AfterSetBasePikCellCanvas);
    drawGrdPik2_.SetAfterSetCanvasFoo(dg.AfterSetBasePikCellCanvas);
    //drwGrdConc_.SetAfterSetCanvasFoo( dg.AfterSetCanvasFormArchiveViewGrdPhysChem );


    InitializePicTableGrd( grdPic1 );
    InitializePicTableGrd( grdPic2 );

    for( int i=0; i<2; ++i )
        frameGraph_[i]->AddSeries();

    my::Grd::AddCopyItemToPopupMenu(grdPic1);
    my::Grd::AddCopyItemToPopupMenu(grdPic2);

    frameGraph_[0]->ScrollBox1->Parent = PanelLeftBottom;
    frameGraph_[0]->ScrollBox1->Align = alClient;
    frameGraph_[0]->ScrollBox1->Visible = 0;
    delete frameGraph_[0]->Splitter1;
    frameGraph_[1]->ScrollBox1->Parent = PanelLeftBottom;
    frameGraph_[1]->ScrollBox1->Align = alClient;
    frameGraph_[1]->ScrollBox1->Visible = 0;
    delete frameGraph_[1]->Splitter1;

    ComboboxList::RegisterCombobox( MyGetExePath()+"edpath.txt", edPath, &OnAddItem2File );

    TIniFile *ini = AppIni();
    using namespace IniId;
    using my::Grd::SetVisColsInfo;
    pnLeft->Width = ini->ReadInteger(MAIN, LEFT_PANEL_WIDTH, pnLeft->Width);
    PanelLeftBottom->Height = ini->ReadInteger(MAIN, LEFT_BOTTTOM_PANEL_HEIGHT, PanelLeftBottom->Height);

    const AnsiString ss = ini->ReadString( MAIN, GRD_PIC_VIS_COLS, "");
    SetVisColsInfo(grdPic1, ss);
    SetVisColsInfo(grdPic2, ss );

    my::Grd::NormalizeGridHeight( grdPhysChem );
    MakeLabelAsButton( lblFilter );

    RadioButton1->Checked = mustMakeTree_;
    RadioButton2->Checked = !RadioButton1->Checked;

    FormArchiveView = this;
}
//---------------------------------------------------------------------------
__fastcall TFormArchiveView::~TFormArchiveView()
{
    using namespace IniId;
    using my::Grd::GetVisColsInfo;
    TIniFile *ini = AppIni();
    ini->WriteInteger(MAIN, LEFT_PANEL_WIDTH, pnLeft->Width);
    ini->WriteInteger(MAIN, LEFT_BOTTTOM_PANEL_HEIGHT, PanelLeftBottom->Height);
    ini->WriteString( MAIN, GRD_PIC_VIS_COLS, GetVisColsInfo(grdPic1) );
    ini->WriteString( MAIN, SELECTED_NODE, NodePathToStr( trv1->Selected ) );

    frameGraph_[0]->Clear();
    frameGraph_[1]->Clear();
    UnregisterAllLabelsAsButton( this );
    ComboboxList::UnregisterAll( this );
    Ini::SaveWindowPlacement( Handle, ini, "Placement" );

    Hromat900::Instance().CancelReadDeviceArchive();

    FormArchiveView = NULL;
}
//---------------------------------------------------------------------------
void __fastcall TFormArchiveView::FormActivate(TObject *Sender)
{
    TIniFile *ini = AppIni();
    using namespace IniId;
    Ini::LoadWindowPlacement( Handle, AppIni(), "Placement" );
    
    if(RadioButton1->Checked)
    {
        SetComboBoxTextItemIndex(edPath, HtgArchivePath() );
        //edPathChange(edPath);
        Realign();
        ActiveControl = trv1;
        treePath_ = ini->ReadString( MAIN, SELECTED_NODE, "");
        SelectNodeByPath(trv1, treePath_ );
        edPath->Repaint();
    }

    OnActivate = NULL;
}
//---------------------------------------------------------------------------
void __fastcall TFormArchiveView::FormCreate(TObject *Sender)
{
	//
}
//---------------------------------------------------------------------------

void __fastcall TFormArchiveView::FormClose(TObject *Sender,
      TCloseAction &Action)
{
    if( isAnalyzePerforming_ )
    {
        isAnalyzePerforming_ = 0;
        Action = caNone;
        return;
    }
    //
}
//---------------------------------------------------------------------------
void __fastcall TFormArchiveView::FormShow(TObject *Sender)
{
    //
}
//---------------------------------------------------------------------------
void __fastcall TFormArchiveView::FormHide(TObject *Sender)
{
    //
}
//------------------------------------------------------------------------------


HromatogramReportData TFormArchiveView::GetReportData(const HtgItem& itm)
{
    HromatogramReportData reportData;
    const unsigned htgFile = itm.file;
    reportData.caption.sprintf("Хроматограма %d", htgFile);
    reportData.title = FormatHtgItem(itm);
    reportData.grd1 = grdPic1;
    reportData.grd2 = grdPic2;
    reportData.grdPhysChem = grdPhysChem;
    reportData.chart1 = frameGraph_[0]->chrt_;
    reportData.chart2 = frameGraph_[1]->chrt_;
    reportData.dt = itm.dateTime;

    return reportData;
}


void TFormArchiveView::HandleEndReadArchive()
{
    //trv1Change(NULL, NULL);
}

void TFormArchiveView::SetDeviceArchive(const HtgItemsArray& deviceArchive )
{
    deviceArchive_ = deviceArchive;
    MakeTreeFromHtgItemsArray(deviceArchive_);
    SelectNodeByPath(trv1, treePath_ );
}

void TFormArchiveView::GetSelectedArchiveHtgItems(HtgItemsArray& items)
{
    GetSelectedHtgItems(items);
    for(unsigned i=0; i<items.size(); ++i)
    {
        HtgItem &itm = items[i];
        HtgItemsArray::iterator
            e = deviceArchive_.end(),
            fnd = std::find_if( deviceArchive_.begin(), e,
                IsHtgItemFileIndex(items[i].file) );
        if(fnd==e)
        {
            itm.size[0] = 0;
            itm.size[1] = 0;
            itm.sign = -1;
        }
        else
            itm = *fnd;
    }
}

void TFormArchiveView::GetSelectedHtgItems(HtgItemsArray& ret)
{
    std::vector<TTreeNode*> nds;
    GetAllSelectedNodes( trv1, nds);
    ret.clear();

    for( int iNode=0; iNode<nds.size(); ++iNode )
    {
        TTreeNode *nd = nds[iNode];
        if( nd->Level!=3 ) continue;

        HtgItemList::MapT5::iterator fnd = map5_.find(nd);
        if( fnd!=map5_.end() )
            ret.push_back( *(fnd->second) );
    }
    std::sort( ret.begin(), ret.end() );
}
//------------------------------------------------------------------------------
void TFormArchiveView::MakeTreeFromHtgItemsArray(const HtgItemsArray& itemsList )
{
    MakeHromatogramsTreeFromHtgItemsArray( itemsList, trv1,
        map4_, map5_, items_, filter_.rgm, filter_.izm, filter_.grad );
}
//------------------------------------------------------------------------------
void TFormArchiveView::CalculateMidleValues(bool okrugl)
{
    TTreeNode *ndSel = trv1->Selected;
    if(ndSel==NULL || ndSel->Level<2 ) return;

    analyze_.conc.clear();
    analyze_.count = 0;
    analyze_.okrug = okrugl;
    analyze_.conc.resize( Gas::Count() );

    if( ndSel->Level==3 )
        for( int iNode=0; iNode<trv1->SelectionCount; ++iNode )
        {
            HtgItemList::MapT5::const_iterator iHtg = map5_.find(trv1->Selections[iNode]);
            if( iHtg!=map5_.end() )
                ProcesItem( *(iHtg->second), analyze_);
        }

    if( ndSel->Level==2 )
        for( int iNode=0; iNode<ndSel->Count; ++iNode )
        {
            HtgItemList::MapT5::const_iterator iHtg = map5_.find(ndSel->Item[iNode]);
            if( iHtg!=map5_.end() )
                ProcesItem( *(iHtg->second), analyze_);
        }
    trv1->Repaint();
    boost::shared_ptr<TFormAnalyze> dlg( new TFormAnalyze(NULL) );

    const AnsiString sTime = analyze_.dateMin==analyze_.dateMax ?
        FormatDateTime("hh:nn", analyze_.dateMin) :
        FormatDateTime("dd MMMM yyyy hh:nn", analyze_.dateMin)+"-"+FormatDateTime("dd MMMM yyyy hh:nn", analyze_.dateMax);


    dlg->Label2->Caption = MYSPRINTF_( "%s, %d хроматограм",
        sTime, analyze_.count );
    ShowAnalyzeHtgResult( analyze_, dlg->grdConc, dlg->grdPhyChe );
    dlg->ShowModal();
    trv1->Repaint();
}
//------------------------------------------------------------------------------
bool TFormArchiveView::OnAddItem2File(TComboBox* cb, const AnsiString& fileName, const AnsiString& item)
{
    const bool ret = DirectoryExists(item);
    return ret;
}
//------------------------------------------------------------------------------
AnsiString TFormArchiveView::GetTreeNodePath(TTreeNode *node)
{
     return ( node && node->Level==2) ? HtgArchivePath() +
        "\\" + node->Parent->Parent->Text +
        "\\" + node->Parent->Text +
        "\\" + node->Text : AnsiString("");
}
//---------------------------------------------------------------------------
void TFormArchiveView::SetCurrentHtg( const HtgItem &htg )
{


    if( !( MyFileExists(htg.fileName[0]) ) )
    {
        ResetCurrentHtg();
        Memo1->Lines->Text = "Отсутствует файл хроматограммы";
        return;
    }

    try
    {
        Hromatogramm htg0( htg.fileName[0] );

        ShowHromatogrammMain( &htg0, frameGraph_[0], grdPhysChem, grdPic[0]  );

        my::Grd::NormalizeGridHeight( grdPic[0] );


        if( MyFileExists(htg.fileName[1])  )
        {
            Hromatogramm htg1( htg.fileName[1] );
            ShowHromatogrammMain( &htg1, frameGraph_[1], NULL, grdPic[1]  );
            my::Grd::NormalizeGridHeight( grdPic[1] );
        }



        Panel4->Caption = " Стандартные условия  сгорания 25°C, 101.325 кПа. "
            "Стандартные условия  измерения 20°C, 101.325 кПа. \""+
            HromatFileSign::FormatCode( htg0.GetSign() )+"\"";
        PageControl1->Visible = 1;

        AnsiString fn = MakeHromatHTMLReport( GetReportData(htg), CheckBox4->Checked, CheckBox5->Checked );
        CppWebBrowser1->Navigate( WideString( fn ));
        

    } catch(...)
    {
        const DWORD dlgResult = ::MessageBox
        (
        	Handle, MYSPRINTF_
        	(
            	"Некорректные данные хроматограммы\n"
                "%d от %s\n"
                "Удалить файлы хроматограммы из архива?",
        		htg.file,  FormatDateTime("dd.mm.yyyy hh:nn", htg.dateTime)
        	).c_str(),
        	"Некорректные данные хроматограммы", MB_YESNO | MB_ICONERROR | MB_SYSTEMMODAL
        );
        if( dlgResult==IDYES )
        {
        	::DeleteFile( htg.fileName[0].c_str() );
            ::DeleteFile( htg.fileName[1].c_str() );
        }
        Memo1->Lines->Text = "Некорректные данные хроматограммы";
        ResetCurrentHtg();
    }
}
//------------------------------------------------------------------------------
void TFormArchiveView::ResetCurrentHtg( )
{
    PageControl1->Visible = 0;
    ShowHromatogrammMain( NULL, frameGraph_[0], grdPhysChem, grdPic[0]  );
    ShowHromatogrammMain( NULL, frameGraph_[1], grdPhysChem, grdPic[1]  );
}
//------------------------------------------------------------------------------
void TFormArchiveView::HandleEndReadArchiveFile(unsigned file, TDateTime dateTime)
{
    typedef HtgItemList::MapT5::iterator IterT;
    for( IterT i=map5_.begin(); i!=map5_.end(); ++i)
    {
        const HtgItem& itm = *(i->second);
        if(
            itm.file!=file ||
            itm.dateTime!=dateTime 

            ) continue;
        TTreeNode* nd = i->first;
        //if( nd->IsVisible )
        nd->MakeVisible();
        trv1->Selected = nd;
    }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void __fastcall TFormArchiveView::pnLeftTopCanResize(TObject *Sender,
      int &NewWidth, int &NewHeight, bool &Resize)
{
    Resize = NewWidth>50 && NewHeight>50;
}
//---------------------------------------------------------------------------
void __fastcall TFormArchiveView::pnLeftCanResize(TObject *Sender,
      int &NewWidth, int &NewHeight, bool &Resize)
{
	Resize = NewWidth>20;
}
//---------------------------------------------------------------------------
void __fastcall TFormArchiveView::trv1Compare(TObject *Sender,
      TTreeNode *node1, TTreeNode *node2, int Data, int &compareResult)
{
	compareResult = (int)node1->Data - (int)node2->Data;
}
//---------------------------------------------------------------------------

void __fastcall TFormArchiveView::PageControl1Change(TObject *Sender)
{
    TTabSheet *activePage = PageControl1->ActivePage;
    TFrameGraph *frameGraph = NULL;
    if(activePage==TabSheet5) frameGraph=frameGraph_[0];
    if(activePage==TabSheet1) frameGraph=frameGraph_[1];
    Splitter3->Visible = frameGraph;
    PanelLeftBottom->Top = 100000;
    PanelLeftBottom->Visible = frameGraph;
    frameGraph_[0]->ScrollBox1->Visible = activePage==TabSheet5;
    frameGraph_[1]->ScrollBox1->Visible = activePage==TabSheet1;

}
//---------------------------------------------------------------------------

void __fastcall TFormArchiveView::FormMouseWheelUp(TObject *Sender,
      TShiftState Shift, TPoint &MousePos, bool &Handled)
{
    frameGraph_[0]->OnMouseWheel( GetVCLControlAtPos( this,  MousePos ), true );
    frameGraph_[1]->OnMouseWheel( GetVCLControlAtPos( this,  MousePos ), true );
}
//---------------------------------------------------------------------------

void __fastcall TFormArchiveView::FormMouseWheelDown(TObject *Sender,
      TShiftState Shift, TPoint &MousePos, bool &Handled)
{
    frameGraph_[0]->OnMouseWheel( GetVCLControlAtPos( this,  MousePos ), false );
    frameGraph_[1]->OnMouseWheel( GetVCLControlAtPos( this,  MousePos ), false );
}
//---------------------------------------------------------------------------
void __fastcall TFormArchiveView::Button1Click(TObject *Sender)
{

    AnsiString dir = edPath->Text;
    if( !SelectDirectory("Путь к файлам хроматограмм", "", dir) ||
        !DirectoryExists(dir) )
        return;
    if( edPath->Items->IndexOf(dir)==-1 )
        ComboboxList::AddNewItem(edPath, dir);
    SetComboBoxTextItemIndex(edPath, dir );
    edPathChange(NULL);
}
//---------------------------------------------------------------------------


void __fastcall TFormArchiveView::trv1Change(TObject *Sender,
      TTreeNode *obj)
{
    if( trv1->SelectionCount>1  ) return;
    HtgItemList::MapT5::const_iterator i = map5_.find(trv1->Selected);
    if( i!=map5_.end() )
    {
        SetCurrentHtg( *(i->second) );
    }
    else
    {     
        Memo1->Lines->Text = "";
        ResetCurrentHtg( );
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormArchiveView::grdPhysChemMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
    my::Grd::SetupVisibleColsOnMouseDownRow0(Sender, Button, Shift, X, Y, 0);
}
//------------------------------------------------------------------------------
void __fastcall TFormArchiveView::menuAnalyze1Click(TObject *Sender)
{
    CalculateMidleValues(true);
}
//------------------------------------------------------------------------------
void __fastcall TFormArchiveView::PopupMenu1Popup(TObject *Sender)
{
    TTreeNode *nd = trv1->Selected;
    menuAnalyze1->Enabled = nd!=NULL && (nd->Level==2 || nd->Level==3);
    menuAnalyze2->Enabled = menuAnalyze1->Enabled; 
}
//------------------------------------------------------------------------------
void __fastcall TFormArchiveView::lblFilterClick(TObject *Sender)
{
    treePath_ =  NodePathToStr( trv1->Selected );
    boost::shared_ptr<TFormHtgFilter> dlg( new TFormHtgFilter(NULL) );
    dlg->Label2->Caption = filter_.rgm==-1 ? AnsiString(" ") : IntToStr(filter_.rgm);
    dlg->CheckBox1->Checked = filter_.izm;
    dlg->CheckBox2->Checked = filter_.grad;
    TPoint pt;
    ::GetCursorPos( &pt );
    dlg->Top = pt.y; dlg->Left = pt.x;
    dlg->ShowModal();

    if(dlg->ModalResult!=mrOk) return;

    filter_.rgm = StrToIntDef(dlg->Label2->Caption, -1);
    filter_.izm = dlg->CheckBox1->Checked;
    filter_.grad = dlg->CheckBox2->Checked;
    const AnsiString s = ": "+HtgFilter2Str( filter_ ), s1 = "Фильтр";
    lblFilter->Caption = s1 + (s.IsEmpty() ? AnsiString("...") : s);
    if(RadioButton1->Checked)
        MakeHromatogramsTree( edPath->Text, trv1, map4_, map5_, items_, filter_.rgm,
            filter_.izm, filter_.grad );
    else
        MakeTreeFromHtgItemsArray( deviceArchive_ );

    SelectNodeByPath(trv1, treePath_ );


}
//---------------------------------------------------------------------------
void __fastcall TFormArchiveView::menuAnalyze2Click(TObject *Sender)
{
    CalculateMidleValues(false);
}
//---------------------------------------------------------------------------

void __fastcall TFormArchiveView::edPathChange(TObject *Sender)
{
    treePath_ =  NodePathToStr( trv1->Selected );
    MakeHromatogramsTree( edPath->Text, trv1, map4_, map5_, items_, filter_.rgm,
        filter_.izm, filter_.grad );
    SelectNodeByPath(trv1, treePath_ );
}
//---------------------------------------------------------------------------

void __fastcall TFormArchiveView::N1Click(TObject *Sender)
{
    if( ::MessageBox( Screen->ActiveForm->Handle,
        "Вы уверены, что хотите удалить выбранные хроматограммы из архива?",
        "Архив Хромат-900", MB_YESNO | MB_ICONEXCLAMATION)!=IDYES)
        return;

    std::vector<TTreeNode*> nds;
    GetAllSelectedNodes( trv1, nds);
    for( int iNode=0; iNode<nds.size(); ++iNode )
    {
        DeleteHtgFile(nds[iNode], map5_);
        //nds[iNode]->Delete();
    }
}
//---------------------------------------------------------------------------

void __fastcall TFormArchiveView::N2Click(TObject *Sender)
{
    HtgItemsArray items;
    GetSelectedArchiveHtgItems(items);
    Hromat900::Instance().ReadDeviceArchiveFile( items );
}
//---------------------------------------------------------------------------



void __fastcall TFormArchiveView::RadioButton2Click(TObject *Sender)
{

    if( RadioButton2->Checked )
    {
        Panel1->Visible = 0;
        trv1->Items->Clear();
        Hromat900::Instance().InitializeDeviceArchive();
    }
    else
    {
        Hromat900::Instance().CancelReadDeviceArchive();
        const AnsiString sPath = HtgArchivePath();
        MakeHromatogramsTree( sPath, trv1, map4_, map5_, items_, filter_.rgm,
            filter_.izm, filter_.grad );
        SelectNodeByPath(trv1, treePath_ );
        SetComboBoxTextItemIndex(edPath, sPath );
        Panel1->Visible = 1;
    }
}
//---------------------------------------------------------------------------


void __fastcall TFormArchiveView::trv1Click(TObject *Sender)
{
    treePath_ =  NodePathToStr( trv1->Selected );    
}
//---------------------------------------------------------------------------

void __fastcall TFormArchiveView::trv1GetImageIndex(TObject *Sender,
      TTreeNode *Node)
{
    if( Node->Level==3 )
    {
        HtgItemList::MapT5::const_iterator i = map5_.find(Node);
        if( i!=map5_.end() )
        {
            if( HtgItemFilesExists(*(i->second)) )
                Node->ImageIndex = 4;
            else
                Node->ImageIndex = 5;
        }

    }
}
//---------------------------------------------------------------------------

void __fastcall TFormArchiveView::N3Click(TObject *Sender)
{
    HtgItemsArray items;
    GetSelectedArchiveHtgItems(items);
    items.erase( std::remove_if( items.begin(), items.end(), &HtgItemFilesExists ), items.end() );

    Hromat900::Instance().ReadDeviceArchiveFile( items );    
}
//---------------------------------------------------------------------------

void __fastcall TFormArchiveView::N5Click(TObject *Sender)
{
    ComboboxList::DeleteCurrentItem(edPath);
    edPath->ItemIndex = edPath->Items->Count==0 ? -1 : 0;
    edPathChange(edPath);     
}
//---------------------------------------------------------------------------


void __fastcall TFormArchiveView::CheckBox4Click(TObject *Sender)
{
    trv1Change( NULL, NULL);
}
//---------------------------------------------------------------------------

void __fastcall TFormArchiveView::CheckBox5Click(TObject *Sender)
{
    trv1Change( NULL, NULL);    
}
//---------------------------------------------------------------------------

void __fastcall TFormArchiveView::N6Click(TObject *Sender)
{


    TTreeNode *ndSel = trv1->Selected;
    if(ndSel==NULL || ndSel->Level<2 ) return;

    analyze_.conc.clear();
    analyze_.count = 0;
    analyze_.okrug = false;
    analyze_.conc.resize( Gas::Count() );

    if( ndSel->Level==3 )
        for( int iNode=0; iNode<trv1->SelectionCount; ++iNode )
        {
            HtgItemList::MapT5::const_iterator iHtg = map5_.find(trv1->Selections[iNode]);
            if( iHtg!=map5_.end() )
                ProcesItem( *(iHtg->second), analyze_);
        }

    if( ndSel->Level==2 )
        for( int iNode=0; iNode<ndSel->Count; ++iNode )
        {
            HtgItemList::MapT5::const_iterator iHtg = map5_.find(ndSel->Item[iNode]);
            if( iHtg!=map5_.end() )
                ProcesItem( *(iHtg->second), analyze_);
        }
    trv1->Repaint();
    
    TFormIzobutChart *dlg = new TFormIzobutChart(NULL);
    dlg->Panel1->Visible = false;
    dlg->Panel2->Visible = false;

    for(int n=0; n<analyze_.IzobutConc.size(); ++n)
    {
        dlg->AddPoint(analyze_.IzobutConc[n].first, analyze_.IzobutConc[n].second);
    }

    dlg ->FormStyle = fsNormal; 
    dlg -> Hide();
    dlg->ShowModal();
    trv1->Repaint();
    delete dlg;
}
//---------------------------------------------------------------------------

