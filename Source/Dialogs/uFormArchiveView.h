//---------------------------------------------------------------------------

#ifndef uFormArchiveViewH
#define uFormArchiveViewH
//---------------------------------------------------------------------------
#define VCL_IOSTREAM

#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <FileCtrl.hpp>
#include <ExtCtrls.hpp>
#include <Grids.hpp>
#include <TeEngine.hpp>
#include <TeeProcs.hpp>
#include <Series.hpp>
#include <Menus.hpp>
#include <ComCtrls.hpp>
#include <ImgList.hpp>
#include <Buttons.hpp>
#include <Graphics.hpp>
#include "SHDocVw_OCX.h"
#include <OleCtrls.hpp>
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// my
#include "Hromat900.hpp"
#include "Hromatogram.hpp"
#include "..\DevSys\hromat900_ini.h"
#include "VCLUtils\DrawGridCell.h"


//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//std
#include <map>
//---------------------------------------------------------------------------
namespace AnalyzeHromatogram
{
    typedef struct
	{
		std::vector<std::vector<double> > conc;
    	unsigned count;
        TDateTime dateMin, dateMax;
        bool okrug;
        std::vector<std::pair<TDateTime,double> > IzobutConc;
	} Tag;
};

namespace AnalyzeHromatogram
{
    // анализировать файл хроматограммы VDouble conc_[Gas::COUNT], physChem[3];
	void ProcesArchiveFile( const AnsiString& fn, Tag&);
    void SetInfoGrdTttls( TStringGrid* grd);
};



namespace HtgItemList
{
    typedef std::pair< unsigned, std::pair<unsigned,unsigned> > HourMinuteSecond;
    typedef std::map<HourMinuteSecond, HtgItem> MapT1; // время - хроматограмма
    typedef std::map<unsigned, MapT1> MapT2;           // день - время - хроматограмма
    typedef std::map<unsigned, MapT2> MapT3;           // мес€ц - день - время - хроматограмма
    typedef std::map<unsigned, MapT3> MapT4;         // год - мес€ц - день - время - хроматограмма
    typedef std::map<TTreeNode*, const HtgItem*> MapT5;
    typedef std::vector<const HtgItem*> List;
};

struct HtgFilter
{
    int rgm;
    bool izm, grad;
};


