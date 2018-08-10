//------------------------------------------------------------------------------
#include <algorithm>
//------------------------------------------------------------------------------
#pragma hdrstop
//------------------------------------------------------------------------------
#include "hromat_sys.h"
//------------------------------------------------------------------------------
#include "AnsiStringUtils_.h"
#include "bcd_.h"
#include "..\my_include\CRC16.h"
#include "..\my_include\FooCalledOnce.hpp"
#include "xmlUtils_.h"
#include "MyFileCtrl.h"
#include "MyProcEntry.h"

//vcl
#include <math.hpp>
#include <DateUtils.hpp>
#include <FileCtrl.hpp>
#include <inifiles.hpp>

#include "AppEvts.h"


// для работы с сиситемой ввода-вывода хроматографа
#include "TransferManage.h"
#include "HromatTask.h"
#include "TaskList.h"
// для определения функции PerformModbusTransfer и GetHromatMaxRxDFrameSize
#include "PerformTransfer.h"

// формы
#include "Unit1.h"
#include "MyAppProfile.h"
#include "MyAppWinPath.hpp"
#include "guicon.h"
#include "MySynchronize.h"
//#include "HromatCfgSysHlpr.h"
//#include "uFormHromatSysConfigDialog.h"
#include "grdtns_.h"
#include "cnfdntl_.h"
#include "CurHtg.h"
#include "myformat.hpp"
#include "MyExcpt.hpp"
#include "uHromatSetsDlg.h"
#include "HromatGas.hpp"
#include "hrmtstat.h"
#include "uAppSetsDlg.h"
#include "TransmitInterfaceSets.h"
#include "TaskList.h"
#include "uFrmWait.h"
#include "uFormArchiveView.h"

//------------------------------------------------------------------------------
#pragma package(smart_init)
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void CopyWordToPChar( char *p, unsigned short int v )
{ *p = v>>8; *(p+1)=v; }


//------------------------------------------------------------------------------
Hromat900T::Hromat900T() :
	logLifeTime_( MakeMyLogQuote("Hromat900T.[members]") ),
    xml_( TiXML::GiveNotNulElement(ProfileXmlElem(), "Config") ),
    trnsfrMan_( new TransferManager ),
    // параметры хроматографа
    hromatSysConfig_( HromatVar::GetList().size()+1 ),
    enebleReadDetectorIndication_( TiXML::GetIntAtrDef(xml_, "enable_read_detectors_indications", 1) ),
    isWarningSoundEnabled_( TiXML::GetIntAtrDef(xml_, "разрешить_звук", 1) ),
    currHtgHelper_(),
    userDiskretTimeMSec_ ( TiXML::GetIntAtrDef( xml_, "diskrete_time_msec_last_readed", 50) ),

    durHtg1_ ( TiXML::GetIntAtrDef( xml_, "durhtg1minute", 1) ),
    detectorsCount_( TiXML::GetIntAtrDef( xml_, "detectors_count", 2) ),

    htgArchivePath_( READ_XML_STR_( HTGS_DIR_SEKT, DefaultHtgArchivePath() )),
    mestoOtboraProby_( READ_XML_STR_( "место_отбора_пробы", "..." )),
    hromatType_( READ_XML_STR_( "тип_хроматографа", "..." ))
{
	LOG_FOO_LIFETIME_QUOTES;
    ASSERT_FUNCTION_CALLED_ONCE__;
    Gas::Count();
    HromatVar::GetDiskreteTimeVarIndex();
    HromatVar::GetRoundingPerGOSTIndex();
    HromatVar::GetKoefDiskreteTimeIndex();
    //InitializeTaskList();
    trnsfrMan_->SetOnNotifyTaskList(&CheckAndCorrectTaskList);
}
//------------------------------------------------------------------------------
Hromat900T::~Hromat900T()
{
	LOG_FOO_LIFETIME_QUOTES;
    ASSERT_FUNCTION_CALLED_ONCE__;
}
//------------------------------------------------------------------------------
double Hromat900T::GetKoefficient(unsigned n) const
{
    return n<hromatSysConfig_.size() ? hromatSysConfig_[n] : 0;
}
//------------------------------------------------------------------------------
double Hromat900T::DetectorsCount()
{
    return detectorsCount_;
}

bool Hromat900T::GraduateIsEnabled() const
{
    double x = hromatSysConfig_.at( 63 );
    bool y = x==0;
    return y;
}

// длительность хроматограммы в минутах
unsigned Hromat900T::HtgDuration() const
{
    return Ceil( hromatSysConfig_.at( 5 ) );
}

