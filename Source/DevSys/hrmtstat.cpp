//---------------------------------------------------------------------------
#include <math>


#pragma hdrstop

#include "hrmtstat.h"
#include "kostik_fround.h"
#include <SysUtils.hpp>
//------------------------------------------------------------------------------
#pragma package(smart_init)
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
const double CONST_R = 8.31451;
const double CONST_T = 20;
const double CONST_P = 101.325;
const double Mair = 28.9626;
const double Zair = 0.99963;
const double myKT = CONST_P/CONST_R/(CONST_T+273.15);
//------------------------------------------------------------------------------
double Pow2(double x) { return x*x; }
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
namespace Gas
{
    const Item* CH4()
    {
        int idxCH4 = GetIdx("CH4");
        const Item* gases = GetItems();  
        return idxCH4>-1 && idxCH4<Count() ? &( gases[idxCH4] ): NULL;
    }
}

// расчёт неопределённостей
void CalculateIndetermination( HromatStatistic& dt)
{
    if(Gas::CH4()==NULL) return;
	const Gas::Item* gasV = Gas::GetItems(); 

    const unsigned gasCnt = Gas::Count();
    for( unsigned i=0; i<gasCnt; ++i )
    {
        const Gas::Item&
            gas = gasV[i];          // ссылка на значения констант для i-ого газа
        double
            &c = dt.conc[i],        // ссылка на концентрацию i-ого газа
            &dC = dt.dConc[i];       // ссылка на неопределённость i-ого газа
        dC = gas.Ux0+gas.Ux1*c;
    }
}
//------------------------------------------------------------------------------
// расчёт коэффициента сжимаемости реального газа
void CalculateZMix( HromatStatistic& dt )
{
    if(Gas::CH4()==NULL) return;
	const Gas::Item* gasV = Gas::GetItems(); 
    
    const unsigned gasCnt = Gas::Count();
    double &zmix = dt.zmix;
    zmix = 0;

    for( unsigned i=0; i<gasCnt; ++i )
    {

        const Gas::Item& gas = gasV[i];    // ссылка на значения констант для i-ого газа
        const double
            &c = dt.conc[i];   // ссылка на концентрацию i-ого газа

            zmix += ( c * gas.sqrtb / 100 );
    }
    zmix = 1 - Pow2(zmix);

}
//------------------------------------------------------------------------------
// расчёт относительной плотности идеального и реального газа
void CalculateRelativeTightnes( HromatStatistic& dt )
{
    if(Gas::CH4()==NULL) return;
    const Gas::Item ch4 = *Gas::CH4();
	const Gas::Item* gasV = Gas::GetItems();
    
    const unsigned gasCnt = Gas::Count();
    double iProbaD0 = 0, iProbaD1 = 0;
    const double &zmix = dt.zmix;

    for( unsigned i=0; i<gasCnt; ++i )
    {
        const Gas::Item& gas = gasV[i];    // ссылка на значения констант для i-ого газа
        const double
            &c = dt.conc[i],        // ссылка на концентрацию i-ого газа
            &dC = dt.dConc[i],      // ссылка на неопределённость i-ого газа
            kItR = Pow2( (gas.M-ch4.M) * dC / 100 ) ;

        iProbaD0 += c*gas.M/Mair/100;
        iProbaD1 += kItR;
    }
 	//для реального газа     4
    dt.relTghtns  = iProbaD0*Zair/zmix,
    dt.dRelTghtns = std::sqrt( iProbaD1 );


}
//------------------------------------------------------------------------------
// расчёт абсолютной плотности идеального и реального газа
void CalculateAbsoluteTightnes( HromatStatistic& dt )
{
    if(Gas::CH4()==NULL) return;
    const Gas::Item ch4 = *Gas::CH4();
    
	const Gas::Item* gasV = Gas::GetItems();
    
    const unsigned gasCnt = Gas::Count();
    double iProbaRO0 = 0, iProbaRO1 = 0;
    const double &zmix = dt.zmix;

    for( unsigned i=0; i<gasCnt; ++i )
    {
        const Gas::Item& gas = gasV[i];    // ссылка на значения констант для i-ого газа
        const double
            &c = dt.conc[i],        // ссылка на концентрацию i-ого газа
            &dC = dt.dConc[i],      // ссылка на неопределённость i-ого газа
            kItR = Pow2( (gas.M-ch4.M) * dC /100 );

        //расчёт абсолютной плотности
        iProbaRO0 += c * gas.M / 100;
        iProbaRO1 += kItR;
    }
    //расчёт абсолютной плотности  4
    try
    {
        dt.absTghtns = iProbaRO0 * myKT / zmix;
    }
    catch(...)
    {
        dt.absTghtns = 0;
    }
    dt.dAbsTghtns = std::sqrt(iProbaRO1) * myKT;

    // HromatNormValIndet( dt.absTghtns, dt.dAbsTghtns);

    try
    {
        dt.dRelTghtns = dt.relTghtns*dt.dAbsTghtns/dt.absTghtns;
    }
    catch(...)
    {
        dt.dRelTghtns = 0;
    }
}
//------------------------------------------------------------------------------
//расчёт молярной теплоты сгорания идеального газа
void CalculateHm( HromatStatistic& dt )
{
    if(Gas::CH4()==NULL) return;
    const Gas::Item ch4 = *Gas::CH4();

	const Gas::Item* gasV = Gas::GetItems();
    
    const unsigned gasCnt = Gas::Count();
    double
        iProbaHmhigh0 = 0,
        iProbaHmhigh1 = 0,
        iProbaHmlow0 = 0,
        iProbaHmlow1 = 0;

    for( unsigned i=0; i<gasCnt; ++i )
    {
        const Gas::Item& gas = gasV[i];    // ссылка на значения констант для i-ого газа
        const double
            &c = dt.conc[i];        // ссылка на концентрацию i-ого газа

        //расчёт молярной теплоты сгорания идеального газа (высшей)
        iProbaHmhigh0 += c * gas.h0 / 100;

        //расчёт молярной теплоты сгорания идеального газа (низшей)
        iProbaHmlow0  += c * gas.h1 / 100;
    }

    for( unsigned i=0; i<gasCnt; ++i )
    {
        const Gas::Item& gas = gasV[i];    // ссылка на значения констант для i-ого газа
        const double
            &c = dt.conc[i],        // ссылка на концентрацию i-ого газа
            &dC = dt.dConc[i];      // ссылка на неопределённость i-ого газа

        //расчёт молярной теплоты сгорания идеального газа (высшей)
        const double add1 =
        (
            Pow2( (gas.h0-ch4.h0) * dC / 100 ) +
            Pow2( gas.h0 * c * gas.Uh0 / 10000 )
        );

        //расчёт молярной теплоты сгорания идеального газа (низшей)
        const double add2 =
        (
            Pow2( (gas.h1-ch4.h1) * dC / 100 ) +
            Pow2( gas.h1 * c * gas.Uh0 / 10000 )
        );

        iProbaHmhigh1 += add1;
        iProbaHmlow1 += add2;
    }

    //расчёт молярной теплоты сгорания идеального газа (высшей)   2
    dt.Hmhigh = iProbaHmhigh0;
    dt.dHmhigh = std::sqrt( iProbaHmhigh1 );
    // HromatNormValIndet( dt.Hmhigh, dt.dHmhigh);

    //расчёт молярной теплоты сгорания идеального газа (низшей)
    dt.Hmlow = iProbaHmlow0;
    dt.dHmlow = std::sqrt( iProbaHmlow1 );
    //HromatNormValIndet( dt.Hmlow, dt.dHmlow);

    //std::swap (dt.Hmlow, dt.Hmhigh);
    //std::swap (dt.dHmlow, dt.dHmhigh);
}
//------------------------------------------------------------------------------
//расчёт объёмной теплоты сгорания идеального газа
void CalculateHv( HromatStatistic& dt )
{
    if(Gas::CH4()==NULL) return;
    //для идеального газа
    const double
	    iProbaHhigh0 = dt.Hmhigh * myKT,
        iProbaHlow0 = dt.Hmlow * myKT;

    //для реального газа                    2
    dt.Hvhigh = iProbaHhigh0 / dt.zmix;
	dt.dHvhigh = dt.dHmhigh * myKT;
    // HromatNormValIndet( dt.Hvhigh, dt.dHvhigh);

    dt.Hvlow = iProbaHlow0 / dt.zmix;
	dt.dHvlow = dt.dHmlow * myKT;
    // HromatNormValIndet( dt.Hvlow, dt.dHvlow);

    //std::swap (dt.Hvlow, dt.Hvhigh);
    //std::swap (dt.dHvlow, dt.dHvhigh);
}
//------------------------------------------------------------------------------
//расчёт числа Воббе
void CalculateWobbe( HromatStatistic& dt )
{
    if(Gas::CH4()==NULL) return;
    if
    (
        ( dt.relTghtns <= 0 ) ||
        ( dt.dRelTghtns <= 0 ) ||
        ( dt.absTghtns <= 0 ) ||
        ( dt.dAbsTghtns <= 0 )
    )
    {
        dt.Whigh = 0;
        dt.dWhigh = 0;
        dt.Wlow = 0;
        dt.dWlow = 0;
        return;
    }
    const double tmp = Pow2( dt.dAbsTghtns/2/dt.absTghtns );

		//для реального газа        2
    dt.Whigh = dt.Hvhigh / std::sqrt( dt.relTghtns );
    dt.dWhigh = dt.Whigh * std::sqrt
        (
            Pow2( dt.dHvhigh/dt.Hvhigh ) + tmp
        ),

    dt.Wlow = dt.Hvlow / std::sqrt( dt.relTghtns );
    dt.dWlow = dt.Wlow * std::sqrt
        (
            Pow2( dt.dHvlow/dt.Hvlow ) + tmp
        )
    ;
    // HromatNormValIndet( dt.Wlow, dt.dWlow);
}

