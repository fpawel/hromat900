//---------------------------------------------------------------------------
#include <windows.h>
#include <vcl.h>
//---------------------------------------------------------------------------

#pragma hdrstop

// loki
//#include "Loki\threads.h"

#include "TransferManage.h"
#include "myThread.h"

#include "TransmitChan.h"
#include "ReportContext.hpp"
#include "TaskList.h"
#include "HromatConsoleInput.h"
#include "MyExcpt.hpp"
#include "uFrmWait.h"
#include "AnsiStringUtils_.h"
#include "AppEvts.h"
#include "xmlUtils_.h"
#include "MyAppProfile.h"
#include "guicon.h"
#include "MySynchronize.h"
#include "cnfdntl_.h"
#include "MyFTP.h"
#include "HromatModbus.h"
#include "MyProcEntry.h"
#include "Hromat900_ini.h"

#pragma package(smart_init)
#pragma option -xp
//------------------------------------------------------------------------------

typedef std::map<AnsiString,AnsiString> TranslateMap;
TranslateMap MakeExceptionTranslateMap()
{
    TranslateMap ret;
    ret["MyTransferCantOpenPortException"] = "сбой инициализации канала связи";
    ret["MyNoAnswerException"] = "хроматограф не отвечает";
    ret["MyTransferAnswerLengthMismatchException"] = "неверная длина посылки";
    ret["MyTransferAnswerAdressMismatchException"] = "несовпадение адрессов";
    ret["MyTransferAnswerCommandCodeMismatchException"] = "несовпадение кодов команды";
    ret["MyTransferBadCRC16Exception"] = "ненулевая контрольная сумма";
    ret["MyTransferNoAnswerException"] = "хроматограф не отвечает";
    ret["DeviceModbusException"] = "исключительная ситуация хроматографа";
    return ret;
}
//------------------------------------------------------------------------------
const AnsiString& TranslateExceptionType(const AnsiString& s)
{
    const static TranslateMap translateMap = MakeExceptionTranslateMap();
    const static AnsiString strNo = "";
    typedef TranslateMap::const_iterator Ptr;
    const Ptr fnd = translateMap.find( s );
    return fnd==translateMap.end() ? strNo : fnd->second;
}
//------------------------------------------------------------------------------
LRESULT HideTopmostPanel1(TFrmWait *topPanel)
{
    topPanel->Hide();
    topPanel->pb1->Visible = 0;
    topPanel->pb1->Position = 0;
    topPanel->pb1->Min = 0;
    topPanel->pb1->Max = 0;
    topPanel->Panel1->Visible = 0;
    return 0;
}
void HideTopmostPanel(TFrmWait *topPanel)
{
    my::Synchronize1( &HideTopmostPanel1, topPanel );
}
//------------------------------------------------------------------------------
LRESULT ShowTopmostPanel1( TFrmWait *topPanel, const AnsiString& s, bool showProgress )
{
    if(topPanel->Visible && topPanel->Panel2->Caption == s) return 0;
    topPanel->Panel2->Caption = s;
    if(!topPanel->Visible)
        topPanel->FixPosition();
    topPanel->Position = poScreenCenter;
    topPanel->Show();
    topPanel->Repaint();

    if( showProgress )
    {
        topPanel->pb1->Visible = 1;
        topPanel->pb1->Position = 0;
        topPanel->pb1->Min = 0;
        topPanel->pb1->Max = 0;
        topPanel->Panel1->Visible = 1;

    }


    return 0;
}
void ShowTopmostPanel( TFrmWait *topPanel, const AnsiString& s, bool showProgress )
{
    my::Synchronize3( &ShowTopmostPanel1, topPanel, s, showProgress );
}
//------------------------------------------------------------------------------
LRESULT SetTopmostPanelProgress1( TFrmWait *topPanel, unsigned v, unsigned vMax, unsigned vMin )
{
    if( topPanel->pb1->Visible )
    {
        if(v!=-1)
        {
            if(v==-2)
                topPanel->pb1->Position = topPanel->pb1->Position + 1;
            else
                topPanel->pb1->Position = v;
        }
        if(vMax!=-1)
            topPanel->pb1->Max = vMax;
        if(vMin!=-1)
            topPanel->pb1->Min = vMin;

        topPanel->Repaint();
    }
    return 0;
}

void SetTopmostPanelProgress( TFrmWait *topPanel, unsigned v, unsigned vMax, unsigned vMin )
{
    my::Synchronize4( &SetTopmostPanelProgress1, topPanel, v, vMax, vMin );
}


