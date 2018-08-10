//---------------------------------------------------------------------------
#ifndef TransmitInterfaceSetsH
#define TransmitInterfaceSetsH
//---------------------------------------------------------------------------

#include "MyIostream.h"
// тип формы диалога настроек
class TFormAppSetsDlg;


// агрегат с настройками коммуникационного интерфейса
struct TransmitInterfaceSets
{
	// параметры связи
    unsigned char modbusDeviceAddress_;
    bool isTCP_;
    unsigned tmWriteDelay_, timeOut_, silentTime_, rxdMaxFrameSize_;
    // COM порт
    AnsiString comPort_;
    unsigned comPortBaudRate_;
    // tcp
    AnsiString tcpHost_;
    unsigned tcpPort_;
    bool useProxi_;
    AnsiString proxi_;

    bool useFtp;
};

// загрузить TransmitInterfaceSets из ини-файл приложения
TransmitInterfaceSets LoadTransmitInterfaceSets();
// сохранить TransmitInterfaceSets в ини-файл приложения
void SaveTransmitInterfaceSets(const TransmitInterfaceSets&);


void SetCtrlPrgrmmDlgItfcSets( TFormAppSetsDlg *frmDlg, const TransmitInterfaceSets& );
TransmitInterfaceSets GetCtrlPrgrmmDlgItfcSets( TFormAppSetsDlg *frmDlg );

bool IsNotIdenticChanals( const TransmitInterfaceSets&, const TransmitInterfaceSets&);


//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
