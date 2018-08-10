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
*       ������������� �������������
*/
class Hromatogramm : public boost::noncopyable
{
public:
    typedef // ���������� ��������� ���������
    void (__closure *UpdateViewFooT )(const Hromatogramm*);


    // ������������� ������������� �� � ��������������
    explicit Hromatogramm(const HromatogramId&, UpdateViewFooT updateViewFoo = NULL);

    // ��������� �� �����
    Hromatogramm(const AnsiString& fileName );

    /**
    *       ���������� ����������
    */

    LRESULT SetSign(unsigned);                             // ���������� ������� ���������
    LRESULT AddPointsAsBuff(const unsigned char*, const unsigned char*);  // ���������� ����� �������������
    void Finalize( unsigned picFileSize );                                    // ����������� ���������
    void Finalize( );

    void SetDataAndSave(unsigned sign, const unsigned char* points, const unsigned char* pointsEnd);
    
    /*
    void SetUpdateViewFoo(UpdateViewFooT updateViewFoo)
    { updateViewFoo_ = updateViewFoo;    }
    */
    
    /**
    *       ������ ��� ������� � ���������
    */
    const HromatogramId& GetId() const;                        // ������������� �������������
    const HromatPicTable& GetPicTable() const;      // ������� �����
    unsigned GetSign()                  const;      // ������� ���������
    const std::vector<float>& GetPoints()           const;      // ����� �������������
    const HromatPhysChem& GetPhysChem() const;      // ���������������� ����������
    unsigned RGMChan() const
    {
    	return ExtractRGMChanal(id_.rgmChanAndType_);
    }

    AnsiString GetPhysChemValAsStr(unsigned) const;

    // �������� ������� ��� �����
    AnsiString GetFileName() const { return fn_; }
    AnsiString NormalFileName(const AnsiString& path = "") const;
    // �����������
    LRESULT SaveData(const AnsiString& path = "");

private:

    // ���������� ��������� ���������
    const UpdateViewFooT updateViewFoo_;

    // ������������� ��������������
    const HromatogramId id_;

#ifdef CREATE_HROMATOGRAMM_LOG_FILE_
    std::ofstream log_;
#endif

    // ������� ��������� �����
    unsigned sign_;

    // �����
    std::vector<float> pts_;

    // ����
    HromatPicTable pics_;

    // ���������������� ����������
    HromatPhysChem physChem_;

    // ���� � �����
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
*				������� ��� ������������� ������������
*/
void MakeGrdHtgsList(TStringGrid* grd, const HromatogrammsList& htgs );
void MakeListViewdHtgsList(TListView* lv, const HromatogrammsList& htgs );



void ShowHromatogrammMain( const Hromatogramm *htg, TFrame *frame,
    TStringGrid *grdPhysChem, TStringGrid *grdPics  );

// ���������� ������������� �� ������� ������ TFrameGraph
// ����������� ��� ���������� frame � ������� dynamic_cast
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