class TopmostPanelHelper : public boost::noncopyable
{
public:
    TopmostPanelHelper(TFrmWait *topPanel, const AnsiString& s ) : topPanel_(topPanel)
    {
        ShowTopmostPanel( topPanel, s, false );
    }
    ~TopmostPanelHelper()
    {
        HideTopmostPanel(topPanel_);
    }
private:
    TFrmWait *topPanel_;
};




class TransferManager::Impl : public boost::noncopyable
{
public:
    explicit Impl();
    ~Impl();
    //--------------------------------------------------------------------------
    // управление потоком
    void StartThread();
    void StopThread();
    void KillThread();
    bool WasStopedOrTerminated() const
    {
        if( isConInput_ ) return false;
        return thr_->WasStoped() || thr_->IsTerminated();
    }
    //--------------------------------------------------------------------------
	TReportStateSignal::Connections& ReportStateConnections()
    { return signalReportState_.GetConnections(); }
    //--------------------------------------------------------------------------
    TaskList& GetTaskList() { return taskList_; }
    //--------------------------------------------------------------------------
    void DebugEditList();
    //--------------------------------------------------------------------------
    uInt8 ModusAddr() const{ return intrfc_->GetSets().modbusDeviceAddress_; }
    //--------------------------------------------------------------------------
    // Выполнить трансфер приёмопередачи.
	void PerformTransfer( uInt8 commandCode, ByteBuffPtr sendBegin,
    	ByteBuffPtr sendEnd, ByteBuffPtr& reciveBegin, ByteBuffPtr& reciveEnd )
    {
    	intrfc_->PerformTransfer(commandCode, sendBegin, sendEnd, reciveBegin, reciveEnd);
        ReportStateFromThread( RPT_TRNSFR_CNTXT::THREAD_OK, "Ок" );

    }
    //--------------------------------------------------------------------------
    // упрвление списком - добавить задание
    void AddTask( MyAbstructTransfer *trnsfr, bool addTail = ADD_TASK_TAIL)
    {
        if( addTail==ADD_TASK_TAIL )
            taskList_.PushBackTail( PTransfer(trnsfr) );
        else
            taskList_.PushFrontHead( PTransfer(trnsfr) );
    }
    //--------------------------------------------------------------------------
    const TransmitInterfaceSets& GetSets() const;
    void SetSets( const TransmitInterfaceSets& sets);
    //--------------------------------------------------------------------------
    FTPHlpr* GetFTP() { return &ftp_; }
    //--------------------------------------------------------------------------
    AnsiString ActiveInterfaceDescription() const{ return intrfc_->Description(); }
    //--------------------------------------------------------------------------
    void SetConsoleinputMode(bool isConInput) { isConInput_= isConInput; }
    //--------------------------------------------------------------------------
    void HideTopPanel(){ HideTopmostPanel( topPanel_);    }
    void ShowTopPanel( const AnsiString& s, bool showProgress ){ ShowTopmostPanel( topPanel_, s, showProgress); }
    void SetTopPanelProgress( unsigned v, unsigned vMax, unsigned vMin )
    {
        SetTopmostPanelProgress( topPanel_, v, vMax, vMin);
    }
    //--------------------------------------------------------------------------
    void SetOnNotifyTaskList(TOnNotifyTaskList foo)
    {
        onNotifyTaskList_ = foo;        
    }
    //--------------------------------------------------------------------------
private:
    boost::shared_ptr<void> logLifeTime_;
    //--------------------------------------------------------------------------
    //  поток
    boost::shared_ptr<my::Thread> thr_;
	//--------------------------------------------------------------------------
    // данные интерфейса обмена
    boost::shared_ptr<TransmitInterface> intrfc_;
    //--------------------------------------------------------------------------
    // список планируемых трансферов
    TaskList taskList_;
    //--------------------------------------------------------------------------
    TReportStateSignal signalReportState_;
    //--------------------------------------------------------------------------
    // последнее зафиксированное исключение
    PMyExcpt lastExcpt_;
    //--------------------------------------------------------------------------
    // реализация функционала FTP
    FTPHlpr ftp_;

    const bool isAdmin_;
    bool isConInput_;

    TFrmWait *topPanel_;

    TOnNotifyTaskList onNotifyTaskList_;

    //--------------------------------------------------------------------------
    // открыть канал связи с текущими настройками
    bool OpenConnection();
    // закрыть канал связи с текущими настройками
    void CloseConnection();
    //--------------------------------------------------------------------------

