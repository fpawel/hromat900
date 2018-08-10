//---------------------------------------------------------------------------
#include <string>
#include <sstream>
#include <fstream>
//---------------------------------------------------------------------------
#pragma hdrstop
#include "AppEvts.h"
#include "MySynchronize.h"

//#include "get_md5_hash.h"
#include "MyAppProfile.h"
#include "MyFileCtrl.h"
#include "..\my_include\MyNoDestroySingleton.hpp"

#include "xmlUtils_.h"
#include "guicon.h"
#include "HromatCrypt.h"
#include "MyWindow.h"
#include "AnsiStringUtils_.h"
#include "winerr.hpp"
#include "hromat900_ini.h"
#include "MyStrUtils.h"


//------------------------------------------------------------------------------
#pragma package(smart_init)
//------------------------------------------------------------------------------







AnsiString MyEvtAsStr(const MyEvt& evt)
{
    const AnsiString empt = " ";
    return
    MYSPRINTF_("%s\t%s\t%d\t%s\t%s\t%s\t", DateToStr(evt.date_),
        TimeToStr(evt.date_), evt.number_,
        NormString(evt.id_),
        NormString( evt.content_ ),
        NormString( evt.note_ ) );
}

AnsiString GetStrUntilTab(std::istream& strm)
{
    char ch;
    std::string s;

    while( strm.get(ch) && std::iscntrl(ch) ) ;

    if( !std::iscntrl(ch) )
        s.push_back(ch);
        
    while(
        strm.get(ch) &&
        ch!='\t' &&
        !std::iscntrl(ch)
    )
        s.push_back(ch);
    return s.c_str();
}



bool TryStrToMyEvt( const std::string& s, MyEvt& evt)
{   
    std::stringstream sstrm(s);
    std::string sDate, sTime;
    sstrm >> sDate;
    sstrm >> sTime;
    if( !TryStrToDateTime(AnsiString( sDate.c_str() )+" "+sTime.c_str(), evt.date_ ) )
        return false;

    evt.number_ = 0;
    sstrm >> evt.number_;

    evt.id_ = GetStrUntilTab(sstrm);
    evt.content_ = GetStrUntilTab(sstrm);
    evt.note_ = GetStrUntilTab(sstrm);
    return true;
}

class AppEventsJournalT : public boost::noncopyable
{

public:
    explicit AppEventsJournalT();

    void LogMyAppEvt(const AnsiString& id, const AnsiString& content,
        const AnsiString& note );

    bool OpenFile( const AnsiString& fn );

    const MyEvts& GetMyEvtsJournal() const { return evts_; }

