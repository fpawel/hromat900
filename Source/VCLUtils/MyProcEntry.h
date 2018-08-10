//---------------------------------------------------------------------------
#ifndef MyProcEntryH
#define MyProcEntryH
//---------------------------------------------------------------------------

// windows
#include <Tlhelp32.h>

//std
#include <vector>

//vcl
#include <system.hpp>

//boost
#include "boost\shared_ptr.hpp"
#include "boost\noncopyable.hpp"


typedef std::vector<PROCESSENTRY32> ProcEntries;
void GetWin32SystemSnapshot(ProcEntries&);

// ��������� �������������� ���������� ����������
bool IsUniqueAppInstance();

// ����
const std::vector<AnsiString>& MyGetParamStr();
AnsiString FormatMyParamsStr();
AnsiString ExtractFromComandPrompt(const AnsiString& key);

// ���������� �� ������ � �����
bool MyParamStrHas( const AnsiString& s );

// ����������� ���������� ����������� ����������
unsigned ApplicationInstanceCount();



//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
