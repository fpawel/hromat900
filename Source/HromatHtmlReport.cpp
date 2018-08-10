//------------------------------------------------------------------------------
#pragma hdrstop
//------------------------------------------------------------------------------
#include "HromatHtmlReport.h"
//------------------------------------------------------------------------------
#include <Math.hpp>
#include <vector>
#include "MyModFN.hpp"
#include "boost\shared_ptr.hpp"
#include "MyExcpt.hpp"
#include "AnsiStringUtils_.h"
#include "MyProcEntry.h"
#include "MyFileCtrl.h"
#include "MyAppWinPath.hpp"
#include "xlcnstnts.hpp"
#include "HromatGas.hpp"
#include "myconv.h"
#include "uFrmWait.h"
#include "MyAppProfile.h"
#include "htg_.h"
#include "MyModFN.hpp"
#include "htmlreport.h"
#include "uDateTimeRangeDialogForm.h"

#include "SHDocVw_OCX.h"
#include <OleCtrls.hpp>
#include "hromat_sys.h"
#include <numeric>

#include <iostream>
#include "Unit1.h"




//------------------------------------------------------------------------------
#pragma package(smart_init)
//------------------------------------------------------------------------------
#define CONC_GOST "ИБЯЛ.413538.001-02МИ"


struct SConc
{
    AnsiString conc, dConc, diap;
    bool izm;
    static SConc fromf(AnsiString f, AnsiString conc, AnsiString dConc) {
        SConc s;
        s.conc = conc;
        s.dConc = dConc;
        int i = Gas::GetIdx(f);
        if (i>-1 && i<Gas::Count()) {
            const Gas::Item& it = Gas::GetItems()[i];
            s.diap.sprintf("%g-%g", it.min, it.max);
            s.izm = it.sensorId>-1;
        }
        return s;
    }
    
};



typedef std::map< AnsiString, SConc, Gas::Comp > Mapconc;



Mapconc GetConcs(const HromatogramReportData& reportData) {
    Mapconc conc;
    TStringGrid *grd = reportData.grd1;
    for(int ro=0; ro<grd->RowCount-1; ++ro)
        conc[grd->Cells[1][ro+1]] = SConc::fromf( grd->Cells[1][ro+1],grd->Cells[2][ro+1],grd->Cells[3][ro+1] );
    grd = reportData.grd2;
    for(int ro=0; ro<grd->RowCount-1; ++ro)
        conc[grd->Cells[1][ro+1]] = SConc::fromf( grd->Cells[1][ro+1],grd->Cells[2][ro+1],grd->Cells[3][ro+1] );

    return  conc ;
}

struct Fischem
{
    AnsiString name, units, diap, val, d;
    static Fischem c(AnsiString diap, TStringGrid* grd, int ro)
    {
        Fischem r;
        r.name = grd->Cells[0][ro];
        r.units = grd->Cells[1][ro];
        r.diap = diap;
        r.val = grd->Cells[2][ro];
        r.d = grd->Cells[3][ro];
        return r;
    }
    typedef std::vector<Fischem> Lst;
    static Lst getFischems(TStringGrid *grd)
    {

        Fischem r[8] = {
            c("", grd, 1),
            c("0,669 - 1,210", grd, 2),
            c("840 - 1440", grd, 3),
            c("765 - 1260", grd, 4),
            c("35 - 57,7", grd, 5),
            c("31,8 - 52,5", grd, 6),
            c("", grd, 7),
            c("", grd, 8) };
        Lst ret(r,r+8) ;
        return ret;
    }
};

typedef TiXmlElement* Elem;

const HTML::TextAttr
        bold1 = HTML::TextAttr().Bold(),
        centerBold = bold1.Align("center"),
        rightBold = bold1.Align("right");

