//------------------------------------------------------------------------------
#ifndef MySynchronizeH
#define MySynchronizeH
//------------------------------------------------------------------------------
//std
//#include <vector>
// windows
#include <basetsd.h>
// my
//#include "..\my_include\free_types_.h"
#include "MyIostream.h"




// функтор обработки событий оконной продцедуры
typedef LRESULT (__closure *MyWindowMessageMethod )();
namespace my
{
    void FixMainThread();
    bool IsMainThread();
	//--------------------------------------------------------------------------
	LRESULT SynchronizeMthd( MyWindowMessageMethod );
    void PostCallClosure( MyWindowMessageMethod );
    //--------------------------------------------------------------------------
    template<typename F, typename P1>
    struct AdptCall1
	{
    	explicit AdptCall1(F f, P1 p1): f_(f), p1_(p1) {}
    	LRESULT Call() const  { return f_(p1_); }
    	const F f_; const P1 p1_;
    };
    //--------------------------------------------------------------------------
    template<typename F, typename P1, typename P2>
    struct AdptCall2
	{
    	explicit AdptCall2(F f, P1 p1, P2 p2): f_(f), p1_(p1), p2_(p2) {}
    	LRESULT Call() const  { return f_(p1_,p2_); }
    	const F f_; const P1 p1_; const P2 p2_;
    };
    //--------------------------------------------------------------------------
    template<typename F, typename P1, typename P2, typename P3>
    struct AdptCall3
	{
    	explicit AdptCall3(F f, P1 p1, P2 p2, P3 p3):
            f_(f), p1_(p1), p2_(p2), p3_(p3) {}
    	LRESULT Call() const  { return f_(p1_,p2_, p3_); }
    	const F f_;
        const P1 p1_;
        const P2 p2_;
        const P3 p3_;
    };
    //--------------------------------------------------------------------------
    template<typename F, typename P1, typename P2, typename P3, typename P4>
    struct AdptCall4
	{
    	explicit AdptCall4(F f, P1 p1, P2 p2, P3 p3, P4 p4):
            f_(f), p1_(p1), p2_(p2), p3_(p3), p4_(p4) {}
    	LRESULT Call() const  { return f_(p1_,p2_,p3_,p4_); }
    	const F f_;
        const P1 p1_;
        const P2 p2_;
        const P3 p3_;
        const P3 p4_;
    };
    //--------------------------------------------------------------------------

    template<class F, typename P1>
    LRESULT Synchronize1( F f, P1 p1 )
    {
    	const AdptCall1<F,P1> adpt1( f, p1 );
        return SynchronizeMthd( &adpt1.Call );

    };
    //--------------------------------------------------------------------------
    template<class F, typename P1, typename P2>
    LRESULT Synchronize2( F f, P1 p1, P2 p2 )
    {
    	const AdptCall2<F,P1,P2> adpt2( f, p1, p2 );
        return SynchronizeMthd( &adpt2.Call );

    };
    //--------------------------------------------------------------------------
    template<class F, typename P1, typename P2, typename P3>
    LRESULT Synchronize3( F f, P1 p1, P2 p2, P3 p3 )
    {
    	const AdptCall3<F,P1,P2,P3> adpt3( f, p1, p2, p3 );
        return SynchronizeMthd( &adpt3.Call );

    };
    //--------------------------------------------------------------------------
    template<class F, typename P1, typename P2, typename P3, typename P4>
    LRESULT Synchronize4( F f, P1 p1, P2 p2, P3 p3, P4 p4 )
    {
    	const AdptCall4<F, P1, P2, P3, P4 >  adpt4( f, p1, p2, p3, p4 );
        return SynchronizeMthd( &adpt4.Call );

    };
    //--------------------------------------------------------------------------
};


//------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------
