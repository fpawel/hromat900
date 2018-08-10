//---------------------------------------------------------------------------
#include <DateUtils.hpp>
#pragma hdrstop

#include "uFormHromatArchiveDlg.h"
#include "SGrdHlpr.h"

// общие
#include "TaskList.h"
#include "iniUtils_.h"
#include "xmlUtils_.h"
#include "MyAppProfile.h"
#include "uDrawGrids.h"

//  Хромат 900
#include "hromat_sys.h"
#include "htg_.h"
#include "HromatTask.h"
#include "TransferManage.h"
#include "Unit1.h"
#include "uFrmWait.h"
#include "MyVclUtils_.h"
#include "uFormTimeFilterDlg.h"
#include "uFormArchiveView.h"


//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
//---------------------------------------------------------------------------
enum{ COL_COUNT = 6 };
//---------------------------------------------------------------------------





namespace MFI
{   
    struct IsDateTimeInterval
    {
        bool operator()(const HtgItem& itm) const
        {
            return itm.dateTime<t1 || itm.dateTime>t2;
        }
        TDateTime t1, t2;
    };

    struct IsNotInStringGridSelection
    {
        bool operator()(const HtgItem& itm) const
        {
            const unsigned file = itm.file;
            const AnsiString s = IntToStr(file);
            for( int ro=grd->Selection.Top; ro<=grd->Selection.Bottom; ++ro )
                if(grd->Cells[1][ro]==s)
                    return 0;
            return 1;
        }
        TStringGrid *grd;
    };

    struct IsInStringGridSelection
    {
        bool operator()(const HtgItem& itm) const
        {
            const unsigned file = itm.file;
            const AnsiString s = IntToStr(file);
            for( int ro=grd->Selection.Top; ro<=grd->Selection.Bottom; ++ro )
                if(grd->Cells[1][ro]==s)
                    return 1;
            return 0;
        }
        TStringGrid *grd;
    };

        bool IsDateLess(const HtgItem& itm1, const HtgItem& itm2)
        {
            return itm1.dateTime<itm2.dateTime;
        }

    void FilterDateTime(HtgItemsArray& lst, TDateTime dateFrom, TDateTime dateTo)
    {
        IsDateTimeInterval foo;
        foo.t1 = dateFrom;
        foo.t2 = dateTo;
        lst.erase( std::remove_if( lst.begin(), lst.end(), foo ), lst.end() );
    }
};
//---------------------------------------------------------------------------
TFormHromatArchiveDlg *FormHromatArchiveDlg;
//---------------------------------------------------------------------------
__fastcall TFormHromatArchiveDlg::TFormHromatArchiveDlg(TComponent* Owner)
    : TForm(Owner),
    formArchiveView( new TFormArchiveView(NULL, false) ),
    xml_( TiXML::GiveNotNulElement( ProfileXmlElem(), "HROMAT_ARCHIVE_DIALOG_FORM") ),
    statusPanel_(NULL), isActivated_(0)
{
    formArchiveView->Panel1->Visible = 0;
    MakeLabelAsButton( Label3 );
    MakeLabelAsButton( Label4 );
    MakeLabelAsButton( Label1 );
    items_ = origItems_;
    formArchiveView->Panel3->Parent = this;
    formArchiveView->Align = alClient;
    Panel5->Parent = formArchiveView->pnLeft;
    Panel5->Align = alTop;
}
//---------------------------------------------------------------------------
__fastcall TFormHromatArchiveDlg::~TFormHromatArchiveDlg()
{
    UnregisterAllLabelsAsButton( this );
    if( !isActivated_ ) return;
    TiXML::SaveWindowPlacement( TiXML::GiveNotNulElement( xml_, "Placement" ), Handle );
    //WRITE_XML_STR_( "COLS1", my::Grd::GetVisColsInfo(grd1) );
}
//---------------------------------------------------------------------------
void __fastcall TFormHromatArchiveDlg::FormActivate(TObject *Sender)
{
    if( isActivated_ ) return;
    TiXML::LoadWindowPlacement( TiXML::GiveNotNulElement( xml_, "Placement" ), Handle );
    isActivated_ = true;
}
//---------------------------------------------------------------------------