	// потоковая продцедура
	void __fastcall Execute();
    //--------------------------------------------------------------------------
    // отправить отчёт
    LRESULT SignalReportState(unsigned context, const AnsiString& s)
    { signalReportState_.Signal2(context, s); return 0; }
    //--------------------------------------------------------------------------
    // отправить отчёт
    void ReportStateFromThread( unsigned context, const AnsiString& s)
    {
    	my::Synchronize2( &SignalReportState, context, s );
    }
    //--------------------------------------------------------------------------
    void ReportFTPStateFromThread( unsigned context, const AnsiString& s)
    {
        context = context==0 ? RPT_TRNSFR_CNTXT::ERR : RPT_TRNSFR_CNTXT::THREAD_OK;
        ReportStateFromThread( context, s);
    }
    //--------------------------------------------------------------------------
    // захват текущего исключения - можно вызывать из потока
    void CatchCurentException();
    //--------------------------------------------------------------------------
    //подготовить задание из списка - вызывать из потока
    PTransfer PrepareCurentTask();
    //--------------------------------------------------------------------------
    //              реакциИ на событиЯ
    LRESULT HandleBeginTransfer();
    LRESULT HandleEndTransfer();
    LRESULT HandleCurentException();

    void Delay_(unsigned tm)
    {
	    const unsigned tmStart = GetTickCount();
        while( GetTickCount()-tmStart < tm && !WasStopedOrTerminated() ) ;
    }

    LRESULT ShowBadHromatKeyMessage()
    {
        ::MessageBox( Screen->ActiveForm->Handle, "Ошибка! Неверный ключ проверки подлинности!",
                "Ошибка CRC16!", MB_OK | MB_ICONERROR | MB_SYSTEMMODAL  );
        if( !IsAdminAutorized() )
            std::exit(0);
        return 0;
    }
    void __fastcall OnBtnCloseClick( TObject* )
    {
        StopThread();
    }
};
//------------------------------------------------------------------------------
TransferManager::Impl::Impl() :
    logLifeTime_( MakeMyLogQuote("MyTransfer::Manager.[members]") ),
    // оконный объект данного модуля
	thr_( new my::Thread( &Execute ) ),
    signalReportState_(),
    intrfc_( new TransmitInterface(
    	&WasStopedOrTerminated,
    	&ReportStateFromThread
        ) )
        ,ftp_(), isAdmin_( IsAdminAutorized() ),
    isConInput_(0), onNotifyTaskList_(0)

{
    LOG_FOO_LIFETIME_QUOTES;
    thr_->SetPriority( static_cast<TThreadPriority>(
    	TiXML::GetXMLInteger( ProfileXmlElem(), TRANSFERS_THREAD_PRIORITY_ID, tpIdle, tpTimeCritical, tpNormal )
    ));
    ftp_.SetReportFTPStateFunctor( &ReportFTPStateFromThread );

    topPanel_ = new TFrmWait(NULL);
    topPanel_->Panel2->Font->Size = 14;
    topPanel_->btnClose->OnClick = OnBtnCloseClick;
}
//------------------------------------------------------------------------------
TransferManager::Impl::~Impl()
{
	LOG_FOO_LIFETIME_QUOTES;
    try
    {
    	TiXML::SetIntAtr( ProfileXmlElem(), TRANSFERS_THREAD_PRIORITY_ID, thr_->GetPriority() );
        signalReportState_.GetConnections().DisconnectAll();
    }
    catch (...)
    {
        assert(false);
    }

}
//------------------------------------------------------------------------------
// открыть канал связи с текущими настройками
bool TransferManager::Impl::OpenConnection()
{
    if( intrfc_->IsConnected() ) return 1;
    ReportStateFromThread( RPT_TRNSFR_CNTXT::ITFC, "Выполняется соединение..." );

    const TransmitInterfaceSets sets = intrfc_->GetSets();
    if( sets.isTCP_ && sets.useFtp && !ftp_.IsConnected() )
        ftp_.Connect( sets.tcpHost_, "analitpribor2", "analitpribor0561" );

    //ShowTopPanel( MYSPRINTF_("Выполняется установка соединения с %s...", intrfc_->Description()), 0 );
    const bool opened = intrfc_->OpenChanal();
    //HideTopPanel();
    if( !opened ) return false;
    ReportStateFromThread( RPT_TRNSFR_CNTXT::ITFC, "Соединение установлено" );



    return 1;
}
//------------------------------------------------------------------------------
    // закрыть канал связи с текущими настройками
