//---------------------------------------------------------------------------
#ifndef htg_H
#define htg_H
//---------------------------------------------------------------------------
//std
#include <vector>

// boost
#include "boost\noncopyable.hpp"
#include "boost\shared_ptr.hpp"

//vcl
#include <SysUtils.hpp>
#include <grids.hpp>
#include <ComCtrls.hpp>



// my
#include "..\Application\MyIostream.h"
#include "..\Application\BinStream.hpp"
#include "MyExcpt.hpp"
#include "Hromat900.hpp"
#include "Hromatogram.hpp"



DECLARATE_AND_DEFINE_MY_EXCEPTION_CLASS_( CantReadHtg, CantReadIStreamException );



/**
*       Представление хроматограммы
*/
class Hromatogramm : public boost::noncopyable
{
public:
    typedef // обработчик изменения состояния
    void (__closure *UpdateViewFooT )(const Hromatogramm*);


    // Представление хроматограммы по её идентификатору
    explicit Hromatogramm(const HromatogramId&, UpdateViewFooT updateViewFoo = NULL);

    // прочитать из файла
    Hromatogramm(const AnsiString& fileName );

    /**
    *       Управление состоянием
    */

    LRESULT SetSign(unsigned);                             // установить признак обработки
    LRESULT AddPointsAsBuff(const unsigned char*, const unsigned char*);  // добавление точек хроматограммы
    void Finalize( unsigned picFileSize );                                    // фиксировать состояние
    void Finalize( );

    void SetDataAndSave(unsigned sign, const unsigned char* points, const unsigned char* pointsEnd);
    
    /*
    void SetUpdateViewFoo(UpdateViewFooT updateViewFoo)
    { updateViewFoo_ = updateViewFoo;    }
    */
    
    /**
    *       Методы для доступа к состоянию
    */
    const HromatogramId& GetId() const;                        // идентификатор хроматограммы
    const HromatPicTable& GetPicTable() const;      // таблица пиков
    unsigned GetSign()                  const;      // признак обработки
    const std::vector<float>& GetPoints()           const;      // точки хроматограммы
    const HromatPhysChem& GetPhysChem() const;      // физикохимические показатели
    unsigned RGMChan() const
    {
    	return ExtractRGMChanal(id_.rgmChanAndType_);
    }

    AnsiString GetPhysChemValAsStr(unsigned) const;

    // получить текущее имя файла
    AnsiString GetFileName() const { return fn_; }
    AnsiString NormalFileName(const AnsiString& path = "") const;
    // сохраниться
    LRESULT SaveData(const AnsiString& path = "");

private:

    // обработчик изменения состояния
    const UpdateViewFooT updateViewFoo_;

    // идентификатор хроматограммыы
    const HromatogramId id_;

#ifdef CREATE_HROMATOGRAMM_LOG_FILE_
    std::ofstream log_;
#endif

    // признак обработки файла
    unsigned sign_;

    // точки
    std::vector<float> pts_;

    // пики
    HromatPicTable pics_;

    // физикохимические показатели
    HromatPhysChem physChem_;

    // путь к файлу
    AnsiString fn_;

    LRESULT UpdateView() const;

    LRESULT  DoFinalize();
};
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
HromatogrammConstPtr GetHromatogrammConstPtr( const Hromatogramm* );
//---------------------------------------------------------------------------





/**
*				Утилиты для представления хроматограмм
*/
void MakeGrdHtgsList(TStringGrid* grd, const HromatogrammsList& htgs );
void MakeListViewdHtgsList(TListView* lv, const HromatogrammsList& htgs );



void ShowHromatogrammMain( const Hromatogramm *htg, TFrame *frame,
    TStringGrid *grdPhysChem, TStringGrid *grdPics  );

// отобразить хроматограмму на графике фрэйма TFrameGraph
// проверяется тип ппараметра frame с помощью dynamic_cast
void ShowHTGGraph( const Hromatogramm * htg, TFrame *frame );
void PrintHTG( const Hromatogramm *htg, TFrame *frame );

AnsiString HromatogrammCaption_0( const Hromatogramm *htg);
AnsiString HromatogrammCaption_1( const Hromatogramm *htg);
AnsiString HromatogrammIdCaption( const HromatogramId& id);
AnsiString FormatHTGFileInfo(const ModbusFileInfo& fi);



void InitializeGrdHtgsList( TStringGrid* grd );
void InitializeGrdHtgsList( TListView* lv );

struct ShowHTGInGrd
{
	ShowHTGInGrd(TStringGrid*, HromatogrammsList::const_iterator beg);
    void operator()( const HromatogrammPtr& pHtg) const;
	TStringGrid* grd_;
    const HromatogrammsList::const_iterator beg_;
};


int HromotogrammIndexOfIdInList(const HromatogrammsList &htgs, unsigned id);

struct HromatogrammIdLess
{
    bool operator()( const HromatogrammConstPtr& lhs, const HromatogrammConstPtr& rhs ) const;

};

struct HromatogrammDateLess
{
    bool operator()( const HromatogrammConstPtr& lhs, const HromatogrammConstPtr& rhs ) const;

};

AnsiString GetHromatogrammFileName(const HromatogramId& id_, const AnsiString& path="" );







//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
