//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

#pragma hdrstop

//my
#include "CommSets.h"

#include "uHromatSetsDlg.h"
#include "DrawGridCell.h"
#include "SGrdHlpr.h"
#include "AnsiStringUtils_.h"
#include "bcd_.h"

//#include "Unit1.h"

//vcl
#include <Math.hpp>
#include <FileCtrl.hpp>
#include <DateUtils.hpp>

#include <dialogs.hpp>
#include "MyVclUtils_.h"
#include "xmlUtils_.h"
#include "MyAppProfile.h"
#include "mywinsock2.h"
#include "uFrameGrid.h"

#include "MyAppWinPath.hpp"
#include "..\MY_INCLUDE\myconv.h"
#include "uFormGridColsDlg.h"
#include "Hromat900.hpp"
#include "Unit1.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormHromatSetsDlg *FormHromatSetsDlg;
//---------------------------------------------------------------------------
const GrdCell noCell = MakeGrdCell( NULL, -1, -1);
//------------------------------------------------------------------------------
AnsiString StringList2Str( const StringList& lst )
{
    AnsiString ret;
    for( unsigned i=0; i<lst.size(); ++i )
        ret += lst[i];
    return ret;
}
//------------------------------------------------------------------------------
bool LessStringList::operator()( const StringList& lhs, const StringList& rhs) const
{
    return StringList2Str(lhs)<StringList2Str(rhs);
}
//------------------------------------------------------------------------------
AnsiString HromatConfigVal2Str(unsigned idx, double val)
{
    const HromatVar::Items& vars = HromatVar::GetList();
    const HromatVar::Item& var = vars[idx];
    const unsigned asInt = Ceil(val);
    if(var.type=="time")
        return FormatDateTime( "hh:nn", val );
    if(var.type=="int")
        return IntToStr( asInt );
    if( !var.values.empty() && asInt<var.values.size() )
        return var.values[asInt];
    return FloatToStr( MyFloatToDouble(val) );
}
//------------------------------------------------------------------------------
MyDouble Str2HromatConfigVal(unsigned idx, const AnsiString& s )
{
    const HromatVar::Items& vars = HromatVar::GetList();
    const HromatVar::Item& var = vars[idx];
    if( !var.values.empty() )
    {
        const int idx = MyStringListIndexOf(var.values, s);
        return MyDouble( idx, idx>-1 );
    }
    if(var.type=="time")
    {
        TDateTime v;
        const bool isOk = TryStrToTime(s+":00", v);
        return MyDouble( v.Val, isOk );
    }    
    MyDouble ret = MyStrToD( s ),
        min = MyStrToD( var.min ), max = MyStrToD( var.max );
    const bool badValue = !ret.second || min.second && ret.first<min.first ||
        max.second && ret.first>max.first ||
        (var.type=="int") && !MyStrToInt( s ).second;
    ret.second &= !badValue;
    return ret;
}

