//------------------------------------------------------------------------------
#pragma hdrstop
//------------------------------------------------------------------------------
#include "boost/assign/list_of.hpp"
//------------------------------------------------------------------------------
#include <Math.hpp>
#include <set>
#include <fstream>
//------------------------------------------------------------------------------
#include "CurHtg.h"
#include "HromatModbus.h"
#include "bcd_.h"
#include "MySynchronize.h"
#include "uFormHromatogramm.h"
#include "AnsiStringUtils_.h"
#include "Unit1.h"
#include "PerformTransfer.h"
#include "guicon.h"
#include "Hromat900.hpp"
#include "grdtns_.h"
#include "my_array_size.h"
#include "HromatPhysChem.h"
#include "MyAppWinPath.hpp"
#include "MyFileCtrl.h"
#include "uFormIzobutChart.h"
#include "hromat_sys.h"
#include "MyAppProfile.h"
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
#pragma package(smart_init)
//------------------------------------------------------------------------------
typedef std::pair<TDateTime,unsigned> DateTimeFileIndex;
typedef std::pair<DateTimeFileIndex, std::pair<unsigned, unsigned> >  GraduationInfo;
//------------------------------------------------------------------------------
void LogCurHtgInfo( const ModbusFileInfo* nfo )
{
    AnsiString sDate = "";
    try{ sDate = FormatDateTime("dd.mm.yy h.n.s", nfo[0].date_ ); } catch(...) {}
    const unsigned fileType = ExtractModbusFileType(nfo[0].type_);

    MYWCOUT_("Текущая хроматограмма :\n"
    "\tфайл\t- %d\n"
    "\tсоздан\t- %s\n"
    "\tтип\t- %d %s\n"
    "\tДетектор 1:\n"
    "\t\tразмер\t- %d байт\n"
    "\t\tпризнак\t- %d %s\n"
    "\tДетектор 2:\n"
    "\t\tразмер\t- %d байт\n"
    "\t\tпризнак\t- %d %s\n",
        nfo[0].index_,
        sDate,
        fileType, HromatFileType::ToStr(fileType),
        nfo[0].size_,
        nfo[0].sign_, HromatFileSign::FormatCode(nfo[0].sign_),
        nfo[1].size_,
        nfo[1].sign_, HromatFileSign::FormatCode(nfo[1].sign_)
    ));
}