//------------------------------------------------------------------------------
void CalculateHromatStatistic( HromatStatistic& dt)
{
    if(Gas::CH4()==NULL) return;
    // расчёт неопределённостей
    CalculateIndetermination( dt );

    // расчёт коэффициента сжимаемости реального газа
    CalculateZMix( dt );

	// расчёт относительной плотности идеального и реального газа
    CalculateRelativeTightnes( dt );

    // расчёт абсолютной плотности идеального и реального газа
    CalculateAbsoluteTightnes( dt );

    //расчёт молярной теплоты сгорания идеального и реального газа
    CalculateHm( dt );

    //расчёт объёмной теплоты сгорания идеального и реального газа
    CalculateHv( dt );

    //расчёт числа Воббе
    CalculateWobbe( dt );

}
//------------------------------------------------------------------------------
AnsiString MyFormatFloat( double v, int n )
{
	if( n<1 ) n = 1;
    if( n>50 ) n = 50;
    return FormatFloat( "#0." + AnsiString::StringOfChar('0', n), v);
}


AnsiString HromatStatisticConcToStr( const HromatStatistic& stat, unsigned idx )
{
    const double val = stat.conc[idx], dVal = stat.dConc[idx];
    int n;
    KostikGOSTRound( val, &n );
    return MyFormatFloat( val, n ) + " ± " + MyFormatFloat( dVal, n );
}

