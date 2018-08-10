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

// Получить идентификатор ПО
const HromatSoftID& GetSoftId();

// Получить начальный вектор смещения хромата для подсчёта CRC16
unsigned short int GetHromatCRC16Key();
void SetupHromatCRC16Key();
void ChangeHromatCRC16Key();

// вызавть диалог идентификатора ПО
bool SoftIDSetupDialog();
bool VerifySoftId();

// вызвать диалог авторизации
void ExecuteAutorization(const char *actionCaption);

// вызвать диалог управления правами пользователей
void ExecuteUserControlDialog();

bool WasAutorized();

User AutorizedUser();
bool IsAdminAutorized();

bool IsLabourAutorized();

//      добавить трансфер установки ключа подсчёта подлинности
//      hromat_sys.cpp
void AddToTaskList_SetHromatCRC16Key(unsigned short oldV, unsigned short newV);


#endif
