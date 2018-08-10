//------------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
//------------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
//------------------------------------------------------------------------------
#include "uFormAnalyze.h"
#include "SGrdHlpr.h"
#include "uFrmWait.h"
#include "uDrawGrids.h"
#include "HromatPhysChem.h"
#include "HromatGas.hpp"
#include "MyVclUtils_.h"
#include "hromatogram.hpp"
//------------------------------------------------------------------------------
void SeGrdConcTttls(TStringGrid* grd )
	{
		const unsigned cnt = Gas::Count();
    	grd->RowCount = cnt+1;
    	grd->ColCount = 3;

    	grd->Cells[0][0] = "Газ";
    	grd->Cells[1][0] = "Содержание";
    	grd->Cells[2][0] = " ± ";
        const Gas::Item* gases = Gas::GetSortedItems();
    	for( unsigned i=0; i<cnt; ++i )
    		grd->Cells[0][i+1] = gases[i].name;
    	grd->Height = grd->DefaultRowHeight*(grd->RowCount+1);
	}
//------------------------------------------------------------------------------
TFormAnalyze *FormAnalyze;
//------------------------------------------------------------------------------

__fastcall TFormAnalyze::TFormAnalyze(TComponent* Owner)
    : TForm(Owner), drwGrdConc_(grdConc)
{
    SeGrdConcTttls( grdConc );
    MakeGrdPhysChem( grdPhyChe );
    const DrawMyGrid& dg = DrawMyGrid::Instance();
    drwGrdConc_.SetAfterSetCanvasFoo( &dg.AfterSetCanvasFormArchiveViewGrdPhysChem );
    my::Grd::NormalizeGridHeight( grdPhyChe );
    my::Grd::NormalizeGridHeight( grdConc );
    ClientHeight = std::max(grdPhyChe->Height, grdConc->Height) + Panel1->Height + 10;
}
//------------------------------------------------------------------------------
__fastcall TFormAnalyze::~TFormAnalyze()
{
    UnregisterAllLabelsAsButton( this );
}
//------------------------------------------------------------------------------
void __fastcall TFormAnalyze::Label6Click(TObject *Sender)
{
    boost::shared_ptr<TFrmWait> frmWait( new TFrmWait(NULL) );
    frmWait->Panel2->Caption = "Подготовка отчёта...";
    frmWait->Show();
    frmWait->Repaint();
    HromatReportData rprtDt;
    const unsigned
        gasCnt = Gas::Count(), mainGasCount = Gas::GrauatorsCount()+1,
        otherGasCount = gasCnt-mainGasCount;

    rprtDt.gasMain.resize(mainGasCount);
    rprtDt.concMain.resize(mainGasCount);
    rprtDt.dConcMain.resize(mainGasCount);
    rprtDt.orderConcMain.resize(mainGasCount);

    rprtDt.gasOther.resize(otherGasCount);
    rprtDt.concOther.resize(otherGasCount);
    rprtDt.dConcOther.resize(otherGasCount);
    rprtDt.orderConcOther.resize(otherGasCount);

    rprtDt.physChem.resize(PHY_CHEM::COUNT);
    rprtDt.dPhysChem.resize(PHY_CHEM::COUNT);

    const Gas::Item *gases = Gas::GetItems();

    for( unsigned i=0; i<PHY_CHEM::COUNT; ++i )
    {
        rprtDt.physChem[i] = grdPhyChe->Cells[2][i+1];
        rprtDt.dPhysChem[i] = grdPhyChe->Cells[3][i+1];
    }
    for( unsigned i=0; i<mainGasCount; ++i )
	{
        const AnsiString sGas = grdConc->Cells[0][i+1];
        const int idx = Gas::GetIdx(sGas);
        const AnsiString sGas1 = idx==-1 ? sGas : gases[idx].Format1();

        rprtDt.gasMain[i] = sGas1;
        rprtDt.concMain[i] = grdConc->Cells[1][i+1];
        rprtDt.dConcMain[i] = grdConc->Cells[2][i+1];

        if(idx>-1)
            rprtDt.orderConcMain[i] = gases[idx].FormatOrders();

    }
    for( unsigned i=0; i<otherGasCount; ++i )
	{
        const AnsiString sGas = grdConc->Cells[0][i+1+mainGasCount];
        const int idx = Gas::GetIdx(sGas);
        const AnsiString sGas1 = idx==-1 ? sGas : gases[idx].Format1();
        rprtDt.gasOther[i] = sGas1;
        rprtDt.concOther[i] = grdConc->Cells[1][i+1+mainGasCount];
        rprtDt.dConcOther[i] = grdConc->Cells[2][i+1+mainGasCount];
        if(idx>-1)
            rprtDt.orderConcOther[i] = gases[idx].FormatOrders();
    }
    rprtDt.title = Label2->Caption;
}
//---------------------------------------------------------------------------

