//---------------------------------------------------------------------------
#include <map>
#include <iterator>
#include <fstream>
//------------------------------------------------------------------------------
#pragma hdrstop

//vcl
#include <DateUtils.hpp>

// boost
#include "boost\dynamic_bitset.hpp"

#include "HromatTask.h"
#include "PerformTransfer.h"
#include "bcd_.h"
#include "HromatModbus.h"
#include "..\uFormHromatogramm.h"
#include "MySynchronize.h"
#include "MyExcpt.hpp"
#include "my_array_size.h"
#include "htg_.h"
#include "AnsiStringUtils_.h"
#include "TransferManage.h"
#include "AppEvts.h"
//#include "HromatCfgSysHlpr.h"
#include "MyModbusException.h"
#include "MyModbusDefine.hpp"
#include "cnfdntl_.h"
#include "HromatCrypt.h"
#include "..\MY_INCLUDE\myconv.h"


#include "uFrmWait.h"
#include "Unit1.h"
#include "MyFileCtrl.h"
#include "CurHtg.h"
#include "guicon.h"
#include "uFormArchiveView.h"
#include "HromatHtmlReport.h"
#include "uFormReportAnalizeArchive.h"
#include "hromat_sys.h"
//------------------------------------------------------------------------------
#pragma package(smart_init)
//------------------------------------------------------------------------------

void ShowCriticalErrorMessage(const AnsiString& msg)
{
    static const struct
    {
        LRESULT Foo(const AnsiString& msg) const
        {
            MyWCout("Критический сбой! Выход из приложения! "+msg+"\n");
            ::MessageBox(Screen->ActiveForm->Handle, msg.c_str(), "Критическая ошибка!",
                MB_OK | MB_APPLMODAL | MB_ICONERROR );
            Application->Terminate();
            return 0;
        }
    } h;
    my::Synchronize1( &h.Foo, msg );
}


