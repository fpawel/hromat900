//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#pragma hdrstop
//---------------------------------------------------------------------------
#include "htg_.h"
#include "CRC16.h"
#include "uFrameGraph.h"

#include "my_draw_.h"


//#include "uFormHromatogramm.h"
#include "AnsiStringUtils_.h"

#include "SGrdHlpr.h"
#include "MySynchronize.h"

#include "HromatModbus.h"
#include "HromatPhysChem.h"
//#include "MyAppProfile.h"
#include "MyAppWinPath.hpp"
#include "AppEvts.h"
#include "cnfdntl_.h"
#include "MyWindow.h"
#include "HromatPic.h"
#include "..\my_include\win\WinErr.hpp"
#include "BinStream.hpp"


//vcl
#include <DateUtils.hpp>
#include <FileCtrl.hpp>
#include "MyFileCtrl.h"

//std
#include <fstream>





//---------------------------------------------------------------------------
#pragma package(smart_init)
//------------------------------------------------------------------------------

AnsiString GetHromatogrammFileName(const HromatogramId& id_, const AnsiString& path )
{
    Word year, month, day, hour, minute, second, milliSecond;
    DecodeDateTime( id_.date_, year, month, day, hour, minute, second, milliSecond);

    const unsigned
        rgm = ExtractRGMChanal(id_.rgmChanAndType_),
        type = ExtractModbusFileType(id_.rgmChanAndType_);

    const AnsiString
        parDir = path.IsEmpty() ? HtgArchivePath() : path,
        endName = AnsiString().sprintf(
            "%d_%d_%d_D_%d_id_%d_type_%d_rgm_%d",
            day, month, year,
            id_.dtctr_+1, id_.fileIndex_, type, rgm );
    return MakeFileNameAsDate( parDir, "htg", id_.date_, endName );
}

bool HromatogrammIdLess::operator()( const HromatogrammConstPtr& lhs, const HromatogrammConstPtr& rhs ) const
{
    return lhs->GetId().fileIndex_ < rhs->GetId().fileIndex_;
}

bool HromatogrammDateLess::operator()( const HromatogrammConstPtr& lhs, const HromatogrammConstPtr& rhs ) const
{
    return lhs->GetId().date_ < rhs->GetId().date_;
}

struct HromatogrammIdEqual
{
    explicit HromatogrammIdEqual(unsigned fileIndex) : fileIndex_(fileIndex) {}
    bool operator()( const HromatogrammConstPtr& ob) const
    {
        return ob->GetId().fileIndex_ == fileIndex_;
    }
    const unsigned fileIndex_;
};
//------------------------------------------------------------------------------
int HromotogrammIndexOfIdInList(const HromatogrammsList &htgs, unsigned id)
{
    const HromatogrammPtr *beg = htgs.begin(), *end = htgs.end(),
        *fnd =  std::find_if(beg, end, HromatogrammIdEqual(id) );
    const int ret = (fnd==end) ? -1 : fnd-beg;
    return ret;
}
//------------------------------------------------------------------------------


enum { CURENT_FILE_FORMAT_CODE = 0x01 };
struct EmptyDeleter { void operator()( const Hromatogramm* ) {} };
HromatogrammConstPtr GetHromatogrammConstPtr( const Hromatogramm* pHtg)
{
    return HromatogrammConstPtr(pHtg, EmptyDeleter() );
}

enum{
	HTG_ID_NUM,
    HTG_ID_DTCTR,
    HTG_ID_DATE,
    HTG_ID_TIME,
    HTG_ID_DUR,
    HTG_ID_TYPE,
    HTG_ID_STATE,
    HTG_ID_FILE,
    HTG_ID_PICS_COUNT,
    HTG_ID_RGM_CHAN,
    HTG_ID_FIELDS_COUNT
};
const ConstCString HTG_ID_FIELDS_NAMES[HTG_ID_FIELDS_COUNT] =
{"№", "Детектор", "Дата", "Время", "Длит.", "Тип", "Обработка", "Файл", "Пиков", "Канал"};

 // Представление хроматограммы создаём по её идентификатору