//------------------------------------------------------------------------------
void SkipNotFloatInput( TObject *Sender, char &keyChar)
{
    TEdit *ed = static_cast<TEdit *>(Sender);
    if(keyChar=='-' && ed->SelStart!=0 )
        keyChar = 0;
    else
        SkipNotFloatInput( ed->Text, keyChar);

}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
__fastcall TFormHromatSetsDlg::TFormHromatSetsDlg(TComponent* Owner, const VDouble& vals)
	: TForm(Owner),
    xml_( TiXML::GiveNotNulElement( ProfileXmlElem(), "Диалог_параметров_хроматографа" ) ), activated_(false)
    , var2Cell_(HromatVar::GetList().size(), noCell ),
    originalVals_(vals)
{
    const HromatVar::Items& vars = HromatVar::GetList();
    const HromatVar::Groups& groups = HromatVar::GetGroups();
    for( unsigned nGrp=0; nGrp<groups.size(); ++nGrp )
    {
        const HromatVar::Group& grp = groups[nGrp];
        TFrameGrid*  
        frm =  new TFrameGrid(this, ScrollBox1, grp.name );
        frm->Name = "rpn"+IntToStr(nGrp);
        frm->Top = 1000000;
        frm->Colapse();
        frm->drwGrd.SetAfterSetCanvasFoo( &AfterSetCanvasGrd );
        frm->grd1->OnSelectCell = &grdSelectCell;
        frm->grd1->OnMouseDown = &StringGrid1MouseDown;

        const unsigned cnt = grp.vars.size();
        frm->SetRowCount( cnt+1 );
        TStringGrid *grd = frm->grd1;    

        grd->ColCount = GrdValsCol::count;
        grd->Cells[GrdValsCol::num]     [0] = "№";
        grd->Cells[GrdValsCol::name]    [0] = "Параметр";
        grd->Cells[GrdValsCol::val]     [0] = "Значение";
        grd->Cells[GrdValsCol::units]   [0] = "Единицы измерения";
        grd->Cells[GrdValsCol::min]     [0] = "Мин.";
        grd->Cells[GrdValsCol::max]     [0] = "Макс.";

        StringgridControl::RegisterOneEditControl( edGrd, grd  );
        for( unsigned nVar = 0; nVar<cnt; ++nVar )
        {
            const unsigned varIdx = grp.vars[nVar];
            const HromatVar::Item& var = vars[ varIdx ];
            const GrdCell cell = MakeGrdCell( grd, 2, nVar+1);
            var2Cell_[ varIdx ] = cell;
            cell2Var_[ cell ] = varIdx;
            grd->Cells[GrdValsCol::num][nVar+1] = nVar+1;
            grd->Cells[GrdValsCol::name][nVar+1] = var.name;
            grd->Cells[GrdValsCol::val][nVar+1] = HromatConfigVal2Str(varIdx, vals[varIdx] );
            grd->Cells[GrdValsCol::units][nVar+1] = var.units;
            grd->Cells[GrdValsCol::min][nVar+1] = var.min;
            grd->Cells[GrdValsCol::max][nVar+1] = var.max;
            my::Grd::SetEditing(grd, 0);

            if(!var.visible)
            {
                grd->RowHeights[nVar+1]=0;
                continue;
            }
            if(var.type=="time")
            {
                StringgridControl::RegisterRowControl( edTime, grd, nVar+1 );
                continue;
            }
            
            if( !var.values.empty() )
            {
                TComboBox *cb;
                ValsList2ComboBox::const_iterator iCb = valsList2ComboBox_.find(var.values);
                if( iCb==valsList2ComboBox_.end() )
                {
                    cb = new TComboBox(this);
                    cb->Parent = this;
                    cb->Name = MYSPRINTF_( "CB_%d_%d_%d", nGrp, varIdx, nVar);
                    cb->Font->Assign( grd->Font );
                    cb->Style = csDropDownList;
                    for( int i=0; i<var.values.size(); ++i )
                    {
                        const AnsiString s = var.values[i];
                        cb->Items->Add(s);
                    }
                    cb->OnKeyDown = &edGrdKeyDown;
                    cb->DropDownCount = 1000;
                    valsList2ComboBox_.insert( std::make_pair(var.values, cb) );
                }
                else
                    cb = iCb->second;
                StringgridControl::RegisterRowControl( cb, grd, nVar+1 );
            }
        }
        frames_.push_back( boost::shared_ptr<TFrameGrid>(frm) );
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormHromatSetsDlg::FormActivate(TObject *Sender)
{
	if( activated_ ) return;
    activated_ = true;
    TiXML::LoadWindowPlacement( TiXML::GiveNotNulElement( xml_, "Placement" ), Handle );
    for( unsigned i=0; i<frames_.size(); ++i )
        my::Grd::SetColWidthsAsStr( frames_[i]->grd1, READ_XML_STR_( "ширина_колонок"+IntToStr(i),
            "280 85 70 70 120") );
}
//------------------------------------------------------------------------------
__fastcall TFormHromatSetsDlg::~TFormHromatSetsDlg()
{
    if( activated_ ) TiXML::SaveWindowPlacement( TiXML::GiveNotNulElement( xml_, "Placement" ), Handle );
    for( unsigned i=0; i<frames_.size(); ++i )
        WRITE_XML_STR_( "ширина_колонок"+IntToStr(i), my::Grd::ColWidthsToStr(frames_[i]->grd1) );
    ProfileXmlDoc()->SaveFile();

    StringgridControl::UnregisterAll( this );

}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void TFormHromatSetsDlg::UpdateValues(const VDouble& originalVals)
{
    originalVals_ = originalVals;
    const HromatVar::Items& vars = HromatVar::GetList();
    for( unsigned iVar=0; iVar<vars.size(); ++iVar )
    {
        //const HromatVar::Item& var = vars[iVar];
        const GrdCell cell = var2Cell_[iVar];
        TStringGrid *grd = cell.first;
        const int row = cell.second.second;
        if(grd!=0 && row!=-1 && row<grd->RowCount)
            grd->Cells[GrdValsCol::val][row] = HromatConfigVal2Str(iVar, originalVals_[iVar] );
    }
}
//------------------------------------------------------------------------------
VDouble TFormHromatSetsDlg::Result() const
{
    const HromatVar::Items& vars = HromatVar::GetList();
    VDouble ret( vars.size()+1 );
    for( unsigned iVar=0; iVar<vars.size(); ++iVar )
    {
        //const HromatVar::Item& var = vars[iVar];
        const GrdCell cell = var2Cell_[iVar];
        TStringGrid *grd = cell.first;
        const int row = cell.second.second;
        if(grd!=0 && row<grd->RowCount)
        {
            const AnsiString s = grd->Cells[GrdValsCol::val][row];
            const MyDouble v = Str2HromatConfigVal( iVar, s );
            ret[iVar] = v.first;
        }
    }
    return ret;
}
//------------------------------------------------------------------------------
bool TFormHromatSetsDlg::ValidateResult( VDouble& ret )
{
    const HromatVar::Items& vars = HromatVar::GetList();
    ret.resize( vars.size()+1 );
    for( unsigned iVar=0; iVar<vars.size(); ++iVar )
    {
        //const HromatVar::Item& var = vars[iVar];
        const GrdCell cell = var2Cell_[iVar];
        TStringGrid *grd = cell.first;
        const int row = cell.second.second;
        if(grd!=0 && row<grd->RowCount)
        {
            const AnsiString s = grd->Cells[GrdValsCol::val][row];
            const MyDouble v = Str2HromatConfigVal( iVar, s );
            if( !v.second )
            {
                ShowUserInputError( grd->Cells[GrdValsCol::name][row], s);
                return 0;
            }
            ret[iVar] = v.first;
        }
    }
    return 1;
}
//------------------------------------------------------------------------------
int TFormHromatSetsDlg::Cell2Var( TObject *Sender, int ACol, int ARow )
{
    TStringGrid* grd = static_cast<TStringGrid*>(Sender);
    const GrdCell cell = MakeGrdCell( grd, ACol, ARow);
    const HromatVar::Items& vars = HromatVar::GetList();
    Cell2VarMap::const_iterator i = cell2Var_.find(cell);
    return ( i!=cell2Var_.end() && i->second<vars.size() ) ? i->second : -1; 

}
//------------------------------------------------------------------------------
void __fastcall TFormHromatSetsDlg::grdSelectCell(TObject *Sender, int ACol,
      int ARow, bool &CanSelect)
{
    TStringGrid* grd = static_cast<TStringGrid*>(Sender);
    Label1->Caption =
        grd->Cells[1][ARow]+" "+grd->Cells[2][ARow]+" "+
        grd->Cells[2][ARow]+" "+grd->Cells[3][ARow]+" "+
        grd->Cells[4][ARow]+" "+grd->Cells[5][ARow];

    bool vis = ARow>0 && ACol==GrdValsCol::val;
    if(vis)
    {
        const int varIdx = Cell2Var( Sender, ACol, ARow );
        const HromatVar::Items& vars = HromatVar::GetList();
        if( varIdx==-1 || vars[varIdx].readOnly || !vars[varIdx].visible)
            vis = 0;
    }
    my::Grd::SetEditing(grd, vis);
}
//------------------------------------------------------------------------------
void TFormHromatSetsDlg::AfterSetCanvasGrd( TStringGrid* grd, int col, int row,
	MyGridCellState drawState, TRect rect, int& txtX, int& txtY )
{
    if( IsLeftAligmentAndNoAutoColWidthCell(grd, col, row) ) txtX = rect.left + 3;

	TCanvas &cnv = *grd->Canvas;

    if ( col!=GrdValsCol::val )
        cnv.Brush->Color  = grd->FixedColor;

    if ( row==0 )
    {
        cnv.Brush->Color  = 0x00DDDDDD;
        cnv.Font->Size  = grd->Font->Size+1;
    }

    if ( drawState & mygdHot )
        cnv.Font->Color  = clNavy;

    if ( col==GrdValsCol::val && drawState & mygdSelected )
    {
        cnv.Font->Color  = clWhite;
        cnv.Brush->Color = clMenuHighlight;
    }
}
//------------------------------------------------------------------------------
void TFormHromatSetsDlg::OnDeterminateColumnWidth( TStringGrid* grd, int col, int row, bool& ff)
{
    ff = !IsLeftAligmentAndNoAutoColWidthCell( grd, col, row );
}
//------------------------------------------------------------------------------
bool TFormHromatSetsDlg::IsLeftAligmentAndNoAutoColWidthCell( TStringGrid* grd, int col, int row )
{
    return true;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void __fastcall TFormHromatSetsDlg::edGrdExit(TObject *Sender)
{
    TEdit* ed = static_cast<TEdit*>(Sender);
    TStringGrid* grd = StringgridControl::GetActiveGrid();
    const AnsiString s = ed->Text;
    const int varIdx = Cell2Var( grd, grd->Col, grd->Row );
    if( !Str2HromatConfigVal(varIdx, s ).second )
    {
        ShowUserInputError( grd->Cells[GrdValsCol::name][grd->Row], s);
        ed->Visible = 1;
        ActiveControl = ed;
    }
}
//------------------------------------------------------------------------------
void __fastcall TFormHromatSetsDlg::edGrdKeyDown(TObject *Sender,
      WORD &Key, TShiftState Shift)
{
    TWinControl *ctrl = dynamic_cast<TWinControl*>(Sender);
    TStringGrid* grd = StringgridControl::GetActiveGrid();
    const bool
        escapeKey = Key==VK_ESCAPE,
        enterKey  = Key==VK_RETURN
    ;

    if( escapeKey )
    {
        const TNotifyEvent onExit = GetCtrlOnExit( ctrl );
        SetCtrlOnExit( ctrl, NULL );
        ctrl->Visible = 0;
        SetCtrlOnExit( ctrl, onExit );
        grd->Cells[grd->Col][grd->Row] = StringgridControl::GetStoredValue(ctrl);

    }
    if( escapeKey || enterKey )
        ActiveControl = grd;
}
//---------------------------------------------------------------------------
void __fastcall TFormHromatSetsDlg::edGrdKeyPress(TObject *Sender,
      char &Key)
{
    TEdit* ed = static_cast<TEdit*>(Sender);
    TStringGrid* grd = StringgridControl::GetActiveGrid();
    const AnsiString s = ed->Text;

    const int varIdx = Cell2Var( grd, grd->Col, grd->Row );
    const HromatVar::Items& vars = HromatVar::GetList();
    if( varIdx==-1 ) return;
    const HromatVar::Item& var = vars[varIdx];
    if( var.readOnly || !var.visible ) return;
    if(var.type=="int")
        SkipNotIntegerInput( s, Key);
    else
        SkipNotFloatInput( Sender, Key);
}
//---------------------------------------------------------------------------
void __fastcall TFormHromatSetsDlg::Button2Click(TObject *Sender)
{
    AnsiString fn = READ_XML_STR_( "файл", GetPersonalDir()+"sysconfig.ini");
	if( !PromptForFileName(fn, "", "", "Сохранить паракметры конфигурации хроматографа", "", true) )
    	return;
    WRITE_XML_STR_( "файл", fn );
    ::DeleteFile( fn.c_str() );
    boost::shared_ptr<TIniFile> inI( new TIniFile(fn) );
    TIniFile &ini = *inI.get();
    const HromatVar::Items& vars = HromatVar::GetList();
    for( unsigned i=0; i<vars.size(); ++i )
    {
        const GrdCell &cell = var2Cell_[i];
        TStringGrid *grd = cell.first;
        const int row = cell.second.second;
        if(grd!=0 && row!=-1 && row<grd->RowCount)
        {
            const HromatVar::Item& var = vars[i];
            const AnsiString val = grd->Cells[GrdValsCol::val][ row ];
            if( !val.IsEmpty() )
                ini.WriteString("main", IntToStr(i)+"_"+vars[i].name, val  );
        }
        else {
            int a = 0;
        }

    }

}
//---------------------------------------------------------------------------
void __fastcall TFormHromatSetsDlg::Button1Click(TObject *Sender)
{
    AnsiString fn = READ_XML_STR_( "файл", GetPersonalDir()+"sysconfig.ini");
	if( !PromptForFileName(fn, "", "", "Открыть паракметры конфигурации хроматографа", "", false) )
        return;
    WRITE_XML_STR_( "файл", fn );

    boost::shared_ptr<TIniFile> inI( new TIniFile(fn) );
    TIniFile &ini = *inI.get();
    const HromatVar::Items& vars = HromatVar::GetList();
    for( unsigned i=0; i<vars.size(); ++i )
    {
        const AnsiString val = ini.ReadString("main", IntToStr(i)+"_"+vars[i].name, ""  );
        const GrdCell &cell = var2Cell_[i];
        TStringGrid *grd = cell.first;
        const int row = cell.second.second;
        if(grd!=0 && row!=-1 && row<grd->RowCount)
            grd->Cells[GrdValsCol::val][ row ] = val;
    }
//    Form1->UpdateDetector2Info( hromatSysConfig_.size()>3 && hromatSysConfig_.at( 3 )==2 );
}
//---------------------------------------------------------------------------
void __fastcall TFormHromatSetsDlg::StringGrid1MouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
    //
    TStringGrid* grd = static_cast<TStringGrid*>(Sender);
    // ячейка под курсором
	const TPoint hotCell = my::Grd::HotCell(grd);
	if( !( Button==mbLeft && hotCell.y==0 ) ) return;
    boost::shared_ptr<TFormGridColsDlg> dlg( new TFormGridColsDlg(NULL, grd, true, 0) );
    dlg->clb->ItemEnabled[1] = 0;
    dlg->ShowModal();
    const AnsiString s = my::Grd::ColWidthsToStr(grd);
    for( unsigned i=0; i<frames_.size(); ++i )
        my::Grd::SetColWidthsAsStr( frames_[i]->grd1, s);
}
//---------------------------------------------------------------------------