double Hromat900T::FlowRateCfg(bool dtctr)
{
    return hromatSysConfig_.at( 6 + dtctr );
}

double Hromat900T::ColumnTemperatureCfg()
{
    return hromatSysConfig_.at( 8 );
}

bool Hromat900T::IsRoundingGOST() const
{
    return hromatSysConfig_.at( HromatVar::GetRoundingPerGOSTIndex() );
}
//------------------------------------------------------------------------------
void Hromat900T::InitializeTaskList()
{
    trnsfrMan_->GetTaskList().Clear();

    /**
    *       инициализация - еденичные задания
    */
    // запрос параметров конфигурации хроматографа
    trnsfrMan_->AddTask( new ReadHromatConfigTask( &HandleReadedHromatSysConfig,
        hromatSysConfig_ )  );
    trnsfrMan_->AddTask( new ReadBasePicTableTask(0) );
    trnsfrMan_->AddTask( new ReadBasePicTableTask(1) );
    // Установить в хроматограф текущие дату и время
    trnsfrMan_->AddTask( new SetHromatDatetimeTask );
    /**
    *       Задания, выполняющиеся постоянно
    */
    // запрос состояния хроматографа
    trnsfrMan_->AddTask( new HandleReadDeviceState(&HandleDeviceStateReaded) );
    // Запрос показаний каналов
    trnsfrMan_->AddTask( new HandleReadModbusFloatValues(
        0, HromatChanal::COUNT, CONTINUESLY_TASK, "Запрос показаний каналов",
        &HandleChnslReaded_ ));    

    // Запрос информации о текущих хроматограммах из регистров MODBUS
    currHtgHelper_.reset( new CurrHtgHelperT(enebleReadDetectorIndication_) );
    trnsfrMan_->AddTask( new ProcessCurrenHromatogramm( *currHtgHelper_.get() ) );
}
//------------------------------------------------------------------------------
void Hromat900T::CheckAndCorrectTaskList(TaskList& taskList)
{
    const bool goodTaskList =
        (taskList.CountTasksOfClass<HandleReadDeviceState>()==1) &&
        (taskList.CountTasksOfClass<ProcessCurrenHromatogramm>()==1);
    if(!goodTaskList) InitializeTaskList();
}
//------------------------------------------------------------------------------
void Hromat900T::RequestMeasurementArchives()
{
    trnsfrMan_->StopThread();
    trnsfrMan_->ShowTopPanel("Запрос архива измерений...");
    trnsfrMan_->AddTask( new  ReadMeasurmentsJournalsTask, TransferManager::ADD_TASK_HEAD );
    trnsfrMan_->StartThread();
}
//------------------------------------------------------------------------------

void Hromat900T::SetCicle(unsigned cicle)
{
    const StopThreadInThisFoo stopThreadInThisFoo;
    trnsfrMan_->GetTaskList().EraseTasksOfClass<HromatCicleControl>();
    trnsfrMan_->AddTask( new HromatCicleControl( cicle ),
    	TransferManager::ADD_TASK_HEAD );
}
//------------------------------------------------------------------------------
void Hromat900T::WriteBasePicTable( bool dtctr, const HromatBasePicTable& tbl)
{
    const StopThreadInThisFoo stopThreadInThisFoo;
    trnsfrMan_->AddTask( new WriteBasePicTableTask(dtctr, tbl), TransferManager::ADD_TASK_HEAD );
}
//------------------------------------------------------------------------------
void Hromat900T::ReadBasePicTable(bool dtctr)
{
    const StopThreadInThisFoo stopThreadInThisFoo;
    trnsfrMan_->AddTask( new ReadBasePicTableTask(dtctr), TransferManager::ADD_TASK_HEAD );
}
//------------------------------------------------------------------------------
void Hromat900T::HandleReadedHromatSysConfig(const VDouble& newHromatSysConfig)
{
    if(Form1)
    {
        Form1->menuHromatParams->Enabled = true;
    }

    hromatSysConfig_ = newHromatSysConfig;
    if( TFormHromatSetsDlg* dlg = dynamic_cast<TFormHromatSetsDlg*>(Screen->ActiveForm) )
        dlg->UpdateValues(hromatSysConfig_);

    using namespace HromatVar;
    userDiskretTimeMSec_ = hromatSysConfig_[GetDiskreteTimeVarIndex()] * 1000.0 *
        hromatSysConfig_[GetKoefDiskreteTimeIndex()];

    TiXML::SetIntAtr( xml_, "diskrete_time_msec_last_readed", userDiskretTimeMSec_);

    detectorsCount_ = hromatSysConfig_.at( 3 );
    TiXML::SetIntAtr( xml_, "detectors_count", detectorsCount_);

    ProfileXmlDoc()->SaveFile();

    Form1->UpdateDetector2Info( DetectorsCount()==2 );

    if(!this->GraduateIsEnabled())
        Form1->Label2->Caption = "Автоматическая градуировка запрещена";
            


}
//------------------------------------------------------------------------------
void Hromat900T::HandleChnslReaded_(const double* dt, const double* dtEnd)
{
	assert( dtEnd-dt==HromatChanal::COUNT );
    MyWCout("Получены показания каналов.\n");
    if( Form1!=NULL )
        Form1->HandleChnlsReaded(dt,dtEnd);

}
//------------------------------------------------------------------------------
void Hromat900T::HandleDeviceStateReaded( unsigned deviceStatecode, unsigned cicleType)
{
    Form1->HandleDeviceStateReaded( deviceStatecode, cicleType);
    currentHromatCicleType_ = cicleType;
}

