//---------------------------------------------------------------------------
#include <windows.h>

//std
#include <sstream>

#pragma hdrstop

// loki
#include "loki\threads.h"

#include "TransmitChan.h"
#include "..\my_include\CRC16.h"

#include "bcd_.h"


#include "CommSetsIOHelper.h"
#include "mywinsock2.h"


//my
#include "hromat900_ini.h"
#include "ReportContext.hpp"
#include "AnsiStringUtils_.h"
#include "AppEvts.h"
#include "xmlUtils_.h"
#include "MyModbusException.h"
#include "MyAppProfile.h"
#include "guicon.h"
#include "winRS232.h"


//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
int MyTransferException::count_ = 0;
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void CheckModbusStr(const uInt8 *b, const uInt8 *e, unsigned char devAddr, unsigned char commandCode)
{
	const int rxdLen = e-b;
    assert(rxdLen>=0);
    if( rxdLen==0 ) THROW_TRANSFER_EXCEPTION_(NoAnswer, "����������� �� ��������");
    if( rxdLen<4 ) THROW_TRANSFER_EXCEPTION_(AnswerLengthMismatch, "����� ������� ������ ������ 4");

    if( b[0]!=devAddr ) THROW_TRANSFER_EXCEPTION_(AnswerAdressMismatch, "������������ ��������");


    const bool
        cmdCodeMatch    = ( b[1]    ==    commandCode ),
        modbusError     = ( b[1]    ==  ( commandCode | 0x80 ) );

    if( !cmdCodeMatch && !modbusError )
        THROW_TRANSFER_EXCEPTION_(AnswerCommandCodeMismatch, "������������ ����� ������");

    if( GetCRC16( b, e )!=0 ) THROW_TRANSFER_EXCEPTION_(BadCRC16, "��������� ���");
    assert( IsModbusStr(b, e, devAddr, commandCode ) );
}
//------------------------------------------------------------------------------

// ������� ������ � cout
AnsiString PrintVInt8( const VInt8& v, const AnsiString ss, int lim = -1 )
{
    const unsigned maxIdx = (lim==-1) ? v.size() : std::min(lim, v.size() );
    return MyBuffToStr( v.begin(), v.begin() + maxIdx, ss );
}

