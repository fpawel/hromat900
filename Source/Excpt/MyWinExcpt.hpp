//------------------------------------------------------------------------------
#ifndef MY_WINDOES_EXCEPTION_CLASS_DECLARATION__HEADER_INCLUDED__________
#define MY_WINDOES_EXCEPTION_CLASS_DECLARATION__HEADER_INCLUDED__________
//------------------------------------------------------------------------------

//my
#include "MyExcpt.hpp"

	/**
	*  	���������� ��� �������������� �� ������ ������ ������� Win32 API
	*/    

	class MyWindowsException : public MyException
    {
 	public:

    	MyWindowsException(unsigned long int code, const FileLine& loc,
        	const AnsiString& msg, PMyExcpt nest = PMyExcpt() );
        MyWindowsException(const FileLine& loc,
            const AnsiString& msg, PMyExcpt nest = PMyExcpt() );
        unsigned Code() const { return code_; }
      	virtual AnsiString ClassName() const;
    private:
    	unsigned code_;
    	AnsiString MakeMsg(const AnsiString& msg, unsigned long int code );
    };
    
    /**
	*		������ ��� ������ ������� Win32 API.
    *		���� ����������� ������ ����� FALSE, ������������ ���������� MyWindowsException.
    *		������: MY_SAFE_CALL_WIN_API(SetupComm,(hComm_, txdBuffSize_, rxdBuffSize_) );
	*/
    #define MY_SAFE_CALL_WIN_API(foo,params)\
		if(!::##foo##params ) throw MyExcptionFactory::Create<MyWindowsException>\
		(::GetLastError(),__FILE_LINE__,"::"#foo);




//------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------