//------------------------------------------------------------------------------
void ExtractDtctrHTGInfo(const VDouble& modbusRegs, bool dtctr, ModbusFileInfo* pFi)
{
    ModbusFileInfo& fi = pFi[dtctr];
    fi.index_ =  Ceil( modbusRegs[dtctr] );
    fi.size_ = Ceil( modbusRegs[2 + dtctr*2] );
    fi.sign_ = Ceil( modbusRegs[2 + dtctr*2 + 1] );
    MYWCOUT_(" -- HTG%d:%d размер:%d признак:%d -- \n", dtctr, fi.size_, fi.sign_ ));
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void __fastcall Hromat900T::ClickWriteHromatSysConfig( TObject* )
{
    TFormHromatSetsDlg *frm = dynamic_cast<TFormHromatSetsDlg*>( Screen->ActiveForm );
    if( frm==NULL ) return;
    VDouble dlgResult;
    if( !frm->ValidateResult( dlgResult ) ) return;
    hromatSysConfig_ = dlgResult;

    trnsfrMan_->StopThread();
    trnsfrMan_->AddTask( new WriteHromatConfigTask(hromatSysConfig_), TransferManager::ADD_TASK_HEAD );
    trnsfrMan_->StartThread();
    trnsfrMan_->ShowTopPanel("Установка параметров конфигурации...");

}
//------------------------------------------------------------------------------
void __fastcall Hromat900T::ClickReadHromatSysConfig(TObject*)
{
    // приостановить поток
    trnsfrMan_->StopThread();
    trnsfrMan_->AddTask( new ReadHromatConfigTask( &HandleReadedHromatSysConfig, hromatSysConfig_ ),
        TransferManager::ADD_TASK_HEAD );
    trnsfrMan_->StartThread();
    trnsfrMan_->ShowTopPanel("Запрос параметров конфигурации...");

}
//------------------------------------------------------------------------------
void Hromat900T::SetupSysConfigDialog()
{
    if( !IsLabourAutorized() )
    {
        ShowLabourMsg();
        return;
    }    
    const VDouble oldHromatSysConfig = hromatSysConfig_;
	boost::shared_ptr<TFormHromatSetsDlg> frmDlg( new TFormHromatSetsDlg(NULL, hromatSysConfig_) );
    frmDlg->btnWrite->OnClick = ClickWriteHromatSysConfig;
    frmDlg->btnRead->OnClick = ClickReadHromatSysConfig;
    frmDlg->ShowModal();

    VDouble dlgResult;
    if( !frmDlg->ValidateResult( dlgResult ) )
        dlgResult = frmDlg->Result();
    hromatSysConfig_ = dlgResult;
}
//------------------------------------------------------------------------------
void Hromat900T::SetupApplicationDialog()
{
    if( !IsLabourAutorized() )
    {
        ShowLabourMsg();
        return;
    }

    Hromat900T& dev = Hromat900::Instance();
    TransferManager& trnsfrMan = dev.GetTransferManager();
    boost::shared_ptr<TFormAppSetsDlg> dlg( new TFormAppSetsDlg(NULL) );

    const TransmitInterfaceSets currentItfsSets = trnsfrMan_->GetSets();

    using namespace AppSetsGrdItems::Row;
    #define CCELL_(ss) dlg->grd1->Cells[2][AppSetsGrdItems::Row::##ss##]
    CCELL_(Detector1) = Form1->GetDetectorType(0);
    CCELL_(Detector2) = Form1->GetDetectorType(1);
    CCELL_(Path) = htgArchivePath_;

    CCELL_(MestoOtboraProby) = mestoOtboraProby_;

    CCELL_(HromatType) = hromatType_;

    CCELL_(EnableReadDetectorIndication) = enebleReadDetectorIndication_ ?    "Да" : "Нет";

    CCELL_(EnableWarningSound) = isWarningSoundEnabled_ ? "Да" : "Нет";
    CCELL_(DiskreteTime) = userDiskretTimeMSec_;

    CCELL_(DurHtg1) = durHtg1_;
    CCELL_(DetectorsCount) = detectorsCount_;

         // инициализировать диалог настройками канала связи
    SetCtrlPrgrmmDlgItfcSets( dlg.get(), currentItfsSets );

    dlg->ShowModal();
    if( dlg->ModalResult!=mrOk ) return;

    Form1->grdToolInfo->Cells[1][0] = CCELL_(ModbusAddy);
    // типы детекторов
    Form1->SetDetectorType( 0, CCELL_(Detector1) );
    Form1->SetDetectorType( 1, CCELL_(Detector2) );
    enebleReadDetectorIndication_ = CCELL_(EnableReadDetectorIndication)=="Да";
    isWarningSoundEnabled_ = CCELL_(EnableWarningSound)=="Да";

    durHtg1_ = StrToIntDef( CCELL_(DurHtg1), 1);


    userDiskretTimeMSec_ = CCELL_(DiskreteTime).ToInt();

    //путь к архиву хроматограмм
	htgArchivePath_ = CCELL_(Path);
    mestoOtboraProby_ = CCELL_(MestoOtboraProby);
    hromatType_ = CCELL_(HromatType);
    if(CCELL_(DetectorsCount)!=AnsiString(detectorsCount_))
    {
        detectorsCount_ = StrToInt( CCELL_(DetectorsCount) );
        Form1->UpdateDetector2Info( detectorsCount_==2 );
    }
    

    WRITE_XML_STR_( HTGS_DIR_SEKT, htgArchivePath_ );

    WRITE_XML_STR_( "место_отбора_пробы", mestoOtboraProby_ );
    WRITE_XML_STR_( "тип_хроматографа", hromatType_ );
    TiXML::SetIntAtr( xml_, "detectors_count", detectorsCount_);
    WRITE_XML_STR_( "durhtg1minute", durHtg1_ );



    const TransmitInterfaceSets dlgItfsSets = GetCtrlPrgrmmDlgItfcSets( dlg.get() );
    const bool mustStopThread = !trnsfrMan.WasStopedOrTerminated() && dlg->IsItfcChnged();
    if( mustStopThread ) trnsfrMan.StopThread();
    // устанавливаем параметры соединения
    trnsfrMan.SetSets( dlgItfsSets );
    TiXML::SetIntAtr(xml_, "enable_read_detectors_indications",
        enebleReadDetectorIndication_);
    TiXML::SetIntAtr(xml_, "разрешить_звук", isWarningSoundEnabled_);
    TiXML::SetIntAtr( xml_, "diskrete_time_msec_last_readed", userDiskretTimeMSec_);

    SaveAppConfigFiles();

    if( mustStopThread ) trnsfrMan.StartThread();



}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
unsigned Hromat900T::Get_ReadDeviceArchiveFile_TaskCount()
{
    return trnsfrMan_->GetTaskList().CountTasksOfClass<ReadArchiveHTGTask>();
}
//------------------------------------------------------------------------------
Hromat900T::StopThreadInThisFoo::StopThreadInThisFoo() :
    isStarted_( !Hromat900::Instance().GetTransferManager().WasStopedOrTerminated() )
{
	if( isStarted_ ) Hromat900::Instance().GetTransferManager().StopThread();
}
//------------------------------------------------------------------------------
Hromat900T::StopThreadInThisFoo::~StopThreadInThisFoo()
{
	if( isStarted_ )
        Hromat900::Instance().GetTransferManager().StartThread();
}
//------------------------------------------------------------------------------
void Hromat900T::InitializeDeviceArchive()
{
    if( !MyParamStrHas( "-debug" ) &&
        currentHromatCicleType_!=HROMAT_CICLE::OSTANOV )
    {
        const AnsiString sCurrCicleType = HROMAT_CICLE::ToStr(currentHromatCicleType_);
        if( ::MessageBox( Screen->ActiveForm->Handle,
            MYSPRINTF_(
            "Чтение архива хроматограмм рекомендуется производить\n"
            "в режиме остановленных измерений.\n"
            "В настоящий момент хроматограф находится в режиме \"%s\".\n"
            "Вы уверены, что планируете считать архив?", sCurrCicleType).c_str(),
            "Хромат-900", MB_YESNO	 | MB_ICONEXCLAMATION)!=IDYES)
        return;
    }


    trnsfrMan_->StopThread();

    TaskList& taskList = trnsfrMan_->GetTaskList();
    taskList.EraseTasksOfClass<HandleStartArchiveProcess>();
    taskList.EraseTasksOfClass<ReadArchiveHTGTask>();


    trnsfrMan_->ShowTopPanel("Запрос структуры  архива...");
    trnsfrMan_->GetTaskList().EraseTasksOfClass<HandleStartArchiveProcess>();
    trnsfrMan_->AddTask( new ReadHromatConfigTask( &HandleReadedHromatSysConfig,
        hromatSysConfig_ )  );
    trnsfrMan_->AddTask( new HandleStartArchiveProcess( trnsfrMan_.get() ),
        TransferManager::ADD_TASK_HEAD);
    trnsfrMan_->StartThread();
}
//------------------------------------------------------------------------------
void Hromat900T::CancelReadDeviceArchive()
{
    const StopThreadInThisFoo stopThreadInThisFoo;
    TaskList& taskList = trnsfrMan_->GetTaskList();
    taskList.EraseTasksOfClass<HandleStartArchiveProcess>();
    taskList.EraseTasksOfClass<ReadArchiveHTGTask>();
}
void Hromat900T::ReadDeviceArchiveFile( const HtgItemsArray& items )
{
    const unsigned sz = items.size();
    if( sz==0 ) return;
    trnsfrMan_->StopThread();

    trnsfrMan_->ShowTopPanel("Подготовка считывания архива хроматограмм...");
    CancelReadDeviceArchive();
    trnsfrMan_->AddTask( new ReadHromatConfigTask( &HandleReadedHromatSysConfig,
        hromatSysConfig_ )  );
    trnsfrMan_->AddTask( new ReadArchiveHTGTask( items,
        trnsfrMan_.get() ), TransferManager::ADD_TASK_HEAD );
    trnsfrMan_->StartThread();
}
//------------------------------------------------------------------------------
//      Получить менеджер трансферов
TransferManager& Hromat900T::GetTransferManager()
{
    return *trnsfrMan_.get();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/**
*		Данная функция выполняет трансфер приёмопередачи.
*		Объявлена файле "PerformTransfer.h"
*/
void PerformModbusTransfer
(
	uInt8 commandCode,			// код комманды
	ByteBuffPtr sendBegin,		// ссылка на передаваемые данные
    ByteBuffPtr sendEnd,
    ByteBuffPtr& reciveBegin,
    ByteBuffPtr& reciveEnd
)
{
	Hromat900::Instance().GetTransferManager().PerformTransfer(
        commandCode, sendBegin, sendEnd, reciveBegin, reciveEnd );
}

/**
*		Максимальный размер куска запрашиваемых данных
*		Объявлена файле "PerformTransfer.h"
*/
unsigned GetHromatMaxRxDFrameSize()
{
    return Hromat900::Instance().GetTransferManager().GetSets().rxdMaxFrameSize_;
}

/**
*       добавить трансфер установки ключа подсчёта подлинности
*       cnfdntl_.h
*/
void AddToTaskList_SetHromatCRC16Key(unsigned short oldV, unsigned short newV)
{
    Hromat900::Instance().GetTransferManager().AddTask(
        new ChangeHromatCRC16KeyTask( oldV, newV ), TransferManager::ADD_TASK_HEAD );
}

/**
*       Декларация функционала FTP
*		Объявлена файле "PerformTransfer.h"
*/

FTPHlpr* GetFTP()
{
    return Hromat900::Instance().GetTransferManager().GetFTP();
}

bool IsTCPConnection()
{
    return Hromat900::Instance().GetTransferManager().IsTCP();
}

bool FTPEnable()
{
    Hromat900T& h = Hromat900::Instance();
    return h.GetTransferManager().FTPEnable();
}

void ShowTopInfoPanel(const AnsiString& s)
{
    Hromat900::Instance().GetTransferManager().ShowTopPanel(s);
}

void HideTopInfoPanel()
{
    Hromat900::Instance().GetTransferManager().HideTopPanel();
}

//Промежуток времени между двумя точками хроматоргаммы, мсек #include "Hromat900.hpp"
unsigned GetDiskretTimeMSec()
{
    return Hromat900::Instance().GetUserDiskretTimeMSec();
}


AnsiString DefaultHtgArchivePath()
{
    return GetPersonalProfileDir()+"\\Архив_хроматограмм";
}

AnsiString HtgArchivePath()
{
    return Hromat900::Instance().GetHtgArchivePath();    
}


////////////////////////////////////////////////////////////////////////////////