Hromatogramm::Hromatogramm(const HromatogramId& id, UpdateViewFooT updateViewFoo) :
    id_(id),

    sign_(HromatFileSign::PERFORMING),
    pts_(0),
    pics_(0),
    physChem_(),
    updateViewFoo_( updateViewFoo ),

    fn_()
{
    HromatPhysChem nullPhysChem = {0};
    physChem_ = nullPhysChem;
#ifdef CREATE_HROMATOGRAMM_LOG_FILE_

    const AnsiString
        parDir = Private::Single::Instance().HtgsDir(),
        endName = AnsiString().sprintf("D%d", id_.dtctr_+1),
        logFileName = MakeFileNameAsDate( parDir, "log", id_.date_, endName );
    log_.open( logFileName.c_str() );
#endif

}
//------------------------------------------------------------------------------
// Получить идентификатор хроматограммы
const HromatogramId& Hromatogramm::GetId() const { return id_; }
//------------------------------------------------------------------------------
unsigned Hromatogramm::GetSign() const{ return sign_; }
//------------------------------------------------------------------------------
const std::vector<float>& Hromatogramm::GetPoints() const { return pts_; }
//------------------------------------------------------------------------------
const HromatPicTable& Hromatogramm::GetPicTable() const { return pics_; }
//------------------------------------------------------------------------------
const HromatPhysChem& Hromatogramm::GetPhysChem() const      // физикохимические показатели
{
    return physChem_;
}
//------------------------------------------------------------------------------
AnsiString Hromatogramm::GetPhysChemValAsStr(unsigned fld) const
{
	return PhysChemFieldAsStr( physChem_, fld);
}
//------------------------------------------------------------------------------
LRESULT Hromatogramm::AddPointsAsBuff(const unsigned char* htg, const unsigned char* htgEnd)
{
    for( ; htg<htgEnd; htg+=4 )
    {
        union
        {
            long asInt_;
            unsigned char asBytes_[4];
        } y;
        y.asBytes_[0] = htg[0];
        y.asBytes_[1] = htg[1];
        y.asBytes_[2] = htg[2];
        y.asBytes_[3] = htg[3];
        // const double yVal = ( (y.asInt_ - 0x7FFFFF)*4.0 / (0xFFFFFF - 0x7FFFFF)*1.0 );
        // 19 05 2009
        const long
            d1 = y.asInt_ - 0x7FFFFF,
            d2 = 0xFFFFFF - 0x7FFFFF;
        const double
            v1 = d1*12.0,
            v2 = d2*1.0,
            yVal = v1 / v2;
        pts_.push_back(yVal);

#ifdef CREATE_HROMATOGRAMM_LOG_FILE_
        log_ <<  MYSPRINTF_(
        "%d.\t"
        "ADC=[%s, %s, %s, %s]\t"
        "ADC=%d\t"
        "U=%s\n",
        pts_.size(),
        IntToHex(htg[0],2),	IntToHex(htg[1],2), IntToHex(htg[2],2),	IntToHex(htg[3],2),
        y.asInt_,
        FormatFloat("######0.0##########", yVal)
        );
#endif
    }
    SaveData();
#ifdef CREATE_HROMATOGRAMM_LOG_FILE_
    log_.flush();
#endif
    my::SynchronizeMthd( &UpdateView );
    return 0;
}
//------------------------------------------------------------------------------
LRESULT Hromatogramm::DoFinalize()
{
    //if( !id_.type_ )
    SaveData();
    UpdateView();
    return 0;
}
//------------------------------------------------------------------------------
LRESULT Hromatogramm::UpdateView() const
{
	if( updateViewFoo_!=0 )
        updateViewFoo_(this);
    return 0;
}
//------------------------------------------------------------------------------
AnsiString Hromatogramm::NormalFileName(const AnsiString& path) const
{
    Word year, month, day, hour, minute, second, milliSecond;
    DecodeDateTime( id_.date_, year, month, day, hour, minute, second, milliSecond);

    const unsigned
        rgm = ExtractRGMChanal(id_.rgmChanAndType_),
        type = ExtractModbusFileType(id_.rgmChanAndType_);

    const AnsiString
        parDir = path.IsEmpty() ? HtgArchivePath() : path,
        endName = AnsiString().sprintf(
            "%d_%d_%d_D_%d_id_%d_type_%d_rgm_%d",
            day, month, year,
            id_.dtctr_+1, id_.fileIndex_, type, rgm );
    return MakeFileNameAsDate( parDir, "htg", id_.date_, endName );
}
//------------------------------------------------------------------------------
LRESULT Hromatogramm::SaveData(const AnsiString& path )
{
    fn_ = NormalFileName(path);
    // колличество точек
    const unsigned ptsCount = pts_.size();
    // колличество пиков
    const unsigned picsCount = pics_.size();
    ForceDirectories( ExtractFilePath(fn_) );
    std::ofstream strm( fn_.c_str(), std::ios::binary );

    if( !strm.is_open() )
    {
    	ShowLastWindowsError("Сбой при сохранении данных на диск!\nПроверте путь к архиву хроматограмм!");
        return 0;
    }

    const AutoCloseFileStreamHelper<std::ofstream> autoCloseFileStreamHelper(strm);
    try
    {

        const MyBinaryOStreamHlpr myStrm( strm );

        enum { CURENT_FILE_FORMAT_CODE = 0x01 };
        myStrm << (unsigned)CURENT_FILE_FORMAT_CODE;        // формат файла
        // дата создания хроматограммы
        myStrm << id_.date_.Val;
        // индекс файла  хроматограммы
        myStrm << id_.fileIndex_;
        // тип файла (градуировочный, измерительный)
        myStrm << id_.rgmChanAndType_;
        // связанный детектор
        myStrm << id_.dtctr_;
        // признак обработки файла
        myStrm << sign_;
        // колличество точек
        myStrm << ptsCount;
        // точки
        myStrm.WriteArray(pts_.begin(), ptsCount );
        // колличество пиков
        myStrm << picsCount;
        // пики
        myStrm.WriteArray(pics_.begin(), picsCount );

        // ФХП
        myStrm << physChem_;

        // записать в жопу файла CRC16
        myStrm.FixCRC16();
    }
    catch(const PMyExcpt& e)
    {
        if( e->TypeOf<MyCantWriteOStreamException>() )
        {
            ShowLastWindowsError("Сбой при сохранении данных на диск!\nПроверте путь к архиву хроматограмм!");
            Application->Terminate();
        }
    }
    return 0;
}
//------------------------------------------------------------------------------