void TransferManager::Impl::CloseConnection()
{
    intrfc_->CloseChanal();
    const TransmitInterfaceSets sets = intrfc_->GetSets();
    if( sets.isTCP_ ) ftp_.Disconnect();
}
//------------------------------------------------------------------------------
// управление потоком
void TransferManager::Impl::StartThread()
{
	LOG_FOO_LIFETIME_QUOTES;
    my::FixMainThread();
    lastExcpt_.reset();
    thr_->Run();
}
//------------------------------------------------------------------------------
void TransferManager::Impl::KillThread()
{
    StopThread();
}
//------------------------------------------------------------------------------
void TransferManager::Impl::StopThread()
{
	if( thr_->IsTerminated() ) return;
    LOG_FOO_LIFETIME_QUOTES;

    ShowTopPanel("Выполняется закрытие соединения...", 0);

    intrfc_->BreakConnection();

    const unsigned tmStartTerminate = ::GetTickCount();
    const TThreadPriority threadPriority = thr_->GetPriority();
    thr_->SetPriority( tpTimeCritical );
    thr_->Stop();
    thr_->SetPriority( threadPriority );
    lastExcpt_.reset();
    //LogMyAppEvt( "Опрос остановлен!", "normal", ::GetTickCount()-tmStartTerminate );
    Delay_( std::min(intrfc_->GetSets().tmWriteDelay_, 500 ) );
    //HideTopPanel();
}

