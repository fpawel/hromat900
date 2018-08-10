//---------------------------------------------------------------------------
#ifndef Unit1H
#define Unit1H
#define VCL_IOSTREAM
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Menus.hpp>
#include <Dialogs.hpp>
#include <Grids.hpp>
#include <Buttons.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <ActnList.hpp>
#include <StdActns.hpp>
#include <ImgList.hpp>
#include <ToolWin.hpp>
#include <Graphics.hpp>
#include "SHDocVw_OCX.h"
#include <OleCtrls.hpp>
#include <MPlayer.hpp>
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// std
#include <vector>

// boost
#include "boost\noncopyable.hpp"
#include "boost\shared_ptr.hpp"

#include "hromat900_ini.h"
#include "MshrmtArc.hpp"
#include "Hromat900.hpp"

// my
#include "free_types_.h"
#include "VCLUtils\DrawGridCell.h"


#include "MyIostream.h"

//---------------------------------------------------------------------------
// 			Предварительные объявления ( для PIMPL )
//---------------------------------------------------------------------------
class TFormHromatogramm;
// тип формы диалога настроек
class TFormCtrlPrgrmmDlg;
typedef boost::shared_ptr<TFormCtrlPrgrmmDlg> PFormCtrlPrgrmmDlg;
//---------------------------------------------------------------------------
// class TiXmlElement - элемент дерева XML - декларирован в #include "..\my_include\tinyxml.h"
// предварительное объявление для организации файлового ввода/вывода
class TiXmlElement;
//class TiXmlDocument;
//---------------------------------------------------------------------------
class TFrameDropDownPanel;

