//------------------------------------------------------------------------------
#include <DateUtils.hpp>
#include <algorithm>
//------------------------------------------------------------------------------
#pragma hdrstop
//------------------------------------------------------------------------------
#include "uFormHromatogramm.h"

// my
#include "DrawGridCell.h"
#include "AnsiStringUtils_.h"


#include "xmlUtils_.h"
#include "uChartHelper.h"
#include "MouseEvt.h"
#include "SGrdHlpr.h"
#include "my_array_size.h"


#include "uPikTblHlpr.h"
#include "uFrameGraph.h"
#include "bcd_.h"
#include "Unit1.h"
#include "htg_.h"
#include "MyVclUtils_.h"
#include "HromatPic.h"
#include "HromatPhysChem.h"
#include "MyAppProfile.h"
#include "hromat_sys.h"

//------------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
//TFormHromatogramm *FormHromatogramm;
//------------------------------------------------------------------------------
TFormHromatogramm *FrmHTG[2];
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
AnsiString HromatogrammCaption( const HromatogrammConstPtr& htg)
{
    const HromatogramId& id = htg->GetId();
    const unsigned ptsCount = htg->GetPoints().size(),
    sign = htg->GetSign();
    const TDateTime duration = IncMilliSecond( 0, ptsCount*50);

    return AnsiString().sprintf( "%d   %s   -   %s   Обработка: %s Канал %d",
        id.fileIndex_,
        FormatDateTime( "hh:nn", id.date_ ),
        HromatFileType::ToStr( ExtractModbusFileType(id.rgmChanAndType_) ),
        HromatFileSign::FormatCode(sign),
        ExtractRGMChanal(id.rgmChanAndType_)
    );
}
//------------------------------------------------------------------------------
struct ComboBoxItmIdxHolder : public boost::noncopyable
{

    explicit ComboBoxItmIdxHolder(TComboBox *cb) :
        cb_(cb), itmIdx_( cb_->ItemIndex ), onChnge_( cb_->OnChange )
    {
        cb_->OnChange = NULL;
        cb_->ItemIndex = -1;
    }
    ~ComboBoxItmIdxHolder()
    {
        cb_->ItemIndex = itmIdx_;
        cb_->OnChange = onChnge_;
    }
    TComboBox *cb_;
    int itmIdx_;
    const TNotifyEvent onChnge_;
};
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
__fastcall TFormHromatogramm::TFormHromatogramm(TComponent* Owner,
	bool dtctrNum )	: TForm(Owner),
    // номер связанного с данным окном детектора
    dtctrNum_(dtctrNum),
    name_( MYSPRINTF_("D%d", dtctrNum) ),
    xml_( TiXML::GiveNotNulElement( ProfileXmlElem(), "DETECTOR"+IntToStr(dtctrNum+1) ) ),
    basePicsXML_( TiXML::GiveNotNulElement(xml_,"BASE_PICS_D"+IntToStr(dtctrNum) ) ),

    frameGraph_( new TFrameGraph(this, tbsGraph) ),

    pikTblHlpr_( new PikTableHlpr(dtctrNum_, grdBasePik, grdPik, grdHtgsList ) ),
    grph_( new TFastLineSeries(this) ),
    htgIdx_(-1)
{   
    assert(dtctrNum<3);
    #define BUTTON_STYLE_MULTILINE_(nn) SetWindowLong(##nn##->Handle, GWL_STYLE, GetWindowLong(##nn##->Handle,\
    	GWL_STYLE)|BS_MULTILINE);
    BUTTON_STYLE_MULTILINE_(btnSaveBasePicTable);
    BUTTON_STYLE_MULTILINE_(Button2);
    #undef BUTTON_STYLE_MULTILINE_
    
    // сразу же создаём график
    frameGraph_->AddSeries(grph_);
    frameGraph_->SetActiveSeries(0);
    frameGraph_->OnWheelMouse = OnWheelMouse;
    SetActiveHtgIndex(-1);

    InitializeGrdHtgsList(grdHtgsList);
    grdHtgsList->ColWidths[1] = 0;

    my::Grd::AddCopyItemToPopupMenu(grdPik);
    my::Grd::AddCopyItemToPopupMenu(grdBasePik);
    my::Grd::AddCopyItemToPopupMenu( grdHtgsList );


    my::Grd::SetVisColsInfo( grdPik, READ_XML_STR_(DETECTOR_PIK_TBL_VIS_COLS, "" ) );

    const HromatBasePicTable basePics = MakeBasePicTableFromXML(basePicsXML_);
    ShowHromatBasePicTable( grdBasePik, basePics.begin(), basePics.end() );

    cbHtgElem->Width = Panel1->Width - 40;

}