//------------------------------------------------------------------------------
AnsiString GraduationInfo2Str(const GraduationInfo& grad)
{
    return MYSPRINTF_("%d, %s, %s",
        grad.first.second,
        MyFormatDateTime1( grad.first.first),
        GraduateResult2Str(grad.second.first) );
}
//------------------------------------------------------------------------------
void ReadCurrentHtgInfo(ModbusFileInfo* curHtgInfo, GraduationInfo& grad)
{
    ByteBuffPtr rxd, rxdEnd;
	ReadModbusRegistersData( CURRENT_HTG_INDEX_D0_REG, 22+3*4, rxd, rxdEnd );
    std::vector<double> dt(6);
    TransformBCDDataToFloat( rxd, rxd+6*4, &dt[0] );
    curHtgInfo[0].index_ =  Ceil( dt[0]);       // 292 293    0 1
    curHtgInfo[0].detector_ = 0;
    curHtgInfo[1].index_ =  Ceil( dt[1]);       // 298 299    6 7
    curHtgInfo[1].detector_ = 1;

    // фиксировать информацию о файле
    ReadHromatFileInfo(
        curHtgInfo[0].index_,       // индекс первого файла
        HromatFileSpace::HTG_NAME,  // номер файловой области от 0
        0,                          // номер детектора
        &curHtgInfo[0]		        // выходной буфер
    );

    const bool isDet2 = Hromat900::Instance().DetectorsCount()==2;

    if(isDet2)
    {
        ReadHromatFileInfo(
            curHtgInfo[1].index_,       // индекс первого файла
            HromatFileSpace::HTG_NAME,  // номер файловой области от 0
            1,                          // номер детектора
            &curHtgInfo[1]		        // выходной буфер
        );
    }


    curHtgInfo[0].size_ =   Ceil( dt[2] );      // 294 295    2 3
    curHtgInfo[0].sign_ =   Ceil( dt[3] );      // 296 297    4 5
    curHtgInfo[1].size_ =   Ceil( dt[4] );      // 300 301    8 9
    curHtgInfo[1].sign_ =   Ceil( dt[5] );      // 302 303    10 11

    MyWCout("Получены данные о файлах текущих хроматограмм.\n");
    LogCurHtgInfo( curHtgInfo );
    grad.second.second = Ceil( BCDToFloat(rxd + 4*6) );   // 304 305    12 13
    const ByteBuffPtr pGrad = rxd + 4*7;
    grad.first.first = ExtractHromatDateTime( pGrad);
    grad.first.second = Ceil( BCDToFloat(pGrad + 8) );
    grad.second.first = Ceil( BCDToFloat(pGrad + 8 + 4) );

    MYWCOUT_("Последняя градуировка: %s\n", GraduationInfo2Str(grad)  ));

    // 306 307    14 15
    // 308 309    16 17
    // 310 311    18 19
    // 312 313    20 21
    // 314 315    22 23
    // 316 317    24 25
    // 318 319    26 27
    struct{
        double progressHtg;
        LRESULT Show()
        {
            AnsiString s;
            s.sprintf( "Выполнено: %g", progressHtg);
            Form1->Panel2->Caption = s + " %";
            if(IsIzobut()) {
                FormIzobutChart->SetProgress(progressHtg, Hromat900::Instance().HtgDuration()*1.0);
            }
            return 0;
        }
    } h;
    h.progressHtg = Ceil( BCDToFloat(rxd + 52) );
    my::SynchronizeMthd( h.Show );

    MYWCOUT_("Прогресс считывания хроматограммы: %g\n", h.progressHtg  ));


    LRESULT EndPahase();


}


void ReadNextGraduateDate()
{
    ByteBuffPtr rxd, rxdEnd;
	ReadModbusRegistersData( 322, 4, rxd, rxdEnd );
    TDateTime dttm = ExtractHromatDateTime(rxd);
    struct{
        AnsiString s;
        LRESULT Show()
        {
            if(Hromat900::Instance().GraduateIsEnabled())
                Form1->Label2->Caption = s;
            else
                Form1->Label2->Caption = "Автоматическая градуировка запрещена";
            return 0;
        }
    } h;
    h.s = "Следующая градуировка " + FormatDateTime("dd.mm.yy hh:nn", dttm );
    my::SynchronizeMthd( h.Show );
    MYWCOUT_("Дата следующей градуировки: %s\n", h.s  ));
}

//------------------------------------------------------------------------------
class ProcessGraduation : public boost::noncopyable
{
public:
	//explicit ProcessGraduation( ) {}
    //~ProcessGraduation() {}
    void Perform( const GraduationInfo& );
private:
    typedef std::pair<TDateTime,unsigned> DateTimeFileIndex;
    typedef std::set< DateTimeFileIndex > SetT1;
    SetT1 set1_;
};
//------------------------------------------------------------------------------
void ProcessGraduation::Perform( const GraduationInfo& grad )
{
    if( grad.first.first.Val==0 || set1_.find(grad.first)!=set1_.end() )
        return;
    HromatGraduateItem graduate;
    if(!ReadLastHromatGraduation( graduate.Ki, graduate.Rk, graduate.Rkmax ) )
        MY_THROW_( "Ошибка ReadLastHromatGraduation!" );
    graduate.dateTime = grad.first.first;
    graduate.fileIndex = grad.first.second;
    graduate.result = grad.second.first;
    graduate.ballon = grad.second.second;
    Grdtn::Save( graduate );
    set1_.insert(grad.first);
    Grdtn::ShowLast();
}
//------------------------------------------------------------------------------



