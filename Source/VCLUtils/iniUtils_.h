//---------------------------------------------------------------------------
#ifndef iniUtils_H
#define iniUtils_H
//---------------------------------------------------------------------------
#include <Forms.hpp>
#include <inifiles.hpp>

namespace Ini
{   
    void SaveWindowPlacement( HWND hWnd, TCustomIniFile* ini, const AnsiString& sekt );
    void LoadWindowPlacement( HWND hWnd, TCustomIniFile* ini, const AnsiString& sekt );

    int GetInteger( TCustomIniFile* ini, const AnsiString& sekt,
    	const AnsiString& id, int valMin, int valMax, int defVal);

};







//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
