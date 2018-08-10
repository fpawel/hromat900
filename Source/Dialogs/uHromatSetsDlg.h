//---------------------------------------------------------------------------

#ifndef uHromatSetsDlgH
#define uHromatSetsDlgH
//---------------------------------------------------------------------------
#include "DrawGridCell.h"
#include "my_array_size.h"
#include "GrdCell.hpp"
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Grids.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
#include <CheckLst.hpp>
#include <Buttons.hpp>
#include <Series.hpp>
#include <Chart.hpp>
#include <TeEngine.hpp>
#include <TeeProcs.hpp>
//------------------------------------------------------------------------------
// std
#include <vector>
#include <map>
//------------------------------------------------------------------------------
class TiXmlElement;
class TFrameGrid;
//------------------------------------------------------------------------------
typedef std::vector<AnsiString> StringList;
typedef std::vector<double> VDouble;
struct LessStringList
{
    bool operator()( const StringList&, const StringList& ) const;
};
//------------------------------------------------------------------------------
namespace GrdValsCol
{
    enum
    {
        num,
        name,
        val,
        units,
        min,
        max,
        count
    };
};
//------------------------------------------------------------------------------
class TFormHromatSetsDlg : public TForm
{
__published:	// IDE-managed Components
    TPanel *Panel1;
    TLabel *Label1;
    TPanel *Panel2;
    TButton *Button1;
    TButton *Button2;
    TButton *btnRead;
    TButton *btnWrite;
    TPanel *Panel3;
    TScrollBox *ScrollBox1;
    TEdit *edGrd;
    TDateTimePicker *edTime;
	void __fastcall FormActivate(TObject *Sender);
    void __fastcall edGrdExit(TObject *Sender);
    void __fastcall edGrdKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
    void __fastcall edGrdKeyPress(TObject *Sender, char &Key);
    void __fastcall Button2Click(TObject *Sender);
    void __fastcall Button1Click(TObject *Sender);
    void __fastcall StringGrid1MouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
private:	// User declarations
    TiXmlElement* xml_;
    bool activated_;
    std::vector< boost::shared_ptr<TFrameGrid> > frames_;

    typedef std::map<GrdCell,unsigned> Cell2VarMap;
    Cell2VarMap cell2Var_;
    std::vector<GrdCell> var2Cell_;

    typedef std::map<StringList, TComboBox*, LessStringList> ValsList2ComboBox;
    ValsList2ComboBox valsList2ComboBox_;

    VDouble originalVals_;

    void AfterSetCanvasGrd( TStringGrid* grd, int col, int row,
		MyGridCellState drawState, TRect rect, int& txtX, int& txtY );
    void OnDeterminateColumnWidth( TStringGrid* grd, int col, int row, bool& ff);
    void __fastcall grdSelectCell(TObject *Sender, int ACol,
      int ARow, bool &CanSelect);
    bool IsLeftAligmentAndNoAutoColWidthCell( TStringGrid* grd, int col, int row );

    int Cell2Var( TObject *grd, int col, int row );

public:		// User declarations
	__fastcall TFormHromatSetsDlg(TComponent* Owner, const VDouble&);
    virtual __fastcall ~TFormHromatSetsDlg();
    bool ValidateResult( VDouble& );
    VDouble Result() const;
    void UpdateValues(const VDouble& originalVals);
};
//------------------------------------------------------------------------------
extern PACKAGE TFormHromatSetsDlg *FormHromatSetsDlg;
//------------------------------------------------------------------------------
#endif
