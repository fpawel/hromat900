//---------------------------------------------------------------------------
#include <vcl.h>
//std
#include <math>
#include <fstream>
#pragma hdrstop

#include "uFrameGraph.h"
#include "uChartHelper.h"
#include "AnsiStringUtils_.h"
#include "MyVclUtils_.h"

#include <Math.hpp>
#include <DateUtils.hpp>
#include "uFrameDropDownPanel.h"
#include "MyModFN.hpp"
#include "MyFileCtrl.h"
#include "IniFileMacro.h"
#include "MyAppWinPath.hpp"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

DEFINE_NONDELETEABLE_INIFILE_EX_( Graph, "graph.ini" );

void SetCheckedBox(TCheckBox* cb, bool checked)
{
	TNotifyEvent onClick = cb->OnClick;
    cb->OnClick = NULL;
    cb->Checked = checked;
    cb->OnClick = onClick;
}


void InitTrackBar(TTrackBar* tb)
{
	tb->Max = 1000000;
	tb->Min = 0;
    tb->Position = 0;
}

void SetTrackBarPos(TTrackBar* tb, int position)
{
    TFrame* frame = dynamic_cast<TFrame*>(tb->Owner);
    assert(frame);
    TForm* form =  dynamic_cast<TForm*>(frame->Owner);
    assert(frame);
	if( form->ActiveControl==tb ) return;
	TNotifyEvent onClick = tb->OnChange;
    tb->OnChange = NULL;
   	tb->Position = position>=tb->Max-10 ? tb->Max-10 : position;
    tb->OnChange = onClick;
}