// прочитать из файла
Hromatogramm::Hromatogramm(const AnsiString& fn) :
    id_(), updateViewFoo_(NULL), fn_(fn), physChem_( NullPhysChem() )
{
    std::ifstream strm( fn_.c_str(), std::ios::binary );
    const AutoCloseFileStreamHelper<std::ifstream> autoCloseFileStreamHelper(strm);

    const MyBinaryIStreamHlpr myStrm( strm );
    HromatogramId& id = *const_cast<HromatogramId*>(&id_);

    // формат файла
    if( myStrm.Get<unsigned>()!=CURENT_FILE_FORMAT_CODE)
        MY_THROW_CLASS_(MyCantReadIStreamException, "Несовпадение версии формата!");

    // дата создания хроматограммы
    myStrm >> id.date_.Val;
    
    // индекс файла  хроматограммы
    myStrm >> id.fileIndex_;

    // тип файла (градуировочный, измерительный)
    myStrm >> id.rgmChanAndType_;

    // связанный детектор
    myStrm >> id.dtctr_;

    // признак обработки файла
    myStrm >> sign_;

	// колличество точек
    const unsigned ptsCount = myStrm.Get<unsigned>();
    enum {MAX_HTG_PTS_COUNT = 3600*20+1000};
    if(ptsCount>MAX_HTG_PTS_COUNT)
        MY_THROW_CLASS_(MyCantReadIStreamException, "Некорректные данные: длина хроматограммы!");

    // точки
    pts_.resize(ptsCount);
    myStrm.ReadArray(pts_.begin(), ptsCount );

    // колличество пиков
    const unsigned picsCount = myStrm.Get<unsigned>();
    if(picsCount>100)
        MY_THROW_CLASS_(MyCantReadIStreamException, "Некорректные данные: колличество пиков!");

    // пики
    pics_.resize(picsCount);
    myStrm.ReadArray(pics_.begin(), picsCount );
    NormalizePicNameInTable2(pics_);
        
    // ФХП
    myStrm >> physChem_;

    // CRC16
    if( !myStrm.CheckCRC16() ) MY_THROW_CLASS_(MyCantReadIStreamException,
        "Некорректные данные: несовпадение CRC16");

}
//------------------------------------------------------------------------------
void Hromatogramm::SetDataAndSave(unsigned sign, const unsigned char* points,
    const unsigned char* pointsEnd)
{
    sign_ = sign;
    my::Synchronize2( &AddPointsAsBuff, points, pointsEnd );
    Finalize();
}
//------------------------------------------------------------------------------
LRESULT Hromatogramm::SetSign(unsigned sign)
{
    sign_ = sign;
    UpdateView();
    return 0;
}
//------------------------------------------------------------------------------
void Hromatogramm::Finalize(unsigned picFileSize)
{
    const AnsiString sHtg = MYSPRINTF_( "Хроматограмма %d %d", id_.dtctr_, id_.fileIndex_ );
    
    const bool
        htgType = ExtractModbusFileType(id_.rgmChanAndType_),
        isKalibrFile = htgType==1,
        fileHasPhysChem     = !isKalibrFile && picFileSize>=(HROMAT_PHIS_CHEM_INFO_SIZE + 2),
        cantFindPhysChem    = !isKalibrFile && !fileHasPhysChem;
    const unsigned
        picTableSize = picFileSize-2-(fileHasPhysChem ? HROMAT_PHIS_CHEM_INFO_SIZE : 0);
    const bool
        badPicTableSize = picTableSize%HROMAT_IDENTIFIED_PIC_STRUCT_SIZE!=0;
    const unsigned
        picsCount = badPicTableSize ? 0 : picTableSize / HROMAT_IDENTIFIED_PIC_STRUCT_SIZE;



    const bool badFileSize = cantFindPhysChem || badPicTableSize;

    if(!badFileSize )
    {
        VInt8 smmryFile(picFileSize);
        uInt8* pSmmryFile = reinterpret_cast<uInt8*>( smmryFile.begin() );

        // считываем файл
        ReadHromatFile(
            id_.fileIndex_,			                // индекс файла
            picFileSize,                         // колличество байт
            0,                                      // смещение от начала файла
            HromatFileSpace::PIK_DATA,        // номер файловой области от 0
            id_.dtctr_,                              // номер детектора
            pSmmryFile,                              // выходной буффер
            NULL,
            " "
        );

        go_to_recalc_crc16:
        if( CalculateHromatCRC16( pSmmryFile, picFileSize, GetHromatCRC16Key() )==0)
        {
            // выдераем из буфера таблицу пиков
            if(picsCount>0) {
                HromatPicTable picsToRead( picsCount );
                std::copy( pSmmryFile, pSmmryFile+picTableSize,
                    reinterpret_cast<uInt8*>( picsToRead.begin() ) );
                NormalizePicNameInTable2(picsToRead);
                pics_ = picsToRead;
            }

            // выдераем из буфера фхп
            if(fileHasPhysChem)
                std::copy( pSmmryFile+picTableSize, pSmmryFile+picTableSize+
                HROMAT_PHIS_CHEM_INFO_SIZE, reinterpret_cast<uInt8*>( &physChem_ ) );
            else
                physChem_ = NullPhysChem();
        }
        
        else
        {
            // лагать
            LogMyAppEvt("Ошибка! Несоответствие CRC16 в табице пиков!", sHtg );

            ::MessageBox(Application->Handle,
                ("Ошибка! Несоответствие CRC16 в табице пиков!"+sHtg).c_str(),
                "Ошибка CRC16!", MB_OK | MB_ICONERROR | MB_SYSTEMMODAL  );
        }
    }

    // финализировать хроматограмму
    my::SynchronizeMthd( &DoFinalize);
}
//------------------------------------------------------------------------------