/**
*		Трансфер считывания заданной хроматограммы
*/
class ProcessHTGFile
{
public:
	explicit ProcessHTGFile(const ModbusFileInfo& currentHtgFileInfo, const bool& enebleReadDetectorIndication );
    ~ProcessHTGFile();
    void Perform();

    bool IsComplete() const
    {
        return BadFileSign() || (picsReaded_ || PicsWillNotBe() ) && !MustReadHTG();
    }

    AnsiString Descr() const {    return "ХТГ:"+HromatogrammIdCaption( htg_->GetId() ); }

private:
    bool PicsWillNotBe() const 
    {
        using namespace HromatFileSign;
        const unsigned sgn = thisFileInfo_.sign_;
        const unsigned skips[] = {
            TEMP_COLS_OUT_RANGE,
            CONS_D1_OUT_RANGE,
            CONS_D2_OUT_RANGE,
            CONS_OUT_RANGE,
            P_SAMPLE_OUT_RANGE,
            P_AUXILIARY_GAS_OUT_RANGE,
            P_CARRIER_GAS_OUT_RANGE,
            USER_BREAKED,
            ERROR_BREAKED,
            HEATING
        };
        const unsigned * const skipsEnd = skips + array_size(skips);
        return std::find(skips, skipsEnd, sgn)!=skipsEnd;
    }
    bool MustReadPic() const
    {
        const unsigned sgn = thisFileInfo_.sign_;
        return !PicsWillNotBe() && sgn==HromatFileSign::PERFORMED && !picsReaded_;
    }
    bool MustReadHTG() const
    {
        return enebleReadDetectorIndication_ && offset_<thisFileInfo_.size_;
    }
    bool BadFileSign() const
    {
        const unsigned sign = thisFileInfo_.sign_;
        return
        sign==HromatFileSign::USER_BREAKED      ||
        sign==HromatFileSign::ERROR_BREAKED     ||
        sign==HromatFileSign::POWER_OFF         ||
        HromatFileSign::IsInvalide(sign)
        ;
    }

    const bool& enebleReadDetectorIndication_;
    const ModbusFileInfo& currHtgFileInfo_;
    const bool dtctr_;
    unsigned offset_;       			// смещение
    bool picsReaded_;
    ModbusFileInfo thisFileInfo_;
    HromatogrammPtr htg_;


    void ReadPicTable();
    void ReadHTG();

    LRESULT AddHromatogramm();
    LRESULT OnReadHTGDataPart(ByteBuffPtr htg, ByteBuffPtr htgEnd);
    LRESULT EndPahase();