void __fastcall TFormHromatogramm::VisibleChanging()
{
    if (Visible) {
        FormStyle = fsNormal;
    }
    else
        FormStyle = fsMDIChild;
}

//------------------------------------------------------------------------------
__fastcall TFormHromatogramm::~TFormHromatogramm()
{
	WRITE_XML_STR_( DETECTOR_PIK_TBL_VIS_COLS, my::Grd::GetVisColsInfo(grdPik) );
    const HromatBasePicTable basePics = MakeBasePicTableFromGrd(grdBasePik);
    SaveHromatBasePicTable( basePicsXML_, basePics.begin(), basePics.end() );
    frameGraph_->RemoveSeries(grph_);
}
//------------------------------------------------------------------------------
TFastLineSeries *TFormHromatogramm::Graph()
{
    return grph_;
}
//------------------------------------------------------------------------------
TChart *TFormHromatogramm::Chart()
{
    return frameGraph_->chrt_;
}
//------------------------------------------------------------------------------
void __fastcall TFormHromatogramm::FormClose(TObject *Sender,
      TCloseAction &Action)
{
    Action = caNone;
    ShowWindow(Handle, SW_MINIMIZE);
}
//------------------------------------------------------------------------------
void TFormHromatogramm::OnWheelMouse(TObject*, TShiftState, const TPoint &mousePos, bool &Handled, bool up)
{
    TWinControl *ctrl = GetVCLControlAtPos( this,  mousePos );
    if( ctrl==cbHtgElem || ctrl==lblDtctrElemInfo)
    {
        up = !up;
        ActiveControl=NULL;
        const int count = cbHtgElem->Items->Count, oldIdx = cbHtgElem->ItemIndex;
        int idx = oldIdx + (up ? 1 : -1);
        if( count>0)
        {
            if(idx==-2 ) idx=count-1;
            if(idx>=count ) idx=-1;
        }
        cbHtgElem->ItemIndex = idx;
        cbHtgElem->OnChange(cbHtgElem);
    }
}
//------------------------------------------------------------------------------
void TFormHromatogramm::SetCbItemText(int n)
{
    if( n>-1 && n<(int)htgs_.size() )
    {
        const ComboBoxItmIdxHolder cbhldr(cbHtgElem);
        cbHtgElem->Items->Strings[n] = HromatogrammCaption(htgs_[n]);
    }
}
//------------------------------------------------------------------------------
void TFormHromatogramm::UpdateActiveHtgCaption()
{
    assert( htgIdx_==-1 || htgIdx_<cbHtgElem->Items->Count );
    const int sz = (int)htgs_.size();

    if( htgIdx_>-1 && htgIdx_<sz )
        lblDtctrElemInfo->Caption = AnsiString().sprintf("%d из %d", htgIdx_+1, sz);
    else
        lblDtctrElemInfo->Caption = sz==0 ? AnsiString( "Нет элементов") :
        AnsiString().sprintf("%d элементов", sz );
}
//------------------------------------------------------------------------------
void TFormHromatogramm::ClearHtgView()
{
    TChartSeries *grph = frameGraph_->GetSeries(0);
    grph->Active = 0;
    grph->Clear();
    my::Grd::ClearTStringGrid( grdPik, 1, 1 );
}