void Hromatogramm::Finalize()
{
    const AnsiString sHtg = MYSPRINTF_( "Хроматограмма %d %d", id_.dtctr_, id_.fileIndex_ );

    // фиксировать информацию о файле
    ModbusFileInfo fileInfo;

    const bool hasGotFileInfo = ReadHromatFileInfo(
	        id_.fileIndex_,                      // индекс
            HromatFileSpace::PIK_NAME,    // номер файловой области от 0
            id_.dtctr_,                          // номер детектора
            &fileInfo		                    // выходной буфер
        )==true && id_.fileIndex_==fileInfo.index_;
    if(hasGotFileInfo)
        Finalize(fileInfo.size_);
    else
        LogMyAppEvt( "Ошибка! Неверный формат таблицы пиков!", sHtg, FormatModbusFileInfo( fileInfo ) );
		//LogMyAppEvt( "Ошибка! Неверный формат таблицы пиков!", sHtg, "" );
    
}
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
ShowHTGInGrd::ShowHTGInGrd(TStringGrid* grd, HromatogrammsList::const_iterator beg) :
    	grd_(grd), beg_(beg)
{}
void ShowHTGInGrd::operator()( const HromatogrammPtr& pHtg) const
{
    const int row = 1 + &pHtg - beg_;
    const Hromatogramm& htg = *( pHtg.get() );
    const HromatogramId& id = htg.GetId();
    const unsigned ptsCount = htg.GetPoints().size();
    const TDateTime duration = IncMilliSecond( 0, ptsCount*50);
    const unsigned sign = htg.GetSign();

    const bool htgType = ExtractModbusFileType(id.rgmChanAndType_);

    const AnsiString roStr[HTG_ID_FIELDS_COUNT] =
    {
        	row,
        	1 + id.dtctr_,
            FormatDateTime("dd.mm.yy",id.date_),
            FormatDateTime("hh:nn:ss",id.date_),
            FormatDateTime("hh:nn:ss", duration),
            HromatFileType::ToStr( htgType ),
            HromatFileSign::FormatCode(sign),
            id.fileIndex_,
            htg.GetPicTable().size(),
            htg.RGMChan()

    };

    for( int i=0; i<HTG_ID_FIELDS_COUNT; ++i)
        my::Grd::SetCellByTtl(grd_, HTG_ID_FIELDS_NAMES[i],	row, roStr[i] );
}
//---------------------------------------------------------------------------
struct ShowHTGInListView
{
	ShowHTGInListView(TListView*, HromatogrammsList::const_iterator beg);
    void operator()( const HromatogrammPtr& pHtg) const;
	TListView* lv_;
    const HromatogrammsList::const_iterator beg_;
};
//---------------------------------------------------------------------------
ShowHTGInListView::ShowHTGInListView(TListView* lv, HromatogrammsList::const_iterator beg) :
    	lv_(lv), beg_(beg)
{}
void ShowHTGInListView::operator()( const HromatogrammPtr& pHtg) const
{
    const int row = &pHtg - beg_;
    const Hromatogramm& htg = *( pHtg.get() );
    const HromatogramId& id = htg.GetId();
    const unsigned ptsCount = htg.GetPoints().size();
    const TDateTime duration = IncMilliSecond( 0, ptsCount*50);
    const unsigned sign = htg.GetSign();
    const bool htgType = ExtractModbusFileType(id.rgmChanAndType_);

    const AnsiString roStr[HTG_ID_FIELDS_COUNT] = {
        	row+1,
        	1 + id.dtctr_,
            FormatDateTime("dd.mm.yy",id.date_),
            FormatDateTime("hh:nn:ss",id.date_),
            FormatDateTime("hh:nn:ss", duration),
            HromatFileType::ToStr( htgType ),
            HromatFileSign::FormatCode( sign),
            id.fileIndex_,
            htg.GetPicTable().size()
    	};

    for( int i=1; i<HTG_ID_FIELDS_COUNT; ++i)
        lv_->Items->Item[i]->SubItems->Add(roStr[i]);

}
//---------------------------------------------------------------------------