Elem addGost(Elem e, AnsiString rowSpan)
{
    using namespace HTML;

    e->SetAttribute("rowspan", rowSpan.c_str() );
    e->SetAttribute("align", "center");
    
    AddText( AddTag( e, "p"), CONC_GOST);
}


        void RemElem(Elem e)
        {
            if(e->Parent()!=NULL) e->Parent()->RemoveChild(e);
        }


AnsiString MakeHromatHTMLReport( const HromatogramReportData& reportData,
    bool incGraph1, bool incGraph2 )
{

    struct{
        
        void saveImg(AnsiString fn, TChart *ch)
        {
            TAlign al = ch->Align;
            ch->Align = alNone;
            ch->Width = Ceil(ch->Width * 0.9);
            ch->SaveToMetafileEnh( fn );
            ch->Align = al;
        }
    } h;
    const AnsiString
        sDir = MyGetExePath(),
        fn = sDir+"отчёт.html",
        pict1fn = "chart1.mft",
        pict2fn = "chart2.mft";
	ForceDirectories(sDir);
    
    const std::string sTtl = ("Хроматограмма " + reportData.title).c_str();

    Mapconc concizm = GetConcs(reportData), concneizm;

    for( Mapconc::iterator i=concizm.begin(); i!=concizm.end();++i ) {
        if( !(i->second.izm) ) {
            concneizm.insert(*i);
            concizm.erase(i);
        }
    }
    Fischem::Lst fischem = Fischem::getFischems(reportData.grdPhysChem);
    const int
        
        izmCount = concizm.size(),
        neizmCount = concneizm.size();

    std::string ssEmpt(40,'_'); const char *sEmpt = ssEmpt.c_str();

    using namespace HTML;
    Elem body, head;
    boost::shared_ptr<TiXmlDocument> doc( HTML::CreateDocument(sTtl, body, head ) );
    AddStyleSheet( head, "template.css");

    AddText( body, "Протокол анализа, хроматограмма " + std::string(reportData.title.c_str()), TextAttr().Class("header1") );
    TableT tbl;
    ElemsT rows;

    int cc = 6, rc = izmCount+neizmCount+6+2;
    Elem etbl =  AddTable( body, cc, rc, tbl, &rows);
    etbl->SetAttribute("class", "table1");

    int ro = 0;

    AddText( tbl[0][ro], "Место отбора пробы", TextAttr().Align("left") );
    tbl[0][ro]->SetAttribute("colspan", "2");
    RemElem( tbl[1][ro] );
    AddText( tbl[2][ro], Hromat900::Instance().MestoOtboraProby().c_str(), TextAttr().Align("center").Editiable() );
    tbl[2][ro]->SetAttribute("colspan", "3");
    RemElem( tbl[3][ro] );
    RemElem( tbl[4][ro] );
    AddText( tbl[5][ro++], (FormatDateTime("dd.MM.yyyy г.", reportData.dt) ).c_str(), TextAttr().Align("right") );

    rows[ro]->Clear();
    Elem e = AddTag( rows[ro++], "td");
    e->SetAttribute("colspan", "6");
    AddText( e, "Содержание измеряемых компонентов", centerBold );

    AddText( tbl[0][ro], "Компонент" );
    AddText( tbl[1][ro], "Единица величины" );
    AddText( tbl[2][ro], "Диапазон измерения" );
    AddText( tbl[3][ro], "Результат измерения" );
    AddText( tbl[4][ro], "Абсолютная неопределённость (±)" );
    AddText( tbl[5][ro++], "Нормативный документ" );

    AnsiString ssConcCount = IntToStr(izmCount);
    const char *sConcCount = ssConcCount.c_str();

    e = tbl[1][ro];
    AddText( e, AnsiString().sprintf( "Концентрация C, %s", Form1->MeasureUnits).c_str() );
    e->SetAttribute("rowspan", sConcCount);

    e = tbl[5][ro];
    addGost( e, sConcCount);
    e->SetAttribute("rowspan", sConcCount);

    for( int i=1; i<concizm.size();++i ) {
        RemElem( tbl[1][ro+i] );
        RemElem( tbl[5][ro+i] );
    }

    for( Mapconc::iterator i=concizm.begin(); i!=concizm.end();++i ) {
        AddText( tbl[0][ro], i->first.c_str() );
        AddText( tbl[2][ro], i->second.diap.c_str() );
        AddText( tbl[3][ro], i->second.conc.c_str() );
        AddText( tbl[4][ro++], i->second.dConc.c_str() );
    }

    rows[ro]->Clear();
    e = AddTag( rows[ro++], "td");
    e->SetAttribute("colspan", "6");
    AddText( e, "Неизмеряемые компоненты", centerBold );


    AddText( tbl[0][ro], "Компонент" );
    AddText( tbl[1][ro], "Единица величины" );
    AddText( tbl[2][ro], "Диапазон значений" );
    AddText( tbl[3][ro], "Значение" );
    AddText( tbl[4][ro], "Абсолютная неопределённость (±)" );
    AddText( tbl[5][ro++], "Нормативный документ" );

    ssConcCount = IntToStr(neizmCount);
    sConcCount = ssConcCount.c_str();

    e = tbl[1][ro];
    AddText( e, AnsiString().sprintf( "Концентрация C, %s", Form1->MeasureUnits).c_str() );
    e->SetAttribute("rowspan", sConcCount);

    e = tbl[5][ro];
    //AddText( e, "ГОСТ 31369");
    //e->SetAttribute("rowspan", sConcCount);
    addGost( e, sConcCount);

    for( int i=1; i<concneizm.size();++i ) {
        RemElem( tbl[1][ro+i] );
        RemElem( tbl[5][ro+i] );
    }

    for( Mapconc::iterator i=concneizm.begin(); i!=concneizm.end();++i ) {
        AddText( tbl[0][ro], i->first.c_str() );
        AddText( tbl[2][ro], i->second.diap.c_str() );
        AddText( tbl[3][ro], i->second.conc.c_str() );
        AddText( tbl[4][ro++], i->second.dConc.c_str() );
    }

    rows[ro]->Clear();
    e = AddTag( rows[ro++], "td");
    e->SetAttribute("colspan", "6");

    AddText( tbl[0][ro], "Подпись ответственного лица", TextAttr().Align("right") );
    tbl[0][ro]->SetAttribute("colspan", "5");
    RemElem( tbl[1][ro] );
    RemElem( tbl[2][ro] );
    RemElem( tbl[3][ro] );
    RemElem( tbl[4][ro] );
    AddText( tbl[5][ro], "", TextAttr().Align("left").Editiable() );


    if(incGraph1) {
        h.saveImg( pict1fn, reportData.chart1 );

        Elem ebr = AddTag( body, "p");
        ebr->SetAttribute("class", "pagebreak1" );
        AddText( ebr, "График хроматограммы детектора 1", centerBold);

        e = AddTag( body, "img");
        e->SetAttribute("src", pict1fn.c_str());
        e->SetAttribute("alt", "График хроматограммы детектора 1");
    }

    if(incGraph2) {
        h.saveImg( pict2fn, reportData.chart2 );

        Elem ebr = AddTag( body, "p");
        ebr->SetAttribute("class", "pagebreak1" );
        AddText( ebr, "График хроматограммы детектора 2", centerBold);

        e = AddTag( body, "img");
        e->SetAttribute("src", pict2fn.c_str());
        e->SetAttribute("alt", "График хроматограммы детектора 2");
    }

    doc->SaveFile( fn.c_str() );
    return fn;
}