//---------------------------------------------------------------------------
SetHromatDatetimeTask::SetHromatDatetimeTask() :
    HandleHromatTask( ONCE_TASK, "Синхронизация даты и времени" )
{}
//------------------------------------------------------------------------------
void SetHromatDatetimeTask::Perform()
{
    // Установить в хроматограф текущие дату и время
    HromatDateTimeAsBuffer dtAsBuff;
    Word year, month, day, hour, minute, second, milliSecond;
    DecodeDateTime( Now(), year, month, day, hour, minute, second, milliSecond);

    dtAsBuff.date.day = day;
    dtAsBuff.date.month = month;
    dtAsBuff.date.year = year;
    WriteModbus4Bytes( HROMAT_INTERNAL_DATE_WRT, dtAsBuff.dt );

    dtAsBuff.time.hour = hour;
    dtAsBuff.time.minute = minute;
    dtAsBuff.time.second = second;
    dtAsBuff.time.hsec = 0;
    WriteModbus4Bytes( HROMAT_INTERNAL_TIME_WRT, dtAsBuff.dt );
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------

HromatCicleControl::HromatCicleControl(unsigned cicle) :
    HandleHromatTask( ONCE_TASK, "установить тип цикла "+HROMAT_CICLE::ToStr( cicle ) ),
    cicle_(cicle)
{}
void HromatCicleControl::Perform()
{
    const VInt8 dt = FloatToBCD(cicle_);
    WriteModbus4Bytes( HROMAT_CICLE_TYPE_WRT, &dt.at(0) );
}
//------------------------------------------------------------------------------
HandleReadModbusRegistersData::HandleReadModbusRegistersData( unsigned short regAddr,
	uInt8 regCount, bool sendOnce, const AnsiString& what ):
    HandleHromatTask(sendOnce,what), regAddr_(regAddr), regCount_(regCount)
{
}
//------------------------------------------------------------------------------
void HandleReadModbusRegistersData::Perform()
{
	ByteBuffPtr rxd, rxdEnd;
	ReadModbusRegistersData( regAddr_, regCount_, rxd, rxdEnd );
    DoOnGetData(rxd, rxdEnd);
}
//---------------------------------------------------------------------------
HandleReadModbusFloatValues::HandleReadModbusFloatValues( unsigned short regAddr,
	uInt8 count, bool sendOnce, const AnsiString& what,
    TVoid_PDoublePDoubleMthd mthd ) :
    	HandleReadModbusRegistersData( regAddr, count*2, sendOnce, what ),
    	result_(count), mthd_(mthd)
{}
//---------------------------------------------------------------------------
void HandleReadModbusFloatValues::DoOnGetData(ByteBuffPtr& rxd, ByteBuffPtr& rxdEnd)
{
	TransformBCDDataToFloat( rxd, rxdEnd, &result_[0] );
}
//---------------------------------------------------------------------------
void HandleReadModbusFloatValues::DoOnEndTransfer() // finale - не трогать
{
    if(mthd_!=NULL)
	    mthd_( result_.begin(), result_.end() );
}
//---------------------------------------------------------------------------
HandleReadDeviceState::HandleReadDeviceState(AfeterReadFooT foo) :
    HandleHromatTask(CONTINUESLY_TASK, "Запрос статуса хроматографа" ),
    foo_(foo)
{
}
//------------------------------------------------------------------------------
void HandleReadDeviceState::Perform()                 VIRTUAL_FINAL_
{
    cicleType_ = ReadModbusUnsigned( HROMAT_CICLE_TYPE_REG );
    ByteBuffPtr rxd = 0, rxdEnd = 0;
    ReadModbusRegistersData(  35, 1, rxd, rxdEnd );
    deviceStatecode_ = rxdEnd-rxd==2 ?  (rxd[1] << 8) + rxd[0] : 0;
}
//------------------------------------------------------------------------------
void HandleReadDeviceState::DoOnEndTransfer()         VIRTUAL_FINAL_
{
    foo_(deviceStatecode_, cicleType_);
}
//------------------------------------------------------------------------------

/**
*		Запрос инфы о текущей хр-ме
*/
ProcessCurrenHromatogramm::ProcessCurrenHromatogramm(CurrHtgHelperT& currHtgHelper) :
    HandleHromatTask( CONTINUESLY_TASK, "Обработка текущей хроматограммы" ),
    currHtgHelper_(currHtgHelper)
{
}
//------------------------------------------------------------------------------
void ProcessCurrenHromatogramm::Perform()
{
    currHtgHelper_.Process();
}
//------------------------------------------------------------------------------
void ProcessCurrenHromatogramm::DoOnEndTransfer()
{
    currHtgHelper_.DoOnEndTransfer();
}
//------------------------------------------------------------------------------



/**
*		Старт обработки архивов
*/
HandleStartArchiveProcess::HandleStartArchiveProcess( TransferManager* trmn ) :
    HandleHromatTask( ONCE_TASK, "Инициализация архива хроматограм" ),
    trmn_( trmn )
{
    

}
HandleStartArchiveProcess::~HandleStartArchiveProcess()
{
    
}

//------------------------------------------------------------------------------
void HandleStartArchiveProcess::Perform() 			VIRTUAL_FINAL_
{   
    items_.clear();
    const unsigned
    archiveFilesCount = ReadModbusUnsigned( HROMAT_SAVED_FILES_COUNT_REG );
    if( archiveFilesCount==0 ) return;

    ModbusFileInfoList d1Lst(archiveFilesCount), d2Lst(archiveFilesCount);

    // читать инфу о файлах от 0 до  archiveFilesCount
    if( trmn_->WasStopedOrTerminated() ) return;
    LogMyAppEvt("Запрос заголовка архивов", MYSPRINTF_("Размер: %d файлов", archiveFilesCount) );
    
    isOk_ = archiveFilesCount==ReadHromatFilesInfo
    (
	    0,                                      // индекс первого файла
        archiveFilesCount,                      // колличество файлов
        HromatFileSpace::HTG_NAME,              // номер файловой области от 0
        0,                                      // номер детектора
        &d1Lst.at(0)                    // выходной буфер
    );
    if(!isOk_) return;
    if( trmn_->WasStopedOrTerminated() ) return;

    isOk_ = archiveFilesCount==ReadHromatFilesInfo
    (
	    0,                                      // индекс первого файла
        archiveFilesCount,                      // колличество файлов
        HromatFileSpace::HTG_NAME,        // номер файловой области от 0
        1,                                      // номер детектора
        &d2Lst.at(0)                    // выходной буфер
    );
    if(!isOk_) return;
    MyWCout( "   ---   D0:   ---   \n");
    LogMyAppEvt("Получен заголовок архивов", MYSPRINTF_("Размер: %d файлов", archiveFilesCount) );


    for( unsigned i=0; i<archiveFilesCount; ++i )
    {
        if( d1Lst[i].index_ == 0xFFFF ||
            d1Lst[i].date_.Val==0
        ) continue;
        HtgItem itm;
        itm.dateTime = d1Lst[i].date_;
        itm.file = d1Lst[i].index_;
        itm.rgm = ExtractRGMChanal( d1Lst[i].type_);
        itm.type =  ExtractModbusFileType( d1Lst[i].type_);
        itm.sign = d1Lst[i].sign_;
        itm.size[0] = d1Lst[i].size_;
        itm.size[1] = d2Lst[i].size_;
        items_.push_back(itm);
    }
    std::sort( items_.begin(), items_.end() );

    //my::SynchronizeCout( MyBuffToStr(  )
}
//------------------------------------------------------------------------------
void HandleStartArchiveProcess::DoOnEndTransfer()		VIRTUAL_FINAL_
{

    if( trmn_->WasStopedOrTerminated() || !isOk_) return;
    if( Form1==NULL ) return;

    for( unsigned i=0; i<items_.size(); ++i )
    {
        HtgItem &itm = items_[i];
        HromatogramId htgId;
        htgId.date_ = itm.dateTime;
        htgId.fileIndex_ = itm.file;
        htgId.rgmChanAndType_ = (itm.rgm<<4)+itm.type;

        htgId.dtctr_ = 0;
        itm.fileName[0] = GetHromatogrammFileName(htgId);
        htgId.dtctr_ = 1;
        itm.fileName[1] = GetHromatogrammFileName(htgId);
    }

    const AnsiString ss = Screen->ActiveForm->Caption;
    TFormArchiveView* dlg = dynamic_cast<TFormArchiveView*>(Screen->ActiveForm);
    if( dlg )
        dlg->SetDeviceArchive(items_);
}
//------------------------------------------------------------------------------
/**
*		Трансфер для считывания хроматограммы из архива
*/
ReadArchiveHTGTask::ReadArchiveHTGTask(const HtgItemsArray& items,
    TransferManager* trmn) :
    HandleHromatTask( ONCE_TASK, "Чтение архива хроматограмм" ),
    items_(items), trmn_(trmn)
{
    htgs_[0].resize( items_.size() );
    htgs_[1].resize( items_.size() );

    for( unsigned i=0; i<items_.size(); ++i )
    {
        const HtgItem &itm = items[i];

        // формировать индекс хроматограммы из инфы о файле
        HromatogramId id;
        id.date_ = itm.dateTime;
        id.fileIndex_ = itm.file;
        id.rgmChanAndType_ = (itm.rgm<<4)+itm.type;

        // создать представление хроматограммы и внести его в окно хр-м
        HromatogrammPtr htg;

        id.dtctr_ = 0;
        htgs_[0][i].reset(  new Hromatogramm(id) );
        id.dtctr_ = 1 ;
        htgs_[1][i].reset(  new Hromatogramm(id) );

    }
}
//------------------------------------------------------------------------------
bool ReadArchiveHTGTask::ReadHTG(unsigned i, bool detector)
{
    HromatogrammPtr htg = htgs_[detector][i];
    const HromatogramId id = htg->GetId();
    const unsigned htgItemFile = items_[i].file;
    unsigned
        htgItemSize = items_[i].size[detector],
        htgItemSign = items_[i].sign;

    if( HromatFileSign::IsInvalide(htgItemSign) || htgItemSize==0 )
    {
        ModbusFileInfo fi;
        const bool isOk_ = ReadHromatFilesInfo
        (
	        htgItemFile,                    // индекс первого файла
            1,                              // колличество файлов
            HromatFileSpace::HTG_NAME,      // номер файловой области от 0
            detector,                       // номер детектора
            &fi                             // выходной буфер
        );
        if(!isOk_) return false;
        htgItemSize = fi.size_;
        htgItemSign = fi.sign_;
        if(htgItemSize<2) return false;
    }

    VInt8 htgFile( htgItemSize-2 );
    // читать хр-му
    const bool ret = ReadHromatFile
    (
        htgItemFile,			                // индекс файла
        htgItemSize-2,                  // колличество байт
        0,                              // смещение от начала файла
        HromatFileSpace::HTG_DATA,      // номер детектора
        detector,                       // номер файловой области от 0
        htgFile.begin(),                // выходной буффер
        NULL,
        "архив хроматограмм"
    );
    if(ret)
        htg->SetDataAndSave( htgItemSign, htgFile.begin(), htgFile.end() );
    return ret;
}
//------------------------------------------------------------------------------
void ReadArchiveHTGTask::Perform()					VIRTUAL_FINAL_
{
    if( items_.empty() ) return;
    readed_.clear();
    bad_.clear();

    trmn_->ShowTopPanel( FormatHtgItem(items_[0]), true );
    trmn_->SetTopPanelProgress( 0, items_.size()*2, 0 );
    for( unsigned i=0; i<items_.size(); ++i )
    {
        if( trmn_->WasStopedOrTerminated() ) return;
        trmn_->ShowTopPanel( FormatHtgItem(items_[i]) );

        bool isDet2 = Hromat900::Instance().DetectorsCount()>1;

        HtgItemsArray &itms = ReadHTG(i, 0) && (!isDet2 || ReadHTG(i, 1)) ? readed_ : bad_;


        itms.push_back(items_[i]);
        trmn_->SetTopPanelProgress( -2 );
        trmn_->SetTopPanelProgress( -2 );
        OnAfterEndReadArchiveFile( itms[i].file, itms[i].dateTime );
    }
}
//------------------------------------------------------------------------------
void ReadArchiveHTGTask::DoOnEndTransfer()
{
    TFormArchiveView* dlg = dynamic_cast<TFormArchiveView*>(Screen->ActiveForm);
    if( dlg )
        dlg->HandleEndReadArchive();
}
//------------------------------------------------------------------------------
void LogBasePicTable(const HromatBasePicTable& dt)
{
    for( unsigned i=0; i<dt.size(); ++i )
    {
        LogMyAppEvt( dt[i].name_,
        "С="+FormatFloat("######0.0#######", MyFloatToDouble(dt[i].conc_) ),
        "tу="+FormatFloat("######0.0#######", MyFloatToDouble(dt[i].t_) )
         );
    }
}



/**
*		Прочитать и отобразить базовую таблицу пиков
*/
ReadBasePicTableTask::ReadBasePicTableTask(bool dtctr) :
    HandleHromatTask( ONCE_TASK, AnsiString().sprintf("Детектор %d: запрос опорной таблицы пиков.",dtctr+1) ),
    dtctr_(dtctr)
{
    if( FrmHTG[dtctr_] ) FrmHTG[dtctr_]->Button2->Enabled = 0;
}
ReadBasePicTableTask::~ReadBasePicTableTask()
{
    if( FrmHTG[dtctr_] ) FrmHTG[dtctr_]->Button2->Enabled = 1;
}

//------------------------------------------------------------------------------
void ReadBasePicTableTask::DoOnBeginTransfer()      VIRTUAL_FINAL_
{
}
//------------------------------------------------------------------------------
void ReadBasePicTableTask::DoOnEndTransfer()         VIRTUAL_FINAL_
{
    if (dtctr_==true && Hromat900::Instance().DetectorsCount()!=2)
        return;
    if( FrmHTG[dtctr_] )
        FrmHTG[dtctr_]->SetBasePikTbl(dt_);
}
//------------------------------------------------------------------------------
void ReadBasePicTableTask::Perform()				 VIRTUAL_FINAL_
{
    if (dtctr_==true && Hromat900::Instance().DetectorsCount()!=2)
        return;

    ShowTopInfoPanel( What() );

    // выбрать активный детектор
    WriteModbusFloat(  0x02, dtctr_ );
    const unsigned idx = 0;

    ModbusFileInfo fileInfo;
    if( !ReadHromatFileInfo(
	    idx,                                        // индекс первого файла
        HromatFileSpace::BASE_PIK_NAME,             // номер файловой области от 0
        dtctr_,                                     // номер детектора
        &fileInfo		                            // выходной буфер
        ) )
    {
        LogMyAppEvt("Ошибка: отсутствует опорная таблица пиков!", "Детектор "+IntToStr(dtctr_+1) );
        return;
    }


    if( (  (fileInfo.size_<HROMAT_BASE_PIC_STRUCT_SIZE + 2) || ( (fileInfo.size_-2) % HROMAT_BASE_PIC_STRUCT_SIZE)!=0) )
    {
        LogMyAppEvt("Ошибка: неверный размер опорной таблицы.",
            MYSPRINTF_("Детектор %d: %d байт не кратно размеру строки %d байт",
                dtctr_+1, fileInfo.size_, HROMAT_BASE_PIC_STRUCT_SIZE));
        return;
    }

    fileInfo.size_-= 2;
    dt_.resize( fileInfo.size_ / HROMAT_BASE_PIC_STRUCT_SIZE  );
    ReadHromatFile(
	        idx,			                                // индекс файла
            fileInfo.size_,                                 // колличество байт
            0,                                              // смещение от начала файла
            HromatFileSpace::BASE_PIK_DATA,           // номер файловой области от 0
            dtctr_,                                         // номер детектора
            reinterpret_cast<uInt8*>( dt_.begin() ),         // выходной буффер
            NULL,
            " "
        );
    NormalizePicNameInTable2( dt_ );
    LogBasePicTable(dt_);

}
//------------------------------------------------------------------------------
/**
*		Записать базовую таблицу пиков
*/
WriteBasePicTableTask::WriteBasePicTableTask(bool dtctr, const HromatBasePicTable& dt) :
    HandleHromatTask( ONCE_TASK, AnsiString().sprintf("Детектор %d: установка опорной таблицы пиков.",dtctr+1) ),
    dtctr_(dtctr), dt_(dt)
{
    NormalizePicNameInTable2(dt_);
    if( FrmHTG[dtctr_] ) FrmHTG[dtctr_]->btnSaveBasePicTable->Enabled = 0;

}
//------------------------------------------------------------------------------
WriteBasePicTableTask::~WriteBasePicTableTask()
{
    if( FrmHTG[dtctr_] ) FrmHTG[dtctr_]->btnSaveBasePicTable->Enabled = 1;
}
//------------------------------------------------------------------------------
void WriteBasePicTableTask::Perform()				 VIRTUAL_FINAL_
{
    ShowTopInfoPanel( What() );
    
    // выбрать активный детектор
    WriteModbusFloat(  0x02, dtctr_ );
    const int idx = 0; // индекс файла

    // записать данные
    const uInt8
        *pBeg = reinterpret_cast<ByteBuffPtr>( dt_.begin() ),
        *pEnd = reinterpret_cast<ByteBuffPtr>( dt_.end() );


    VInt8 aFile( pBeg, pEnd );

    unsigned short int crc16 = CalculateHromatCRC16( pBeg, pEnd-pBeg, 0xFFFF );

    aFile.push_back( crc16 >>8 );
    aFile.push_back( crc16 );

    // записать данные
    ByteBuffPtr
        pFile = &*aFile.begin(),
        pFileEnd = &*aFile.end();

    WriteModbusFile( idx,
        pFile,
        pFileEnd,                               // данные
        0,                                      // смещение от начала файла
        HromatFileSpace::BASE_PIK_DATA,   // номер файловой области от 0
        dtctr_                                  // номер детектора
    );
    // связать детектор с опорной таблицей пиков
    WriteModbusFloat(  0x1F, idx );
    LogBasePicTable(dt_);
}
//------------------------------------------------------------------------------
void WriteBasePicTableTask::DoOnEndTransfer()         VIRTUAL_FINAL_
{

}
//------------------------------------------------------------------------------
/**
*		Прочитать параметры конфигурации
*/
ReadHromatConfigTask::ReadHromatConfigTask(OnReaded onReaded, const VDouble& hromatSysConfig) :
    HandleHromatTask( ONCE_TASK, "Запрос параметров конфигурации" ),
    onReaded_(onReaded),  hromatSysConfig_(hromatSysConfig)
{   
}
//------------------------------------------------------------------------------
void ReadHromatConfigTask::DoOnEndTransfer()         VIRTUAL_FINAL_
{
    onReaded_(hromatSysConfig_);
}
//------------------------------------------------------------------------------
void ReadHromatConfigTask::Perform()				 VIRTUAL_FINAL_
{
    ShowTopInfoPanel( What() );
    const unsigned fileSize = (hromatSysConfig_.size()-1)*sizeof(double)+2;
    char *pBuff = reinterpret_cast<char*>( &hromatSysConfig_.at(0) );
    const unsigned readedBytesCount = ReadHromatFile
    (
		0,			                        	// индекс файла
        fileSize,					            // колличество байт
        0,                                      // смещение от начала файла
        HromatFileSpace::CONFIG_VALUES, 		// номер файловой области от 0
        0,                                      // номер детектора
        pBuff,         							// выходной буффер
        NULL,
        " "
    );
    /*
    if( readedBytesCount!=fileSize )
        ShowCriticalErrorMessage( MYSPRINTF_(
            "Размер файла параметров %d "
            "конфигурации не соответсвует vars.ini %d",
            readedBytesCount, fileSize
        ));
    */
}
//------------------------------------------------------------------------------


/**
*		Записать параметры конфигурации
*/
WriteHromatConfigTask::WriteHromatConfigTask( const VDouble& hromatSysConfig ) :
    HandleHromatTask( ONCE_TASK, "Установка параметров конфигурации" ),
    hromatSysConfig_(hromatSysConfig), writen_(false)
{
    if(Form1) Form1->menuHromatParams->Enabled = 0;
}
WriteHromatConfigTask::~WriteHromatConfigTask()
{
    if(Form1) Form1->menuHromatParams->Enabled = 1;
}
//------------------------------------------------------------------------------
void WriteHromatConfigTask::Perform()
{
    ShowTopInfoPanel( What() );
    char *pBuff = const_cast<char*>( reinterpret_cast<const char*>( &hromatSysConfig_.at(0) ) );
    const unsigned fileSize = (hromatSysConfig_.size()-1)*sizeof(double)+2;

    unsigned short int crc16 = CalculateHromatCRC16( pBuff,
        fileSize-2, 0xFFFF );

    pBuff[fileSize - 2] = crc16 >>8;
    pBuff[fileSize - 1] = crc16;

    WriteModbusFile( 0,
        pBuff,
        pBuff+fileSize,                         // данные
        0,                                      // смещение от начала файла
        HromatFileSpace::CONFIG_VALUES,        // номер файловой области от 0
        0                                       // номер детектора
    );
    writen_ = true;
}

void WriteHromatConfigTask::DoOnEndTransfer()         VIRTUAL_FINAL_
{
    //if(writen_) LogHromatSysConfig(hromatSysConfig_);

}
//------------------------------------------------------------------------------
ChangeHromatCRC16KeyTask::ChangeHromatCRC16KeyTask(unsigned short oldV, unsigned short newV ) :
    HandleHromatTask( ONCE_TASK, "Установка ключа проверки подлинности" ),
    old_(oldV), new_(newV), badKeyDetected_(false)
{
    if(Form1) Form1->menuSetHromatKey->Enabled = 0;
    
}
ChangeHromatCRC16KeyTask::~ChangeHromatCRC16KeyTask()
{
    if(Form1) Form1->menuSetHromatKey->Enabled = 1;
}
//------------------------------------------------------------------------------
void ChangeHromatCRC16KeyTask::Perform()
{
    ShowTopInfoPanel( What() );
    badKeyDetected_ = !WriteHromatCRC16Key(new_, old_);
}

void ChangeHromatCRC16KeyTask::DoOnEndTransfer()
{
    if( !badKeyDetected_ )
    {
        LogMyAppEvt( "Ключ проверки подлинности установлен." );
        return;
    }
    LogMyAppEvt( "Ошибка изменеия CRC16KEY", "CRC16KEY не совпадает!" );

    ::MessageBox( Application->Handle,
        "Введено неверное значение ключа проверки подлинности.\n"
        "Повторите ввод ключа.", "Ключ проверки подлинности",
            MB_OK | MB_ICONERROR | MB_SYSTEMMODAL );
    SetupHromatCRC16Key();

}

//------------------------------------------------------------------------------
/**
*		Прочитать архивы измерений
*/
ReadMeasurmentsJournalsTask::ReadMeasurmentsJournalsTask() :
    HandleHromatTask( ONCE_TASK,  "Запрос журналов измерений" ),
    hasGotJurnalAnalize_(0),
    hasGotJurnalAnalizeArchive_(0)
{
    if(Form1) Form1->menuGetHromatJournals->Enabled = 0;

    // Файл "Анализ"
    Jurnal_Analize t1 = {0};
    jurnalAnalize_ = t1;

    // Файл "Архив анализов"(12k)
    Jurnal_AnalizeArchive  t2 = {0};
     jurnalAnalizeArchive_ = t2;

    // Файл "Архив градуировок"
    //Jurnal_GraduateArchive t3 = {0};
    //jurnalGraduateArchive_ = t3;
}
//------------------------------------------------------------------------------
ReadMeasurmentsJournalsTask::~ReadMeasurmentsJournalsTask()
{
    if(Form1) Form1->menuGetHromatJournals->Enabled = 1;
}
//------------------------------------------------------------------------------
void ReadMeasurmentsJournalsTask::DoOnEndTransfer()         VIRTUAL_FINAL_
{
    if( !hasGotJurnalAnalize_ && !hasGotJurnalAnalizeArchive_)
        return;

    static struct H {
        TFormReportAnalizeArchive *fuck;
        boost::shared_ptr<TFormReportAnalizeArchive> dlg;

        H() : dlg( new TFormReportAnalizeArchive(fuck) ) {
            dlg->OnClose = onClose;
        }

        LRESULT Do1(const Jurnal_Analize* pjurnalAnalize, const Jurnal_AnalizeArchive* pjurnalAnalizeArchive) {

            if(pjurnalAnalize!=NULL)    {
                AnsiString fn = Report_Jurnal_Analize(*pjurnalAnalize);
                dlg->CppWebBrowser1->Navigate( WideString( fn ));
            }

            if(pjurnalAnalizeArchive!=NULL)    {
                AnsiString fn = Report_Jurnal_AnalizeArchive(*pjurnalAnalizeArchive);
                dlg->CppWebBrowser2->Navigate( WideString( fn ));
            }

            if(pjurnalAnalize!=NULL||pjurnalAnalizeArchive!=NULL) {
                Form1->Hide();
                dlg->Show();
            }

            return 0;

        }
        void Do(const Jurnal_Analize* pjurnalAnalize, const Jurnal_AnalizeArchive* pjurnalAnalizeArchive) {
            my::Synchronize2( &Do1, pjurnalAnalize, pjurnalAnalizeArchive );
        }
        void __fastcall onClose(TObject*, TCloseAction&)
        {
            if( !Form1->Visible )
                Form1->Show();
        }


    } h;

    h.Do( hasGotJurnalAnalize_ ? &jurnalAnalize_ : NULL,
        hasGotJurnalAnalizeArchive_ ? &jurnalAnalizeArchive_ : NULL);

}
//------------------------------------------------------------------------------
void ReadMeasurmentsJournalsTask::Perform()				 VIRTUAL_FINAL_
{
    ShowTopInfoPanel( What() );
    
    uInt8
        *pJurnalAnalize = reinterpret_cast<uInt8*>( &jurnalAnalize_ ),
        *pJurnalAnalizeArchive = reinterpret_cast<uInt8*>( &jurnalAnalizeArchive_ );


    hasGotJurnalAnalize_ = GetHromatFile
    (
	    3,			                        	// индекс файла
        sizeof(Jurnal_Analize),            		// колличество байт
        HromatFileSpace::GRADUATION, 			// номер файловой области от 0
        0,                                  	// номер детектора
        pJurnalAnalize, 		                // выходной буффер
        "\"анализ\""
    );
    hasGotJurnalAnalizeArchive_ = GetHromatFile
    (
	    4,			                        	// индекс файла
        sizeof(Jurnal_AnalizeArchive),            		// колличество байт
        HromatFileSpace::GRADUATION, 			// номер файловой области от 0
        0,                                  	// номер детектора
        pJurnalAnalizeArchive, 		                // выходной буффер
        "\"архив анализов\""
    );
}

