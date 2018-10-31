//---------------------------------------------------------------------------
#pragma hdrstop

#include "TransmitInterfaceSets.h"
#include "hromat900_ini.h"
#include "uAppSetsDlg.h"
#include "iniUtils_.h"
#include "xmlUtils_.h"
#include "MyAppProfile.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
void SetComboBox( TComboBox* cb, const AnsiString& s )
{
    cb->ItemIndex = cb->Items->IndexOf( s );
}
//---------------------------------------------------------------------------
TransmitInterfaceSets LoadTransmitInterfaceSets()
{
	TiXmlElement*
    xml_ = TiXML::GiveNotNulElement( ProfileXmlElem(), TRANSFERS_SEKT );
	TransmitInterfaceSets sets_;

	sets_.modbusDeviceAddress_	=	READ_XML_INT_( MODBUS_DEVICE_ADDRESS, 1, 127, 1);
    sets_.isTCP_ 				=	READ_XML_INT_( TCP_ACTIVE_ID, 0,1, 1);
    sets_.tmWriteDelay_	 	 = READ_XML_INT_( WRITE_DELAY_ID, 0, 2000, 150);
    sets_.timeOut_			 = TiXML::GetIntAtrDef( xml_, TIME_OUT_ID, 30000);

    sets_.silentTime_        = READ_XML_INT_( RXD_FRAME_SILENT_TIME, 5, 1000, 50);
    sets_.rxdMaxFrameSize_   = READ_XML_INT_( MAXIMUM_RXD_FRAME_SIZE, 100, 10000, 1000);

    // конфигурация RS-232
    xml_ = TiXML::GiveNotNulElement( ProfileXmlElem(), COMPORT_SEKT );
    sets_.comPort_ = READ_XML_STR_( COMPORT_IDX_ID, "COM1" ) ;
    sets_.comPortBaudRate_ =  READ_XML_INT_( COMPORT_BOUDRATE_ID, 9600, 1000000, 9600 );

    // конфигурация TCP
    xml_ = TiXML::GiveNotNulElement( ProfileXmlElem(), TCP_SEKT  );
    sets_.tcpHost_ =  READ_XML_STR_( IP_HOST_ID,       "192.168.1.5");
    sets_.tcpPort_  = READ_XML_INT_( TCP_PORT_NUM_ID, 0, 100000, 502);
    sets_.useProxi_ = READ_XML_INT_( USE_PROXI_ID, 0, 1, 0);
    sets_.proxi_ = READ_XML_STR_( PROXI_SERVER_ID,       "192.168.0.1:3128");
    sets_.useFtp  = READ_XML_INT_( "использовать_FTP",0,1,0);
    sets_.tcpLocalAddr_ = TiXML::GetStrAtr(xml_, TCP_LOCAL_ADDR_ID, "");

    return sets_;
}
//---------------------------------------------------------------------------
void SaveTransmitInterfaceSets(const TransmitInterfaceSets& sets_)
{
    LOG_FOO_LIFETIME_QUOTES;

    TiXmlElement*
    xml_ = TiXML::GiveNotNulElement( ProfileXmlElem(), TRANSFERS_SEKT );
	WRITE_XML_INT_( TCP_ACTIVE_ID, sets_.isTCP_  );
    WRITE_XML_INT_( WRITE_DELAY_ID, sets_.tmWriteDelay_);
    WRITE_XML_INT_( TIME_OUT_ID, sets_.timeOut_);

    WRITE_XML_INT_( RXD_FRAME_SILENT_TIME, sets_.silentTime_);
    WRITE_XML_INT_( MAXIMUM_RXD_FRAME_SIZE, sets_.rxdMaxFrameSize_);
    WRITE_XML_INT_( MODBUS_DEVICE_ADDRESS, sets_.modbusDeviceAddress_);

    // конфигурация TCP
    xml_ = TiXML::GiveNotNulElement( ProfileXmlElem(), TCP_SEKT );
    WRITE_XML_STR_( IP_HOST_ID,          sets_.tcpHost_);
    WRITE_XML_INT_( TCP_PORT_NUM_ID,    sets_.tcpPort_);
    WRITE_XML_STR_( PROXI_SERVER_ID, sets_.proxi_ );
    WRITE_XML_INT_( USE_PROXI_ID, sets_.useProxi_ );
    WRITE_XML_INT_( "использовать_FTP", sets_.useFtp );
    TiXML::SetAtr( xml_, TCP_LOCAL_ADDR_ID, sets_.tcpLocalAddr_ );

    // конфигурация порта
    xml_ = TiXML::GiveNotNulElement( ProfileXmlElem(), COMPORT_SEKT );
    WRITE_XML_STR_( COMPORT_IDX_ID, sets_.comPort_ );
    WRITE_XML_INT_( COMPORT_BOUDRATE_ID, sets_.comPortBaudRate_ );






}
//---------------------------------------------------------------------------
void SetCtrlPrgrmmDlgItfcSets( TFormAppSetsDlg *dlg, const TransmitInterfaceSets& sets_ )
{
    #define CCELL_(ss) dlg->grd1->Cells[2][AppSetsGrdItems::Row::##ss##]
    CCELL_(ModbusAddy) = sets_.modbusDeviceAddress_;
    CCELL_(COM) = sets_.comPort_;
    CCELL_(COMBoudRate) = sets_.comPortBaudRate_;
    CCELL_(IP) = sets_.tcpHost_;
    CCELL_(TCPPort) = sets_.tcpPort_;
    CCELL_(ConnectChanal) = sets_.isTCP_ ? "TCP" : "COM";
    CCELL_(Timeout) = sets_.timeOut_;
    CCELL_(WaitDelay) = sets_.tmWriteDelay_;
    CCELL_(SilentTime) = sets_.silentTime_;
    CCELL_(RXDFrameSize) = sets_.rxdMaxFrameSize_;
    CCELL_(Timeout) = sets_.timeOut_;

    CCELL_(UseProxi) = sets_.useProxi_ ? "Да" : "Нет";
    CCELL_(Proxi) = sets_.proxi_;

    CCELL_(UseFTP) = sets_.useFtp ? "Да" : "Нет";

    CCELL_(IP_local) = sets_.tcpLocalAddr_;

    #undef CCELL_(ss)
}
//---------------------------------------------------------------------------
TransmitInterfaceSets GetCtrlPrgrmmDlgItfcSets( TFormAppSetsDlg *dlg )
{
	TransmitInterfaceSets sets_;

    #define CCELL_(ss) dlg->grd1->Cells[2][AppSetsGrdItems::Row::##ss##]
    sets_.modbusDeviceAddress_ = CCELL_(ModbusAddy).ToInt();
    sets_.comPort_ = CCELL_(COM);
    sets_.comPortBaudRate_ = CCELL_(COMBoudRate).ToInt();
    sets_.tcpHost_ = CCELL_(IP);
    sets_.tcpPort_ = CCELL_(TCPPort).ToInt();
    sets_.isTCP_  = CCELL_(ConnectChanal)=="TCP";
    sets_.timeOut_ = CCELL_(Timeout).ToInt();
    sets_.tmWriteDelay_ = CCELL_(WaitDelay).ToInt();
    sets_.silentTime_ = CCELL_(SilentTime).ToInt();
    sets_.rxdMaxFrameSize_ = CCELL_(RXDFrameSize).ToInt();
    sets_.timeOut_ = CCELL_(Timeout).ToInt();

    sets_.useProxi_ = CCELL_(UseProxi) == "Да";
    sets_.proxi_ = CCELL_(Proxi);

    sets_.useFtp = CCELL_(UseFTP) =="Да";

    sets_.tcpLocalAddr_ = CCELL_(IP_local);

    return sets_;

}
//---------------------------------------------------------------------------
bool IsNotIdenticChanals( const TransmitInterfaceSets& lhs, const TransmitInterfaceSets& rhs)
{
	#define NOT_EQ_(nn) lhs.##nn##_!=rhs.##nn##_
	return NOT_EQ_(isTCP) ||
    !rhs.isTCP_ && ( NOT_EQ_(comPort) || NOT_EQ_(comPortBaudRate) ) ||
    rhs.isTCP_ && (  NOT_EQ_(tcpHost) || NOT_EQ_(tcpPort)  || NOT_EQ_(tcpLocalAddr) );
    #undef NOT_EQ_(nn)
}
