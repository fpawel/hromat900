//------------------------------------------------------------------------------
#ifndef HromatHtmlReportH
#define HromatHtmlReportH
//------------------------------------------------------------------------------
#include "Hromatogram.hpp"
#include "MshrmtArc.hpp"
#include "Hromat900.hpp"
#include <vector>
#include <chart.hpp>
#include <grids.hpp>


AnsiString MakeHromatHTMLReport( const HromatogramReportData& reportData,
    bool incGraph1, bool incGraph2);

AnsiString Report_Jurnal_Analize(const Jurnal_Analize& d);

AnsiString Report_Jurnal_AnalizeArchive(Jurnal_AnalizeArchive jurnalAnalizeArchive_);




//------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------
