//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#pragma hdrstop
//---------------------------------------------------------------------------
#include "HromatPic.h"
#include "SGrdHlpr.h"
#include "AnsiStringUtils_.h"
#include "listViewHelpr_.h"
#include "xmlUtils_.h"
#include "..\MY_INCLUDE\myconv.h"
#include "DrawGridCell.h"
#include "Unit1.h"
//---------------------------------------------------------------------------

#pragma package(smart_init)

struct ShowHromatPic
{
	TStringGrid* grd_;
    const HromatPicData* beg_;
    explicit ShowHromatPic( TStringGrid*, const HromatPicData* );
    void operator()( const HromatPicData& ) const;
};
//------------------------------------------------------------------------------

struct ShowHromatBasePic
{
	TStringGrid* grd_;
    const HromatBasePicData* beg_;
    explicit ShowHromatBasePic( TStringGrid*, const HromatBasePicData* );
    void operator()( const HromatBasePicData& ) const;
};

struct SaveHromatBasePic
{
	TiXmlElement *xml_;
    explicit SaveHromatBasePic( TiXmlElement *xml);
    void operator()( const HromatBasePicData& ) const;
};




#define MY_DEFIS AnsiString("-")

AnsiString PicValToStr(float v)
{
    return v>0 ? FloatToStr( MyFloatToDouble(v) ) : MY_DEFIS;
}

void ShowPicValInGrd(TStringGrid* grd, int col, int row, float t)
{
    AnsiString s = t>0 ? PicValToStr(t) : MY_DEFIS;
	grd->Cells[col][row] = s;
}

void PicConcToStr( AnsiString& sC, AnsiString& sD, float c, float d )
{
	const unsigned
            countAfterCommaDigitsConc  = CountAfterCommaDigits( MyFloatToDouble(c)  ),
            countAfterCommaDigitsIndet  = CountAfterCommaDigits( MyFloatToDouble(d)  ),
            countAfterCommaDigits = std::max( countAfterCommaDigitsConc, countAfterCommaDigitsIndet);

    sC = c>=0 ? MyFormatFloat(c, countAfterCommaDigits) : MY_DEFIS;
    sD = d>=0 ? MyFormatFloat(d, countAfterCommaDigits) : MY_DEFIS;
}

void ShowPicConcInGrd( TStringGrid* grd, int col, int row, float c, float d)
{
	AnsiString sC, sD;
    PicConcToStr( sC, sD, c, d );
    grd->Cells[col][row] = sC;
    grd->Cells[col+1][row] = sD;
}


SaveHromatBasePic::SaveHromatBasePic( TiXmlElement *xml) :	xml_(xml)
{}
void SaveHromatBasePic::operator()( const HromatBasePicData& pic) const
{
	TiXmlElement *elem = xml_->LinkEndChild( new TiXmlElement("PIC") )->ToElement();
    TiXML::SetAtr( elem, "NM", pic.name_ );
    TiXML::SetAtr( elem, "C", FloatToStr( MyFloatToDouble(pic.conc_) ) );
    TiXML::SetAtr( elem, "T", FloatToStr( MyFloatToDouble(pic.t_) ) );
    TiXML::SetAtr( elem, "enable", FloatToStr( MyFloatToDouble(pic.enable) ) );
}


ShowHromatPic::ShowHromatPic( TStringGrid* grd, const HromatPicData* beg ) : grd_(grd), beg_(beg) {}
void ShowHromatPic::operator()( const HromatPicData& pik ) const
{
    AnsiString rows[PIK_FIELD::COUNT];
    std::fill( rows, rows+PIK_FIELD::COUNT, "" );
    const int row = &pik - beg_ + 1;
    /*
    grdPik->Cells[0][0] = "№";
    grdPik->Cells[1][0] = "Компонент";
    grdPik->Cells[2][0] = "C, моль,%";
    grdPik->Cells[3][0] = "±C, моль,%";
    grdPik->Cells[4][0] = "tу, c";
    grdPik->Cells[5][0] = "A, В*с";
    grdPik->Cells[6][0] = "H, В";
    */
    grd_->Cells[0][row] = row;
    grd_->Cells[1][row] = pik.name_;

    ShowPicConcInGrd( grd_, 2, row, pik.conc_, pik.dConc_);
    //ShowPicConcInGrd( grd_, 4, row, pik.hourConc_, pik.dHourConc_);
    ShowPicValInGrd( grd_, 4, row, pik.t_);
    ShowPicValInGrd( grd_, 5, row, pik.t1_);
    ShowPicValInGrd( grd_, 6, row, pik.t2_);

    ShowPicValInGrd( grd_, 7, row, pik.amplitude_);
    ShowPicValInGrd( grd_, 8, row, pik.area_);
}
//------------------------------------------------------------------------------
ShowHromatBasePic::ShowHromatBasePic( TStringGrid* grd, const HromatBasePicData* beg ) : grd_(grd), beg_(beg) {}
void ShowHromatBasePic::operator()( const HromatBasePicData& pik ) const
{
    const int row = 1 + &pik - beg_;
    //"№", "Наименование", "Конц.", "t,c", "A, В*с", "H, В"
    grd_->Cells[0][row] = row;
    grd_->Cells[1][row] = pik.name_;
    grd_->Cells[2][row] = FloatToStr( MyFloatToDouble(pik.conc_) );

    my::Grd::SelectRow( grd_, row, pik.enable );

    ShowPicValInGrd( grd_, 3, row, pik.t_);
}
//------------------------------------------------------------------------------
void ShowHromatPicTable( TStringGrid* grd, const HromatPicData* beg, const HromatPicData* end )
{
	my::Grd::ClearTStringGrid( grd, 1, 1 );
    const int sz = end-beg;
    assert(sz>=0);
    my::Grd::SetRowVis(grd, 0, sz);
    grd->RowCount = sz+1;
    if(sz==0) return;
    std::for_each( beg, end, ShowHromatPic( grd, beg ) );
    grd->FixedRows = 1;
    my::Grd::RedrawGrid(grd);
}
//------------------------------------------------------------------------------

