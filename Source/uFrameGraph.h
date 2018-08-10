//---------------------------------------------------------------------------


#ifndef uFrameGraphH
#define uFrameGraphH
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
#include <ComCtrls.hpp>

#include <Buttons.hpp>
#include <Menus.hpp>


// boost
#include "boost\noncopyable.hpp"
#include "boost\shared_ptr.hpp"
#include <CheckLst.hpp>
#include <Dialogs.hpp>
#include <DateUtils.hpp>

class CrossHairHelper;
//---------------------------------------------------------------------------
#include "free_types_.h"

class TFrameDropDownPanel;

class TFrameGraph : public TFrame
{
__published:	// IDE-managed Components
	TScrollBox *ScrollBox1;
	TChart *chrt_;
    TSplitter *Splitter1;
	TPanel *grpBxY;
	TLabel *Label5;
	TLabel *Label6;
	TEdit *edYMin;
	TCheckBox *cbAutoYMin;
	TEdit *edYMax;
	TCheckBox *cbAutoYMax;
	TPanel *grpBxX;
	TLabel *Label2;
	TLabel *Label3;
	TEdit *edXMin;
	TCheckBox *cbAutoXMin;
	TEdit *edXMax;
	TCheckBox *cbAutoXMax;
	TColorDialog *ColorDialog1;
    TPanel *panelGraphTools;
	TCheckBox *cbAutoLimitY;
	TCheckBox *cbNoZoom;
    TCheckBox *cbShowPics;
    TPanel *panelView;
    TLabel *btnBrushColor;
    TLabel *Label4;
    TLabel *Label7;
    TCheckBox *cbCrossHair;
    TLabel *Label1;
	void __fastcall cbAutoXMinClick(TObject *Sender);
	void __fastcall cbAutoXMaxClick(TObject *Sender);
	void __fastcall cbAutoYMinClick(TObject *Sender);
	void __fastcall cbAutoYMaxClick(TObject *Sender);
	void __fastcall edXMinChange(TObject *Sender);
	void __fastcall edXMaxChange(TObject *Sender);
	void __fastcall edYMinChange(TObject *Sender);
	void __fastcall edYMaxChange(TObject *Sender);
	void __fastcall chrt_MouseMove(TObject *Sender, TShiftState Shift, int X,
          int Y);
	void __fastcall cbCrossHairClick(TObject *Sender);
	void __fastcall chrt_AfterDraw(TObject *Sender);
	void __fastcall chrt_MouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall chrt_MouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall cbAutoLimitYClick(TObject *Sender);
	void __fastcall edXMinKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
	void __fastcall chrt_Zoom(TObject *Sender);
	void __fastcall btnUndoZoomClick(TObject *Sender);
	void __fastcall edDateTimeFormatChange(TObject *Sender);
	void __fastcall cbNoZoomClick(TObject *Sender);
	void __fastcall cbSeriesListClick(TObject *Sender);
	void __fastcall btnBrushColorClick(TObject *Sender);
    void __fastcall FrameMouseWheelUp(TObject *Sender, TShiftState Shift,
          TPoint &MousePos, bool &Handled);
    void __fastcall FrameMouseWheelDown(TObject *Sender, TShiftState Shift,
          TPoint &MousePos, bool &Handled);
    void __fastcall chrt_Resize(TObject *Sender);
    void __fastcall chrt_MouseWheelUp(TObject *Sender, TShiftState Shift,
          TPoint &MousePos, bool &Handled);
    void __fastcall chrt_MouseWheelDown(TObject *Sender, TShiftState Shift,
          TPoint &MousePos, bool &Handled);
    void __fastcall Label4Click(TObject *Sender);
    void __fastcall Label7Click(TObject *Sender);
    void __fastcall cbShowPicsClick(TObject *Sender);
    void __fastcall Label1Click(TObject *Sender);
private:	// User declarations
	TForm *form_;
    
    VDouble xMarks_, x0Marks_, x1Marks_;
    std::vector<AnsiString> names_;

	// для отрисовки курсора графика
    boost::shared_ptr<CrossHairHelper> drawCross_;


    TChartAxis *xAx_, *yAx_;

    double xMin_, yMin_, xMax_, yMax_;

    TFrameDropDownPanel *pnX, *pnY, *pnGraphTools, *pnSeriesList, *pnFile;

    AnsiString fileName_;

    void SetFile( const AnsiString &fn );
    void OpenFile(const AnsiString&);

    void UpdateCurrentGraphInfo();
    void SetAutomaticYOrders10Precent();
    void ShowChartNfo();

    void OnChartMouseOcurence( bool isEntered );

    void __fastcall grph_AfterDrawValues(TObject *sndr);
    void __fastcall grph_BeforDrawValues(TObject *sndr);

    AnsiString TimeToStr(TDateTime x)
    {
        return FormatDateTime(chrt_->BottomAxis->DateTimeFormat,x);
    }

    

public:		// User declarations
	__fastcall TFrameGraph(TComponent* Owner, TWinControl* parent);
    __fastcall ~TFrameGraph();

    TChartSeries* AddSeries(TChartSeries *ser = NULL);   

    void RemoveSeries(unsigned idx);
    void RemoveSeries(TChartSeries *ser);

    TChartSeries* GetSeries(unsigned) const;
    TChartSeries* ActiveGraph();
    bool IsValidSeries(TChartSeries*);

   	double XMin() const { return xMin_; }
    double YMin() const { return yMin_; }
    double XMax() const { return xMax_; }
    double YMax() const { return yMax_; }

    void OnMouseWheel( TWinControl* ctrl, bool isUp );

    void __fastcall FormMouseWheelUp(TObject *Sender,
      TShiftState Shift, const TPoint &mousePos, bool &Handled);

    void __fastcall FormMouseWheelDown(TObject *Sender,
      TShiftState Shift, const TPoint &mousePos, bool &Handled);

    void SetXMarks( const VDouble& marks, const VDouble &x0, const VDouble &x1,
    	const std::vector<AnsiString>& names );

    int SeriesCount() const;

    void UndoZoom();

    void SetActiveSeries(int);
    void SetActiveSeries(TChartSeries*);
    int GetActiveSeries();

    void (__closure *OnSetActiveSeries )(TChartSeries*, int);
    void (__closure *OnAddSeries )(TChartSeries*);
    void (__closure *OnRemoveSeries )(TChartSeries*);
    void (__closure *OnBeforClearSeriesList )();
    void (__closure *OnWheelMouse)(TObject*, TShiftState,
        const TPoint &mousePos, bool &Handled, bool up);

    void Clear();
};
//---------------------------------------------------------------------------
extern PACKAGE TFrameGraph *FrameGraph;
//---------------------------------------------------------------------------
#endif
