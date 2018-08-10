//---------------------------------------------------------------------------
#ifndef TransmitInterfaceSetsH
#define TransmitInterfaceSetsH
//---------------------------------------------------------------------------

#include "MyIostream.h"
// ��� ����� ������� ��������
class TFormAppSetsDlg;


// ������� � ����������� ����������������� ����������
struct TransmitInterfaceSets
{
	// ��������� �����
    unsigned char modbusDeviceAddress_;
    bool isTCP_;
    unsigned tmWriteDelay_, timeOut_, silentTime_, rxdMaxFrameSize_;
    // COM ����
    AnsiString comPort_;
    unsigned comPortBaudRate_;
    // tcp
    AnsiString tcpHost_;
    unsigned tcpPort_;
    bool useProxi_;
    AnsiString proxi_;

    bool useFtp;
};

// ��������� TransmitInterfaceSets �� ���-���� ����������
TransmitInterfaceSets LoadTransmitInterfaceSets();
// ��������� TransmitInterfaceSets � ���-���� ����������
void SaveTransmitInterfaceSets(const TransmitInterfaceSets&);


void SetCtrlPrgrmmDlgItfcSets( TFormAppSetsDlg *frmDlg, const TransmitInterfaceSets& );
TransmitInterfaceSets GetCtrlPrgrmmDlgItfcSets( TFormAppSetsDlg *frmDlg );

bool IsNotIdenticChanals( const TransmitInterfaceSets&, const TransmitInterfaceSets&);


//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
