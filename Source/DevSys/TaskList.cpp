
//---------------------------------------------------------------------------
#pragma hdrstop

#include "TaskList.h"

//my
#include "..\my_include\MyDynamicBitset.h"
#include "MySynchronize.h"

//vcl
#include <dialogs.hpp>
#include "guicon.h"
#include "AnsiStringUtils_.h"


//------------------------------------------------------------------------------

#pragma package(smart_init)

//------------------------------------------------------------------------------
TaskList::TaskList() : mainFormThreadId_( GetCurrentThreadId() )
{}
//------------------------------------------------------------------------------
void TaskList::DebugEdit_GUI()
{
	Merge();
	// получить итераторы списка
    std::vector<IterT> itrs;
    {
    IterT i = body_.begin();
    for( ; i!=body_.end(); ++i ) itrs.push_back(i);
    }
    const unsigned sz = itrs.size();
    if( sz==0 ) return;
    AnsiString s;
    for( unsigned i = 0; i<sz; ++i )
    	s = s + AnsiString().sprintf("%d. %s\n", i+1, (*itrs[i])->What() );
    AnsiString num;
    if( !InputQuery("", s, num) ) return;
    my::BitSet bs = my::StrFToBitset( num.c_str() );
    AnsiString sss = my::BitsetToStrF(bs).c_str();
    for( unsigned i=0; i<sz; ++i) if( my::GetBsBit( i+1, bs) )
    {
    	MYWCOUT_("Удалён трансфер %d. %s...\n", i+1, (*itrs[i])->What() ));
    	body_.erase(itrs[i]);
    }

}
//------------------------------------------------------------------------------
void TaskList::PrintTaskList() const
{
    // получить итераторы списка
    std::vector<ListT::const_iterator> itrs;
    for( ListT::const_iterator i = body_.begin(); i!=body_.end(); ++i ) itrs.push_back(i);

    MyWCout( "Запланированные трансферы:\n");
    for( unsigned i = 0; i<itrs.size(); ++i )
    	MYWCOUT_("%d. %s\n", i+1, (*itrs[i])->What() ));
}
void TaskList::EraseTasks( const AnsiString& nums)
{
    std::vector<ListT::iterator> itrs;
    for( ListT::iterator i = body_.begin(); i!=body_.end(); ++i ) itrs.push_back(i);

    my::BitSet bs = my::StrFToBitset( nums.c_str() );
    for( unsigned i=0; i<itrs.size(); ++i) if( my::GetBsBit( i+1, bs) )
    {
        MYWCOUT_("Удалён трансфер %d. %s...\n", i+1, (*itrs[i])->What() ));
    	body_.erase(itrs[i]);
    }
    Merge();

}
//------------------------------------------------------------------------------
void TaskList::DebugEdit_console()
{
	PrintTaskList();
    MyWCout( "Введите номера удаляемых: ");
    std::string nums;
    if( !std::getline(std::cin, nums) ) return;
    EraseTasks( nums.c_str() );
}
//------------------------------------------------------------------------------
void TaskList::PushBackTail( PTransfer task )
{
    	assert( mainFormThreadId_== GetCurrentThreadId() );
    	tail_.push_back( task );
}
//------------------------------------------------------------------------------
void TaskList::PushFrontHead( PTransfer task )
{
    	assert( mainFormThreadId_== GetCurrentThreadId() );
    	head_.push_front( task );
}
//------------------------------------------------------------------------------
void TaskList::PopFrontBody()
{
    	assert( mainFormThreadId_!=GetCurrentThreadId() || InSendMessage()==TRUE   );
        body_.pop_front();
}
//------------------------------------------------------------------------------
void TaskList::PushBackBody( PTransfer task )
{
    	assert( mainFormThreadId_!=GetCurrentThreadId() || InSendMessage()==TRUE   );
    	body_.push_back( task );
}
//------------------------------------------------------------------------------
PTransfer TaskList::FrontBody() const
{
    assert( mainFormThreadId_!=GetCurrentThreadId() || InSendMessage()==TRUE   );
    return body_.empty() ? PTransfer() : body_.front();
}
//------------------------------------------------------------------------------
LRESULT TaskList::Merge()
{
    	//assert( InSendMessage()==TRUE );
    	body_.insert( body_.begin(), head_.begin(), head_.end() );
        head_.clear();
        while( !tail_.empty() )
        {
        	body_.push_back( tail_.front() );
        	tail_.pop_front();
        }
	return 0;
}