//------------------------------------------------------------------------------
void __fastcall TransferManager::Impl::Execute()
{
    LOG_FOO_LIFETIME_QUOTES;

    ReportStateFromThread( RPT_TRNSFR_CNTXT::START_THREAD, "Опрос: старт" );
    MyWCout( "Запущен поток опроса хроматографа.\n" );

    const bool
		mustISkipVerifyHromatKeyAndSoftId = MyParamStrHas( "-debug" ) &&
            MyParamStrHas( "-skipverify" );
    bool
    	isSoftIdVerified 		= mustISkipVerifyHromatKeyAndSoftId,
    	isHromatKeyIdVerified 	= mustISkipVerifyHromatKeyAndSoftId;

    struct Hlpr
    {
        LRESULT Perform()
        {
            if(foo) foo(*taskList);
            return 0;
        }
        ~Hlpr()
        {
            my::SynchronizeMthd( &Perform );
        }
        TaskList* taskList;
        TOnNotifyTaskList foo;
    };

	for
    (
    	PTransfer curentTask = PrepareCurentTask();
        !thr_->IsTerminated();
        curentTask = PrepareCurentTask() )
    {
        Hlpr hlpr;
        hlpr.taskList = &taskList_;
        hlpr.foo = onNotifyTaskList_;
    	try
    	{
    	    if(curentTask.get()==NULL) continue;
            if( !OpenConnection() )
                break;

            if( !isSoftIdVerified )
            {   
            	SignalReportState(RPT_TRNSFR_CNTXT::BEG, "Проверка идентификатора ПО" );
            	if( !VerifySoftId() ) continue;
                isSoftIdVerified = true;
            }


            if( !isHromatKeyIdVerified )
            {
                SignalReportState(RPT_TRNSFR_CNTXT::BEG, "Верификация ключа проверки подлинности" );
                const unsigned short hromatKey = GetHromatCRC16Key();
                if( !WriteHromatCRC16Key( hromatKey, hromatKey ) )
                {
                    my::SynchronizeMthd( &ShowBadHromatKeyMessage );
                    break;
                }
                isHromatKeyIdVerified = true;
                LogMyAppEvt( "Ключ проверки подлинности верифицирован."  );
            }


            //обработка события начала трансфера
    		if( my::SynchronizeMthd( &HandleBeginTransfer )==false )
            {
            	taskList_.PopFrontBody();
                continue;
            }

            // выполнить трансфер
            //intrfc_->OpenChanal();
            curentTask->Perform();


            if( thr_->IsTerminated() ) break;

            //обработка события окончания трансфера
            if( my::SynchronizeMthd(  &HandleEndTransfer )==true )
                taskList_.PushBackBody( curentTask ) ;
            taskList_.PopFrontBody();

        }// try
   	 	catch(...)
    	{
        	CatchCurentException();
            MYWCOUT_COLORED_( "<\\%s> !!! Прервано c ошибкой !!!\n", curentTask->What() ),
                MY_SET_CONSOLE_RED_TEXT );
            if(!WasStopedOrTerminated() )
                Delay_(1000);
    	}
    }// for( PTaskData curentTask = taskList_.Front();

    // закрыть текущее соединение
    CloseConnection();
    ReportStateFromThread( RPT_TRNSFR_CNTXT::STOP_THREAD, "Опрос остановлен" );
    MyWCout( "Выход из потоковой продцедуры.\n" );
    HideTopPanel();
}
//---------------------------------------------------------------------------
void TransferManager::Impl::CatchCurentException()
{
    //my::SynchronizeMthd( &HideTopmostPanel );
    const PMyExcpt excpt = RethrowMyException(__FILE_LINE__);
    // закрыть канал, если ошибка приёмопередачи
    if( !excpt->TypeOf<MyTransferNoAnswerException>() )
        CloseConnection();

    MYWCOUT_COLORED_( "ОШИБКА! %s\n", excpt->Root()->Message() ), MY_SET_CONSOLE_RED_TEXT );
    SignalReportState(RPT_TRNSFR_CNTXT::ERR, excpt->Message() );  

    if( lastExcpt_.get()!=NULL &&
        lastExcpt_->Root()->Location()==excpt->Root()->Location() ) return;
    lastExcpt_ = excpt;
    my::SynchronizeMthd( &HandleCurentException );
}
//------------------------------------------------------------------------------
// реакция на событие "начало трансфера"
LRESULT TransferManager::Impl::HandleBeginTransfer()
{
	if( thr_->IsTerminated() ) return 0;
	const PTransfer curentTask = taskList_.FrontBody();
    assert(curentTask);
    
    const LRESULT ret = static_cast<LRESULT>( curentTask->HandleBeginTransfer() );
    if(ret!=0)
    {
    	SignalReportState(RPT_TRNSFR_CNTXT::BEG, curentTask->What() );
        MyWCout( MYSPRINTF_("<%s>\n" , curentTask->What() ) );
    }

	return ret;
}
//---------------------------------------------------------------------------
// реакция на событие "окончание трансфера"
LRESULT TransferManager::Impl::HandleEndTransfer()
{
	if( thr_->IsTerminated() ) return 0;
	const PTransfer curentTask = taskList_.FrontBody();
    assert(curentTask);
    SignalReportState( RPT_TRNSFR_CNTXT::END, curentTask->What() );
    MyWCout( MYSPRINTF_("<\\%s>\n", curentTask->What() ) );
    lastExcpt_.reset();
    HideTopPanel();
    return static_cast<LRESULT>( curentTask->HandleEndTransfer() );
}
//---------------------------------------------------------------------------
LRESULT TransferManager::Impl::HandleCurentException()
{
	if( thr_->IsTerminated() || !lastExcpt_.get() ) return 0;
    AnsiString msg = "Ошибка: ";
    if( lastExcpt_->TypeOf<MyTransferException>()  )
        msg = msg + "(" + IntToStr( MyTransferException::Count() ) + ") ";
    msg = msg + TranslateExceptionType( lastExcpt_->ClassName() );

    if( MyParamStrHas("-debug") )
        LogMyAppEvt( msg, lastExcpt_->Message(),
        FormatMyException( lastExcpt_.get(), "%eLine:%eFile %eMsg; " ) );
    else
        MYWCOUT_("%s\n", FormatMyException( lastExcpt_.get(), "%eLine:%eFile %eMsg; " ) ));


    if( lastExcpt_->TypeOf<MyAccessViolationException>() )
    {
        ::MessageBox(Screen->ActiveForm->Handle,
            "Критический сбой модуля hromat900.exe!\n\n"
            "Нарушение границ области памяти модуля!\n\n"
            "Приложение будет закрыто!",
    		"Критический сбой ПО!", MB_OK | MB_ICONERROR | MB_SYSTEMMODAL );
        Application->Terminate();

    }

    return 0;
}
//---------------------------------------------------------------------------
//подготовить задание из списка - вызывать из потока
PTransfer TransferManager::Impl::PrepareCurentTask()
{
	assert(thr_);
    if( thr_->IsTerminated() ) return PTransfer();
    my::SynchronizeMthd( &taskList_.Merge );
    PTransfer ret  = taskList_.FrontBody();
    return ret;
}
//------------------------------------------------------------------------------
void TransferManager::Impl::DebugEditList()
{
    LOG_FOO_LIFETIME_QUOTES;
    const bool isThreadSatrted = !thr_->IsTerminated();
    if(isThreadSatrted) KillThread();
    AddTask( CreateSingleBackgroundTask(&taskList_.DebugEdit_GUI, "Debug edit list" ), ADD_TASK_HEAD);
    if(isThreadSatrted) StartThread();
}
//------------------------------------------------------------------------------
const TransmitInterfaceSets& TransferManager::Impl::GetSets() const
{
    return intrfc_->GetSets();
}
//------------------------------------------------------------------------------
void TransferManager::Impl::SetSets( const TransmitInterfaceSets& sets)
{
    intrfc_->SetSets(sets);
}
//------------------------------------------------------------------------------


