//---------------------------------------------------------------------------

#ifndef cnfdntl_H
#define cnfdntl_H
//---------------------------------------------------------------------------

#include <vector>
#include <string>
#include <system.hpp>
#include "..\Application\MyIostream.h"
#include "..\DevSys\SoftID.hpp"

void SaveConfidential();
const Users& GetRegisteredUsers();

// �������� ������������� ��
const HromatSoftID& GetSoftId();

// �������� ��������� ������ �������� ������� ��� �������� CRC16
unsigned short int GetHromatCRC16Key();
void SetupHromatCRC16Key();
void ChangeHromatCRC16Key();

// ������� ������ �������������� ��
bool SoftIDSetupDialog();
bool VerifySoftId();

// ������� ������ �����������
void ExecuteAutorization(const char *actionCaption);

// ������� ������ ���������� ������� �������������
void ExecuteUserControlDialog();

bool WasAutorized();

User AutorizedUser();
bool IsAdminAutorized();

bool IsLabourAutorized();

//      �������� �������� ��������� ����� �������� �����������
//      hromat_sys.cpp
void AddToTaskList_SetHromatCRC16Key(unsigned short oldV, unsigned short newV);


#endif