void MakeGrdHtgsList(TStringGrid* grd, const HromatogrammsList& htgs )
{
    const int sz = (int)htgs.size();
    my::Grd::ClearTStringGrid( grd, 1, 1 );
    grd->RowCount = sz+1;
    grd->FixedRows = sz>0 ? 1 : 0;
    std::for_each( htgs.begin(), htgs.end(), ShowHTGInGrd(grd, htgs.begin() ) );
}

void MakeListViewdHtgsList(TListView* lv, const HromatogrammsList& htgs )
{
    const int sz = (int)htgs.size();
    lv->Clear();
    for( int i=0; i<sz; ++i )
    {
        TListItem *itm = lv->Items->Add();
        itm->Caption = IntToStr(i+1);
        const Hromatogramm& htg = *htgs[i].get();
        const HromatogramId& id = htg.GetId();
        const unsigned ptsCount = htg.GetPoints().size();
        const TDateTime duration = IncMilliSecond( 0, ptsCount*50);
        const unsigned sign = htg.GetSign();
        const bool htgType = ExtractModbusFileType(id.rgmChanAndType_);

        const AnsiString roStr[HTG_ID_FIELDS_COUNT] = {
        	"",
        	1 + id.dtctr_,
            FormatDateTime("dd.mm.yy",id.date_),
            FormatDateTime("hh:nn:ss",id.date_),
            FormatDateTime("hh:nn:ss", duration),
            HromatFileType::ToStr( htgType ),
            HromatFileSign::FormatCode(sign),
            id.fileIndex_,
            htg.GetPicTable().size()
    	};

        for( int i=1; i<HTG_ID_FIELDS_COUNT; ++i)
            itm->SubItems->Add(roStr[i]);
    }
}

