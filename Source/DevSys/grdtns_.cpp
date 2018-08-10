//---------------------------------------------------------------------------
#include <fstream>
#include <stdlib>

#pragma hdrstop

#include <forms.hpp>
#include <windows.h>
#include "grdtns_.h"
//my
#include "CRC16.h"
#include "MyAbstructTransfer.hpp"
#include "HromatModbus.h"
#include "TransferManage.h"
#include "AnsiStringUtils_.h"
#include "MyWindow.h"
#include "AppEvts.h"
#include "binstream.hpp"
#include "winerr.hpp"
#include "hromatgas.hpp"
#include "hromat900.hpp"
#include "MyAppProfile.h"
#include "MyFileCtrl.h"
#include "myconv.h"
#include "uFormGraduate.h"
#include "MySynchronize.h"


//---------------------------------------------------------------------------

#pragma package(smart_init)

#define GRDTNS_Ki_FN "grdtns.ki"
#define GRDTNS_Rk_FN "grdtns.rk"
#define GRDTNS_Rkmax_FN "grdtns.rkmax"
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
namespace Grdtn
{
//------------------------------------------------------------------------------
AnsiString GetFileName() { return GetAppProfileDir()+"\\graduations."; }
//------------------------------------------------------------------------------
bool GetLastHromatGraduateItem(HromatGraduateItem& ret)
{
    const AnsiString fn = GetFileName();
    ForceDirectories( ExtractFilePath(fn) );
    std::fclose( std::fopen(fn.c_str(), "a+") );
    std::ifstream fstrm( fn.c_str(), std::ios::in | std::ios::binary );
    if( !fstrm.is_open() )
    {
    	ShowLastWindowsError(fn);
    	return 0;
	}
    const AutoCloseFileStreamHelper<std::ifstream> autoCloseFileStreamHelper(fstrm);

    fstrm.seekg(0, std::ios::end);
    const unsigned
        fileBlockSizse = sizeof(HromatGraduateItem)+2,
    	fileSize = fstrm.tellg(), badFileSize = fileSize % fileBlockSizse;
    if( !fileSize || badFileSize ) return 0;
	fstrm.seekg( -1*fileBlockSizse, std::ios::end);
    MyBinaryIStreamHlpr myStrm(fstrm, false);
    myStrm >> ret;
    return myStrm.CheckCRC16();
}
//------------------------------------------------------------------------------
LRESULT Save1( const HromatGraduateItem& itm )
{
    const AnsiString fn = GetFileName();
    ForceDirectories( ExtractFilePath(fn) );

    HromatGraduateItem last;
    if( GetLastHromatGraduateItem(last) && last.dateTime>itm.dateTime)
        return 0;

    //std::fclose( std::fopen(fn.c_str(), "a+") );
    //std::fstream fstrm( fn.c_str(), std::ios::in | std::ios::out | std::ios::binary );
    //const AutoCloseFileStreamHelper<std::fstream> autoCloseFileStreamHelper(fstrm);
    using std::ios;
    std::ofstream fstrm( fn.c_str(), ios::binary | ios::app );
    const AutoCloseFileStreamHelper<std::ofstream> autoCloseFileStreamHelper(fstrm);

    const MyBinaryOStreamHlpr myStrm(fstrm );
    fstrm.seekp(0, ios::end);
    myStrm << itm;
    myStrm.FixCRC16();
    fstrm.close();

    return 0;
}
//------------------------------------------------------------------------------
void Save( const HromatGraduateItem& itm )
{
    my::Synchronize1( Save1, itm );
}
//------------------------------------------------------------------------------
void GetArchive(HromatGraduateArchive& archive )
{
    archive.clear();
    const AnsiString fn = GetFileName();
    ForceDirectories( ExtractFilePath(fn) );
    std::fclose( std::fopen(fn.c_str(), "a+") );
    std::ifstream fstrm( fn.c_str(), std::ios::in | std::ios::binary );
    if( !fstrm.is_open() )
    {
    	ShowLastWindowsError(fn);
    	return ;
	}
    const AutoCloseFileStreamHelper<std::ifstream> autoCloseFileStreamHelper(fstrm);
    fstrm.seekg(0, std::ios::beg);
    for(  ; !fstrm.fail() ;  )
    {
    	HromatGraduateItem grad;
        const MyBinaryIStreamHlpr myStrm(fstrm, false);
        myStrm >> grad;
        if( fstrm.fail() || !myStrm.CheckCRC16() )
            return;
        archive.push_back(grad);
    }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void CreateExample()
{
    ::DeleteFile( GetFileName().c_str() );
	HromatGraduateItem item;
    const unsigned gasCnt = Gas::GrauatorsCount();
    for( unsigned nYear = 2008; nYear<2011; ++nYear )
    for( unsigned nMonth = 1; nMonth<11; ++nMonth )
	for( unsigned nDay = 1; nDay<13; ++nDay )
	{
		item.dateTime = EncodeDate(nYear, nMonth, nDay).Val;
        item.Ki.dateTime = DecodeDateTime(item.dateTime);
        item.Rk.dateTime = DecodeDateTime(item.dateTime);
        item.Rkmax.dateTime = DecodeDateTime(item.dateTime);
		for	( unsigned i=0; i<gasCnt; ++i )
        {
			item.Ki.v[i] = 90 + std::random(20);
            item.Rk.v[i] = 80 + std::random(20);
            item.Rkmax.v[i] = 99 + std::random(20);
        }
        item.fileIndex = nYear + nMonth + nDay;
        item.result = item.fileIndex % 3;

        Save( item );
	}
}
//------------------------------------------------------------------------------
void Report()
{
    HromatGraduateArchive archive;
    GetArchive(archive );
    const unsigned
        gasCnt = Gas::GrauatorsCount(),
        sz = archive.size();
    const AnsiString
        fileName = FindReportFileName(".txt"),
        ttl = "Хромат-900. Архив градуировок.";
    const Gas::Item *gas = Gas::GetItems();
    std::ofstream strm( fileName.c_str() );
    strm << "Архив градуировок, " << MyFormatDateTime1( Now() ).c_str() << "\n\n";

    for( unsigned nItem=0; nItem<sz; ++nItem  )
    {
        const HromatGraduateItem& itm = archive[nItem];

        /*
        MYSPRINTF_("%d, %s, %s",
        grad.first.second,
        FormatDateTime("dd MMMM yyyy, hh:nn", grad.first.first),
        GraduateResult2Str(grad.second) );
        */

        strm << MYSPRINTF_(
        "Градуировка №%d, хроматограмма %d,\n"
        "Баллон %d, %s.\n"
        "Расчёт параметров %s. Завершена %s.\n",
        nItem+1, itm.fileIndex,
        itm.ballon,  GraduateResult2Str(itm.result),
        MyFormatDateTime1( EncodeDateTime(itm.Ki.dateTime) ),
        MyFormatDateTime1( itm.dateTime )).c_str();
        strm << MYSPRINTF_("Компонент\tKi\tRk\tRk_max\n" ).c_str();
        for( unsigned nGas=0; nGas<gasCnt; ++nGas )
        {
            AnsiString s;
            s.sprintf("%s\t%g\t%g\t%g\n",
                    gas[nGas].name,
                    MyDoubleToFloat( itm.Ki.v[nGas] ),
                    MyDoubleToFloat( itm.Rk.v[nGas] ),
                    MyDoubleToFloat( itm.Rkmax.v[nGas] )
                    );
            strm << s.c_str();
        }
        strm << "\n";
    }
    strm.close();
    ::ShellExecute( NULL, "open", fileName.c_str(), "",
        ExtractFilePath( fileName ).c_str(), SW_SHOWMAXIMIZED );
}
//------------------------------------------------------------------------------
void ShowLast()
{
    static struct
    {
        LRESULT Method() const
        {

            HromatGraduateItem itm;
            if( !Grdtn::GetLastHromatGraduateItem(itm) )
            {
                ::MessageBox( Screen->ActiveForm ? Screen->ActiveForm->Handle : NULL,
                    "Отсутствуют данные последней градуировки.",
    	            "Хромат-900", MB_OK | MB_APPLMODAL | MB_ICONEXCLAMATION  );
                return 0;
            }
            TFormGraduate *dlg = new TFormGraduate(NULL, itm );
            dlg->ShowModal();
            return 0;
        }
    } helper;
    my::PostCallClosure( &helper.Method );
}
//------------------------------------------------------------------------------
}; // namespace Grdtn
//------------------------------------------------------------------------------

