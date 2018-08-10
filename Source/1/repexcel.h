//------------------------------------------------------------------------------
#ifndef repexcelH
#define repexcelH
//------------------------------------------------------------------------------
#include "Hromatogram.hpp"
#include "Hromat900.hpp"
#include <vector>
#include <chart.hpp>
#include <grids.hpp>



void MakeHromatExcelReport(const HromatReportData&);




void MakeHromatExcelReport( const HromatogramReportData& );

AnsiString ExcelColName(int n);




//------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------
