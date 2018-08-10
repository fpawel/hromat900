//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "autorize_form.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
//TAutorizeForm *AutorizeForm;
//---------------------------------------------------------------------------
__fastcall TAutorizeForm::TAutorizeForm(TComponent* Owner)
    : TForm(Owner)
{
    mayIComing = NULL;
    m_selectedItem = NULL;
    ErrorLabel->Caption = "";

    m_usersBar = new ExpressBar( UsersBarPanel );
    
    //addUser( "void" );
    //addUser( "Вася Пупкин-" );
    //addUser( "Администратор" );
}
//---------------------------------------------------------------------------
__fastcall TAutorizeForm::~TAutorizeForm()
{
    if ( m_usersBar )
    {
        delete m_usersBar;
    }
}
//---------------------------------------------------------------------------
void TAutorizeForm::addUser( const std::string &name, IconType iconType )
{
    ExpressBarItem *userItem = new ExpressBarItem();
    userItem->setName( name.c_str() );

    Graphics::TBitmap *bitmap = new Graphics::TBitmap();
    ImageList->GetBitmap( iconType, bitmap );
    userItem->setIcon( bitmap );
    userItem->setIconAdjustment( ExpressBarItem::IconAdjustment_Left );
    userItem->setNameAdjustment( ExpressBarItem::TextAdjustment_Left );

    userItem->onSelect = onUserSelected;

    m_usersBar->add( userItem );
}
//---------------------------------------------------------------------------
std::string TAutorizeForm::current()
{
  return m_current;
}
//---------------------------------------------------------------------------
void TAutorizeForm::setCurrent( const std::string &name )
{
  m_current = name;
}
//---------------------------------------------------------------------------
void TAutorizeForm::onUserSelected( ExpressBarItem *item )
{
    setCurrent(item->name().c_str());
    NameEdit->Text = item->name();
    PasswordEdit->Text = "";
    PasswordEdit->SetFocus();

    if ( m_selectedItem )
    {
        m_selectedItem->unselect();
    }
    m_selectedItem = item;
}
//---------------------------------------------------------------------------
void TAutorizeForm::error( const char* msg )
{
    //ModalResult = mrNone;
    ErrorLabel->Caption = msg;
}
//---------------------------------------------------------------------------
void TAutorizeForm::clearError()
{
    ErrorLabel->Caption = "";
}
//---------------------------------------------------------------------------
void __fastcall TAutorizeForm::NameEditChange(TObject *)
{
    clearError();
}
//---------------------------------------------------------------------------
void __fastcall TAutorizeForm::OkButtonClick(TObject *)
{
    if ( mayIComing )
    {
        if ( !mayIComing( NameEdit->Text.c_str(), PasswordEdit->Text.c_str() ) )
        {
            ModalResult = mrNone;
        }
    }
}
//---------------------------------------------------------------------------
void TAutorizeForm::close()
{
    //Close();
}
//---------------------------------------------------------------------------
bool TAutorizeForm::show()
{
    int modalResult = ShowModal();
    bool result = modalResult == mrOk;
    return result;
}
//---------------------------------------------------------------------------
void __fastcall TAutorizeForm::CancelButtonClick(TObject *)
{
    ModalResult = mrCancel;
    Close();
}
//---------------------------------------------------------------------------
void __fastcall TAutorizeForm::FormShow(TObject *)
{
    if ( m_usersBar )
    {
      for (unsigned int i = 0; i < m_usersBar->count(); i++)
      {
        ExpressBarItem* item = m_usersBar->item(i);
        if ( item->name().c_str() == m_current )
        {
          item->select();
          return;
        }
      }
    }
}
//---------------------------------------------------------------------------