const char* PhsChm( const HromatPhysChem& phch, unsigned i)
{
    static AnsiString s1;
    const double v = GetPhysChemVal(phch, i, 0);
    const unsigned
        countAfterCommaDigitsVal  = CountAfterCommaDigits( v  ),
        countAfterCommaDigits = std::min( countAfterCommaDigitsVal, 2);
    s1 = MyFloatToStr(v);
    return s1.c_str();

}

bool MeasureDataDateLess(const MeasureData& lhs, const MeasureData& rhs ) {
    return EncodeDateTime(lhs.dateTime)<EncodeDateTime(rhs.dateTime);
}

using namespace HTML;

void MakeTbl1(Elem body, TableT& tbl, int rowCount) {

    const unsigned
        gasCnt = Gas::Count(),
        pyCheCnt = 6;
    ElemsT rows;
    int cc = gasCnt+pyCheCnt+1, rc = rowCount+2;
    Elem etbl =  AddTable( body, cc, rc, tbl, &rows);
    etbl->SetAttribute("class", "table1");

    Elem e = tbl[0][0];
    AddText( e, "Дата" );
    e->SetAttribute("rowspan", "2");
    RemElem( tbl[0][1] );

    AddText( tbl[1][0], "Содержание компонентов, молярная доля, %", TextAttr().Align("center") );
    tbl[1][0]->SetAttribute("colspan", IntToStr(gasCnt).c_str() );
    for (int i=1; i<gasCnt; ++i) RemElem( tbl[i+1][0] );

    for( unsigned i=0; i<gasCnt; ++i ) {
        int ii = Gas::GetItems()[i].reportnum;
        AddText( tbl[1+ii][1], Gas::Formula(i).c_str() );
    }

    AddText( tbl[cc - 6][0], "Физико-химические показатели", TextAttr().Align("center") );
    tbl[cc - 6][0]->SetAttribute("colspan", "6" );

    AddText( tbl[ 1 + gasCnt + 0][1], "Hsv,МДж/м3" );
    AddText( tbl[ 1 + gasCnt + 1][1], "Hiv,МДж/м3" );
    AddText( tbl[ 1 + gasCnt + 2][1], "p,кг/м3" );
    AddText( tbl[ 1 + gasCnt + 3][1], "d" );
    AddText( tbl[ 1 + gasCnt + 4][1], "Ws,МДж/м3" );
    AddText( tbl[ 1 + gasCnt + 5][1], "Wi,МДж/м3" );
}

