//---------------------------------------------------------------------------

#ifndef uFormHromatogrammH
#define uFormHromatogrammH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <Chart.hpp>
#include <ExtCtrls.hpp>
#include <Series.hpp>
#include <TeEngine.hpp>
#include <TeeProcs.hpp>
#include <Buttons.hpp>
#include <Menus.hpp>
#include <Grids.hpp>
//---------------------------------------------------------------------------

//STD
#include <vector>

// boost
#include "boost\noncopyable.hpp"
#include "boost\shared_ptr.hpp"

//my
#include "hromat900_ini.h"
#include "hromatogram.hpp"
#include "Hromat900.hpp"
//---------------------------------------------------------------------------
class PikTableHlpr;
class TFrameGraph;
class TiXmlElement;
//---------------------------------------------------------------------------
class TFormHromatogramm : public TForm
{
__published:	// IDE-managed Components
	TPageControl *PageControl1;
    TTabSheet *tbsGraph;
	TTabSheet *TabSheet3;
	TStringGrid *grdBasePik;
    TPanel *PanelRight;
    TButton *btnSaveBasePicTable;
	TButton *Button2;
    TTabSheet *TabSheet2;
    TStringGrid *grdPik;
    TTabSheet *TabSheet4;
    TPanel *pnDtctrElemInfo;
    TStringGrid *grdHtgsList;
    TPanel *lblDtctrElemInfo;
    TPanel *Panel1;
    TComboBox *cbHtgElem;
	TPanel *Panel2;
    TTimer *Timer1;
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall btnSaveBasePicTableClick(TObject *Sender);
    void __fastcall cbHtgElemChange(TObject *Sender);
    void __fastcall grdPikMouseWheelDown(TObject *Sender,
          TShiftState Shift, TPoint &MousePos, bool &Handled);
    void __fastcall grdPikMouseWheelUp(TObject *Sender, TShiftState Shift,
          TPoint &MousePos, bool &Handled);
    void __fastcall Button2Click(TObject *Sender);
	void __fastcall grdPikMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall grdHtgsListDblClick(TObject *Sender);
    void __fastcall Timer1Timer(TObject *Sender);
    void __fastcall grdBasePikKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
private:	// User declarations

	//TiXmlElement *xml_, *guiXml_;
	// реализация отрисовки таблиц
    //boost::shared_ptr<my::DrawGridCellManager> drawGrdBasePik_, drawGrdPik_;
    const AnsiString name_;

    boost::shared_ptr<PikTableHlpr> pikTblHlpr_;
    TiXmlElement* xml_, *basePicsXML_;
    
    // окно с графиком и полем выбора слоя
    TFrameGraph *frameGraph_;

    // список хроматграмм
    HromatogrammsList htgs_;

    TFastLineSeries *grph_;

    int htgIdx_;

    void UpdateActiveHtgCaption();

    void SetCbItemText(int );

    void OnWheelMouse(TObject*, TShiftState, const TPoint &mousePos, bool &Handled, bool up);

    void ClearHtgView();

    struct {
        int idxToSet, idx;
    } h_;

    DYNAMIC void __fastcall VisibleChanging(void);
    
public:		// User declarations
    const bool dtctrNum_;
	__fastcall TFormHromatogramm(TComponent* Owner, bool dtctrNum);
    __fastcall virtual ~TFormHromatogramm();

    void AddHtg(HromatogrammPtr htg);
    void AddHtg(HromatogrammPtr* htg, HromatogrammPtr* htgEnd);

    void UpdateView( const Hromatogramm* );

    void SetActiveHtgIndex(int);
    int GetActiveHtgIndex() const;

    unsigned HtgCount();
    HromatogrammPtr GetHtg(unsigned);


    void SetBasePikTbl( const HromatBasePicTable& );
    HromatBasePicTable GetBasePikTbl();

    TFastLineSeries *Graph();
    TChart *Chart();
};
//---------------------------------------------------------------------------
extern PACKAGE TFormHromatogramm *FormHromatogramm;
extern PACKAGE TFormHromatogramm *FrmHTG[2];
//---------------------------------------------------------------------------
#endif
