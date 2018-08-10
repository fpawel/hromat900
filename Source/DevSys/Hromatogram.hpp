//------------------------------------------------------------------------------
#ifndef MY_HROMAT_HROMATOGRAM_HEADER_INCLUDED_______
#define MY_HROMAT_HROMATOGRAM_HEADER_INCLUDED_______
//------------------------------------------------------------------------------
#define VCL_IOSTREAM
//------------------------------------------------------------------------------
#include <iostream>
#include <vector>
//------------------------------------------------------------------------------
#include <system.hpp>
#include <Chart.hpp>
#include <grids.hpp>
//------------------------------------------------------------------------------
#include "boost\shared_ptr.hpp"


/**
*       ������������� �������������
*/
class Hromatogramm;
typedef boost::shared_ptr<Hromatogramm> HromatogrammPtr;
typedef boost::shared_ptr<const Hromatogramm> HromatogrammConstPtr;
typedef std::vector<HromatogrammPtr> HromatogrammsList;
/**
*       ���������� ������������� �������������
*/
typedef struct
{
    TDateTime date_;                // ���� �������� �������������
    unsigned fileIndex_;            // ������ �����  �������������
    bool dtctr_;                    // ��������� ��������
    unsigned char rgmChanAndType_;  // ��� ����� (��������������, �������������)
    unsigned short int version_;
} HromatogramId;

AnsiString DefaultHtgArchivePath();

AnsiString HtgArchivePath();

typedef struct HromatogramReportDataTag
{
    AnsiString title, caption;
    TChart *chart1, *chart2;
    TStringGrid *grd1, *grd2, *grdPhysChem;
    TDateTime dt; 
} HromatogramReportData;

typedef struct HromatReportDataTag
{
    std::vector<AnsiString>
        physChem, dPhysChem,
        gasMain, gasOther,
        concMain, dConcMain, orderConcMain,
        concOther, dConcOther, orderConcOther;
    AnsiString title;
} HromatReportData;

//------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------
