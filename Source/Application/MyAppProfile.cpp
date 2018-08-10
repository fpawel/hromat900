
//---------------------------------------------------------------------------
#pragma hdrstop

#include "MyAppProfile.h"
#include "MyProcEntry.h"
#include "AnsiStringUtils_.h"
#include "xmlUtils_.h"
#include "loki\singleton.h"
#include "uFormAppProfileDlg.h"
#include "..\my_include\free_types_.h"
#include "..\my_include\FooCalledOnce.hpp"
#include "MyFileCtrl.h"
//#include "MyShllFldr.h"
#include "MyAppWinPath.hpp"
#include "MyModFN.hpp"

//---------------------------------------------------------------------------

#pragma package(smart_init)

//------------------------------------------------------------------------------
#define APPLICATION_ID_MUTEX_NAME "F44C0214-6718-445B-B2E5-1200C7DCC16E-"
#define PROFILE_CMD_PRMT_KEY  "-profile:"
#define APP_DIR_NAME  "Õðîìàò-900"
#define APP_DIR_NAME1  "\\"APP_DIR_NAME"\\"

enum{ PROFILE_CMD_PRMT_KEY_LEN  = 9 };
//------------------------------------------------------------------------------
AnsiString GetPersonalDir()
{
    //return ShllFldr::Path(ShllFldr::PERSONAL)+APP_DIR_NAME1;
    AnsiString ret = MyGetExePath();
    ForceDirectories(ret);
    return MyGetExePath();
}
//------------------------------------------------------------------------------
AnsiString GetAppDataDir()
{
    // return ShllFldr::Path(ShllFldr::APPDATA)+APP_DIR_NAME1;
    AnsiString ret = MyGetExePath();
    ForceDirectories(ret);
    return MyGetExePath();
}
//------------------------------------------------------------------------------
AnsiString GetAppProfileDir()
{
    AnsiString ret = GetAppDataDir()+GetApplicationProfileId();
    ForceDirectories(ret);
    return MyGetExePath();
}
//------------------------------------------------------------------------------
AnsiString GetPersonalProfileDir()
{
    AnsiString ret = GetAppDataDir()+GetApplicationProfileId();
    ForceDirectories(ret);
    return MyGetExePath();
}
//------------------------------------------------------------------------------
AnsiString FindReportFileName( const AnsiString& sExt )
{
    const AnsiString path = MyGetExePath()+"Îò÷¸òû", baseName ="\\îò÷¸ò",
        baseName1 = path+baseName;
    ForceDirectories(path);
    AnsiString ret = baseName1+sExt;
    for( int n = 1; MyFileExists(ret); ++n )
    {
        if( ::DeleteFile( ret.c_str() )!=0 )
            return ret;
        ret = baseName1+IntToStr(n)+sExt;
    }
    return ret;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
boost::shared_ptr<void> CreateApplicationProfileMutex(AnsiString id)
{   
	const AnsiString s = APPLICATION_ID_MUTEX_NAME+id;
	return boost::shared_ptr<void>( ::CreateMutex( NULL, TRUE, s.c_str() ), ::CloseHandle );
}
//------------------------------------------------------------------------------
bool CheckCorrectApplicationProfileId(AnsiString id)
{
	boost::shared_ptr<void> hMutex = CreateApplicationProfileMutex(id);
    return ::GetLastError()==0;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
AnsiString ExtractApplicationProfileIdFromComandPrompt()
{
	ASSERT_FUNCTION_CALLED_ONCE__;
 	return ExtractFromComandPrompt("-profile:");
}
//------------------------------------------------------------------------------
TiXmlDocument* CheckedLoadXML( const AnsiString& fileName)
{
	//CheckedDeleteFileSignDialog(fileName);
    return TiXML::CreateXMLDocument(fileName.c_str());
}



//------------------------------------------------------------------------------
class ThisLibraryT : public boost::noncopyable
{

public:
	explicit ThisLibraryT();
    AnsiString Id() const { return id_; }

    TiXmlDocument* ProfileDoc() { return xmlDocProfile_; }
    TiXmlElement *ProfileElem() { return profileElem_; }

    TiXmlElement *TopElem() { return topElem_; }

    TiXmlDocument* MainAppDoc()	{		return xmlDocMain_;	}
	TiXmlElement* MainAppElem()	{		return topElem_;	}

    void SaveAppConfigFiles();

    AnsiString Izobut() const {return izobut_;}
    bool IsIzobut() const {return isIzobut_;}

private:
	const AnsiString appXmlFileName_;
	TiXmlDocument* xmlDocMain_;
	TiXmlElement *topElem_, *profilesElem_;

	const AnsiString id_;
    const AnsiString profileXmlFileName_;
    TiXmlDocument* xmlDocProfile_;
    TiXmlElement *profileElem_;

    void PromptForApplicationProfileId(AnsiString& ret);
    TiXmlElement* CreateProfileXmlElem();
    AnsiString PromptForApplicationProfileId();
    AnsiString ExtractApplicationProfileId();
    const bool isIzobut_;
    const AnsiString izobut_;

};
//------------------------------------------------------------------------------
ThisLibraryT::ThisLibraryT() :
	appXmlFileName_( GetAppDataDir()+"\\"+ChangeFileExt( ExtractFileName(ParamStr(0)), ".xml") ),
    xmlDocMain_( CheckedLoadXML( appXmlFileName_ ) ),
    topElem_( TiXML::GiveNotNullElementByTag( xmlDocMain_, "APPLICATION" ) ),
    profilesElem_( TiXML::GiveNotNullElementByTag( topElem_, "PROFILES" ) ),
    id_( ExtractApplicationProfileId() ),
    profileXmlFileName_( GetAppDataDir()+id_+"\\config.xml" ),
    xmlDocProfile_( CheckedLoadXML( profileXmlFileName_) ),
    profileElem_ ( CreateProfileXmlElem() )  ,
    isIzobut_( TiXML::GetStrAtr( topElem_, "USE_IZOBUT", "1" )==1 ),
    izobut_( TiXML::GetStrAtr( topElem_, "IZOBUT", "i-C4H8" ) )

{
	ASSERT_FUNCTION_CALLED_ONCE__;
    MyForceDirectory( GetAppDataDir()+"\\"+id_);
    TiXML::SetAtr( topElem_, "IZOBUT", izobut_ );
    TiXML::SetAtr( topElem_, "USE_IZOBUT", isIzobut_ ? "1" : "0" );
    /*
    if( !id_.IsEmpty() )
    {
    	TiXML::GiveNotNullElementByTag( profilesElem_, id_ );
    	xmlDocMain_->SaveFile();
    }
    */
}
//------------------------------------------------------------------------------
void ThisLibraryT::SaveAppConfigFiles()
{
	xmlDocMain_->SaveFile( appXmlFileName_.c_str() );
    MySignFile( appXmlFileName_ );
    xmlDocProfile_->SaveFile( profileXmlFileName_.c_str() );
    MySignFile( profileXmlFileName_ );
}
//------------------------------------------------------------------------------
TiXmlElement* ThisLibraryT::CreateProfileXmlElem()
{
	ASSERT_FUNCTION_CALLED_ONCE__;
	return id_.IsEmpty() ?
    TiXML::GiveNotNullElementByTag( topElem_, "DEFAULT_PROFILE" ) :
    TiXML::GiveNotNullElementByTag( xmlDocProfile_, "MAIN" );
}
//------------------------------------------------------------------------------
AnsiString ThisLibraryT::ExtractApplicationProfileId()
{
	ASSERT_FUNCTION_CALLED_ONCE__;
	AnsiString ret;
	ret = ExtractApplicationProfileIdFromComandPrompt();
    if( ret.IsEmpty() )
    	ret = PromptForApplicationProfileId();
    if( ret.IsEmpty() )
    	return "";
    if( !CheckCorrectApplicationProfileId(ret) )
    	return "";
    return ret;
}
//------------------------------------------------------------------------------
AnsiString ThisLibraryT::PromptForApplicationProfileId()
{
	ASSERT_FUNCTION_CALLED_ONCE__;
	boost::shared_ptr<TFormAppProfileDlg> dlg( new TFormAppProfileDlg(NULL,
    	profilesElem_, xmlDocMain_) );
    dlg->ShowModal();
    return dlg->Result();
}


//------------------------------------------------------------------------------
typedef Loki::SingletonHolder
    	<
    		ThisLibraryT,
        	Loki::CreateUsingNew,
    		Loki::NoDestroy
    	> ThisLibrary;
//------------------------------------------------------------------------------
AnsiString GetApplicationProfileId()
{
	return ThisLibrary::Instance().Id();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
TiXmlDocument* MainAppXmlDoc()
{
	return ThisLibrary::Instance().MainAppDoc();
}

AnsiString Izobut()
{
    return ThisLibrary::Instance().Izobut();
}

bool IsIzobut()
{
    return ThisLibrary::Instance().IsIzobut();
}


TiXmlElement* MainAppXmlElem()
{
	return ThisLibrary::Instance().MainAppElem();
}
//------------------------------------------------------------------------------
TiXmlDocument* ProfileXmlDoc()
{
	return ThisLibrary::Instance().ProfileDoc();
}
//------------------------------------------------------------------------------
TiXmlElement* ProfileXmlElem()
{
	return ThisLibrary::Instance().ProfileElem();
}
//------------------------------------------------------------------------------
void SaveAppConfigFiles()
{
	ThisLibrary::Instance().SaveAppConfigFiles();
}
//------------------------------------------------------------------------------