void SetTrackBarPosAsVal(TTrackBar* tb, double len, double val)
{
	//const double trackVal = ( (ordrs.maxX-ordrs.minX)*tbXMin->Position)/tbXMin->Max + ordrs.minX;
	const int tbLen = tb->Max - tb->Min, offSet = Ceil( tbLen * 0.1 ),
    posMin = tb->Min + offSet, posMax = tb->Max - offSet;
    SetTrackBarPos(tb, posMin + Ceil( (posMax-posMin)*( val )/len ) );
}
//------------------------------------------------------------------------------
double GetTrackBarPosAsPercent(TTrackBar* tb)
{
	//const double trackVal = ( (ordrs.maxX-ordrs.minX)*tbXMin->Position)/tbXMin->Max + ordrs.minX;
	const int tbLen = tb->Max - tb->Min, offSet = Ceil( tbLen * 0.1 ),
    posMin = tb->Min + offSet, posMax = tb->Max - offSet,
    dPos = tb->Position - posMin, lenPos = posMax - posMin;
    return dPos*1.0/(lenPos*1.0);
}
//------------------------------------------------------------------------------
void SetTEditText( TEdit* ed, const AnsiString& s )
{
    TNotifyEvent onClick = ed->OnChange;
    ed->OnChange = NULL;
    const int selStr = ed->SelStart, selLen = ed->SelLength;
   	ed->Text = s;
    ed->SelStart = selStr;
    ed->SelLength = selLen;
    ed->OnChange = onClick;
}
//------------------------------------------------------------------------------
void MakeUpDownCircled(TObject* sndr)
{
	TUpDown* ctrl =  dynamic_cast<TUpDown*>(sndr);
    if( ctrl->Position == ctrl->Min )
    	ctrl->Position = ctrl->Max;
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
__fastcall TFrameGraph::TFrameGraph(TComponent* Owner, TWinControl* parent )
	: TFrame(Owner),
    form_( static_cast<TForm*>(Owner) ),
    drawCross_ ( new CrossHairHelper(chrt_) ),
    xAx_(chrt_->BottomAxis), yAx_(chrt_->LeftAxis),
    OnSetActiveSeries(NULL),
    OnAddSeries(NULL),
    OnRemoveSeries(NULL),
    OnBeforClearSeriesList(NULL)
{
	static unsigned count = 0;
    Name = "FrameGraph"+IntToStr(count++);

    Parent = parent;
    Align = alClient;

	drawCross_->OnMouseOcurence = OnChartMouseOcurence;

    MakeLabelAsButton( btnBrushColor );
    MakeLabelAsButton( Label4 );
    MakeLabelAsButton( Label1 );
    MakeLabelAsButton( Label7 );

    //TFrameDropDownPanel*
    ScrollBox1->Visible = 0;
    
    pnX = new TFrameDropDownPanel(this, ScrollBox1, "Ось X (время)");
    pnX->AddAllClienPanel(grpBxX );
    pnY = new TFrameDropDownPanel(this, ScrollBox1, "Ось Y");
    pnY->AddAllClienPanel(grpBxY );

    pnGraphTools = new TFrameDropDownPanel(this, ScrollBox1, "Инструменты");
    pnGraphTools->AddAllClienPanel(panelGraphTools);

    pnGraphTools = new TFrameDropDownPanel(this, ScrollBox1, "Вид");
    pnGraphTools->AddAllClienPanel(panelView);

    pnX->Expand();
    pnX->Colapse();
    ScrollBox1->Visible = 1;
    
    const AnsiString colorsFN = MyGetExePath() + "graph.colors";
    if( MyFileExists(colorsFN) )
    	ColorDialog1->CustomColors->LoadFromFile( colorsFN );

    TIniFile *ini = IniGraph();
    chrt_->BackColor = ini->ReadInteger( "colors", "фон", chrt_->BackColor );
    chrt_->Color = ini->ReadInteger( "colors", "шкала", chrt_->Color );

    ::DragAcceptFiles( Handle, TRUE );
}
//---------------------------------------------------------------------------
__fastcall TFrameGraph::~TFrameGraph()
{
	ColorDialog1->CustomColors->SaveToFile( MyGetExePath() + "graph.colors" );
    Clear();

}
//---------------------------------------------------------------------------
void TFrameGraph::Clear()
{
    const MyChartSeriesList lst = StoreChartSeriesList( chrt_ );
    TIniFile *ini = IniGraph();
    ini->WriteInteger( "colors", "шкала", chrt_->Color );
    for( int idx=0; idx<lst.size(); ++idx )
    {
    	TChartSeries *ser = lst[idx];
        ini->WriteInteger( "colors", idx, ser->SeriesColor );
        RemoveSeries(ser);
    }
}
//---------------------------------------------------------------------------
void TFrameGraph::SetFile( const AnsiString &fn )
{
   fileName_ = fn;
   chrt_->Title->Visible = !fn.IsEmpty();
   chrt_->Title->Text->Text = "Файл \""+fn+"\"";
}
//---------------------------------------------------------------------------
void TFrameGraph::OpenFile(const AnsiString& fn)
{
	if( OnBeforClearSeriesList )
    	OnBeforClearSeriesList();

	for( int idx=0; idx<chrt_->SeriesCount(); ++idx )
    {
    	TChartSeries *ser = chrt_->Series[idx];
        if( OnRemoveSeries ) OnRemoveSeries(ser);
    }

	if( MyLoadTChartFromFile( fn, chrt_ ) )
    	SetFile( fn );
}
//---------------------------------------------------------------------------
TChartSeries* TFrameGraph::ActiveGraph()
{
    const int
    	cnt = SeriesCount();
    if(cnt==0) return NULL;
    return static_cast<TChartSeries*>( chrt_->Series[0] );
}
//---------------------------------------------------------------------------
TChartSeries* TFrameGraph::AddSeries(TChartSeries *ser)
{
	if( ser==NULL )ser =  new TFastLineSeries(this);
    ser->AfterDrawValues = grph_AfterDrawValues;

    ser->XValues->DateTime = true;
    ser->ParentChart = chrt_;
    const int idx = chrt_->SeriesCount()-1;

    TIniFile *ini = IniGraph();
    ser->SeriesColor = ini->ReadInteger( "colors", idx, ser->SeriesColor );
    if( OnAddSeries ) OnAddSeries(ser);
    return ser;
}
//---------------------------------------------------------------------------
void TFrameGraph::RemoveSeries(unsigned idx)
{
	assert( idx<(unsigned)chrt_->SeriesCount() );
    TChartSeries* ser = chrt_->Series[idx];
    if( OnRemoveSeries ) OnRemoveSeries(ser);
    ser->ParentChart = NULL;
}
void TFrameGraph::RemoveSeries(TChartSeries *ser)
{
	const int sz = chrt_->SeriesCount();
    for( int idx=0; idx<sz; ++idx )
    	if( chrt_->Series[idx]==ser )
        {
        	RemoveSeries(idx);
            return;
        }
}
//---------------------------------------------------------------------------
int TFrameGraph::SeriesCount() const
{
    return chrt_->SeriesCount();
}
//---------------------------------------------------------------------------
void TFrameGraph::SetActiveSeries(int n)
{
    const int sz = chrt_->SeriesCount();
    n = (n>-1 && n<sz) ? n : -1;
    TChartSeries *ser = (n>-1 && n<sz) ? chrt_->Series[n] : NULL;
    for( int i=0; i<sz; ++i ) chrt_->Series[i]->Active = (i==n);
    if( OnSetActiveSeries ) OnSetActiveSeries(ser, n);
}
//---------------------------------------------------------------------------
void TFrameGraph::SetActiveSeries(TChartSeries* ser)
{
	const int sz = chrt_->SeriesCount();
    //int n = -1;
    for( int i=0; i<sz; ++i )
    {
    	chrt_->Series[i]->Active = (chrt_->Series[i]==ser);
        if( chrt_->Series[i]->Active && OnSetActiveSeries )
        	OnSetActiveSeries(ser, i);
    }
}
//---------------------------------------------------------------------------
int TFrameGraph::GetActiveSeries()
{
    return chrt_->SeriesList->IndexOf( ActiveGraph() );
}
//---------------------------------------------------------------------------
bool TFrameGraph::IsValidSeries(TChartSeries* ser)
{
	const int sz = chrt_->SeriesCount();
    for( int i=0; i<sz; ++i )
        if( chrt_->Series[i]==ser )
        	return true;
    return false;

}
//---------------------------------------------------------------------------
void TFrameGraph::UndoZoom()
{
	cbAutoLimitY->Checked = 0;
	chrt_->UndoZoom();
    xAx_->Automatic = 1;
    yAx_->Automatic = 1;
}
//---------------------------------------------------------------------------
TChartSeries* TFrameGraph::GetSeries(unsigned n) const
{
	return chrt_->Series[n];
}
//---------------------------------------------------------------------------
void TFrameGraph::OnChartMouseOcurence( bool isEntered )
{
	pnY->Label5->Caption = "Ось Y";
    pnX->Label5->Caption = "Ось X (время)";
}
//---------------------------------------------------------------------------
void TFrameGraph::OnMouseWheel( TWinControl* ctrl, bool isUp )
{
	if( ctrl==edXMin ) IncAxOrder( xAx_, ax_Min, isUp ); else
    if( ctrl==edXMax ) IncAxOrder( xAx_, ax_Max, isUp ); else
    if( ctrl==edYMin ) IncAxOrder( yAx_, ax_Min, isUp ); else
    if( ctrl==edYMax ) IncAxOrder( yAx_, ax_Max, isUp );
}
//---------------------------------------------------------------------------
void TFrameGraph::SetAutomaticYOrders10Precent()
{
	TChartSeries* ser = ActiveGraph();
    if( ser==0 || !cbAutoLimitY->Checked ) return;
    const std::pair<double,double> yLims = GetYLimitsOfSeries(ser);
    // границы по оси Y
    double YAxisMin = chrt_->LeftAxis->Minimum, YAxisMax = chrt_->LeftAxis->Maximum;
    yAx_->CalcMinMax(YAxisMin, YAxisMax);
    const double yLimOffset = (yLims.second - yLims.first)*0.05,
    	yMin = yLims.first - yLimOffset, yMax = yLims.second + yLimOffset;
    	if( ( std::fabs(YAxisMin-yMin) >yLimOffset || fabs(YAxisMax-yMax) >yLimOffset )&&
        	(yMin-yLimOffset<yMax+yLimOffset) )
        	yAx_->SetMinMax(yMin-yLimOffset, yMax+yLimOffset);
}
//---------------------------------------------------------------------------
void TFrameGraph::ShowChartNfo()
{

    // границы по оси Y X
    yMin_=yAx_->Minimum;
    yMax_=yAx_->Maximum;
    xMin_=xAx_->Minimum;
    xMax_=xAx_->Maximum;

   	SetTEditText( edXMin, this->TimeToStr(xMin_ ) );
    SetTEditText( edXMax, this->TimeToStr(xMax_ ) );


    SetTEditText( edYMin, FormatFloat( yAx_->AxisValuesFormat, yMin_ ) );
    SetTEditText( edYMax, FormatFloat( yAx_->AxisValuesFormat, yMax_ ) );

    const double
    	absScale = std::fabs(yMax_-yMin_),
        vScale = yMax_==0 ? 0 : std::fabs(absScale*100.0/yMax_);
    const AnsiString strVScale = yMax_==0 || absScale==0 ? AnsiString(" ") : FormatFloat( "#0.#", vScale)+"%";
    
    cbAutoXMin->Checked = xAx_->AutomaticMinimum;
    cbAutoXMax->Checked = xAx_->AutomaticMaximum;
    cbAutoYMin->Checked = yAx_->AutomaticMinimum;
    cbAutoYMax->Checked = yAx_->AutomaticMaximum;
    
    cbNoZoom->OnClick = NULL;
    cbNoZoom->Checked = cbAutoXMin->Checked && cbAutoXMax->Checked && cbAutoYMin->Checked && cbAutoYMax->Checked;
    cbNoZoom->OnClick = cbNoZoomClick;
}
//---------------------------------------------------------------------------
void TFrameGraph::UpdateCurrentGraphInfo()
{
	SetAutomaticYOrders10Precent();
    ShowChartNfo();
}
//---------------------------------------------------------------------------
void TFrameGraph::SetXMarks(const VDouble& marks, const VDouble &x0,
	const VDouble &x1, const std::vector<AnsiString>& names)
{
    xMarks_ = marks;
    x0Marks_ = x0;
    x1Marks_ = x1;
    names_ = names;

    for( int i=0; i<chrt_->SeriesCount(); ++i )
        chrt_->Series[i]->Repaint();
}
//---------------------------------------------------------------------------
void __fastcall TFrameGraph::cbAutoXMinClick(TObject *Sender)
{
	xAx_->AutomaticMinimum = cbAutoXMin->Checked;
}
//---------------------------------------------------------------------------
void __fastcall TFrameGraph::cbAutoXMaxClick(TObject *Sender)
{
	xAx_->AutomaticMaximum = cbAutoXMax->Checked;
}
//---------------------------------------------------------------------------
void __fastcall TFrameGraph::cbAutoYMinClick(TObject *Sender)
{
	yAx_->AutomaticMinimum = cbAutoYMin->Checked;
}
//---------------------------------------------------------------------------
void __fastcall TFrameGraph::cbAutoYMaxClick(TObject *Sender)
{
	yAx_->AutomaticMaximum = cbAutoYMax->Checked;
}
//---------------------------------------------------------------------------
void __fastcall TFrameGraph::edXMinChange(TObject *Sender)
{
    TDateTime tm = 0;
    if( TryStrToTime(edXMin->Text,tm)  )
        SetAxisMin( xAx_, tm.Val );
}
//---------------------------------------------------------------------------

void __fastcall TFrameGraph::edXMaxChange(TObject *Sender)
{
	TDateTime tm = 0;
    if( TryStrToTime(edXMax->Text,tm) )
        SetAxisMax( xAx_, tm.Val );
}
//---------------------------------------------------------------------------
void __fastcall TFrameGraph::edYMinChange(TObject *Sender)
{
    double val;
    if( !MyTryStrToFloat( edYMin->Text, &val)  ) return;
    SetAxisMin( yAx_, val );
    cbAutoLimitY->Checked  = false;
}
//---------------------------------------------------------------------------

void __fastcall TFrameGraph::edYMaxChange(TObject *Sender)
{
    double val;
    if( !MyTryStrToFloat( edYMax->Text, &val)  ) return;
    SetAxisMax( yAx_, val );
    cbAutoLimitY->Checked  = false;
}
//---------------------------------------------------------------------------
void __fastcall TFrameGraph::chrt_MouseMove(TObject *Sender,
      TShiftState Shift, int X, int Y)
{
    TChartSeries* ser = ActiveGraph();
    if( ser==NULL ) return;
    drawCross_->SetColor( ser->SeriesColor );
	if( Shift.Contains(ssLeft)||Shift.Contains(ssRight) )
    	drawCross_->Hide();
    else
		if (cbCrossHair->Checked)
        {
        	drawCross_->SetSeries( ser );
			drawCross_->OnMouseMove(X,Y);
        }

	double x,y;
	ser->GetCursorValues(x,y);
    // получить индекс y по x или -1
	const int yIdx = GetYValueChartSeries(ser, x);
    //const double vX = ser->XScreenToValue(X), vY = ser->YScreenToValue(Y);
    if(yIdx==-1)
    {
    	pnY->Label5->Caption = "Ось Y";
    	pnX->Label5->Caption = "Ось X (время)";
    }
    else
    {

    	pnX->Label5->Caption =
        	AnsiString().sprintf("X=%s", this->TimeToStr(x) );
        pnY->Label5->Caption =
        	AnsiString().sprintf("Y=%s",FormatFloat(
    	yAx_->AxisValuesFormat, ser->YValue[yIdx] ) );
    }
}
//---------------------------------------------------------------------------
void __fastcall TFrameGraph::grph_AfterDrawValues(TObject *sndr)
{
    drawCross_->Reset();
}
//---------------------------------------------------------------------------
void __fastcall TFrameGraph::grph_BeforDrawValues(TObject *sndr)
{
	//
}
//---------------------------------------------------------------------------
void __fastcall TFrameGraph::chrt_AfterDraw(TObject *Sender)
{
    UpdateCurrentGraphInfo();
    if( cbShowPics->Checked )
        for( int i=0; i<chrt_->SeriesCount(); ++i )
            DrawSeriesXMarks(chrt_->Series[i], chrt_->BottomAxis, xMarks_,
                x0Marks_, x1Marks_, names_ );
}
//---------------------------------------------------------------------------
void __fastcall TFrameGraph::cbCrossHairClick(TObject *Sender)
{
    if( !cbCrossHair->Checked ) drawCross_->Hide();
}
//---------------------------------------------------------------------------
void __fastcall TFrameGraph::chrt_MouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	drawCross_->Hide();
}
//---------------------------------------------------------------------------
void __fastcall TFrameGraph::chrt_MouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	drawCross_->Hide();
}
//---------------------------------------------------------------------------
void __fastcall TFrameGraph::FormMouseWheelUp(TObject *Sender,
      TShiftState Shift, const TPoint &mousePos, bool &Handled)
{
	//form_->ActiveControl = NULL;
	OnMouseWheel( GetVCLControlAtPos( this,  mousePos ), true );
}
//---------------------------------------------------------------------------
void __fastcall TFrameGraph::FormMouseWheelDown(TObject *Sender,
      TShiftState Shift, const TPoint &mousePos, bool &Handled)
{
	OnMouseWheel( GetVCLControlAtPos( this,  mousePos ), false );
}
//---------------------------------------------------------------------------
void __fastcall TFrameGraph::edXMinKeyDown(TObject *Sender,
      WORD &Key, TShiftState Shift)
{
	if( Key!=VK_UP && Key!=VK_DOWN ) return;
    const bool isUp = Key==VK_UP;
    Key = 0;
    OnMouseWheel( dynamic_cast<TWinControl*>(Sender), isUp );
}
//---------------------------------------------------------------------------
void __fastcall TFrameGraph::cbAutoLimitYClick(TObject *Sender)
{
	SetAutomaticYOrders10Precent();
}
//---------------------------------------------------------------------------
void __fastcall TFrameGraph::chrt_Zoom(TObject *Sender)
{
	drawCross_->Hide();
    chrt_->Repaint();
}
//------------------------------------------------------------------------------
void __fastcall TFrameGraph::btnUndoZoomClick(TObject *Sender)
{
	UndoZoom();
}
//---------------------------------------------------------------------------
void __fastcall TFrameGraph::edDateTimeFormatChange(TObject *Sender)
{
	const AnsiString s = static_cast<TEdit*>(Sender)->Text;
	xAx_->DateTimeFormat = s;
    xAx_->AxisValuesFormat = s;
}
//---------------------------------------------------------------------------
void __fastcall TFrameGraph::cbNoZoomClick(TObject *Sender)
{
	if( cbNoZoom->Checked )	UndoZoom();
}
//---------------------------------------------------------------------------
void __fastcall TFrameGraph::cbSeriesListClick(TObject *Sender)
{
    drawCross_->SetSeries( ActiveGraph() );
}
//---------------------------------------------------------------------------
void __fastcall TFrameGraph::btnBrushColorClick(TObject *Sender)
{
    ColorDialog1->Color = chrt_->Color;
    if( !ColorDialog1->Execute() ) return;
    chrt_->Color = ColorDialog1->Color;
}
//---------------------------------------------------------------------------