    void CheckThisFileInfo();

};
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
ProcessHTGFile::ProcessHTGFile(const ModbusFileInfo& currentHtgFileInfo,
    const bool& enebleReadDetectorIndication ) :
    enebleReadDetectorIndication_(enebleReadDetectorIndication),
    currHtgFileInfo_(currentHtgFileInfo), dtctr_( currHtgFileInfo_.detector_ ),
    offset_(0), picsReaded_(false), thisFileInfo_(currentHtgFileInfo)
{
    my::SynchronizeMthd( &AddHromatogramm );


}
//------------------------------------------------------------------------------
ProcessHTGFile::~ProcessHTGFile()
{
    LOG_FOO_LIFETIME_QUOTES;
    MYWCOUT_(" -- Окончание задания на считывание хроматограммы %s --\n",Descr() ));
}
//------------------------------------------------------------------------------
LRESULT ProcessHTGFile::AddHromatogramm()
{
    HromatogramId id;
    id.date_ = currHtgFileInfo_.date_;
    id.fileIndex_ = currHtgFileInfo_.index_;
    id.dtctr_ = dtctr_;
    id.rgmChanAndType_ = currHtgFileInfo_.type_;
    htg_.reset( new Hromatogramm(id, &FrmHTG[dtctr_]->UpdateView) );
    MYWCOUT_(" -- Добавлено задание на считывание хроматограммы %s --\n",Descr() ));
    htg_->SetSign( currHtgFileInfo_.sign_ );
    FrmHTG[dtctr_]->AddHtg( htg_ );
    return 0;
}
//------------------------------------------------------------------------------
LRESULT ProcessHTGFile::EndPahase()
{
    if( Form1 )
    	Form1->HandleProcessCurentHTG(thisFileInfo_, offset_);
    return 0;
}
//---------------------------------------------------------------------------
void ProcessHTGFile::Perform()
{
    CheckThisFileInfo();

    if( MustReadHTG() ) ReadHTG();
    if( MustReadPic() ) ReadPicTable();
    
    MYWCOUT_(" - %s %s с признаком %s -\n", Descr(),
        IsComplete() ? "завершено" : "продолжено",
            HromatFileSign::FormatCode( htg_->GetSign() ) ));

    my::SynchronizeMthd( &EndPahase );
}
//---------------------------------------------------------------------------
void ProcessHTGFile::CheckThisFileInfo()
{
    thisFileInfo_ = currHtgFileInfo_;
    const HromatogramId id = htg_->GetId();
    const bool isAnotherActiveHTG = (id.fileIndex_ != thisFileInfo_.index_);
    
    if( isAnotherActiveHTG )
    {
        MYWCOUT_( "%s - изменился индекс активной хроматограммы:%d!\n",
            Descr(), thisFileInfo_.index_ ));
        // фиксировать информацию о файле
        ReadHromatFileInfo(
	    	id.fileIndex_,                      // индекс первого файла
        	HromatFileSpace::HTG_NAME,    // номер файловой области от 0
        	id.dtctr_,                          // номер детектора
        	&thisFileInfo_		                // выходной буфер
        );
        if(thisFileInfo_.size_>=2) thisFileInfo_.size_-=2;
        else thisFileInfo_.size_=0;
    }

    const unsigned sign = thisFileInfo_.sign_;
    MYWCOUT_( "%s - размер=%d смещение=%d признак=%s\n", Descr(),
        thisFileInfo_.size_, offset_, HromatFileSign::FormatCode(sign)  ));
    if( sign!=htg_->GetSign() )
    {
        MYWCOUT_( "Изменился признак обработки %s->%s!\n",
        HromatFileSign::FormatCode(htg_->GetSign()), HromatFileSign::FormatCode(sign)  ));
        my::Synchronize1( &htg_->SetSign,  sign );
    }

    if( offset_>thisFileInfo_.size_ )
        MyWCout( " -- СМЕЩЕНИЕ ПРИ СЧИТЫВАНИИ ПРЕВЫШАЕТ РАЗМЕР ФАЙЛА! --\n");
}
//---------------------------------------------------------------------------
void ProcessHTGFile::ReadHTG()
{
	const unsigned
    	value_ToReadLen = std::min(thisFileInfo_.size_ - offset_, GetHromatMaxRxDFrameSize()*2),
    	toReadLen_div4 = value_ToReadLen / 4,
        toReadLen = ( offset_==0 && IsTCPConnection() ) ? thisFileInfo_.size_ : toReadLen_div4 * 4;

    if( toReadLen==0 )	return;

    const HromatogramId id = htg_->GetId();

    // кусок файла
    VInt8 htgFile_(toReadLen);

    //MYWCOUT_( "%s: читаем %d байт c позиции %d.\n", Descr(), toReadLen, offset_) );

    //const unsigned oldOffest = offset_, readedCount =
    ReadHromatFile
    (
	    id.fileIndex_,			                // индекс файла
        toReadLen,                              // колличество байт
        offset_,                                // смещение от начала файла
        HromatFileSpace::HTG_DATA,        // номер файловой области от 0
        id.dtctr_,                              // номер детектора
        htgFile_.begin(),                       // выходной буффер
        &OnReadHTGDataPart
    );
    //MYWCOUT_( "%s: считано %d, смещение %d->%d\n", Descr(), readedCount, oldOffest, offset_ ) );
}
//---------------------------------------------------------------------------
LRESULT ProcessHTGFile::OnReadHTGDataPart(ByteBuffPtr htg, ByteBuffPtr htgEnd )
{
    const unsigned readedCount = htgEnd-htg, oldOffSet = offset_;
    htg_->AddPointsAsBuff(htg, htgEnd);
    offset_ += (htgEnd-htg);
    MYWCOUT_( "%s: считано %d, смещение %d->%d\n", Descr(), readedCount,
    	oldOffSet, offset_ ) );
    return 0;
}
//---------------------------------------------------------------------------