void TFormHromatogramm::SetActiveHtgIndex(int n)
{
    const bool changed = htgIdx_!=n;
    htgIdx_= n>-1 && n<(int)htgs_.size() ? n : -1;
    UpdateActiveHtgCaption();    

    if( changed ) grph_->Clear();

    cbHtgElem->OnChange = NULL;

    const Hromatogramm *curHtg = htgIdx_!=-1 ? htgs_[htgIdx_].get() : NULL;

    if( htgIdx_!=-1 )
    {
        ShowHromatogrammMain( curHtg, frameGraph_, NULL, grdPik  );
        grph_->Active = 1;
        cbHtgElem->ItemIndex = htgIdx_;
    }
    else
    {
        ClearHtgView();
        cbHtgElem->ItemIndex = -1;
        grph_->Active = 0;
    }

    cbHtgElem->OnChange = cbHtgElemChange;
    frameGraph_->ScrollBox1->Visible = grph_->Active;
}
//------------------------------------------------------------------------------
void TFormHromatogramm::AddHtg(HromatogrammPtr htg)
{
    const bool
        curentAddFirst = htgs_.empty(),
        curentHtgIdxWasLast = !curentAddFirst && ( htgIdx_== (int)htgs_.size()-1)
    ;

    const HromatogramId id = htg->GetId();
    assert(id.dtctr_==dtctrNum_);

    const int idxOfId = HromotogrammIndexOfIdInList(htgs_, id.fileIndex_);
    if( idxOfId==-1 )
    {
        htgs_.push_back( htg );
        std::sort(htgs_.begin(), htgs_.end(), HromatogrammDateLess() );
        const int idx = HromotogrammIndexOfIdInList(htgs_, htg->GetId().fileIndex_);
        assert(idx>-1);
        {
        const ComboBoxItmIdxHolder cbhldr(cbHtgElem);
        cbHtgElem->Items->Insert(idx, "" );
        SetCbItemText(idx);
        ActiveControl = 0;
        }
        if( curentAddFirst ) {
            SetActiveHtgIndex(idx);
        } else if( curentHtgIdxWasLast && idx==( (int)htgs_.size()-1) ) {
            //SetActiveHtgIndex(cbHtgElem->ItemIndex);
            if(Timer1->Enabled==false) {
                Timer1->Interval = Hromat900::Instance().DurHtg1()*1000*60;
                h_.idx = cbHtgElem->ItemIndex;
                h_.idxToSet = idx;
                Timer1->Enabled = 1;
            }
        }
        else
            UpdateActiveHtgCaption();
    }
    else
    {
        htg = htgs_[idxOfId];
        UpdateView( htg.get() );
    }

    MakeGrdHtgsList(grdHtgsList, htgs_ );
}
//------------------------------------------------------------------------------
void TFormHromatogramm::AddHtg(HromatogrammPtr* htg, HromatogrammPtr* htgEnd)
{
    for( ; htg!=htgEnd; ++htg )
    {
        const HromatogramId id = (*htg)->GetId();
        assert(id.dtctr_==dtctrNum_);
        if( HromotogrammIndexOfIdInList(htgs_, id.fileIndex_)==-1 )
            htgs_.push_back( *htg );
    }
    std::sort(htgs_.begin(), htgs_.end(), HromatogrammDateLess() );

    const ComboBoxItmIdxHolder cbhldr(cbHtgElem);
    cbHtgElem->Items->Clear();
    for( unsigned i=0; i<htgs_.size(); ++i )
    {
        cbHtgElem->Items->Add("");
        SetCbItemText(i);
    }
    UpdateActiveHtgCaption();
    MakeGrdHtgsList(grdHtgsList, htgs_ );
}