    TUpdateEvtsJournalSignal::Connections& UpdateEvtsJournalConnrections()
    {
        return updateEvtsJournal_.GetConnections();
    }
    void BackupFile();
    void SaveFile(const AnsiString& fn);

private:
    const TDateTime startMoment_;
    const AnsiString path_, fn_, archiveFn_;
    TiXmlElement* xml_;
    const unsigned maxFileSize_;
    MyEvts evts_;
    TUpdateEvtsJournalSignal updateEvtsJournal_;
    LRESULT AddEvent(const MyEvt& evt)
    {
        evts_.push_back(evt);
        std::stringstream strm;
        strm << DateTimeToStr( evt.date_ ) << "\t" << evt.id_ << "\t" << evt.content_ << "\t" << evt.note_ << "\n";
        MyWCout( strm.str().c_str() );
        updateEvtsJournal_.Signal0();
        return 0;
    }
};
//------------------------------------------------------------------------------
AppEventsJournalT::AppEventsJournalT() :
    startMoment_( Now() ),
    path_( GetAppProfileDir() ),
    fn_( path_+"\\events.log" ),
    archiveFn_( "events.log.rar" ),
    xml_( TiXML::GiveNotNullElementByTag(ProfileXmlElem(), EVENTS_JOURNAL_NODE ) ),
    maxFileSize_( TiXML::GetXMLInteger( MainAppXmlElem(), EVTS_JRNL_MAX_FILE_SIZE,
    	10000, 10000000, 200000 ) )
{
    std::fclose( std::fopen(fn_.c_str(), "a+") );
    OpenFile( fn_ );
    TiXML::SetIntAtr( MainAppXmlElem(), EVTS_JRNL_MAX_FILE_SIZE, maxFileSize_ );
    MainAppXmlDoc()->SaveFile();
}
//------------------------------------------------------------------------------
void AppEventsJournalT::LogMyAppEvt(const AnsiString& id, const AnsiString&
    content, const AnsiString& note)
{
    static unsigned count = 0;
    MyEvt evt;
    evt.date_       = Now();
    evt.id_         = id;
    evt.content_    = content;
    evt.note_       = note;
    evt.number_   = count++;
    evt.showMessage_ = 0;
    my::Synchronize1(&AddEvent, evt);
}
//------------------------------------------------------------------------------
bool AppEventsJournalT::OpenFile( const AnsiString& fn )
{
    std::ifstream strm( fn.c_str() );

    MyEvts evts;
    unsigned short crc16 = 0xA1E7;

    MyEvt evt;
    std::string sLine = "";
    while( std::getline(strm,sLine) && !sLine.empty() && TryStrToMyEvt( sLine, evt) )
    {
        crc16 = CalculateHromatCRC16( (unsigned char*) sLine.c_str(), sLine.size(), crc16);
        evts.push_back(evt);
    }
    strm.close();
    if( sLine.empty() ) return false;

    HromatCryptData( &sLine[0], sLine.size() ,&sLine[0] );

    if( MyStrToInt( sLine.c_str() ).first!=crc16 && !evts.empty() )
    {
        MyMessageBox( NULL, MYSPRINTF_(
            "Файл \"%s\"\nповреждён и будет перезаписан.\n", fn ).c_str(),
    		"Контроль целостности журнала событий",
            MB_OK | MB_ICONWARNING | MB_SYSTEMMODAL, "Хромат-900" );
        return false;
    }

    //SaveFile("c:\\11\\evj1.log");
    std::copy(evts.begin(), evts.end(), std::back_inserter(evts_) );
    //SaveFile("c:\\11\\evj2.log");
    std::sort(evts_.begin(), evts_.end() );
    //SaveFile("c:\\11\\evj3.log");
    
    evts_.erase( std::unique(evts_.begin(), evts_.end() ), evts_.end() );
    //SaveFile("c:\\11\\evj4.log");
    updateEvtsJournal_.Signal0();
    return true;
}
//------------------------------------------------------------------------------
void AppEventsJournalT::SaveFile(const AnsiString& fn)
{
	const AnsiString path = ExtractFilePath(fn);
    if( !ForceDirectories(path) )
    {
        ShowLastWindowsError("ForceDirectories\n" + path +"\n");
        return;
    }

    std::ofstream strm( fn.c_str()  );


    unsigned short crc16 = 0xA1E7;

    const MyEvts::const_iterator eEvts = evts_.end();

    for( MyEvts::const_iterator i= evts_.begin(); i!=eEvts; ++i )
    {
        const AnsiString s = MyEvtAsStr(*i);
        crc16 = CalculateHromatCRC16( (unsigned char*) s.c_str(), s.Length(), crc16);
        strm << s << "\n";
    }
    std::string sCrc16 = AnsiString(crc16).c_str();
    HromatCryptData( &sCrc16[0], sCrc16.size() ,&sCrc16[0] );

    strm << sCrc16;
    strm.close();
}
//------------------------------------------------------------------------------
void AppEventsJournalT::BackupFile()
{
    SaveFile( fn_ );
    if( MyGetFileSize(fn_) < maxFileSize_ ) return;
    // архивировать
    PerformRARArchivation( fn_, DateToPath( Now() )+"\\"+DateToFilename( Now() ) + "_events.log",
        archiveFn_  );
}
//------------------------------------------------------------------------------
struct AppEventsJournalTagT {};
typedef MyNoDestroySingletonHolder<AppEventsJournalT, AppEventsJournalTagT> AppEventsJournal;
//------------------------------------------------------------------------------

void LogMyAppEvt(const AnsiString& id, const AnsiString& content, const AnsiString& note)
{
    AppEventsJournal::Instance().LogMyAppEvt(id, content, note);
}
//------------------------------------------------------------------------------
const MyEvts& GetMyEvtsJournal()
{
    return AppEventsJournal::Instance().GetMyEvtsJournal();
}
//------------------------------------------------------------------------------
boost::shared_ptr<void> ConnectOnUpdateEvtsJournal( TUpdateEvtsJournalSignal::Handler foo)
{
    return AppEventsJournal::Instance().UpdateEvtsJournalConnrections().AutoConnect(foo);
}
//------------------------------------------------------------------------------
// сохранить текущий журнал
void SaveMyAppEvtJournal( const AnsiString& fn )
{
    AppEventsJournal::Instance().SaveFile(fn);
}
//------------------------------------------------------------------------------
// сохранить текущий журнал
void BackupMyAppEvtJournal( )
{
    AppEventsJournal::Instance().BackupFile();
}
//------------------------------------------------------------------------------
// открыть журнал из файла
bool LoadMyAppEvtJournalFromFile( const AnsiString& fn )
{
    return AppEventsJournal::Instance().OpenFile(fn);
}