void PerformDelay(TBoolMthd mthd, unsigned tm)
{
	const unsigned tmStart = GetTickCount();
    while( GetTickCount()-tmStart < tm && !mthd() ) ;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
TransmitInterface::TransmitInterface( TBoolMthd checkCancelTransfer,
    	TVoid_IntStrMthd reportState) :
    logLifeTime_( MakeMyLogQuote("TransmitInterface.[members]") ),
	checkCancelTransfer_(checkCancelTransfer), reportState_(reportState),
    sets_( LoadTransmitInterfaceSets() ),
    tcp_( new VCLClientSocketHlpr ),
    comm_( new my::RS232::Port )
{
    LOG_FOO_LIFETIME_QUOTES;
    boost::shared_ptr<TIniFile> ini( new TIniFile( GetAppProfileDir() + "\\comport.ini" ) );

    // ������������ RS-232
    comm_->SetPortName( sets_.comPort_ );
    my::RS232::Setings commSets;
    my::RS232::LoadSetingsXML( TiXML::GiveNotNulElement( ProfileXmlElem(), COMPORT_SEKT ), commSets );
    comm_->SetSettings(commSets);

    // ��������� ��������� ������ �����
	ApplySettings();

}
//------------------------------------------------------------------------------
TransmitInterface::~TransmitInterface()
{
    LOG_FOO_LIFETIME_QUOTES;

    my::RS232::SaveSetingsXML( TiXML::GiveNotNulElement( ProfileXmlElem(), COMPORT_SEKT ),
        comm_->GetSettings() );
    SaveTransmitInterfaceSets(sets_);
}
//------------------------------------------------------------------------------
void TransmitInterface::Save()
{
    my::RS232::SaveSetingsXML( TiXML::GiveNotNulElement( ProfileXmlElem(), COMPORT_SEKT ),
        comm_->GetSettings() );
    SaveTransmitInterfaceSets(sets_);
}


AnsiString TransmitInterface::Description() const
{
    return chanal_->Description();
}

// �������� txd, ������� ������ �������� � rxd, ������� ������������ ���������
void TransmitInterface::PerformTransfer
(
	unsigned char commandCode,			// ��� ��������
	ByteBuffPtr sendBegin,		// ������ �� ������������ ������
    ByteBuffPtr sendEnd,
    ByteBuffPtr& reciveBegin,			// ������ �� �������� ������
    ByteBuffPtr& reciveEnd
)
{
    // ������� ����� ����� � �������� �����������
    if( !OpenChanal() ) return;

    go_to_begin:
    if( checkCancelTransfer_() ) return;
    // ��������� ��������������� ��������
    PerformDelay( checkCancelTransfer_, sets_.tmWriteDelay_ );

	const int sendStrLen = sendEnd - sendBegin;
    assert(sendStrLen<10000);

	// ����������� �������
    VInt8 txd( sendEnd - sendBegin + 4 );
    // ����������� ������ � �������� � ����������� ������
    txd[0] = sets_.modbusDeviceAddress_;
    txd[1] = commandCode;
    std::copy( sendBegin, sendEnd, txd.begin()+2 );
    const short unsigned int crc16 = GetCRC16 ( txd.begin(), txd.end()-2 );
    *(txd.end()-2) = crc16 >> 8;
    *(txd.end()-1) = crc16;


    assert( IsModbusStr(txd.begin(), txd.end(), sets_.modbusDeviceAddress_ , commandCode) );

    // ������� txd � cout
    MyWCout( PrintVInt8( txd, "TXD " )+"\n" );

    // �������������
    TryPerformTransfer(txd, rxd_);
    // �������� ������������ rxd_ - ������������ ����������
    CheckModbusStr(rxd_.begin(), rxd_.end(), sets_.modbusDeviceAddress_, commandCode);


    // ����������� ���������
    reciveBegin = rxd_.begin()+2;
    reciveEnd   = rxd_.end()-2;

    // ���� ����������� ������ ��� �������������� ��������
    if( rxd_.size()==5 && ( rxd_[1] == (commandCode | 0x80) ) )
    {
    	DeviceModbusException::Tag tag;
        tag.slave = txd[0];
        tag.cmd = txd[1];
        tag.code = rxd_[2];

        if( tag.code==0x06 )
        {
            MyWCout("������ �����.\n") ;
            reportState_( RPT_TRNSFR_CNTXT::HROMAT_BUSY, "����������� �����. ������ ����� �������."  );
            ::Sleep( 1000 );
            goto go_to_begin;
        }

    	throw MyExcptionFactory::Create<DeviceModbusException>( tag, __FILE_LINE__,
            "������ ������������" );
    }
}
//------------------------------------------------------------------------------
void TransmitInterface::TryPerformTransfer(const VInt8& txd, VInt8& rxd_)
{
    // ������� ��������
	chanal_->Purge();
    rxd_.clear();
    
    // ��������� ������
    const unsigned txdSz = txd.size();
    void *pTxdBuf = reinterpret_cast<void*>( const_cast<unsigned char*>(&txd.at(0)) );
    const int actualWritenCount = chanal_->Write( pTxdBuf, txdSz );

    if( (unsigned)actualWritenCount!=txdSz )
    {
        const AnsiString errMsg = AnsiString().sprintf(
            "%s: ������ ��������: �������� %d ���� �� %d!",
            chanal_->Description(), actualWritenCount, txdSz );
        throw MyExcptionFactory::Create<MyException>(__FILE_LINE__, errMsg.c_str() );
    }

    // ���� ���������� �� RxD
    // ��������� ������ ������ ���������� �� RxD
    const unsigned tmReadStart = GetTickCount();
    unsigned tmStart = tmReadStart;
    
    // ���� ���������� �� RxD
    while
    (
        !checkCancelTransfer_() &&
        //!IsModbusStr(rxd_.begin(), rxd_.end(), txd[0], txd[1] ) &&
        (GetTickCount() - tmStart < sets_.timeOut_)
    )
    {
		// ��������� ���������� ������ � ������� �����
        const int rxdSize = chanal_->GetRxDSize();

        // ���� RxD ����, ���������� ����������
        if( rxdSize==0 )
        {
            ::Sleep(1);
            continue;
        }

        // � �������� ��������� ������. ���������
        const unsigned oldReadBuffSize = rxd_.size();
        rxd_.resize(oldReadBuffSize + rxdSize);
        void *pDestinationRead = reinterpret_cast<void*>( &rxd_.at(oldReadBuffSize) );
        chanal_->Read( pDestinationRead, rxdSize );

        // ��� ��� ����� � ��������� ����� RxD. ���� �� 0, ���������� ����������
        // ����� ������� ��� ������� ���������
        ::Sleep( sets_.silentTime_ );
        if( chanal_->GetRxDSize()==0 ) break;
        tmStart = ::GetTickCount();
    }
    
    const unsigned tmEnd = ::GetTickCount(), tmSilent = tmEnd - tmStart;

    MYWCOUT_(" - %d �� - ",
        tmEnd - tmReadStart)  );

    if( tmSilent >= sets_.timeOut_ )
        MyWCout("�������! ") ;

    MYWCOUT_("%d ���� -\n", rxd_.size() )  );

    if( !rxd_.empty() )
        MyWCout( PrintVInt8( rxd_, "RXD ", 64 ) + "\n" );
}
//------------------------------------------------------------------------------
bool TransmitInterface::IsConnected() const
{
    return chanal_->IsConnected();
}
//------------------------------------------------------------------------------
void TransmitInterface::BreakConnection()
{
    tcp_->BreakConnection();
}
// ������� ����� ����� � �������� �����������
bool TransmitInterface::OpenChanal()
{
	if( chanal_->IsConnected() ) return 1;
    const AnsiString dscr = chanal_->Description();
    //reportState_( RPT_TRNSFR_CNTXT::ITFC, MYSPRINTF_("��������� ���������� \"%s\"....", dscr )  );
    MyWCout("������� ���������� ���������� "+ dscr +"\n");

    try
    {
    	chanal_->Connect();
        if( chanal_->ConnectionWasBreaked() )
            return 0;
    }
    catch(...)
    {
    	// ��������� �������, ����� ��������� �� ������ �� ������� ����
    	Sleep(1000);
        throw MyExcptionFactory::Create<MyTransferCantOpenPortException>(
        	__FILE_LINE__, "������ ��� ������������� "+dscr, MY_RETHROW_ );
    }
    return 1;
}
//------------------------------------------------------------------------------
void TransmitInterface::CloseChanal()
{
	if( !chanal_->IsConnected() ) return;

    chanal_->Disconnect();
    MyWCout( "����� ����� \"" + chanal_->Description()+"\" ������.\n" );
    //LogMyAppEvt( "����� ����� ������", chanal_->GetDebugInfo() );
}
//------------------------------------------------------------------------------
// ���������� ��������� �����
void TransmitInterface::SetSets( const TransmitInterfaceSets& newSets)
{   
    const bool mustReopen = IsNotIdenticChanals(sets_, newSets);
    sets_ = newSets;
    Save();
    if ( mustReopen )
    {
        // ������� ������� ����������
    	CloseChanal();
        // ��������� ��������� ������ �����
    	ApplySettings();
    }
}
//------------------------------------------------------------------------------
// ��������� ��������� ������ �����
void TransmitInterface::ApplySettings()
{
	// ������� ����� �����
    if( sets_.isTCP_ ) chanal_ = tcp_.get(); else  chanal_ = comm_.get();
	// COM
    my::RS232::Setings commSets = comm_->GetSettings();
    commSets.dcb_.BaudRate = sets_.comPortBaudRate_;
    comm_->SetPortName( sets_.comPort_ );
    comm_->SetSettings( commSets );
    // ������������ TCP
    tcp_->SetHost( sets_.tcpHost_ );
    tcp_->SetPortNum( sets_.tcpPort_ );    
    LogMyAppEvt( "������� ������������ ������ �����", chanal_->Description() ); 
}