void OkruglitHromatStatistic( HromatStatistic& stat )
{
    const unsigned gasCnt = Gas::Count();
    for( unsigned i=0; i<gasCnt; ++i )
    {
        double &c = stat.conc[i], &dC = stat.dConc[i];
        HromatNormValIndet( c, dC );
    }

    MakeKostikFRound( stat.relTghtns,      4 );
    MakeKostikFRound( stat.dRelTghtns,      4 );


    MakeKostikFRound( stat.absTghtns,      4 );
    MakeKostikFRound( stat.dAbsTghtns,      4 );


    MakeKostikFRound( stat.Hmhigh,         2 );
    MakeKostikFRound( stat.dHmhigh,         2 );


    MakeKostikFRound( stat.Hmlow,          2 );
    MakeKostikFRound( stat.dHmlow,          2 );


    MakeKostikFRound( stat.Hvhigh,         2 );
    MakeKostikFRound( stat.dHvhigh,         2 );


    MakeKostikFRound( stat.Hvlow,          2 );
    MakeKostikFRound( stat.dHvlow,          2 );


    MakeKostikFRound( stat.Whigh,          2 );
    MakeKostikFRound( stat.dWhigh,          2 );


    MakeKostikFRound( stat.Wlow,           2 );
    MakeKostikFRound( stat.dWlow,           2 );


}

HromatPhysChem HromatStatisticToHromatPhysChem( const HromatStatistic& stt)
{
	HromatPhysChem ret = {0};
    ret.relTghtns[0] = stt.relTghtns;
    ret.relTghtns[1] = stt.dRelTghtns;

    ret.absTghtns[0] = stt.absTghtns;
    ret.absTghtns[1] = stt.dAbsTghtns;

    ret.hiHm[0] = stt.Hmhigh;
    ret.hiHm[1] = stt.dHmhigh;

    ret.loHm[0] = stt.Hmlow;
    ret.loHm[1] = stt.dHmlow;

    ret.hiH[0] = stt.Hvhigh;
    ret.hiH[1] = stt.dHvhigh;

    ret.loH[0] = stt.Hvlow;
    ret.loH[1] = stt.dHvlow;

    ret.hiW[0] = stt.Whigh;
    ret.hiW[1] = stt.dWhigh;

    ret.loW[0] = stt.Wlow;
    ret.loW[1] = stt.dWlow;

    return ret;


}

