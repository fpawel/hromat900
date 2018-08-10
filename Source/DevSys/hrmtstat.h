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
        zmix,                           // ����������� ����������� ��������� ����
        relTghtns,      dRelTghtns,     // ������������� ���������
        absTghtns,      dAbsTghtns,     // ���������� ���������
        Hmhigh,         dHmhigh,        // �������� ������� �������� (������)
        Hmlow,          dHmlow,         // �������� ������� �������� (������)
        Hvhigh,         dHvhigh,        // �������� ������� �������� (������)
        Hvlow,          dHvlow,         // �������� ������� �������� (������)
        Whigh,          dWhigh,         // ����� �����  (������)
        Wlow,           dWlow           // ����� �����  (������)
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
        relTghtns,      dRelTghtns,     // ������������� ���������
        absTghtns,      dAbsTghtns,     // ���������� ���������
        Hmhigh,         dHmhigh,        // �������� ������� �������� (������)
        Hmlow,          dHmlow,         // �������� ������� �������� (������)
        Hvhigh,         dHvhigh,        // �������� ������� �������� (������)
        Hvlow,          dHvlow,         // �������� ������� �������� (������)
        Whigh,          dWhigh,         // ����� �����  (������)
        Wlow,           dWlow           // ����� �����  (������)
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
