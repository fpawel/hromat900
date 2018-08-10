//---------------------------------------------------------------------------
#ifndef TransmitChanH
#define TransmitChanH
//---------------------------------------------------------------------------
//std

// boost
#include "boost\noncopyable.hpp"
#include "boost\shared_ptr.hpp"

// my
#include "..\my_include\myport.hpp"
#include "..\my_include\free_types_.h"
#include "TransmitInterfaceSets.h"
#include "MyExcpt.hpp"
#include "MyModbusException.h"

// ���������� ����������
#include "MySynchronize.h"


class VCLClientSocketHlpr;
namespace my{ namespace RS232{ class Port; };};   


bool IsMyTransferException(const PMyExcpt );
bool IsMyTransferNoAnswerException(const PMyExcpt );

// ����� ����������� �������������� ��������, ��������� � �������������� 
int MyTransferExceptionCount();

class TransmitInterface : public boost::noncopyable
{
public:

    explicit TransmitInterface(
    	TBoolMthd checkCancelTransfer,
        TVoid_IntStrMthd reportState );
    ~TransmitInterface();

    // �������� txd, ������� ������ �������� � rxd
    void PerformTransfer
    (
    	unsigned char commandCode,			// ��� ��������
		ByteBuffPtr sendBegin,		// ������ �� ������������ ������
    	ByteBuffPtr sendEnd,
    	ByteBuffPtr& reciveBegin,			// ������ �� �������� ������
        ByteBuffPtr& reciveEnd
    );

    const TransmitInterfaceSets& GetSets() const { return sets_; }
    void SetSets( const TransmitInterfaceSets& );
       
    // ������� ����� ����� � �������� �����������
    bool OpenChanal();
    // ������� ������� ����������
    void CloseChanal();

    bool IsConnected() const;

    AnsiString Description() const;

    void BreakConnection();
    void Save();

private:
    const boost::shared_ptr<void> logLifeTime_;

	const TBoolMthd checkCancelTransfer_;
    const TVoid_IntStrMthd reportState_;
    TransmitInterfaceSets sets_;
    VInt8 rxd_;

    boost::shared_ptr<VCLClientSocketHlpr> tcp_;
    boost::shared_ptr<my::RS232::Port> comm_;
    MyPort* chanal_;

    void TryPerformTransfer(const VInt8& txd, VInt8& rxd);

    // ��������� ��������� ������ �����
    void ApplySettings();
};

	/**
	*		���������� MyTransferException - ������� ��� ���� ���������� ������
	*/
class MyTransferException : public MyExternalException
{
public:
    virtual ~MyTransferException() {}
    virtual AnsiString ClassName() const { return "MyTransferException"; }
    static int Count() { return count_; }
protected:
	MyTransferException(const FileLine& loc, const AnsiString& msg, const PMyExcpt nest ) :
    	MyExternalException(loc, msg, nest )
	{
    	++count_;
    }
private:
	static int count_;
};

	/**
	*		���������� MyTransferCantOpenPortException
	*/
DECLARATE_AND_DEFINE_MY_EXCEPTION_CLASS_( TransferCantOpenPort, ExternalException );

    /**
	*		���������� MyBadAnswerException
	*/
DECLARATE_AND_DEFINE_MY_EXCEPTION_CLASS_( TransferBadAnswer, TransferException );

    /**
	*		���������� MyTransferNoAnswerException
	*/
DECLARATE_AND_DEFINE_MY_EXCEPTION_CLASS_( TransferNoAnswer, TransferBadAnswerException );

    /**
	*		���������� MyTransferAnswerLengthMismatchException
	*/
DECLARATE_AND_DEFINE_MY_EXCEPTION_CLASS_( TransferAnswerLengthMismatch, TransferBadAnswerException );

    /**
	*		���������� MyTransferAnswerAdressMismatchException
	*/
DECLARATE_AND_DEFINE_MY_EXCEPTION_CLASS_( TransferAnswerAdressMismatch, TransferBadAnswerException );

    /**
	*		���������� MyTransferAnswerCommandCodeMismatchException
	*/
DECLARATE_AND_DEFINE_MY_EXCEPTION_CLASS_( TransferAnswerCommandCodeMismatch, TransferBadAnswerException );

    /**
	*		���������� MyTransferBadCRC16Exception
	*/
DECLARATE_AND_DEFINE_MY_EXCEPTION_CLASS_( TransferBadCRC16, TransferBadAnswerException );
#undef MY_THROW_
#undef MY_THROW_CLASS_
#define THROW_TRANSFER_EXCEPTION_(THECLASS, MSG)\
	throw MyExcptionFactory::Create<MyTransfer##THECLASS##Exception>(__FILE_LINE__, MSG)





//------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------

