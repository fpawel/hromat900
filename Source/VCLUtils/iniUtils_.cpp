//---------------------------------------------------------------------------
#pragma hdrstop

#include "iniUtils_.h"
#include <cassert>

//---------------------------------------------------------------------------

#pragma package(smart_init)


namespace Ini
{

void SaveWindowPlacement( HWND hWnd, TCustomIniFile* ini, const AnsiString& sekt )
{
    WINDOWPLACEMENT wndpl = {0};
    GetWindowPlacement(hWnd, &wndpl);
    #define SAVE_(nn) ini->WriteInteger(sekt,""#nn, wndpl.##nn )
    SAVE_(length);
    SAVE_(flags);
    SAVE_(showCmd);
    SAVE_(ptMinPosition.x);
    SAVE_(ptMinPosition.y);
    SAVE_(ptMaxPosition.x);
    SAVE_(ptMaxPosition.y);
    SAVE_(rcNormalPosition.left);
    SAVE_(rcNormalPosition.top);
    SAVE_(rcNormalPosition.right);
    SAVE_(rcNormalPosition.bottom);
    #undef SAVE_
}


void LoadWindowPlacement( HWND hWnd, TCustomIniFile* ini, const AnsiString& sekt )
{
    WINDOWPLACEMENT wndpl = {0};
    GetWindowPlacement(hWnd, &wndpl);
    #define LOAD_(nn) wndpl.##nn = ini->ReadInteger(sekt,""#nn, wndpl.##nn )
    LOAD_(length);
    LOAD_(flags);
    LOAD_(showCmd);
    LOAD_(ptMinPosition.x);
    LOAD_(ptMinPosition.y);
    LOAD_(ptMaxPosition.x);
    LOAD_(ptMaxPosition.y);
    LOAD_(rcNormalPosition.left);
    LOAD_(rcNormalPosition.top);
    LOAD_(rcNormalPosition.right);
    LOAD_(rcNormalPosition.bottom);
    #undef LOAD_
    SetWindowPlacement(hWnd, &wndpl);
}

int GetInteger( TCustomIniFile* ini, const AnsiString& sekt,
	const AnsiString& id, int valMin, int valMax, int defVal)
{
    assert(valMin<=valMax);
    
	const int val = ini->ReadInteger(sekt, id, defVal);
    return ( val>=valMin && val<=valMax ) ? val : defVal;
}

};// namespace Ini

