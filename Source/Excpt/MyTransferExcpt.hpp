//------------------------------------------------------------------------------
#ifndef MY_TRANSFER_EXCEPTION_CLASS_DECLARATION__HEADER_INCLUDED__________
#define MY_TRANSFER_EXCEPTION_CLASS_DECLARATION__HEADER_INCLUDED__________
//------------------------------------------------------------------------------

//my
#include "MyExcpt.hpp"

	namespace TRANSFER_EXCEPTION
    {
		enum
    	{
        	NO_ANSWER,
            LESS_THEN_4,
            ADDRESS_MISMATCH,
            COMMAND_MISMATCH,
            NOT_NULL_CRC16,
            CMD3_ADDRESS_NOT_SUPPORTED,
            MODBUS_EXCEPTION,
            COUNT
    	};
    };

	class MyTransferException : public MyException
    {
 	public:

    	MyTransferException(unsigned context, const FileLine& loc, const AnsiString& msg )
        	:
        	MyException( loc, msg ),
            context_(context)
        {}
      	virtual AnsiString ClassName() const { return "MyTransferException"; }
        unsigned Context() const { return context_; }
    private:
    	unsigned context_;
    };

    bool IsNoAnswerException( const PMyExcpt e)
    {
        if(!e) return false;
    	const MyTransferException *pE = e->As<MyTransferException>(); 
    	return pE!=NULL && pE->Context()==TRANSFER_EXCEPTION::NO_ANSWER;
    }






//------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------