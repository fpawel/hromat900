//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include <Math.hpp>

#include <stdlib.h>
#include <stdio.h>
#include <time.h>



#include "uFormIzobutChart.h"
#include "uFrameGraph.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "CGAUGES"
#pragma resource "*.dfm"
TFormIzobutChart *FormIzobutChart;
//---------------------------------------------------------------------------
__fastcall TFormIzobutChart::TFormIzobutChart(TComponent* Owner)
    : TForm(Owner),
    frameGraph_( new TFrameGraph(this, this) ),
    grph_( new TFastLineSeries(this) ),
    points( new TPointSeries(this) )
{
    grph_->Clear();
    frameGraph_->AddSeries(grph_);
    frameGraph_->AddSeries(points);
    
    grph_->XValues->DateTime = true;
    points->XValues->DateTime = true;
    const AnsiString fmt = "dd.MM.yyyy hh:nn:ss";

    grph_->Active = false;
    points->Active = false;

    frameGraph_->chrt_->BottomAxis->DateTimeFormat = fmt ;
}
//---------------------------------------------------------------------------

//------------------------------------------------------------------------------
void TFormIzobutChart::AddConc(double conc)
{
    AnsiString s;
    s.sprintf("      C\"изобутилен\" =  %2.2f", conc);
    s = s + ", % масс";
    Panel1->Caption = s;
    if(grph_->YValues->Count>0) {
        grph_->AddXY(Now(), grph_->YValues->Value[grph_->YValues->Count-1]);
    }
    grph_->AddXY(Now(), conc);
    points->AddXY(Now(), conc);

    grph_->Active = true;
    points->Active = true;

}

void TFormIzobutChart::AddPoint(TDateTime x, double conc)
{
    Panel1->Caption = "";
    if(grph_->YValues->Count>0) {
        grph_->AddXY(x, grph_->YValues->Value[grph_->YValues->Count-1]);
    }
    grph_->AddXY(x, conc);
    points->AddXY(x, conc);

    grph_->Active = true;
    points->Active = true;

}
//------------------------------------------------------------------------------
void TFormIzobutChart::SetProgress(double percent, double max)
{
    CGauge1->Progress = percent;
}




