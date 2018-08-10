//---------------------------------------------------------------------------
#ifndef MyFileCtrlH
#define MyFileCtrlH

#include <SysUtils.hpp>
#include "MyIostream.h"
#include <vector>

//---------------------------------------------------------------------------
typedef void (__closure *OnFindFileMthd)(const AnsiString& path, const  TSearchRec& sr);
//---------------------------------------------------------------------------

class AutoSRClose : public boost::noncopyable
    {
    public:
        explicit AutoSRClose( TSearchRec& sr ) : sr_(sr) {}
        ~AutoSRClose()
        {
            FindClose(sr_);
        }
    private:
         TSearchRec& sr_;
    };

void MySearchFiles
(
    const AnsiString& strDir,
    const AnsiString& strExt,
    OnFindFileMthd onFindFile,
    const bool& mustCancel,
    int attrs = faAnyFile
);

void MySearchDirectories( const AnsiString& path, OnFindFileMthd onFind );
void MySearchDirectories( const AnsiString& path, std::vector<AnsiString> &result );
void MySearchAllFiles( const AnsiString& mainPath, const AnsiString& mask, std::vector<AnsiString> &files );
bool CanICreateFileThere(const AnsiString& path);

bool MyFileExists(const AnsiString& fn);
unsigned MyGetFileSize(const AnsiString& fn);

AnsiString RemoveInfoFileToFolderAsDate( const AnsiString& fn, unsigned maxSize,
	const AnsiString& ext, const AnsiString& parDir, const AnsiString& addBack = "" );

AnsiString RemoveInfoFileToFolderAsDate_rar( const AnsiString& fn, unsigned maxSize,
	const AnsiString& parDir, const AnsiString& addBack = "" );

bool MyForceDirectory(const AnsiString& path);
bool MyRenameFile(const AnsiString& fn, const AnsiString& newFn);

bool VerifySignOfFileData( const AnsiString& fn, const AnsiString& hashFn = "");

void MySignFile( const AnsiString& fn, const AnsiString& hashExt = ".hash");
void CheckedDeleteFileSignDialog( const AnsiString& fileName, const AnsiString hashFnOrig = "");

void LoadFileToBuff( const AnsiString& fn, std::vector<unsigned char>& buff  );

unsigned short int CalcFileCRC16(const AnsiString& fn);

void PerformRARArchivation( const AnsiString& fn, const AnsiString& inRarFn,
    const AnsiString& rarFn  );

void MyExtractFileFromResource( const AnsiString& fn, const AnsiString& id,
    bool canOverwrite );

void ShowLastWindowsError(const AnsiString& msg = "");


#endif



