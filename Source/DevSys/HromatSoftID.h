//---------------------------------------------------------------------------
#ifndef HromatSoftIDH
#define HromatSoftIDH
//---------------------------------------------------------------------------
#include "SoftID.hpp"
#include <system.hpp>
//---------------------------------------------------------------------------
bool HromatSoftIDSetupDialog(HromatSoftID& destSoftId);
//---------------------------------------------------------------------------
// ��������� ������������� ����������� �� -
bool ValidateHromatInlineSoftId( HromatSoftID& id, bool isAdmin );


//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------