void __fastcall TFrameGraph::FrameMouseWheelUp(TObject *Sender,
      TShiftState Shift, TPoint &MousePos, bool &Handled)
{
    FormMouseWheelUp(Sender, Shift, MousePos, Handled);
}
//---------------------------------------------------------------------------

void __fastcall TFrameGraph::FrameMouseWheelDown(TObject *Sender,
      TShiftState Shift, TPoint &MousePos, bool &Handled)
{
    FormMouseWheelDown(Sender, Shift, MousePos, Handled);    
}
//---------------------------------------------------------------------------

void __fastcall TFrameGraph::chrt_Resize(TObject *Sender)
{
    if( !cbShowPics->Checked )
    {
        chrt_->MarginTop = 0;
        return;
    }

    TCanvas& cnv = *chrt_->Canvas->ReferenceCanvas;
    const TRect rect = chrt_->ChartRect  ;
    if(rect.Height()>0) {
        const int txtH =
            cnv.TextHeight("0"),
            //maxH = rect.Top-chartRect.Top,
            chrtMarginTop =  Ceil( (100.0*txtH)/(1.0*rect.Height() ) );
        chrt_->MarginTop = chrtMarginTop;
    }
}
//---------------------------------------------------------------------------

void __fastcall TFrameGraph::chrt_MouseWheelUp(TObject *Sender,
      TShiftState Shift, TPoint &MousePos, bool &Handled)
{
    OnMouseWheel( GetVCLControlAtPos( this,  MousePos ), true );
    if( OnWheelMouse ) OnWheelMouse(Sender, Shift, MousePos, Handled, true);
}
//---------------------------------------------------------------------------

