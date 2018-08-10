//---------------------------------------------------------------------------
#include <vcl.h>
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#pragma hdrstop
//---------------------------------------------------------------------------
#include "uFormArchiveView.h"
//---------------------------------------------------------------------------
//#include "MyIostream.h"
#include "guicon.h"
//#include "AppEvts.h"
//#include "MyProcEntry.h"
//#include "MyWindow.h"
#include "MyAppProfile.h"
//#include "AnsiStringUtils_.h"
//#include "MyFileCtrl.h"
//#include "xmlUtils_.h"
//#include "HromatCrypt.h"
//#include "MyExcpt.hpp"


USEFORM("Dialogs\uFormArchiveView.cpp", FormArchiveView);
USEFORM("uFrameGraph.cpp", FrameGraph); /* TFrame: File Type */
USEFORM("uFrameDropDownPanel.cpp", FrameDropDownPanel); /* TFrame: File Type */
USEFORM("Dialogs\uFormAppProfileDlg.cpp", FormAppProfileDlg);
USEFORM("Dialogs\uFrmWait.cpp", FrmWait);
USEFORM("Dialogs\uFormCorruptedFilesMessageDialog.cpp", FormCorruptedFilesMessageDialog);
USEFORM("VCLUtils\Dlg\uFormGridColsDlg.cpp", FormGridColsDlg);
//---------------------------------------------------------------------------
#define THIS_APPLICATION_TITLE "Хромат-900"
//---------------------------------------------------------------------------
void RunVCLApplication()
{
    LOG_FOO_LIFETIME_QUOTES;
	try
	{
		Application->Initialize();
		Application->Title = "Архив хроматограм";
		Application->CreateForm(__classid(TFormArchiveView), &FormArchiveView);
		Application->Run();
	}
	catch (Exception &exception)
	{
		Application->ShowException(&exception);
	}
	catch (...)
	{
		try
		{
			throw Exception("");
		}
		catch (Exception &exception)
		{
			Application->ShowException(&exception);
		}
	}
}
//------------------------------------------------------------------------------

WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    // инициализация ГУИшной консоли
    GUIConsole& guiCon = GUIConsole::Instance();

    // инициализация профиля приложения
    const AnsiString appIdInst = GetApplicationProfileId();
    guiCon.InitializePaths();
    guiCon.Create();
    RunVCLApplication();
    delete FormArchiveView;
    SaveAppConfigFiles();
    // закрыть консоль
    ::FreeConsole();
	return 0;
}
//---------------------------------------------------------------------------