bool TuLess( const HromatBasePicData& lhs, const HromatBasePicData& rhs )
{
	return lhs.t_<rhs.t_;
}

void ShowHromatBasePicTable( TStringGrid* grd, const HromatBasePicData* beg, const HromatBasePicData* end )
{
	std::vector<HromatBasePicData> v(beg,end );
    std::sort( v.begin(), v.end(),  TuLess);
	my::Grd::ClearTStringGrid( grd, 1, 1 );
    const int sz = end-beg;
    assert(sz>=0);
    grd->RowCount = sz+1;
    std::for_each( v.begin(), v.end(), ShowHromatBasePic( grd, v.begin() ) );
    if(sz>0) grd->FixedRows = 1;
}
//------------------------------------------------------------------------------
HromatBasePicTable MakeBasePicTableFromGrd(TStringGrid* grd)
{
	const unsigned sz = grd->RowCount-1;
    HromatBasePicTable ret( sz );
    for( unsigned i=0; i<sz; ++i )
    {
        MyDouble val;
        HromatBasePicData& pik = ret[i];
        //"№", "Наименование", "Конц.", "t,c", "A, В*с", "H, В"

        const AnsiString sName = grd->Cells[1][i+1];
        strncpy(pik.name_, sName.c_str(), HROMAT_PIC_NAME_LEN-1);

        #define SET_FLOAT_FIELD_(nn,fld)\
        val = MyStrToD( grd->Cells[nn][i+1] ); assert(val.second); pik.##fld##_ = MyDoubleToFloat(val.first);
        SET_FLOAT_FIELD_(2, conc );
        SET_FLOAT_FIELD_(3, t );
        #undef SET_FLOAT_FIELD_
        pik.enable      = my::Grd::IsRowSelected(grd, i+1);
        pik.nouse2_     = 0;
        pik.nouse3_     = 0;
    }
    return ret;
}
//------------------------------------------------------------------------------
void InitializeBasePicTableGrd( TStringGrid* grdPik)
{
	 //"№", "Наименование", "Конц.", "t,c", "A, В*с", "H, В"
	grdPik->ColCount = 4;
    grdPik->Cells[0][0] = "№";
    grdPik->Cells[1][0] = "Компонент";


    grdPik->Cells[2][0] = AnsiString().sprintf( "Концентрация C, %s", Form1->MeasureUnits);


    grdPik->Cells[3][0] = "Время удерживания tу, c";

    static struct
    {
        void __fastcall grdBasePikKeyDown(TObject *Sender,
            WORD &Key, TShiftState Shift)
        {
            if(Key==VK_ESCAPE) {
                TStringGrid* grd = static_cast<TStringGrid*>(Sender);
                my::Grd::InveertRowselection(grd, grd->Row);
            }
        }
    } h;
    grdPik->OnKeyDown = h.grdBasePikKeyDown;

}
//------------------------------------------------------------------------------
void InitializePicTableGrd( TStringGrid* grdPik)
{
    //"№", "Наименование", "Конц.", "±конц.", "t,c", "t1,с", "t2,с", "A, В*с", "H, В"
	grdPik->ColCount = 9;
    grdPik->Cells[0][0] = "№";
    grdPik->Cells[1][0] = "Компонент";
    grdPik->Cells[2][0] =  AnsiString().sprintf("C, %s", Form1->MeasureUnits);
    grdPik->Cells[3][0] = AnsiString().sprintf("±C, %s", Form1->MeasureUnits);
    grdPik->Cells[4][0] = "tу, c";
    grdPik->Cells[5][0] = "t1, c";
    grdPik->Cells[6][0] = "t2, c";
    grdPik->Cells[7][0] = "A, В*с";
    grdPik->Cells[8][0] = "H, В";
}
//------------------------------------------------------------------------------
HromatBasePicTable MakeBasePicTableFromXML(TiXmlElement *xml)
{
	HromatBasePicTable ret;
    for( TiXmlElement *elem = xml->FirstChildElement(); elem; elem=elem->NextSiblingElement() )
    {
    	HromatBasePicData pic;
        const AnsiString sName = TiXML::GetStrAtr( elem, "NM");
        if( sName.IsEmpty() ) continue;
        strncpy(pic.name_, sName.c_str(), HROMAT_PIC_NAME_LEN-1);
        pic.name_[HROMAT_PIC_NAME_LEN-1] = '\0';
        pic.conc_ 		= MyDoubleToFloat( TiXML::GetFloatAtrDef( elem, "C", 0) );
        pic.t_ 			= MyDoubleToFloat( TiXML::GetFloatAtrDef( elem, "T", 0) );
        pic.enable     = MyDoubleToFloat( TiXML::GetFloatAtrDef( elem, "enable", 1) );
        pic.nouse2_     = 0;
        pic.nouse3_     = 0;
        ret.push_back(pic);
    }
    return ret;
}

void SaveHromatBasePicTable( TiXmlElement *xml, const HromatBasePicData* beg, const HromatBasePicData* end)
{
    xml->Clear();
    std::for_each( beg, end, SaveHromatBasePic( xml ) );
}