/////////////////////////////////////////////////////////////////////////////////
//					class TransferManager
/////////////////////////////////////////////////////////////////////////////////
TransferManager::TransferManager() : impl_( new Impl )
{
}
//------------------------------------------------------------------------------
void TransferManager::ShowTopPanel(const AnsiString& s, bool showProgress)
{
    impl_->ShowTopPanel(s, showProgress);
}
//------------------------------------------------------------------------------
void TransferManager::HideTopPanel()
{
    impl_->HideTopPanel();
}
void TransferManager::SetTopPanelProgress( unsigned v, unsigned vMax, unsigned vMin )
{
    impl_->SetTopPanelProgress( v, vMax, vMin );
}

//------------------------------------------------------------------------------
boost::shared_ptr<void> TransferManager::ConnectOnReportState(TVoid_IntStrMthd method)
{
	return impl_->ReportStateConnections().AutoConnect(method);
}
//------------------------------------------------------------------------------
// упрвление списком - добавить задание
void TransferManager::AddTask( MyAbstructTransfer *trnsfr, bool addTail)
//bool addTail = ADD_TASK_TAIL
{
    impl_->AddTask(trnsfr, addTail);
}
//---------------------------------------------------------------------------
void TransferManager::AddSingleBackgroundTask( void (__closure *aMethod )(),
	const AnsiString& what, bool addTail )
//bool addTail = ADD_TASK_TAIL
{
    impl_->AddTask( CreateSingleBackgroundTask(aMethod, what ) );
}
//---------------------------------------------------------------------------
// управление потоком
void TransferManager::StartThread() { impl_->StartThread(); }
//---------------------------------------------------------------------------
void TransferManager::StopThread() {  impl_->StopThread();  }
//---------------------------------------------------------------------------
bool TransferManager::WasStopedOrTerminated() const
{
    return impl_->WasStopedOrTerminated(); 
}
void TransferManager::DebugEditList()
{
	impl_->DebugEditList();
}
//---------------------------------------------------------------------------
uInt8 TransferManager::ModusAddr() const
{
	return impl_->ModusAddr();
}
//---------------------------------------------------------------------------
// Выполнить трансфер приёмопередачи.
void TransferManager::PerformTransfer
(
	uInt8 commandCode,			// код комманды
	ByteBuffPtr sendBegin,		// ссылка на передаваемые данные
    ByteBuffPtr sendEnd,
    ByteBuffPtr& reciveBegin,
    ByteBuffPtr& reciveEnd
)
{
	impl_->PerformTransfer(commandCode, sendBegin, sendEnd, reciveBegin, reciveEnd);
}

void TransferManager::SetConsoleinputMode(bool isConInput)
{
    impl_->SetConsoleinputMode(isConInput);
}


/**
*       список трансферов
*/
TaskList& TransferManager::GetTaskList()
{
    return impl_->GetTaskList();
}

const TransmitInterfaceSets& TransferManager::GetSets() const
{
    return impl_->GetSets();
}

void TransferManager::SetSets( const TransmitInterfaceSets& sets)
{
    impl_->SetSets(sets);
}

void TransferManager::KillThread()
{
    return impl_->KillThread();
}

FTPHlpr* TransferManager::GetFTP()
{
    return impl_->GetFTP();
}

bool TransferManager::IsTCP() const
{
    return GetSets().isTCP_;
}

bool TransferManager::FTPEnable() const
{
    return GetSets().isTCP_ && GetSets().useFtp;
}

AnsiString TransferManager::ActiveInterfaceDescription() const
{
        return impl_->ActiveInterfaceDescription();
}

void TransferManager::SetOnNotifyTaskList(TOnNotifyTaskList foo)
{
    impl_->SetOnNotifyTaskList(foo);
}


