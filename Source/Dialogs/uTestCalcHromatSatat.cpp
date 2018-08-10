//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "uTestCalcHromatSatat.h"
#include "HromatPhysChem.h"

// my


#include "uDrawGrids.h"
#include "SGrdHlpr.h"
#include "hrmtstat.h"
#include "kostik_fround.h"
#include "AnsiStringUtils_.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormTestCalcStatDlg *FormTestCalcStatDlg;
//---------------------------------------------------------------------------
__fastcall TFormTestCalcStatDlg::TFormTestCalcStatDlg(TComponent* Owner)
    : TForm(Owner), drwGrd1_(grd1)
{
    const DrawMyGrid& dg = DrawMyGrid::Instance();
    MakeGrdPhysChem(grdPhysChem);
    grdPhysChem->ColCount = 3;
    grdPhysChem->Cells[2][0] = " ± ";

    my::DrawGridCellManager& dgcm = my::DrawGridCellManager::GetDrawCellManager(grdPhysChem);
    dgcm.SetAfterSetCanvasFoo( &dg.AfterSetBasePikCellCanvas );

    drwGrd1_.SetAfterSetCanvasFoo( &dg.AfterSetBasePikCellCanvas );

    grd1->Cells[0][0] = "Параметр";
    grd1->Cells[1][0] = "Значение";
    grd1->Cells[2][0] = " ± ";


    const unsigned gasCnt = Gas::Count();
    grd1->RowCount = gasCnt+1;

    for( unsigned i=0; i<gasCnt; ++i )
    {
        grd1->Cells[0][i+1] = Gas::Formula(i);
    }
    grdPhysChem->Height =  grd1->Height;

    my::Grd::AddCopyItemToPopupMenu( grd1 );
    my::Grd::AddCopyItemToPopupMenu( grdPhysChem );

    grd1->FixedCols = 0;
    grdPhysChem->FixedCols = 0;
}
//---------------------------------------------------------------------------
void __fastcall TFormTestCalcStatDlg::Button2Click(TObject *Sender)
{
    grd1->Col=1;
    grd1->Row=1;
    my::Grd::PastCellsFromClipboard(grd1);
}
//---------------------------------------------------------------------------
void __fastcall TFormTestCalcStatDlg::Button1Click(TObject *Sender)
{
    HromatStatistic stat;

    const unsigned gasCnt = Gas::Count();

    for( unsigned i=0; i<gasCnt; ++i )
        stat.conc[i] = StrToFloat( grd1->Cells[1][i+1] );

    CalculateHromatStatistic( stat );

    if( CheckBox1->Checked )
                OkruglitHromatStatistic( stat );

    for( unsigned i=0; i<gasCnt; ++i )
    {
        const double val = stat.conc[i], dVal = stat.dConc[i];
        int n;
        KostikGOSTRound( dVal, &n );
        grd1->Cells[1][1+i] = FloatToStr( val );
        grd1->Cells[2][1+i] = FloatToStr( dVal );
    }
    
    grdPhysChem->Cells[1][1] = FloatToStr( stat.relTghtns);
    grdPhysChem->Cells[2][1] = FloatToStr( stat.dRelTghtns );

    grdPhysChem->Cells[1][2] = FloatToStr( stat.absTghtns );
    grdPhysChem->Cells[2][2] = FloatToStr( stat.dAbsTghtns );

    grdPhysChem->Cells[1][3] = FloatToStr( stat.Hmhigh );
    grdPhysChem->Cells[2][3] = FloatToStr( stat.dHmhigh );

    grdPhysChem->Cells[1][4] = FloatToStr( stat.Hmlow );
    grdPhysChem->Cells[2][4] = FloatToStr( stat.dHmlow );

    grdPhysChem->Cells[1][5] = FloatToStr( stat.Hvhigh );
    grdPhysChem->Cells[2][5] = FloatToStr( stat.dHvhigh );

    grdPhysChem->Cells[1][6] = FloatToStr( stat.Hvlow );
    grdPhysChem->Cells[2][6] = FloatToStr( stat.dHvlow );

    grdPhysChem->Cells[1][7] = FloatToStr( stat.Whigh );
    grdPhysChem->Cells[2][7] = FloatToStr( stat.dWhigh );

    grdPhysChem->Cells[1][8] = FloatToStr( stat.Wlow );
    grdPhysChem->Cells[2][8] = FloatToStr( stat.dWlow );
}
//---------------------------------------------------------------------------