void __fastcall TFrameGraph::chrt_MouseWheelDown(TObject *Sender,
      TShiftState Shift, TPoint &MousePos, bool &Handled)
{
    OnMouseWheel( GetVCLControlAtPos( this,  MousePos ), false );
    if( OnWheelMouse ) OnWheelMouse(Sender, Shift, MousePos, Handled, false);
}
//---------------------------------------------------------------------------

void __fastcall TFrameGraph::Label4Click(TObject *Sender)
{
    TChartSeries* ser = ActiveGraph();
    ColorDialog1->Color = ser->SeriesColor;
    if( !ColorDialog1->Execute() ) return;
    for( int i=0; i<chrt_->SeriesCount(); ++i )
        chrt_->Series[i]->SeriesColor = ColorDialog1->Color;
}
//---------------------------------------------------------------------------

void __fastcall TFrameGraph::Label7Click(TObject *Sender)
{
    TFastLineSeries* ser = dynamic_cast<TFastLineSeries*>( ActiveGraph() );
    if( ser==NULL ) return;
    AnsiString Value = ser->LinePen->Width;
    if( InputQuery( "Толщина линии "+ser->Title, "Задайте толщину линии "+ser->Title, Value) )
        for( int i=0; i<chrt_->SeriesCount(); ++i )
            if( TFastLineSeries* ser1 = dynamic_cast<TFastLineSeries*>( chrt_->Series[i] ) )
                ser1->LinePen->Width = StrToInt(Value);
}
//---------------------------------------------------------------------------
void __fastcall TFrameGraph::cbShowPicsClick(TObject *Sender)
{
    for( int i=0; i<chrt_->SeriesCount(); ++i )
    	chrt_->Series[i]->Repaint();    
}
//---------------------------------------------------------------------------

void __fastcall TFrameGraph::Label1Click(TObject *Sender)
{
    if( chrt_->SeriesCount()==0 ) return;
    AnsiString fn;
    if( !PromptForFileName(fn, "Точки графика (*.txt)|*.txt", "txt",
        "Сохранить точки графика", GetPersonalDir(), true) )
        return;
    TChartSeries *ser = chrt_->Series[0];
    std::ofstream strm( fn.c_str() );
    for( int i=0; i<ser->Count(); ++i )
    {
        strm << ser->YValues->Value[i];
        if( i!=ser->Count()-1 )
            strm << "\n";
    }
    strm.close();
    


}
//---------------------------------------------------------------------------

