//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "uFormAppProfileDlg.h"
#include "AnsiStringUtils_.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormAppProfileDlg *FormAppProfileDlg;
#define PRMPT  "Введите наименование профиля - строку символов, состоящую из цифр, букв или символов \"_\"."

//---------------------------------------------------------------------------

void ExtractDeviceNames(TiXmlElement* elem, std::vector<AnsiString>& names)
{
	names.clear();
    if( !elem ) return;
    for( elem = elem->FirstChildElement(); elem; elem=elem->NextSiblingElement() )
    	names.push_back( elem->Value() );
}
//------------------------------------------------------------------------------
bool IsCorrect(const AnsiString& s)
{
	return !s.IsEmpty() && !std::isdigit(s[1]) && IsAlphaNumber(s);
}


__fastcall TFormAppProfileDlg::TFormAppProfileDlg(TComponent* Owner,
	TiXmlElement *profilesElem, TiXmlDocument* doc)
	: TForm(Owner), profilesElem_(profilesElem), doc_(doc),
    itms( *ListBox1->Items )
{
    std::vector<AnsiString> names;
    ExtractDeviceNames( profilesElem_, names);
    for( unsigned i=0; i<names.size(); ++i )
    	ListBox1->Items->Add(names[i]);
}
//---------------------------------------------------------------------------


void __fastcall TFormAppProfileDlg::ListBox1Click(TObject *Sender)
{
	Button1->Enabled = (ListBox1->ItemIndex>-1);
    Button4->Enabled = Button1->Enabled;
    Button5->Enabled = Button1->Enabled;
}
//---------------------------------------------------------------------------

void __fastcall TFormAppProfileDlg::ListBox1DblClick(TObject *Sender)
{
	const int idx = ListBox1->ItemIndex;
    if(idx>-1) ModalResult = mrOk;

}
//---------------------------------------------------------------------------
void __fastcall TFormAppProfileDlg::FormKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
	if(Key==VK_ESCAPE) ModalResult = mrNo;	
}
//---------------------------------------------------------------------------
void __fastcall TFormAppProfileDlg::ListBox1KeyDown(TObject *Sender,
      WORD &Key, TShiftState Shift)
{
    if(Key==VK_RETURN && ListBox1->ItemIndex>-1) ModalResult = mrOk;
}
//---------------------------------------------------------------------------

void __fastcall TFormAppProfileDlg::Button2Click(TObject *Sender)
{
	AnsiString s = "";
    while( !IsCorrect(s) || itms.IndexOf(s)!=-1 )
    	if( !InputQuery("Добавить профиль", PRMPT, s)  )
        	return;

    itms.Add(s);
    profilesElem_->LinkEndChild( new TiXmlElement(s.c_str()) );
    doc_->SaveFile();
	
}
//---------------------------------------------------------------------------

void __fastcall TFormAppProfileDlg::Button4Click(TObject *Sender)
{
	if(Idx()==-1) return;
    TiXmlElement* elem = profilesElem_->FirstChildElement( itms[Idx()].c_str() );
    if(elem)
    {
    	profilesElem_->RemoveChild(elem);
        doc_->SaveFile();
    }

    itms.Delete( Idx() );
    ListBox1->OnClick(NULL);	
}
//---------------------------------------------------------------------------

void __fastcall TFormAppProfileDlg::Button5Click(TObject *Sender)
{
	const int idx = Idx();
    if(idx==-1) return;
    AnsiString s = itms[idx];
    TiXmlElement* elem = profilesElem_->FirstChildElement( s.c_str() );

    do
    	if( !InputQuery("Переименовать профиль", PRMPT, s)  )
        	return;
    while( !IsCorrect(s) );

    if(elem)
    {
    	elem->SetValue( s.c_str() );
        doc_->SaveFile();
    }
	itms.Delete(idx);
    itms.Insert(idx, s);
    ListBox1->OnClick(NULL);	
}
//---------------------------------------------------------------------------