//------------------------------------------------------------------------------
void TFormHromatogramm::UpdateView( const Hromatogramm* htg)
{
    const int idx = HromotogrammIndexOfIdInList(htgs_, htg->GetId().fileIndex_);
    if( idx==-1 ) return;
    ShowHTGInGrd(grdHtgsList, htgs_.begin() )( htgs_[idx] );
    SetCbItemText(idx);
    if(idx==htgIdx_) {
        ShowHTGGraph(htg, frameGraph_);


        const HromatPicTable& tbl = htg->GetPicTable();

        ShowHromatPicTable( grdPik, tbl.begin(), tbl.end() );


    }


}
//------------------------------------------------------------------------------
void TFormHromatogramm::SetBasePikTbl( const HromatBasePicTable& pics )
{
    ShowHromatBasePicTable( grdBasePik, pics.begin(), pics.end() );
}
//------------------------------------------------------------------------------
HromatBasePicTable TFormHromatogramm::GetBasePikTbl()
{
    return MakeBasePicTableFromGrd(grdBasePik);
}
//------------------------------------------------------------------------------
void __fastcall TFormHromatogramm::btnSaveBasePicTableClick(
      TObject *Sender)
{
    Hromat900::Instance().WriteBasePicTable( dtctrNum_, GetBasePikTbl() );
}
//------------------------------------------------------------------------------
void __fastcall TFormHromatogramm::Button2Click(TObject *Sender)
{
    Hromat900::Instance().ReadBasePicTable(dtctrNum_);
}
//------------------------------------------------------------------------------
void __fastcall TFormHromatogramm::cbHtgElemChange(TObject *Sender)
{
    const int idx = cbHtgElem->ItemIndex;
    if( idx<(int)htgs_.size() )
        SetActiveHtgIndex( idx );
}
//------------------------------------------------------------------------------
void __fastcall TFormHromatogramm::grdPikMouseWheelDown(TObject *Sender,
      TShiftState Shift, TPoint &MousePos, bool &Handled)
{
    OnWheelMouse(Sender, Shift, MousePos, Handled, false);
}
//------------------------------------------------------------------------------
void __fastcall TFormHromatogramm::grdPikMouseWheelUp(TObject *Sender,
      TShiftState Shift, TPoint &MousePos, bool &Handled)
{
    OnWheelMouse(Sender, Shift, MousePos, Handled, true);
}
//------------------------------------------------------------------------------
void __fastcall TFormHromatogramm::grdPikMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	my::Grd::SetupVisibleColsOnMouseDownRow0(Sender, Button, Shift, X, Y, 0);
}
//---------------------------------------------------------------------------


void __fastcall TFormHromatogramm::grdHtgsListDblClick(TObject *Sender)
{
	int col,row;
    TPoint pt;
    ::GetCursorPos(&pt);
    pt = grdHtgsList->ScreenToClient(pt);
	grdHtgsList->MouseToCell(pt.x, pt.y, col, row);
    if( row>0)
    {
    	SetActiveHtgIndex(row-1);
    	PageControl1->ActivePageIndex = 0;
    }    
}
//---------------------------------------------------------------------------



void __fastcall TFormHromatogramm::Timer1Timer(TObject *Sender)
{
    if(h_.idx==cbHtgElem->ItemIndex) {
        h_.idx = cbHtgElem->ItemIndex;
        SetActiveHtgIndex( h_.idxToSet );
    }
    Timer1->Enabled = false;
}
//---------------------------------------------------------------------------



void __fastcall TFormHromatogramm::grdBasePikKeyDown(TObject *Sender,
      WORD &Key, TShiftState Shift)
{
    if(Key==VK_ESCAPE) {
        TStringGrid* grd = (TStringGrid*) Sender; 
        my::Grd::InveertRowselection(grd, grd->Row);

    }    
}
//---------------------------------------------------------------------------