//---------------------------------------------------------------------------
void InitializeGrdHtgsList(TStringGrid* grdHtgsList)
{
    grdHtgsList->ColCount = HTG_ID_FIELDS_COUNT;
    grdHtgsList->FixedCols = 1;
    my::Grd::SetRow(grdHtgsList, 0, HTG_ID_FIELDS_NAMES, HTG_ID_FIELDS_NAMES+HTG_ID_FIELDS_COUNT );
}
//---------------------------------------------------------------------------

//------------------------------------------------------------------------------
void ShowHromatogrammMain( const Hromatogramm *htg, TFrame *frame,
    TStringGrid *grdPhysChem, TStringGrid *grdPic  )
{
    if(grdPhysChem!=NULL)
        my::Grd::ClearTStringGrid(grdPhysChem, 2, 1);        
    my::Grd::ClearTStringGrid( grdPic, 1, 1 );
    grdPic->RowCount = 1;

    ShowHTGGraph( htg, frame );
    if(htg!=NULL)
    {
        const HromatPicTable& tbl = htg->GetPicTable();

        ShowHromatPicTable( grdPic, tbl.begin(), tbl.end() );

        // отобразить ФХП
        if(grdPhysChem!=NULL)
            ShowGrdPhysChem( grdPhysChem, htg->GetPhysChem() );
    }
}


//------------------------------------------------------------------------------
// отобразить хроматограмму на графике фрэйма TFrameGraph
void ShowHTGGraph( const Hromatogramm *htg, TFrame *aFrame )
{
    //const Hromatogramm* htgGet = htg.get();

	TFrameGraph *frameGraph = dynamic_cast<TFrameGraph*>(aFrame);
    if(frameGraph==NULL) return;
    TChartSeries *grph = frameGraph->GetSeries(0);
    grph->Active = 0;
    //TChart* chrt = frameGraph->chrt_;
    grph->Clear();


    std::vector<double> xPicMarks, x0Marks, x1Marks;
    std::vector<AnsiString> picNames_;


    if( htg==NULL )
    {
    	frameGraph->SetXMarks(xPicMarks, x0Marks, x1Marks, picNames_);
        return;
    }

    

    const std::vector<float>& pts = htg->GetPoints();
    const unsigned ptsCount = pts.size();
    TTime x = IncMilliSecond( 0, grph->Count()*GetDiskretTimeMSec());
    for( unsigned i = grph->Count(); i<ptsCount; ++i, x = IncMilliSecond( x, GetDiskretTimeMSec() ) )
    {
        const AnsiString s = FormatDateTime("hh:nn:ss",x);
        grph->AddXY( x, pts[i] );
    }
    // добавит пометки пиков на графике
    const HromatPicTable& tbl = htg->GetPicTable();
    for( unsigned i=0; i<tbl.size(); ++i ) if( tbl[i].t_>0 )
    {

        const double vv = IncMilliSecond( 0, Ceil(tbl[i].t_*1000) );
        xPicMarks.push_back( vv );

        x0Marks.push_back( IncMilliSecond( 0, Ceil(tbl[i].t1_*1000) ) );
        x1Marks.push_back( IncMilliSecond( 0, Ceil(tbl[i].t2_*1000) ) );
        picNames_.push_back( tbl[i].name_ );
    }
    frameGraph->SetXMarks(xPicMarks, x0Marks, x1Marks, picNames_);
    grph->Active = 1;
}
//------------------------------------------------------------------------------
AnsiString HromatogrammIdCaption( const HromatogramId& id)
{
    return AnsiString().sprintf( "Д%d:%d", id.dtctr_+1, id.fileIndex_ );
}
//------------------------------------------------------------------------------
AnsiString FormatHTGFileInfo(const ModbusFileInfo& fi)
{
    //const TDateTime duration = IncMilliSecond( 0, 50*fi.size_/4);
    return AnsiString().sprintf(
        "Д%d %d от %s %s",
        fi.detector_+1, fi.index_, FormatDateTime( "dd.mm.yy", fi.date_ ),
        FormatDateTime( "hh:nn", fi.date_ )
    );
}
//------------------------------------------------------------------------------

