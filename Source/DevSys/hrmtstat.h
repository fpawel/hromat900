//------------------------------------------------------------------------------
#ifndef hrmtstatH
#define hrmtstatH
//------------------------------------------------------------------------------
#include "Hromat900.hpp"
#include "HromatGas.hpp"
//------------------------------------------------------------------------------
typedef struct HromatStatisticTag
{
    double
        conc[MAX_GAZCOMP],
        dConc[MAX_GAZCOMP]
    ;

    double
        zmix,                           // коэффициент сжимаемости реального газа
        relTghtns,      dRelTghtns,     // относительная плотность
        absTghtns,      dAbsTghtns,     // абсолютная плотность
        Hmhigh,         dHmhigh,        // молярная теплота сгорания (высшая)
        Hmlow,          dHmlow,         // молярная теплота сгорания (низшая)
        Hvhigh,         dHvhigh,        // объёмная теплота сгорания (высшая)
        Hvlow,          dHvlow,         // объёмная теплота сгорания (низшая)
        Whigh,          dWhigh,         // число Воббе  (высшее)
        Wlow,           dWlow           // число Воббе  (низшее)
    ;
    char nouse[6];
    unsigned short int crc16;
} HromatStatistic;
//------------------------------------------------------------------------------
typedef struct { char s[256]; } HromatValStr;
//typedef double HromatValStr;
//------------------------------------------------------------------------------
enum
{
    V_CONCENTRATION,
    V_REL_TIGHTNES,
    V_ABS_TIGHTNES,
    V_HM,
    V_HV,
    V_WB
};
typedef struct HromatStatisticRepresentationTag
{
    HromatValStr
        conc[MAX_GAZCOMP],
        dConc[MAX_GAZCOMP]
    ;

    HromatValStr
        relTghtns,      dRelTghtns,     // относительная плотность
        absTghtns,      dAbsTghtns,     // абсолютная плотность
        Hmhigh,         dHmhigh,        // молярная теплота сгорания (высшая)
        Hmlow,          dHmlow,         // молярная теплота сгорания (низшая)
        Hvhigh,         dHvhigh,        // объёмная теплота сгорания (высшая)
        Hvlow,          dHvlow,         // объёмная теплота сгорания (низшая)
        Whigh,          dWhigh,         // число Воббе  (высшее)
        Wlow,           dWlow           // число Воббе  (низшее)
    ;
    char nouse[6];
    unsigned short int crc16;
} HromatStatisticRepresentation;
//------------------------------------------------------------------------------


void CalculateHromatStatistic( HromatStatistic& out);

void OkruglitHromatStatistic( HromatStatistic& stat );

HromatPhysChem HromatStatisticToHromatPhysChem( const HromatStatistic&);

//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