class TFrameGraph;
class TFrmWait;
class TFormArchiveView : public TForm
{
__published:	// IDE-managed Components
	TPanel *PanelFormArchiveView;
    TImageList *ImageList1;
    TPopupMenu *PopupMenu1;
    TMenuItem *menuAnalyze1;
    TMenuItem *menuAnalyze2;
    TPanel *Panel3;
    TPanel *pnLeft;
    TSplitter *Splitter3;
    TPanel *PanelLeftBottom;
    TPanel *Panel1;
    TComboBox *edPath;
    TButton *Button1;
    TTreeView *trv1;
    TSplitter *Splitter2;
    TMenuItem *N1;
    TMenuItem *N2;
    TPanel *Panel5;
    TRadioButton *RadioButton1;
    TRadioButton *RadioButton2;
    TPanel *Panel8;
    TLabel *lblFilter;
    TPanel *Panel2;
    TPageControl *PageControl1;
    TTabSheet *TabSheet3;
    TPanel *Panel4;
    TScrollBox *ScrollBox1;
    TStringGrid *grdPhysChem;
    TPanel *Panel7;
    TStringGrid *grdPic1;
    TPanel *Panel9;
    TTabSheet *TabSheet5;
    TTabSheet *TabSheet1;
    TMemo *Memo1;
    TMenuItem *N3;
    TMenuItem *N4;
    TPopupMenu *PopupMenu2;
    TMenuItem *N5;
    TTabSheet *TabSheet2;
    TCppWebBrowser *CppWebBrowser1;
    TPanel *Panel10;
    TCheckBox *CheckBox4;
    TPanel *Panel11;
    TCheckBox *CheckBox5;
    TStringGrid *grdPic2;
    TPanel *Panel6;
    TMenuItem *N6;
	void __fastcall pnLeftTopCanResize(TObject *Sender, int &NewWidth,
          int &NewHeight, bool &Resize);
	void __fastcall pnLeftCanResize(TObject *Sender, int &NewWidth,
          int &NewHeight, bool &Resize);
	void __fastcall FormActivate(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall FormHide(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall trv1Compare(TObject *Sender, TTreeNode *Node1,
          TTreeNode *Node2, int Data, int &Compare);
    void __fastcall PageControl1Change(TObject *Sender);
    void __fastcall FormMouseWheelUp(TObject *Sender, TShiftState Shift,
          TPoint &MousePos, bool &Handled);
    void __fastcall FormMouseWheelDown(TObject *Sender, TShiftState Shift,
          TPoint &MousePos, bool &Handled);
    void __fastcall Button1Click(TObject *Sender);
    void __fastcall trv1Change(TObject *Sender, TTreeNode *Node);
    void __fastcall grdPhysChemMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall menuAnalyze1Click(TObject *Sender);
    void __fastcall PopupMenu1Popup(TObject *Sender);
    void __fastcall lblFilterClick(TObject *Sender);
    void __fastcall menuAnalyze2Click(TObject *Sender);
    void __fastcall edPathChange(TObject *Sender);
    void __fastcall N1Click(TObject *Sender);
    void __fastcall N2Click(TObject *Sender);
    void __fastcall RadioButton2Click(TObject *Sender);
    void __fastcall trv1Click(TObject *Sender);
    void __fastcall trv1GetImageIndex(TObject *Sender, TTreeNode *Node);
    void __fastcall N3Click(TObject *Sender);
    void __fastcall N5Click(TObject *Sender);
    void __fastcall CheckBox4Click(TObject *Sender);
    void __fastcall CheckBox5Click(TObject *Sender);
    void __fastcall N6Click(TObject *Sender);
private:	// User declarations
    HromatogrammsList htgs_[2];
    TFrameGraph* frameGraph_[2];
    TStringGrid *grdPic[2], *grdPhysChemHtg[2];
    my::DrawGridCellManager drawGrdPik1_, drawGrdPik2_;
    bool isAnalyzePerforming_;
    AnalyzeHromatogram::Tag analyze_;

    HtgItemList::MapT4 map4_;
    HtgItemList::MapT5 map5_;
    HtgItemList::List items_;

    static HtgItemsArray deviceArchive_;

    HtgFilter filter_;
    bool mustMakeTree_;

    AnsiString treePath_;
    
    void SetCurrentHtg( const HtgItem &htg );
    void ResetCurrentHtg( );
    AnsiString GetTreeNodePath(TTreeNode *node);
    bool OnAddItem2File(TComboBox*, const AnsiString& fileName, const AnsiString& item);
    void CalculateMidleValues(bool okrugl);
    void GetSelectedHtgItems(HtgItemsArray& ret);
    void GetSelectedArchiveHtgItems(HtgItemsArray& ret);
public:		// User declarations
	__fastcall TFormArchiveView(TComponent* Owner, bool mustMakeTree = 1);
    __fastcall virtual ~TFormArchiveView();


    void MakeTreeFromHtgItemsArray(const HtgItemsArray& itemsList );
    void SetDeviceArchive(const HtgItemsArray& deviceArchive );
    void HandleEndReadArchive();
    void HandleEndReadArchiveFile(unsigned file, TDateTime dateTime);

    HromatogramReportData GetReportData(const HtgItem& itm);

};
//---------------------------------------------------------------------------
extern PACKAGE TFormArchiveView *FormArchiveView;
//---------------------------------------------------------------------------
void OnAfterEndReadArchiveFile(unsigned file, TDateTime dateTime);


#endif