void __fastcall TFormHromatArchiveDlg::FormClose(TObject *Sender,
      TCloseAction &Action)
{
    Hromat900::Instance().GetTransferManager().StopThread();
    if( Form1 )
    {
        Form1->panelStatus->Parent = Form1;
        ::SetParent( ::GetConsoleWindow(), Form1->Handle );
        Form1->Show();
    }

    if( ModalResult==mrOk ) return;
    Hromat900T& devSys  = Hromat900::Instance();
    devSys.GetTransferManager().StopThread();
    TaskList& taskList = devSys.GetTransferManager().GetTaskList();
    taskList.EraseTasksOfClass<HandleStartArchiveProcess>();
    taskList.EraseTasksOfClass<ReadArchiveHTGTask>();
    devSys.GetTransferManager().StartThread();

}
//------------------------------------------------------------------------------
void TFormHromatArchiveDlg::ShowArchive()
{
    //formArchiveView->Splitter2->Visible = 1;
    //formArchiveView->Splitter2->Left = 100000;
    //formArchiveView->PageControl1->Visible = 1;
}
//------------------------------------------------------------------------------
void TFormHromatArchiveDlg::SetItems(const HtgItemsArray& items)
{
    formArchiveView->MakeTreeFromHtgItemsArray(items);
    origItems_ = items;
    items_ = items;
}
//---------------------------------------------------------------------------

void __fastcall TFormHromatArchiveDlg::Button2Click(TObject *Sender)
{
    ModalResult = mrCancel;
    Close();
}
//---------------------------------------------------------------------------

void __fastcall TFormHromatArchiveDlg::grd1MouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	my::Grd::SetupVisibleColsOnMouseDownRow0(Sender, Button, Shift, X, Y, 0);
}
//---------------------------------------------------------------------------

void __fastcall TFormHromatArchiveDlg::Label3Click(TObject *Sender)
{
    Hromat900::Instance().ReadDeviceArchiveFile( items_ );    
}
//---------------------------------------------------------------------------
void __fastcall TFormHromatArchiveDlg::Label4Click(TObject *Sender)
{
    boost::shared_ptr<TFormTimeFilterDlg> dlg(new TFormTimeFilterDlg(NULL) );

    TDateTime
    minDate = std::min_element( items_.begin(), items_.end(), MFI::IsDateLess )->dateTime,
    maxDate = std::max_element( items_.begin(), items_.end(), MFI::IsDateLess )->dateTime;

    dlg->edDate1->DateTime = minDate;
    dlg->edTime1->DateTime = minDate;
    dlg->edDate2->DateTime = maxDate;
    dlg->edTime2->DateTime = maxDate;
    
    dlg->ShowModal();
    if(dlg->ModalResult==mrCancel) return;

    items_ = origItems_;

    if(dlg->ModalResult==mrOk)
    {
        TDateTime dFrom, dTo;
        Word Year, Month, Day, Hour, Min, Sec, MSec;

        DecodeDate(dlg->edDate1->DateTime, Year, Month, Day);
        DecodeTime(dlg->edTime1->DateTime, Hour, Min, Sec, MSec);
        dFrom = EncodeDateTime(Year, Month, Day, Hour, Min, Sec, MSec);

        DecodeDate(dlg->edDate2->DateTime, Year, Month, Day);
        DecodeTime(dlg->edTime2->DateTime, Hour, Min, Sec, MSec);
        dTo = EncodeDateTime(Year, Month, Day, Hour, Min, Sec, MSec);

        const AnsiString s1 = MyDateTimeToStr(dFrom), s2 = MyDateTimeToStr(dTo);
        MFI::FilterDateTime(items_, dFrom, dTo);
    }
    formArchiveView->MakeTreeFromHtgItemsArray(items_);
}
//---------------------------------------------------------------------------
void __fastcall TFormHromatArchiveDlg::Label1Click(TObject *Sender)
{
    items_ = origItems_;
    formArchiveView->MakeTreeFromHtgItemsArray(items_);
}
//---------------------------------------------------------------------------

