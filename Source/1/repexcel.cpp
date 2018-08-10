//------------------------------------------------------------------------------
#pragma hdrstop
//------------------------------------------------------------------------------
#include "repexcel.h"
//------------------------------------------------------------------------------
#include <ComObj.hpp>
#include <Tlhelp32.h>
#include <Math.hpp>
#include <vector>
#include "MyModFN.hpp"
#include "boost\shared_ptr.hpp"
#include "MyExcpt.hpp"
#include "AnsiStringUtils_.h"
#include "MyProcEntry.h"
#include "MyFileCtrl.h"
#include "MyAppWinPath.hpp"
#include "xlcnstnts.hpp"
#include "HromatGas.hpp"
#include "myconv.h"
#include "uFrmWait.h"
#include "MyAppProfile.h"
#include "htg_.h"
#include "MyModFN.hpp"
#include "htmlreport.h"

#include "SHDocVw_OCX.h"
#include <OleCtrls.hpp>
//------------------------------------------------------------------------------
#pragma package(smart_init)
//------------------------------------------------------------------------------
#define CONC_GOST "ГОСТ 31371.7 ГОСТ 31371.1 ГОСТ 31371.2 (метан по разности) ИБЯЛ.413538.002 ТУ (условия анализа)"


//------------------------------------------------------------------------------
void KillExcel()
{
	ProcEntries snapshot;
    GetWin32SystemSnapshot(snapshot);
    for( unsigned i=0; i<snapshot.size(); ++i )
    if( std::strcmp(snapshot[i].szExeFile,"excel.exe")==0 )
    {
    	HANDLE hProcess = ::OpenProcess( PROCESS_ALL_ACCESS, FALSE, snapshot[i].th32ProcessID );
        DWORD exitCode;
        ::GetExitCodeProcess(hProcess,	&exitCode);
        ::TerminateProcess(hProcess, exitCode);
        ::CloseHandle(hProcess);
    }
}
//------------------------------------------------------------------------------
AnsiString ExcelColName(int n)
{
    n-=1;
    if( n<26 )
        return AnsiString( (char)('A'+n) );
    const int n1 = n/26, n2 = n%26;
    if( n1>26 )
        MY_THROW_( MYSPRINTF_("Слишком большой номер колонки Excel - %d.", n+1) );
    return AnsiString( (char)('A'+n1-1))+AnsiString( (char)('A'+n2));
}
//------------------------------------------------------------------------------
void SetExcelCell(Variant vCells, int col, int row, const AnsiString& s)
{
    vCells.OlePropertyGet("Item", row, col).OlePropertySet("Value", s.c_str() );
}
//------------------------------------------------------------------------------
void SetExcelCell(Variant vCells, int col, int row, TAutoArgsBase& args)
{
    vCells.OlePropertyGet("Item", row, col).OlePropertySet("Value", args );
}
//------------------------------------------------------------------------------
Variant GetExcelRange(Variant vVarSheet, int left, int top, int right, int bottom)
{
    return vVarSheet.OlePropertyGet("Range", MYSPRINTF_("%s%d:%s%d",
        ExcelColName(left), top, ExcelColName(right), bottom  ).c_str() );
}
//------------------------------------------------------------------------------
void SetLineBorder1(Variant vBorder)
{
    using namespace Excel;
    vBorder.OlePropertySet("LineStyle", xlContinuous);
    vBorder.OlePropertySet("Weight", xlThin);
    //vBorder.OlePropertySet("ColorIndex", xlColorIndexAutomatic);
}
//------------------------------------------------------------------------------
void SetRangeAsSimpleGrid(Variant vRange)
{
    using namespace Excel;
    SetLineBorder1( vRange.OlePropertyGet("Borders", xlInsideHorizontal) );
    SetLineBorder1( vRange.OlePropertyGet("Borders", xlInsideVertical) );

    SetLineBorder1( vRange.OlePropertyGet("Borders", xlEdgeBottom) );
    SetLineBorder1( vRange.OlePropertyGet("Borders", xlEdgeLeft) );
    SetLineBorder1( vRange.OlePropertyGet("Borders", xlEdgeRight) );
    SetLineBorder1( vRange.OlePropertyGet("Borders", xlEdgeTop) );
}
//------------------------------------------------------------------------------
void SetRangeTextCenterAligment(Variant vRange)
{
    using namespace Excel;
    vRange.OlePropertySet("HorizontalAlignment", xlHAlignCenter );
    vRange.OlePropertySet("VerticalAlignment", xlVAlignCenter);
}
//------------------------------------------------------------------------------
void SetExcelRange1(Variant vRange, const AnsiString& s)
{
    SetRangeTextCenterAligment(vRange);
    vRange.OleProcedure("Merge");
    vRange.OlePropertySet("WrapText", true);
    //vRange.OlePropertyGet("Item", 1, 1 ).OlePropertySet("Value", s.c_str() );
    vRange.OlePropertySet("Value", s.c_str() );
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void MakeHromatExcelReport(const HromatReportData& reportData)
{
    const AnsiString sDir = MyGetExePath()+"\\Отчёты";
	ForceDirectories(sDir);
    const AnsiString fileName = FindReportFileName( ".xls" );
    MyExtractFileFromResource( fileName, "EXCEL_REPORT_TEMPLATE_FILE", false );
    Variant
        vExcel = CreateOleObject("Excel.Application");
    vExcel.OlePropertySet("Caption", "Отчёт Хромат-900");
    vExcel.OlePropertySet("SheetsInNewWorkbook",1);
    Variant
        vBooks=vExcel.OlePropertyGet("Workbooks");
    vBooks.OleProcedure("Open", fileName.c_str());


    Variant
        vBook=vBooks.OlePropertyGet("Item",1),
        vSheets=vBook.OlePropertyGet("Worksheets"),
        vSheet=vSheets.OlePropertyGet("Item",1),
        vCells = vSheet.OlePropertyGet("Cells");
    vSheet.OlePropertySet("Name","Отчёт Хромат-900");

    //SetExcelCell( vCells, 3, 5, FormatDateTime("dd mmmm yyyy hh:nn", Now()) );
    SetExcelCell( vCells, 1, 5, reportData.title );

    int col0=4, row = 9;
    for( int i=0; i<reportData.physChem.size(); ++i, ++row )
    {
        SetExcelCell( vCells, col0+0, row, reportData.physChem[i] );
        SetExcelCell( vCells, col0+1, row, reportData.dPhysChem[i] );
    }
    ++row;
    const int rowConc0 = row;
    for( int i=0; i<reportData.gasMain.size(); ++i, ++row )
    {
        SetExcelCell( vCells,      1, row, reportData.gasMain[i] );
        SetExcelCell( vCells, col0+0, row, reportData.concMain[i] );
        SetExcelCell( vCells, col0+1, row, reportData.dConcMain[i] );
        SetExcelCell( vCells, col0-1, row, reportData.orderConcMain[i] );
    }

    const int rowConc1 = row++;
    for( int i=0; i<reportData.gasOther.size(); ++i, ++row )
    {
        SetExcelCell( vCells,      1, row, reportData.gasOther[i] );
        SetExcelCell( vCells, col0+0, row, reportData.concOther[i] );
        SetExcelCell( vCells, col0+1, row, reportData.dConcOther[i] );
        SetExcelCell( vCells, col0-1, row, reportData.orderConcOther[i] );
    }

    Variant
    vRange = GetExcelRange(vSheet, 1, rowConc0, 6, row-1);
    SetRangeAsSimpleGrid(vRange);
    SetRangeTextCenterAligment(vRange);
    
    GetExcelRange(vSheet, 1, rowConc0, 1, row-1).OlePropertySet("HorizontalAlignment", Excel::xlHAlignLeft );

    vRange = GetExcelRange(vSheet, 1, rowConc1, 5, rowConc1);
    SetExcelRange1(vRange, "Содержание неизмеряемых компонентов");
    vRange.OlePropertyGet("Font").OlePropertySet("Bold",true);

    vRange = GetExcelRange(vSheet, 2, rowConc0, 2, rowConc1-1);
    SetExcelRange1(vRange, "мол. доля, %");

    vRange = GetExcelRange(vSheet, 2, rowConc1+1, 2, row-1);
    SetExcelRange1(vRange, "мол. доля, %");

    vRange = GetExcelRange(vSheet, 6, rowConc0-1, 6, row-1);
    SetExcelRange1(vRange, CONC_GOST);   
    
    SetExcelCell( vCells, 1, row+1, "Подпись ответственного лица" );
    SetExcelCell( vCells, 6, row+1, FormatDateTime("dd MMMM yyyy", Now() ) );

    //Отключить вывод сообщений с вопросами типа "Заменить файл..."
    vExcel.OlePropertySet("DisplayAlerts",false);
    //Сохранить книгу
    vBook.OleProcedure("Save");
    vExcel.OlePropertySet("Visible", true);
}
//------------------------------------------------------------------------------
void Grid2Excel( TStringGrid* grd, Variant vSheet, const int col0, const int row0 )
{
    const int rc = grd->RowCount, cc = grd->ColCount;
    int theCol = 0;
    Variant
        vCells = vSheet.OlePropertyGet("Cells");
    for( int cl=0; cl<cc; ++cl )
    {
        if( grd->ColWidths[cl]==0 ) continue;
        for( int ro=0; ro<rc; ++ro )
        {
            //vCells.OlePropertyGet("Item", col0+theCol, row0+ro ).OlePropertySet("NumberFormat", "@" );
            SetExcelCell( vCells, col0+theCol, row0+ro, grd->Cells[cl][ro] );
        }
        theCol++;
    }
    if(theCol>1 && rc>1)
    {
        Variant vRange = GetExcelRange(vSheet, col0, row0, col0 + theCol-1,
            row0 + rc-1);
        SetRangeAsSimpleGrid( vRange );
        vRange.OlePropertyGet("Columns").OleProcedure( "AutoFit" );
        SetRangeTextCenterAligment(vRange);
    }
}
//------------------------------------------------------------------------------
void InsertPicture( Variant vSheet, const AnsiString& pictFileName, int col, int row,
    unsigned &pictureRowCount )
{
    const double
        defaultRowHeight = vSheet.OlePropertyGet("Cells").OlePropertyGet("Item", col, row).OlePropertyGet("Height" );
    Variant vRangeGraph1 = GetExcelRange(vSheet, col, row, col, row );
    vSheet.OlePropertyGet("Shapes").OleFunction("AddPicture",
        pictFileName.c_str(), false, true, vRangeGraph1.OlePropertyGet("Left"),
        vRangeGraph1.OlePropertyGet("Top"), -1, -1);
    Variant
        vPict = vSheet.OlePropertyGet("Pictures","Picture 1");
    const double
        pictureHeight = vPict.OlePropertyGet("ShapeRange").OlePropertyGet("Height");
    pictureRowCount = Ceil(pictureHeight/defaultRowHeight) + 1;
}
//------------------------------------------------------------------------------
void MakeHromatExcelReport( const HromatogramReportData& reportData )
{
    const AnsiString
        fileName = FindReportFileName( ".xls" ),
        sDir = ExtractFilePath(fileName),
        pictFileName = sDir + "chart.mft";

    
	ForceDirectories(sDir);

    Variant
        vExcel = CreateOleObject("Excel.Application");
    vExcel.OlePropertySet("Caption", reportData.caption.c_str() );
    vExcel.OlePropertySet("SheetsInNewWorkbook",1);
    Variant
        vBooks=vExcel.OlePropertyGet("Workbooks");
    vBooks.OleProcedure("Add");
    Variant
        vBook=vBooks.OlePropertyGet("Item",1),
        vSheets=vBook.OlePropertyGet("Worksheets"),
        vSheet=vSheets.OlePropertyGet("Item",1),
        vCells = vSheet.OlePropertyGet("Cells"),
        vFont = vCells.OlePropertyGet("Font");;
    vCells.OlePropertySet("NumberFormat", "@" );
    vSheet.OlePropertySet("Name", reportData.caption.c_str() );

    vFont.OlePropertySet("Name", "Arial" );
    vFont.OlePropertySet("Size", "11" );


    SetExcelCell( vCells, 1, 1, reportData.title );
    int col = 1, row = 3;

    SetExcelCell( vCells, 1, row++, "Физико-химические показатели" );
    Grid2Excel( reportData.grdPhysChem, vSheet, col, row );
    row+=reportData.grdPhysChem->RowCount+1;

    SetExcelCell( vCells, col, row++, "Таблица пиков детектора 1" );
    Grid2Excel( reportData.grd1, vSheet, col, row );
    row+=reportData.grd1->RowCount+1;

    unsigned pictureRowCount;

    SetExcelCell( vCells, 1, row++, "График хроматограммы детектора 1" );
    reportData.chart1->SaveToMetafileEnh( pictFileName );
    try {
        InsertPicture( vSheet, pictFileName, col, row, pictureRowCount );
    } catch (...) {}
    row += pictureRowCount;

    SetExcelCell( vCells, 1, row++, "Таблица пиков детектора 2" );
    Grid2Excel( reportData.grd2, vSheet, col, row  );
    row+=reportData.grd2->RowCount+1;

    SetExcelCell( vCells, 1, row++, "График хроматограммы детектора 2" );
    reportData.chart2->SaveToMetafileEnh( pictFileName );
    try {
    InsertPicture( vSheet, pictFileName, col, row, pictureRowCount );
    } catch (...) {}
    row += pictureRowCount;

    ::DeleteFile( pictFileName.c_str() );
    //Отключить вывод сообщений с вопросами типа "Заменить файл..."
    vExcel.OlePropertySet("DisplayAlerts",false);
    //Сохранить книгу
    vBook.OleProcedure("SaveAs",fileName.c_str());

    vSheet.OleProcedure("PrintOut"); 

    vExcel.OlePropertySet("Visible", true);
}
//------------------------------------------------------------------------------
