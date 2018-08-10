//---------------------------------------------------------------------------

#ifndef uAppSetsDlgH
#define uAppSetsDlgH
//---------------------------------------------------------------------------
#include "DrawGridCell.h"
#include "my_array_size.h"
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

namespace AppSetsGrdItems
{
	const char * const ttls[] =
    {
    "��������",
    "����� �����",
    "IP-�����",
    "IP-����",

    "������������ ������-������",
    "������-������ :[����]:",

    "������������ FTP",

    "COM-����",
    "�������� RS-232",
    "MODBUS-�����",
    "������� ������",
    "�������� ��������",
    "������������ �������� ������� �����",
    "������������ ����� �������",
    "��� ��������� 1",
    "��� ��������� 2",
    "���� � ������ ������������",
    "��������� �������������",
    "������ �������������",
    "��������� �������� ������������",
    "����� ������ �����",
    "������������ ����������� �������������",
    "��� ������������",
    "���������� ����������"

    };

    const unsigned rowCount = array_size(ttls);

    namespace Row{ enum {
        ttl,
    	ConnectChanal,
    	IP,
    	TCPPort,

        UseProxi,
        Proxi,

        UseFTP,

    	COM,
    	COMBoudRate,
    	ModbusAddy,
    	Timeout,
    	WaitDelay,
        SilentTime,
    	RXDFrameSize,
    	Detector1,
    	Detector2,
    	Path,
        EnableReadDetectorIndication,
        DiskreteTime,
        EnableWarningSound,
        MestoOtboraProby,
        DurHtg1,
        HromatType,
        DetectorsCount,
        Count
    };}; //namespace Row{ enum {
};
//---------------------------------------------------------------------------

class TiXmlElement;
//------------------------------------------------------------------------------
class TFormAppSetsDlg : public TForm
{
__published:	// IDE-managed Components
    TStringGrid *grd1;
    TComboBox *cbPortName;
    TComboBox *cbCommBR;
    TComboBox *cbDetectorType1;
    TEdit *edGrd;
    TComboBox *cbChanal;
    TEdit *edPath;
    TPanel *Panel1;
    TButton *Button2;
    TButton *Button1;
    TComboBox *ComboBox1;
	void __fastcall Button1Click(TObject *Sender);
	void __fastcall Button2Click(TObject *Sender);
	void __fastcall rgInterfaceClick(TObject *Sender);
	void __fastcall FormActivate(TObject *Sender);
	void __fastcall CheckListBox1ClickCheck(TObject *Sender);
    void __fastcall grd1SelectCell(TObject *Sender, int ACol, int ARow,
          bool &CanSelect);
    void __fastcall grd1SetEditText(TObject *Sender, int ACol, int ARow,
          const AnsiString Value);
    void __fastcall edGrdExit(TObject *Sender);
    void __fastcall edPathEnter(TObject *Sender);
    void __fastcall cbChanalKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
private:	// User declarations
    TiXmlElement* xml_;

    typedef bool (*IsStrCorrectFoo)( const AnsiString& );
    typedef std::map<int, IsStrCorrectFoo> CorrectFooMap;

    // ��� ������ ������� �������� ������ ��� ��������� TLabeledEdit
    CorrectFooMap correctFooMap_;

    bool activated_, itfcChngd_;

    my::DrawGridCellManager drwGrd;

    void AfterSetCanvasGrd( TStringGrid* grd, int col, int row,
		MyGridCellState drawState, TRect rect, int& txtX, int& txtY );
    void OnDeterminateColumnWidth( TStringGrid* grd, int col, int row, bool& ff);
    bool IsLeftAligmentAndNoAutoColWidthCell( TStringGrid* grd, int col, int row );

public:		// User declarations
	__fastcall TFormAppSetsDlg(TComponent* Owner);
    virtual __fastcall ~TFormAppSetsDlg();

    const bool IsItfcChnged() const { return itfcChngd_; }
};
//---------------------------------------------------------------------------
extern PACKAGE TFormAppSetsDlg *FormAppSetsDlg;
//---------------------------------------------------------------------------
#endif
