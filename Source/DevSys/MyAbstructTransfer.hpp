#ifndef MY_ABSTRUCT_TRANSFER_HEADER_INCLUDED_____
#define MY_ABSTRUCT_TRANSFER_HEADER_INCLUDED_____

//std
#include <utility>

// boost
#include "boost\noncopyable.hpp"
#include "boost\shared_ptr.hpp"

//vcl
#include <system.hpp>

//my
#include "..\my_include\free_types_.h"
//------------------------------------------------------------------------------
class MyAbstructTransfer : public boost::noncopyable
{
public:
    MyAbstructTransfer(AnsiString what=" ") : what_(what) {} 
	// Обработка события начала трансфера. Возвращает - выполнять/не выполнять
	virtual bool HandleBeginTransfer() = 0;
    // Обработка события окончания трансфера. Возвращает - повторить/не повторять
	virtual bool HandleEndTransfer() = 0;
	// выполнить. Возвращает - выполнен/не выполнен
    virtual void Perform() = 0;
	AnsiString What() const { return what_; }
    virtual ~MyAbstructTransfer() {}
private:
    const AnsiString what_;
};
//------------------------------------------------------------------------------
typedef boost::shared_ptr<MyAbstructTransfer> PTransfer;
//------------------------------------------------------------------------------
// Делегат для обработки событий трансфера
template<class F>
class SingleBackgroundTask : public MyAbstructTransfer
{
public:
	SingleBackgroundTask( F mthd, const AnsiString what ) : MyAbstructTransfer(what),
    	mthd_(mthd)
    {}
    virtual ~SingleBackgroundTask() {}

private:
    virtual bool HandleBeginTransfer() { mthd_(); return false; }
    virtual bool HandleEndTransfer() { return false; }
    virtual void Perform(){}
    const F mthd_;
};

template<class F>
MyAbstructTransfer* CreateSingleBackgroundTask( F mthd, const AnsiString what = "" )
{
	return new SingleBackgroundTask<F>(mthd, what);
}

///////////////////////////////////////////////////////////////////////////////
// базовый класс для единичных заданий
///////////////////////////////////////////////////////////////////////////////
enum { ONCE_TASK = true, CONTINUESLY_TASK = false };

class HandleHromatTask : public MyAbstructTransfer
{
public:
	explicit HandleHromatTask( bool sendOnce, const AnsiString& what ) :
        MyAbstructTransfer(what),
    	sendOnce_(sendOnce)
    {}
    virtual ~HandleHromatTask(){}
private:
    const bool sendOnce_;

    virtual void DoOnBeginTransfer() {}
    virtual void DoOnEndTransfer() = 0;
	bool HandleBeginTransfer()
    {
    	DoOnBeginTransfer();
    	return true;
    }
    bool HandleEndTransfer() 
    {
    	DoOnEndTransfer();
    	return sendOnce_==ONCE_TASK ? false : true;
    }
};




#endif