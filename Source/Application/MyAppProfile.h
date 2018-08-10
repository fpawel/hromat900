//------------------------------------------------------------------------------
#ifndef MyAppProfileH
#define MyAppProfileH
//------------------------------------------------------------------------------

// vcl
#include <system.hpp>

// boost
//#include "boost\noncopyable.hpp"
#include "boost\shared_ptr.hpp"

// my
#include "tinyxml.h"

AnsiString GetApplicationProfileId();
boost::shared_ptr<void> CreateApplicationProfileMutex(AnsiString id);


AnsiString GetAppProfileDir();
AnsiString GetPersonalProfileDir();

// главный элемент XML файла приложения приложения
TiXmlDocument* ProfileXmlDoc();
TiXmlElement* ProfileXmlElem();

TiXmlDocument* MainAppXmlDoc();
TiXmlElement* MainAppXmlElem();
void SaveAppConfigFiles();
AnsiString FindReportFileName( const AnsiString& ext );

AnsiString Izobut();
bool IsIzobut();









//------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------