void AddMeasureData(const MeasureData& md, TableT& tbl, int ro, AnsiString dateFmt) {
    const unsigned
        gasCnt = Gas::Count(),
        pyCheCnt = 6;
    AnsiString s;

    AddText( tbl[0][ro], FormatDateTime(dateFmt,EncodeDateTime( md.dateTime )).c_str()  );
        for( unsigned i=0; i<gasCnt; ++i ) {
            int ii = Gas::GetItems()[i].reportnum;
            s.sprintf("%s", MyFloatToStr(md.conc[i]) );
            AddText( tbl[1+ii][ro], s.c_str()  );
        }
        const HromatPhysChem pc = md.physChem;
        AddText( tbl[1+gasCnt+0][ro], PhsChm(pc,5) );
        AddText( tbl[1+gasCnt+1][ro], PhsChm(pc,4) );
        AddText( tbl[1+gasCnt+2][ro], PhsChm(pc,0) );
        AddText( tbl[1+gasCnt+3][ro], PhsChm(pc,1) );
        AddText( tbl[1+gasCnt+4][ro], PhsChm(pc,7) );
        AddText( tbl[1+gasCnt+5][ro], PhsChm(pc,6) );
}

AnsiString Report_Jurnal_Analize(const Jurnal_Analize& d)
{
    const AnsiString
        sDir = MyGetExePath(),
        fn = sDir+"журнал_анализ.html";
	ForceDirectories(sDir);


    Elem body, head;
    boost::shared_ptr<TiXmlDocument> doc( HTML::CreateDocument("Среднечасовой отчёт", body, head ) );
    AddStyleSheet( head, "template.css");
    AnsiString s;
    s.sprintf( "Хроматограф %s. Среднечасовой отчёт о содержании компонентов за %s.",
        Hromat900::Instance().HromatType(),
        FormatDateTime("dd.MM.yyyy", EncodeDateTime(d.startDateTime)) )    ;

    AddText( body, s.c_str(), TextAttr().Class("header1") );
    s.sprintf( "Место отбора проб: %s",  Hromat900::Instance().MestoOtboraProby() );
    AddText( body, s.c_str()  );

    std::vector<MeasureData> items;
    for(int n=0; n<std::min( Ceil(d.cntAvgHourData), 24 ); ++n)
        //if( EncodeDateTime(d.AvgHourData[n].dateTime)>Date() )
            items.push_back(d.AvgHourData[n]);
    std::sort( items.begin(), items.end(), MeasureDataDateLess  );
    const unsigned itemsCnt = items.size();


    TableT tbl;
    MakeTbl1(body, tbl, itemsCnt+1);
    for(int n=0; n<itemsCnt; ++n)
        AddMeasureData(items[n], tbl, 2+n, "hh:nn");
    AddMeasureData(d.AvgDayData, tbl, 2+itemsCnt, "Сред. знач.");
	
    AddText( body, "Измерение и расчёт в соответствии с "CONC_GOST  );

    AddTag( body, "p");
    AddTag( body, "p");
    AddText( body, "Подпись ответственного лица _________________________________________" );
    
    doc->SaveFile( fn.c_str() );
    return fn;

}

