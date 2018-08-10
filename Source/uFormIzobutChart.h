//---------------------------------------------------------------------------

#ifndef uFormIzobutChartH
#define uFormIzobutChartH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Chart.hpp>
#include <ExtCtrls.hpp>
#include <Series.hpp>
#include <TeEngine.hpp>
#include <TeeProcs.hpp>
#include "CGAUGES.h"
//---------------------------------------------------------------------------

class TFrameGraph;


class TFormIzobutChart : public TForm
{
__published:	// IDE-managed Components
    TPanel *Panel1;
    TPanel *Panel2;
    TLabel *Label1;
    TCGauge *CGauge1;
private:	// User declarations
    // окно с графиком и полем выбора слоя
    TFrameGraph *frameGraph_;
    TFastLineSeries *grph_;
    TPointSeries *points;
public:		// User declarations
    __fastcall TFormIzobutChart(TComponent* Owner);
    void AddConc(double);
    TChart *Chart();
    void SetProgress(double percent, double max);
    void AddPoint(TDateTime x, double conc);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormIzobutChart *FormIzobutChart;
//---------------------------------------------------------------------------
#endif