void ProcessHTGFile::ReadPicTable()
{
    htg_->Finalize();
    // отобразить в главном окне ФХП
    ShowGrdPhysChem(Form1->grdPhisChem, htg_->GetPhysChem() );

    // найти в таблице пиков пик изобутилена
    // если он есть, то добавить его на чарт
    const HromatPicTable& piks = htg_->GetPicTable();
    for(int i=0; i<piks.size(); ++i) {
        const HromatPicData& pik = piks[i];
        if( IsIzobut() && AnsiString(pik.name_)==Izobut() ) {
            struct {
                double conc;
                LRESULT Do()
                {
                    FormIzobutChart->AddConc(conc);
                    return 0;
                }
            } h;
            h.conc = pik.conc_;
            my::SynchronizeMthd( h.Do );
        }

    }

    picsReaded_ = true;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
CurrHtgHelperT::CurrHtgHelperT(const bool& enebleReadDetectorIndication) :
    enebleReadDetectorIndication_(enebleReadDetectorIndication),
    curHtgIndex_(-1), isCurHtgIndexInitialized_(0), processGraduation_( new ProcessGraduation )
{       
}
//------------------------------------------------------------------------------
void CurrHtgHelperT::ReadHtg(bool detector)
{
    ProcessHTGFilesList &items = procFiles_[detector];
    if( items.empty() ) return;

    MyWCout( "Список:\n");
    for( unsigned i=0; i<items.size(); ++i )
        MYWCOUT_( "\t\t---\t\t%s\n", items[i]->Descr() ));


    PProcessHTGFile item = items.front();
    item->Perform();
    if( item->IsComplete() )
    {
        items.pop_front();
    }
    else
    {
        items.push_back( items.front() );
        items.pop_front();
    }
}
//------------------------------------------------------------------------------
void CurrHtgHelperT::Process()
{
    GraduationInfo grad;
    ReadCurrentHtgInfo( curHtgInfo_, grad );

    const bool isDet2 = Hromat900::Instance().DetectorsCount()==2;

    if( !isCurHtgIndexInitialized_ || curHtgIndex_!=curHtgInfo_[0].index_ )
    {
        isCurHtgIndexInitialized_ = true;
        curHtgIndex_ = curHtgInfo_[0].index_ ;
        ReadHromatFileInfo(
	    	curHtgIndex_,                       // индекс первого файла
        	HromatFileSpace::HTG_NAME,          // номер файловой области от 0
        	0,                                  // номер детектора
        	&curHtgInfo_[0]		                // выходной буфер
        );
        // добавить хроматограмму на считывание
        procFiles_[0].push_back( PProcessHTGFile( new ProcessHTGFile(
            curHtgInfo_[0], enebleReadDetectorIndication_ ) ) ) ;



        if(isDet2)
        {
            ReadHromatFileInfo(
	    	    curHtgIndex_,                       // индекс первого файла
        	    HromatFileSpace::HTG_NAME,          // номер файловой области от 0
        	    1,                                  // номер детектора
        	    &curHtgInfo_[1]		                // выходной буфер
            );
            procFiles_[1].push_back( PProcessHTGFile( new ProcessHTGFile(
                curHtgInfo_[1], enebleReadDetectorIndication_ ) ) ) ;
        }




    }

    processGraduation_->Perform( grad );
    ReadHtg(0);
    if(isDet2)
        ReadHtg(1);
    ReadNextGraduateDate();
}
//------------------------------------------------------------------------------
void CurrHtgHelperT::DoOnEndTransfer()
{    
}
//------------------------------------------------------------------------------