TDateTime dateTimeToDate(TDateTime dt) {
    Word Year, Month, Day;
    DecodeDate(dt, Year, Month, Day);
    return EncodeDate(Year, Month, Day);
}

struct Fuck1{
    Fuck1(TDateTime d1, TDateTime d2) : d1_( dateTimeToDate(d1) ), d2_( dateTimeToDate(d2) ) {}
    bool operator()(const MeasureData& arg) {
        TDateTime dt = dateTimeToDate( EncodeDateTime(arg.dateTime) );
        

        bool ret =  dt<=d2_ && dt>=d1_;
        return !ret;
    }
    const TDateTime d1_, d2_;
};

/*
struct MeasureData
{
    HromatDateTimeInternal dateTime;
	float conc[MAX_GAZCOMP];
    float dConc[MAX_GAZCOMP];
    HromatPhysChem physChem;
};

        AddText( tbl[1+gasCnt+0][ro], PhsChm(pc,5) );
        AddText( tbl[1+gasCnt+1][ro], PhsChm(pc,4) );
        AddText( tbl[1+gasCnt+2][ro], PhsChm(pc,0) );
        AddText( tbl[1+gasCnt+3][ro], PhsChm(pc,1) );
        AddText( tbl[1+gasCnt+4][ro], PhsChm(pc,7) );
        AddText( tbl[1+gasCnt+5][ro], PhsChm(pc,6) );
*/

MeasureData AvgMeasureData( const MeasureData* b, const MeasureData* e )
{
    struct
    {
        double PhsChm( const HromatPhysChem& phch, unsigned i) {
            return GetPhysChemVal(phch, i, 0);
        }

        void set(double val, HromatPhysChem& physChem, unsigned fld) {
            SetPhysChemVal(val, physChem, fld, 0);
        }

        double avg( const std::vector<double>& v) {
            int N = v.size();
            return N==0 ? 0 : std::accumulate<const double*>(v.begin(), v.end(),0.0) / N;
        }
        float avgf( const std::vector<float>& v) {
            int N = v.size();
            return N==0 ? 0 : std::accumulate<const float*>(v.begin(), v.end(), (float)0) / N;
        }
        
    } h;
    std::vector<double>  pc5, pc4, pc0, pc1, pc7, pc6;
    std::vector<float> conc[MAX_GAZCOMP];
    for(  const MeasureData* i=b; i!=e; ++i ) {
        for( int n=0; n<MAX_GAZCOMP; ++n ) {
            float v = i->conc[n];
            conc[n].push_back( v );
        }
            const HromatPhysChem& pc = i->physChem;
            pc5.push_back( h.PhsChm(pc, 5) );
            pc4.push_back( h.PhsChm(pc, 4) );
            pc0.push_back( h.PhsChm(pc, 0) );
            pc1.push_back( h.PhsChm(pc, 1) );
            pc7.push_back( h.PhsChm(pc, 7) );
            pc6.push_back( h.PhsChm(pc, 6) );
    }

    MeasureData r;
    for( int n=0; n<MAX_GAZCOMP; ++n )
        r.conc[n] = h.avgf( conc[n] );
    HromatPhysChem& pc = r.physChem;
    h.set( h.avg(pc5 ), pc, 5);
    h.set( h.avg(pc4 ), pc, 4);
    h.set( h.avg(pc0 ), pc, 0);
    h.set( h.avg(pc1 ), pc, 1);
    h.set( h.avg(pc7 ), pc, 7);
    h.set( h.avg(pc6 ), pc, 6);
    return r;
}