class TForm1 : public TForm
{
private:
	boost::shared_ptr<void> logLifeTime_, firstMemeber_;
__published:	// IDE-managed Components
	TActionList *ActionList1;
	TWindowCascade *WindowCascade1;
	TWindowTileHorizontal *WindowTileHorizontal1;
	TWindowTileVertical *WindowTileVertical1;
	TWindowMinimizeAll *WindowMinimizeAll1;
	TWindowArrange *WindowArrange1;
    TSplitter *Splitter1;
    TWindowClose *WindowClose1;
    TImageList *ImageList1;
    TAction *ActionShowDevSetsDlg;
    TMainMenu *MainMenu1;
    TMenuItem *MenuItem1;
    TMenuItem *MenuItem2;
    TMenuItem *MenuItem7;
    TMenuItem *menuHromatParams;
    TMenuItem *menuWindow;
    TMenuItem *MenuItem14;
    TMenuItem *MenuItem15;
    TMenuItem *MenuItem16;
    TMenuItem *MenuItem17;
    TMenuItem *MenuItem18;
    TMenuItem *MenuItem19;
	TMenuItem *menuDebug;
    TMenuItem *MenuItem22;
    TMenuItem *MenuItem24;
    TMenuItem *N1;
	TPanel *panelStatus;
	TPanel *Panel9;
	TPanel *Panel10;
	TSplitter *Splitter2;
	TPanel *Panel11;
	TSplitter *Splitter3;
	TSplitter *Splitter4;
	TPanel *Panel12;
    TOpenDialog *OpenDialog1;
    TMenuItem *menuThread;
    TMenuItem *N8;
    TMenuItem *menuViewDiskArchive;
    TMenuItem *N6;
    TMenuItem *N7;
    TMenuItem *menuUsersControlDialog;
    TMenuItem *N2;
    TMenuItem *N3;
    TMenuItem *N4;
    TMenuItem *N5;
    TScrollBox *ScrollBox1;
	TTimer *TimerBackup;
	TMenuItem *N10;
	TMenuItem *N12;
	TMenuItem *N13;
	TMenuItem *menuSoftIDDialog;
	TMenuItem *N15;
	TMenuItem *N16;
    TMenuItem *menuSetHromatKey;
    TMenuItem *N19;
    TMenuItem *menuGetHromatJournals;
    TMenuItem *N11;
    TMenuItem *N17;
    TMenuItem *N18;
	TMenuItem *N14;
    TPanel *Panel1;
    TImage *imgCicleStop;
    TImage *imgCicleRun;
    TImage *imgCicleGrad;
    TImage *imgCicleSingle;
    TSpeedButton *btnCicleGrad;
    TSpeedButton *btnCicleSingle;
    TSpeedButton *btnCicleRun;
    TSpeedButton *btnCicleStop;
    TStringGrid *grdDtctr1;
    TStringGrid *grdToolPrms;
    TStringGrid *grdToolInfo;
    TStringGrid *grdDtctr2;
    TStringGrid *grdPhisChem;
    TMenuItem *N20;
    TMenuItem *N9;
    TMenuItem *N21;
    TTimer *Timer1;
    TMenuItem *N22;
    TMediaPlayer *MediaPlayer1;
    TTimer *TimerWarn;
    TButton *Button1;
    TPanel *Panel2;
    TSplitter *Splitter5;
    TPanel *Panel4;
    TSplitter *Splitter6;
    TLabel *Label2;
	void __fastcall stdin1Click(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall FormActivate(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall Panel1CanResize(TObject *Sender, int &NewWidth,
          int &NewHeight, bool &Resize);
    void __fastcall ActionShowDevSetsDlgExecute(TObject *Sender);
    void __fastcall N1Click(TObject *Sender);
	void __fastcall Panel11DblClick(TObject *Sender);
	void __fastcall MenuItem22Click(TObject *Sender);
	void __fastcall grdToolPrmsSelectCell(TObject *Sender, int ACol, int ARow,
          bool &CanSelect);
	void __fastcall WindowCascade1Execute(TObject *Sender);
    void __fastcall MenuItem2Click(TObject *Sender);
    void __fastcall menuThreadClick(TObject *Sender);
    void __fastcall grdToolPrmsMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall N8Click(TObject *Sender);
    void __fastcall menuViewDiskArchiveClick(TObject *Sender);
    void __fastcall N6Click(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall menuUsersControlDialogClick(TObject *Sender);
    void __fastcall N4Click(TObject *Sender);
	void __fastcall TimerBackupTimer(TObject *Sender);
	void __fastcall MenuItem7Click(TObject *Sender);
	void __fastcall FormMouseWheelDown(TObject *Sender, TShiftState Shift,
          const TPoint &MousePos, bool &Handled);
	void __fastcall FormMouseWheelUp(TObject *Sender, TShiftState Shift,
          const TPoint &MousePos, bool &Handled);
	void __fastcall N12Click(TObject *Sender);
	void __fastcall N13Click(TObject *Sender);
	void __fastcall menuSoftIDDialogClick(TObject *Sender);
	void __fastcall N16Click(TObject *Sender);
    void __fastcall menuSetHromatKeyClick(TObject *Sender);
    void __fastcall N19Click(TObject *Sender);
    void __fastcall menuGetHromatJournalsClick(TObject *Sender);
    void __fastcall N18Click(TObject *Sender);
    void __fastcall btnCicleRunClick(TObject *Sender);
    void __fastcall imgCicleStopClick(TObject *Sender);
    void __fastcall N20Click(TObject *Sender);
    void __fastcall N9Click(TObject *Sender);
    void __fastcall N21Click(TObject *Sender);
    void __fastcall Timer1Timer(TObject *Sender);
    void __fastcall N22Click(TObject *Sender);
    void __fastcall Button2Click(TObject *Sender);
    void __fastcall TimerWarnTimer(TObject *Sender);
    void __fastcall Button1Click(TObject *Sender);
private:	// User declarations


    TiXmlElement* xml_;


    // колличество правых панелей
    enum{ RIGHT_PNLS_COUNT = 5 };
    // колличество панелей статуса
    enum{ STATUS_PNLS_COUNT = 5 };

    // правые панели
    boost::shared_ptr<my::DrawGridCellManager> drwRpnGrd_[RIGHT_PNLS_COUNT];
    TStringGrid* rpnGrd_[RIGHT_PNLS_COUNT];
    TFrameDropDownPanel* rpn_[RIGHT_PNLS_COUNT];

    // правые панели
    TPanel* pnStatus_[STATUS_PNLS_COUNT];

    PVoid connectOnTransferManagerReport_;

    AnsiString typeOfDtctr_[2];

    // имя пользователя
    AnsiString user_;

    // текущий тип цикла
    unsigned currentHromatCicleType_;

    struct HromatModeMessage
    {
        bool isNorm;
        AnsiString s;
    };
    typedef std::map<unsigned short, HromatModeMessage> HromatModeMap;
    // карта отображения кода режима хроматографа в описание
    HromatModeMap modes_;

    TObject *btnCicle[HROMAT_CICLE::COUNT], *imgCicle[HROMAT_CICLE::COUNT];

    int GetIndexOfBtnCicle(TObject * control);

    void OnTransferManagerReport(unsigned context, const AnsiString& state);

    bool ExecuteAutorization();

    void InitializeRightPanels();
    void SetLeftPanelHeight(unsigned n);
    void ShowArchivedialog(bool hardDisk);

public:		// User declarations
	__fastcall TForm1(TComponent* Owner);
    __fastcall virtual ~TForm1();

    void ShowHromatJurnals(
        const Jurnal_Analize* pJurnalAnalize,
        const Jurnal_AnalizeArchive* pJurnalAnalizeArchive );

    void SetStatus( const AnsiString s, unsigned idx );

    void HandleChnlsReaded(const double*, const double*);
    void HandleDeviceStateReaded(unsigned,unsigned);
    void HandleProcessCurentHTG(const ModbusFileInfo&, unsigned offset);

    void SetDetectorType(bool, const AnsiString&);
    AnsiString GetDetectorType(bool) const;

    void UpdateDetector2Info(bool visible);

    const AnsiString MeasureUnits;
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
