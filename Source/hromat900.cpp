//---------------------------------------------------------------------------
#include <vcl.h>
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#pragma hdrstop
//---------------------------------------------------------------------------
#include "Unit1.h"
//---------------------------------------------------------------------------
#include "MyIostream.h"
#include "guicon.h"
#include "AppEvts.h"
#include "MyProcEntry.h"
#include "MyWindow.h"
#include "MyAppProfile.h"
#include "AnsiStringUtils_.h"
#include "MyFileCtrl.h"
#include "xmlUtils_.h"
#include "HromatCrypt.h"

#include "hromat_sys.h"
#include "cnfdntl_.h"
#include "MyExcpt.hpp"

#define THIS_APPLICATION_TITLE "Хромат-900"

void OutLine()
{
	std::cout << '\n' << myIO::LongLine(75, '-') << '\n';
}

//---------------------------------------------------------------------------
USEFORM("Unit1.cpp", Form1);
USEFORM("uFormHromatogramm.cpp", FormHromatogramm);
USEFORM("Dialogs\uFrmWait.cpp", FrmWait);
USEFORM("uFrameGraph.cpp", FrameGraph); /* TFrame: File Type */
USEFORM("Dialogs\uFormPikDlg.cpp", FormPikDlg);
USEFORM("VCLUtils\Dlg\uFormGridColsDlg.cpp", FormGridColsDlg);
USEFORM("Dialogs\uFormCorruptedFilesMessageDialog.cpp", FormCorruptedFilesMessageDialog);
USEFORM("Autorize\autorize_form.cpp", AutorizeForm);
USEFORM("Autorize\change_pass_frame.cpp", ChangePassFrame); /* TFrame: File Type */
USEFORM("uFormEvts.cpp", FormEvts);
USEFORM("Autorize\AdminFormUnit.cpp", adminForm);
USEFORM("Dialogs\uFormAppProfileDlg.cpp", FormAppProfileDlg);
USEFORM("Dialogs\uFormSoftIDDlg.cpp", FormSoftIDDlg);
USEFORM("uFormAbout.cpp", FormAbout);
USEFORM("Dialogs\uTestCalcHromatSatat.cpp", FormTestCalcStatDlg);
USEFORM("uFrameGrid.cpp", FrameGrid); /* TFrame: File Type */
USEFORM("Dialogs\uAppSetsDlg.cpp", FormAppSetsDlg);
USEFORM("Dialogs\uHromatSetsDlg.cpp", FormHromatSetsDlg);
USEFORM("uFrameDropDownPanel.cpp", FrameDropDownPanel); /* TFrame: File Type */
USEFORM("Dialogs\uFormArchiveView.cpp", FormArchiveView);
USEFORM("Dialogs\uFormGraduate.cpp", FormGraduate);
USEFORM("Dialogs\uFormAnalyze.cpp", FormAnalyze);
USEFORM("Dialogs\uFormHtgFilter.cpp", FormHtgFilter);
USEFORM("uFormTimeFilterDlg.cpp", FormTimeFilterDlg);
USEFORM("DevSys\uFormReportAnalizeArchive.cpp", FormReportAnalizeArchive);
USEFORM("uDateTimeRangeDialogForm.cpp", DateTimeRangeDialog);
USEFORM("uFormIzobutChart.cpp", FormIzobutChart);
//---------------------------------------------------------------------------
void RunVCLApplication()
{
    LOG_FOO_LIFETIME_QUOTES;
	try
	{
		Application->Initialize();
		Application->Title = THIS_APPLICATION_TITLE;
		Application->CreateForm(__classid(TForm1), &Form1);
        Application->CreateForm(__classid(TFormTestCalcStatDlg), &FormTestCalcStatDlg);
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
    MyExtractFileFromResource( "Windows Hardware Fail.wav", "WARN_SOUND_FILE", true );

    try
    {
        // создать Hromat900
        Hromat900::Instance();

        // инициализация профиля приложения
        const AnsiString appIdInst = GetApplicationProfileId();
        boost::shared_ptr<void> hAppInstIdMtx( CreateApplicationProfileMutex(appIdInst) );
        if( appIdInst.IsEmpty() || !hAppInstIdMtx ) return 0;

        const AnsiString conParam = ExtractFromComandPrompt("-con:");
        if( conParam!="no" )
        {
            const AnsiString &logDir = GetAppProfileDir()+"\\";
            if( conParam=="gui" )
                guiCon.Create(logDir);
            else
                guiCon.Redirect(logDir);
        }
        std::cout << "<WinMain>\n";
        OutLine();

        ExecuteAutorization("Вход в систему управления Хромат-900");
        if( !WasAutorized() ) return 0;

        LogMyAppEvt("Запуск приложения", MYSPRINTF_("Профиль: \"%s\", пользователь: \"%s\"",
    	    appIdInst, NormString( AutorizedUser().name.c_str() )  ), FormatMyParamsStr() );
        OutLine();

        RunVCLApplication();
        MyWCout("Удаляем Form1...\n");
        delete Form1;
        MyWCout("Удаляем Hromat900...\n");
        delete &Hromat900::Instance();
    }
    catch ( ... ) {
    	const PMyExcpt excpt = MY_RETHROW_;
        const AnsiString msg1 = FormatMyExceptionMessages( excpt.get(), "\n\t",
            "%eFile:%eLine %eMsg"  );
        const AnsiString msg = FormatMyExceptionMessages( excpt.get(), "\n", "%eMsg"  );
        MyWCout("Исключение! Выход из приложения!\n"+msg1+"\n" );
        MyMessageBox(NULL, msg.c_str(), THIS_APPLICATION_TITLE,
        	MB_OK | MB_ICONERROR | MB_SYSTEMMODAL, THIS_APPLICATION_TITLE   );
    }

    // последний вывод в журнал событий
    LogMyAppEvt("Выход из приложения");

   

    // закрыть консоль
    ::FreeConsole();
	return 0;
}
//---------------------------------------------------------------------------