AnsiString Report_Jurnal_AnalizeArchive(Jurnal_AnalizeArchive d)
{
    const AnsiString
        sDir = MyGetExePath(),
        fn = sDir+"журнал_анализ_архив.html";
	ForceDirectories(sDir);


    Elem body, head;
    boost::shared_ptr<TiXmlDocument> doc( HTML::CreateDocument("Среднесуточный отчёт", body, head ) );
    AddStyleSheet( head, "template.css");
    AnsiString s;
    s.sprintf( "Хроматограф %s. Среднесуточный отчёт о содержании компонентов.",
        Hromat900::Instance().HromatType() );
    
    AddText( body, s.c_str(), TextAttr().Class("header1") );
    s.sprintf( "Место отбора проб: %s",  Hromat900::Instance().MestoOtboraProby() );
    AddText( body, s.c_str()  );

    
    std::vector<MeasureData> items;
    for(int n=0; n<std::min( Ceil(d.cntAvgDayData), 70 ); ++n)
        items.push_back(d.AvgDayData[n]);


    if(items.empty()) {
        AddTag( body, "p");
        AddText( AddTag( body, "p"), "Нет данных.");
    } else {
    std::sort( items.begin(), items.end(), MeasureDataDateLess  );
    unsigned itemsCnt = items.size();
    const TDateTime minDt = EncodeDateTime( items.begin()->dateTime ), maxDt = EncodeDateTime( items.back().dateTime );
    if (minDt<maxDt) {
        boost::shared_ptr<TDateTimeRangeDialog> dlg( new TDateTimeRangeDialog(NULL) );
        //dlg->DateTimePicker1->MinDate = minDt;
        //dlg->DateTimePicker1->MaxDate = maxDt;
        //dlg->DateTimePicker2->MinDate = minDt;
        //dlg->DateTimePicker2->MaxDate = maxDt;
        dlg->DateTimePicker1->Date = minDt;
        dlg->DateTimePicker2->Date = maxDt;
        dlg->ShowModal();
        items.erase(
            remove_if(
                items.begin(),
                items.end(),
                Fuck1( dlg->DateTimePicker1->Date, dlg->DateTimePicker2->Date ) ),
            items.end() );

    }

    itemsCnt = items.size();
    
    TableT tbl;
    MakeTbl1(body, tbl, itemsCnt+1);
    for(int n=0; n<itemsCnt; ++n)
        AddMeasureData(items[n], tbl, 2+n, "dd.MM.yyyy");
    AddMeasureData( AvgMeasureData( items.begin(), items.end() ),
        tbl, 2+itemsCnt, "Сред. знач.");
    //MeasureData AvgMeasureData( const MeasureData* b, const MeasureData* e )

    AddText( body, "Измерение и расчёт в соответствии с ИБЯЛ.413538.001-02 МИ"  );

     AddTag( body, "p");
    AddTag( body, "p");
    AddText( body, "Подпись ответственного лица _________________________________________" );

    } //  if(items.empty()) {...    } else {

    doc->SaveFile( fn.c_str() );
    return fn;

}
