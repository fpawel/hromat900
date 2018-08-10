//------------------------------------------------------------------------------
#ifndef MY_EXCEPTION_CERTAIN_CLASS_DECLARATION__HEADER_INCLUDED__________
#define MY_EXCEPTION_CERTAIN_CLASS_DECLARATION__HEADER_INCLUDED__________
//------------------------------------------------------------------------------
#include <SysUtils.hpp>
#include "boost\shared_ptr.hpp"
#include "boost\noncopyable.hpp"
	/**
	* ����������� ���� ������ ��������� �� ����������
	*/
    class MyException;
	typedef boost::shared_ptr<const MyException> ConstPMyExcpt;
    typedef boost::shared_ptr<MyException> PMyExcpt;

	/**
    * ��������� ��� ������������ ���������� � ����� ������������� ����������.
	*/
    typedef std::pair<unsigned, AnsiString> FileLine;

    /**
	* ������ ������� �������������� ������� ���������� � ���������� ��������� ��
	* ���� PMyExcpt. ��� ����� ���� ������� ������ �� ����� catch. ���
	* �������� ������� location ������ ������������ ������ _FLINE_.
	* ���� ������������ ��������� ������������� �������������� (�������������)
	* ����������� ��� ��������� ����� ��������� ���������� � �������������
	* ����������� ������������ ����, �� ����� ����� �������� ����������� ����������
	* ������ �������, ������������ ��� �����������.
    */
	PMyExcpt RethrowMyException(const FileLine& location);

	/**
	* ����� ������� ����� ��� ���� ���������� �������. ��� ���������� ������
	* ������������ � ���� sh_ptr<MyException>, ������� �������� ��������� ��
	* ���������� ����������, ����������� �� MyException. � ��������, ���������
	* ����������� ����� ���������� ����� ��������� ������ � ��� ������, ����� �
	* ������������ ������ ���� ����������� �������� ��� �� "������ MyException", �
	* � ��������� ������� ������ �������� � ��� MyException, ����������: "����������
	* ������ ���� ������ �� ��������� ���������".
    */
    class MyException : public boost::noncopyable
    {
    public:

    	MyException(const FileLine& loc, const AnsiString& msg, const PMyExcpt nest = PMyExcpt() );

        /** ����������� ����������. */
	    virtual ~MyException(){}

	    /**
        *   ���������� ��� ������-����������. ������ ����������� ���������������� �
	    *   ����������� �������.
        */
	    virtual AnsiString ClassName() const;

        /**
        *       ����� ������������� ����������
        */
	    const FileLine& Location() const ;

        /**
	    /*      ���������
        */
	    const AnsiString& Message() const;

	    /**
        *       ��������� �� ��������� ����������, ������� ��������, ��� 0
        */
        const MyException* Nested() const;

        /**
        *       ��������� �� ������� �� ��������� ���������� ��� this
        */
        const MyException* Root() const;

        /**
        *   ���������� "������ ��������" ����������, ���������� ��� ������ � �����
	    *   �������������.
        */
	    AnsiString ToString() const;

	    /**
        *   ���������� "������ ��������" �������� ���������� � ���� ����������,
	    *   ��������� � ����. ����������, ���������� ���������� ToString() �
	    *   ������� �� ���������� ������������ � ���� ������.
        */
        AnsiString ToStringAll() const;

        /**
        *       �������������� � �������� ����������� ����
        */
        template<class E>
        const E* const As() const
        {
        	return dynamic_cast<const E* const>(this);
        }

        template<class E>
        bool TypeOf() const
        {
        	return As<E>();
        }
    private:
        // ����� ������������� ����������
	    const FileLine location_;
	    // ���������
	    const AnsiString message_;
	    // ��������� �� ��������� ����������, ������� ��������, ��� 0
	    const PMyExcpt nested_;
    };

    /**
	*		������� ��� ����������� ������� My##THECLASS##Exception,
    *		����������� �� My##THEBASECLASS
	*		� ������������ ���������� �������������
	*		My##THECLASS##Exception(const FileLine& loc, const AnsiString& msg, PMyExcpt nest)
	*/

    #define DECLARATE_AND_DEFINE_MY_EXCEPTION_CLASS_NAME_(THECLASS, THEBASECLASS, THENAME )\
    class My##THECLASS##Exception : public My##THEBASECLASS\
    {\
    public:\
    	My##THECLASS##Exception(const FileLine& loc, const AnsiString& msg, PMyExcpt nest)\
        	:\
 			My##THEBASECLASS##(loc, msg, nest)\
        {}\
    	virtual ~My##THECLASS##Exception(){}\
		virtual AnsiString ClassName() const { return THENAME; }\
    }

    #define DECLARATE_AND_DEFINE_MY_EXCEPTION_CLASS_(THECLASS, THEBASECLASS )\
    DECLARATE_AND_DEFINE_MY_EXCEPTION_CLASS_NAME_(THECLASS, THEBASECLASS, "My"#THECLASS"Exception" )

    /**
	* ������ ��� ��������� �������� ������� FileLine.
	*/
    namespace Private
    {
    	FileLine ExtractFileLine( unsigned line, const char* fileName );
    };


    /**
    *   ��������� ������������� MyException
	*   ������������� ��� �������� �������� ������ ������������. ������� ������
	*   � ��������� ������ �� ���������� ����� ����������, ��������� �,
	*   ��������, ���������� ����������. ��� �������� ������� loc ������
	*   ������������ ������ _FLINE_.
	*/
    struct MyExcptionFactory
    {

        template<class E>
        static PMyExcpt Create(const FileLine& loc, const AnsiString& msg, const PMyExcpt nest = PMyExcpt() )
        {
        	return PMyExcpt( new E(loc, msg, nest) );
        }

        template<class E,typename A>
        static PMyExcpt Create(A arg, const FileLine& loc, const AnsiString& msg, const PMyExcpt nest = PMyExcpt() )
        {
        	return PMyExcpt( new E(arg, loc, msg, nest) );
        }
    };
    #define __FILE_LINE__ Private::ExtractFileLine(__LINE__,__FILE__)

    /**
	*   ���������� ��� ������ RethrowMyException(__FILE_LINE__)
	*/
    #define MY_RETHROW_ RethrowMyException(__FILE_LINE__)
    #define MY_RETHROW_NESTED_(msg) throw PMyExcpt( new MyException(__FILE_LINE__, msg, MY_RETHROW_) );

    /**
	*   ���������� ��� ������ throw MyExcptionFactory::Create<MyException>(__FILE_LINE__, msg )
	*/
    #define MY_THROW_(msg) throw PMyExcpt( new MyException(__FILE_LINE__, msg ) )

    /**
	*   ���������� ��� ������
    *   throw MyExcptionFactory::Create<�����, ����������� �� MyException>(__FILE_LINE__, msg )
	*/
    #define MY_THROW_CLASS_(cls, msg)\
    	throw MyExcptionFactory::Create<##cls##>(__FILE_LINE__, msg )

    //"%eClass" "%eMsg" "%eFile", "%eLine"
    AnsiString FormatMyException( const MyException* excpt,
        const AnsiString& format="%eMsg %eLine %eFile" );
    //--------------------------------------------------------------------------
    AnsiString FormatMyExceptionMessages( const MyException* excpt,
        const AnsiString& delim, const AnsiString& format = "%eMsg" );


    /**
	*		MyExternalException
	*		����� ������� ����� ��� ������������ ����������, ���������� �������� ��
	*		��������� � ��������, ����������� MyException.
	*/
    DECLARATE_AND_DEFINE_MY_EXCEPTION_CLASS_(External, Exception );
	/**
	*      ���������� ��� ������������ std::exception � �������, ����������� �� ����.
	*/
    DECLARATE_AND_DEFINE_MY_EXCEPTION_CLASS_(Std, ExternalException );
    /**
	*  ���������� ��� ������������ Exception �� VCL � �������, ����������� �� ����.
	*/
    DECLARATE_AND_DEFINE_MY_EXCEPTION_CLASS_(VCL, ExternalException );
    /**
	*  ���������� ��� ������������ Exception �� VCL � �������, ����������� �� ����.
	*/
    DECLARATE_AND_DEFINE_MY_EXCEPTION_CLASS_(AccessViolation, ExternalException );
    /**
	*  	���������� ��� �������������� � ������� ����������, ��� �������� �� �������
    *	����������.
	*/
    DECLARATE_AND_DEFINE_MY_EXCEPTION_CLASS_(Unknown, ExternalException );



//------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------
