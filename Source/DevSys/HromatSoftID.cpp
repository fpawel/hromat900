//------------------------------------------------------------------------------
#pragma hdrstop
//------------------------------------------------------------------------------
#include "HromatSoftID.h"
//------------------------------------------------------------------------------
// boost
#include "boost\noncopyable.hpp"
#include "boost\shared_ptr.hpp"
//------------------------------------------------------------------------------
#include "AnsiStringUtils_.h"
#include "autorize_system.h"
#include "AppEvts.h"
#include "MySynchronize.h"

#include "HromatModbus.h"
//------------------------------------------------------------------------------
#include "uFormAbout.h"
#include "uFormSoftIDDlg.h"
#include "MyFileCtrl.h"
//------------------------------------------------------------------------------
#pragma package(smart_init)
//------------------------------------------------------------------------------
const unsigned short
    crc16OfHromat900HrmtlchkDotDLL = CalcFileCRC16( ExtractFilePath(
    ParamStr(0) ) + "hrmtlchk.dll" );

//------------------------------------------------------------------------------
bool StringToHromatSoftIDItem(const AnsiString& ss, HromatSoftID::Item& idItem)
{
	const int len = ss.Length();
    if(len!=6) return 0;
    const MyInt
    	cVer = MyStrToInt( ss.SubString(1,2), 16 ),
        cCrc16 = MyStrToInt( ss.SubString(3,4), 16 );
    const bool ret =
    	cVer.second && cCrc16.second &&
        cVer.first>=0 && cVer.first<=0xFF &&
        cCrc16.first>=0 && cCrc16.first<=0xFFFF;
    if( ret )
    {
    	idItem.version = cVer.first;
        idItem.crc16 = cCrc16.first;
    }
    return ret;
}
//------------------------------------------------------------------------------
bool StringToHromatSoftID(const AnsiString& ss, HromatSoftID& destId)
{
	const int len = ss.Length();
    if(len!=(6*5+4) ) return 0;

    const AnsiString
    	sHromat900 = ss.SubString(1,6),
    	sHrmtlchk = ss.SubString(8,6),
    	sH9001 = ss.SubString(15,6),
    	sUcint = ss.SubString(22,6),
    	sCalculator = ss.SubString(29,6);

    HromatSoftID id = {0};
    const bool ret =
    StringToHromatSoftIDItem(sHromat900, id.hromat900) &&
    StringToHromatSoftIDItem(sHrmtlchk, id.hrmtlchk) &&
    StringToHromatSoftIDItem(sH9001, id.h9001) &&
    StringToHromatSoftIDItem(sUcint, id.ucint) &&
    StringToHromatSoftIDItem(sCalculator, id.calculator);
    if(ret) destId = id;
    return ret;

}
//------------------------------------------------------------------------------
AnsiString HromatSoftIDItemToString(const HromatSoftID::Item& id)
{
	return AnsiString::IntToHex(id.version,2)+AnsiString::IntToHex(id.crc16,4);
}
//------------------------------------------------------------------------------
AnsiString HromatSoftIDToString(const HromatSoftID& id)
{
	return
    HromatSoftIDItemToString(id.hromat900) + "-" +
    HromatSoftIDItemToString(id.hrmtlchk) + "-" +
    HromatSoftIDItemToString(id.h9001) + "-" +
    HromatSoftIDItemToString(id.ucint) + "-" +
    HromatSoftIDItemToString(id.calculator);
}
//------------------------------------------------------------------------------
bool HromatSoftIDSetupDialog(HromatSoftID& destSoftId)
{   
    boost::shared_ptr<TFormSoftIDDlg> dlg( new TFormSoftIDDlg(NULL, destSoftId) );
    dlg->ShowModal();

    const HromatSoftID id = dlg->Result();
    const bool ret =  (dlg->ModalResult==mrOk)&&( id!=NullHromatSoftID() );

    if(ret && !(destSoftId==id) )
    {
    	const AnsiString s = HromatSoftIDToString(id);
    	LogMyAppEvt("Идентификатор ПО изменён.", HromatSoftIDToString(destSoftId), s );
        if( FormAbout )
        	FormAbout->Label5->Caption = s;
        destSoftId = id;
    }
    return ret;
}
//------------------------------------------------------------------------------
bool ValidateHromatInlineSoftIdItem( const HromatSoftID::Item& lhs,
	const HromatSoftID::Item& rhs, const AnsiString& sModul, AnsiString& errMsg )
{
	errMsg = "";
	AnsiString sDetail, sVals;
	if( lhs.version!=rhs.version )
    {
    	sDetail = "Несоответсвие версии модуля";
        sVals = MYSPRINTF_("0x%s!=0x%s", AnsiString::IntToHex( lhs.version,2), AnsiString::IntToHex(rhs.version,2) );
    }

    if( lhs.crc16!=rhs.crc16 )
    {
        sDetail = "Несоответсвие CRC16";
        sVals = MYSPRINTF_("0x%s!=0x%s", AnsiString::IntToHex( lhs.crc16,4), AnsiString::IntToHex(rhs.crc16,4) );
    }
    if( !sDetail.IsEmpty() )
    {
    	errMsg = sModul+" "+sDetail+" "+sVals;
        return false;
    }
    return true;

}
//------------------------------------------------------------------------------
struct HandleBadSoftIdHelper
{
	explicit HandleBadSoftIdHelper(HromatSoftID& softId, const AnsiString& errMsg,
        bool isAdmin, HWND hWnd) :
    	softId_(softId), errMsg_(errMsg), isAdmin_( isAdmin ), hWnd_(hWnd)
    {
    }
	LRESULT HandleBadSoftId()
	{
    	if( errMsg_.IsEmpty() ) return 0;

        LogMyAppEvt( "Ошибка проверки идентификатора ПО", errMsg_ );
    	if( isAdmin_ )
    	{
    		const AnsiString s =
            	"Нажмите \"OK\" для измения идентификатора ПО и продолжения работы.\n"
            	"Нажмите \"NO\" для выхода из приложения.";
        	if( ::MessageBox( hWnd_, (errMsg_+"\n"+s).c_str(), "Сбой верификации встроенного ПО",
                MB_YESNO | MB_ICONERROR | MB_SYSTEMMODAL ) == IDNO)
                	std::exit(0);
        	else
        		HromatSoftIDSetupDialog( softId_ );
    	}
    	else
    	{
    		Application->MessageBox( errMsg_.c_str(), "Сбой верификации встроенного ПО",
        		MB_OK | MB_ICONERROR | MB_SYSTEMMODAL );
            std::exit(0);
    	}
    	return 0;
	}
    HromatSoftID& softId_;
    const AnsiString& errMsg_;
    const bool isAdmin_;
    const HWND hWnd_;
};
//------------------------------------------------------------------------------
// проверить мдентификатор встроенного ПО
bool ValidateHromatInlineSoftId( HromatSoftID& id, bool isAdmin )
{
    const HWND hWnd = Screen->ActiveForm->Handle;
    AnsiString errMsg = "";
    HandleBadSoftIdHelper handleBadSoftIdHelper(id, errMsg, isAdmin, hWnd);

    HromatSoftID::Item hrmtlchkPart;
    hrmtlchkPart.version = 1;
    hrmtlchkPart.crc16 = crc16OfHromat900HrmtlchkDotDLL;

    /*
    if( !ValidateHromatInlineSoftIdItem( id.hrmtlchk, hrmtlchkPart,
        "hrmtlchk.dll", errMsg ) )
    {
    	my::SynchronizeMthd( &handleBadSoftIdHelper.HandleBadSoftId );
        return false;
    }
    */

    // Получить и проверить идентификатор встроенного ПО
    const HromatSoftID readedSoftId=GetHromatInlineSoftId(); 

    if( !ValidateHromatInlineSoftIdItem( id.h9001, readedSoftId.h9001, "h9001.exe", errMsg ) )
    {
		if(!isAdmin) {
    		my::SynchronizeMthd( &handleBadSoftIdHelper.HandleBadSoftId );
			return false;
		} else {
			id.h9001 = readedSoftId.h9001;
            LogMyAppEvt( "Изменён идентификатор h9001 на основании прав администратора", HromatSoftIDItemToString(id.h9001) );
			return true;
		}

    }
    if( !ValidateHromatInlineSoftIdItem( id.ucint, readedSoftId.ucint, "ucint.exe", errMsg ) )
    {

        if(!isAdmin) {
    		my::SynchronizeMthd( &handleBadSoftIdHelper.HandleBadSoftId );
            return false;
		} else {
			id.ucint = readedSoftId.ucint;
			LogMyAppEvt( "Изменён идентификатор ucint на основании прав администратора", HromatSoftIDItemToString(id.ucint) );
			return true;
		}
    }
    if( !ValidateHromatInlineSoftIdItem( id.calculator, readedSoftId.calculator, "calculator.exe", errMsg ) )
    {     
        if(!isAdmin) {
    		my::SynchronizeMthd( &handleBadSoftIdHelper.HandleBadSoftId );
            return false;
		} else {
			id.calculator = readedSoftId.calculator;
			LogMyAppEvt( "Изменён идентификатор calculator на основании прав администратора", HromatSoftIDItemToString(id.calculator) );
			return true;
		}
    }
    LogMyAppEvt( "Идентификатор ПО проверен.", HromatSoftIDToString(id) );
    return true;
}
//------------------------------------------------------------------------------
