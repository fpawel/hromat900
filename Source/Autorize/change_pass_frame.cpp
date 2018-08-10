//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include <assert>
#include "change_pass_frame.h"
//#include <get_md5_hash.h>
#include <functors.h>
#include "cnfdntl_.h"
#include "Softid.hpp"

const TColor GOOD_COLOR = clGreen;//(TColor)RGB( 73, 161, 73 );

const TColor ERROR_COLOR = (TColor)RGB( 0xff, 0x9b, 0x9b );
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TChangePassFrame *ChangePassFrame;
//---------------------------------------------------------------------------
__fastcall TChangePassFrame::TChangePassFrame(TComponent* Owner)
:   TFrame(Owner),
    m_selectedItem( NULL ),
    m_usersBar( NULL )
{
    ErrorLabel->Caption = "";
    CreateErrorLabel->Caption = "";

    m_usersBar = new ExpressBar( UsersPanel );
}
//---------------------------------------------------------------------------
void TChangePassFrame::addUser(
    const std::string &name,
    const std::string &pass,
    int accsess,
    IconType iconType
)
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

    User_ user;
    user.name = name;
    user.password = pass;
    user.barItem = userItem;
    user.accsess = accsess;

    m_users.push_back( user );
}
//---------------------------------------------------------------------------
void TChangePassFrame::removeUser( const std::string &name )
{
    Users_::iterator found = std::find_if(
        m_users.begin(),
        m_users.end(),
        EqualByName( name )
    );

    if ( found != m_users.end() )
    {
        User_ &user = *found;
        m_usersBar->remove( user.barItem->index() );
        m_users.erase( found );
    }

    m_selectedItem = NULL;
}
//---------------------------------------------------------------------------
void TChangePassFrame::error( TEdit *errorField, const std::string &msg )
{
    ErrorLabel->Font->Color = clRed;
    if ( errorField )
    {
        errorField->Color = ERROR_COLOR;
    }
    ErrorLabel->Caption = msg.c_str();
}
//---------------------------------------------------------------------------
void TChangePassFrame::createError( TEdit *errorField, const std::string &msg )
{
    CreateErrorLabel->Font->Color = clRed;
    CreateErrorLabel->Caption = msg.c_str();
    if ( errorField )
    {
        errorField->Color = ERROR_COLOR;
    }    
}
//---------------------------------------------------------------------------
void TChangePassFrame::clearError()
{
    NameEdit->Color = clWindow;
    OldPassEdit->Color = clWindow;
    NewPassEdit->Color = clWindow;
    NewPassConfirmEdit->Color = clWindow;

    CreateNameEdit->Color = clWindow;
    CreatePassEdit->Color = clWindow;
    CreatePassConfirmEdit->Color = clWindow;

    ErrorLabel->Caption = "";
    CreateErrorLabel->Caption = "";     
}
//---------------------------------------------------------------------------
void __fastcall TChangePassFrame::NameEditChange(TObject *)
{
    clearError();    
}
//---------------------------------------------------------------------------
void TChangePassFrame::onUserSelected( ExpressBarItem *item )
{
    NameEdit->Text = item->name();
    Label10->Caption = "";

    User::AccessToStr(1);

    const int i = item->index();
    if(i<m_users.size())
        Label10->Caption = User::AccessToStr( m_users[i].accsess );
    else
        Label10->Caption = "";


    OldPassEdit->Text = "";
    NewPassEdit->Text = "";
    NewPassConfirmEdit->Text = "";

    CreateNameEdit->Text = item->name();
    CreatePassEdit->Text = "";
    CreatePassConfirmEdit->Text = "";

    OldPassEdit->SetFocus();

    if ( m_selectedItem )
    {
        m_selectedItem->unselect();
    }
    m_selectedItem = item;
}
//---------------------------------------------------------------------------
void __fastcall TChangePassFrame::ChangeButtonClick(TObject *)
{
    Users_::iterator found = std::find_if(
        m_users.begin(),
        m_users.end(),
        EqualByName( NameEdit->Text.c_str() )
    );

    std::string oldPassHashed =  OldPassEdit->Text.c_str();

    if ( found != m_users.end() )
    {
        User_ &user = *found;
        if ( user.password == oldPassHashed )
        {
            if ( NewPassEdit->Text == NewPassConfirmEdit->Text )
            {
                if( onPasswordChanged )
                    onPasswordChanged( user.name,  NewPassEdit->Text.c_str()  );
            }
            else
            {
                error( NewPassEdit, "" );
                error( NewPassConfirmEdit, "Введенные пароли не совпадают. Повторите ввод нового пароля и его подтверждения." );
            }
        }
        else
        {
            error( OldPassEdit, "Неверный пароль" );
        }
    }
    else
    {
        error( NameEdit, "Пользователя с таким именем несуществует." );
    }    
}
//---------------------------------------------------------------------------
void TChangePassFrame::changePassword( const std::string &name, const std::string &pass )
{
    Users_::iterator found = std::find_if(
        m_users.begin(),
        m_users.end(),
        EqualByName( name )
    );

    if ( found != m_users.end() )
    {
        (*found).password = pass;
    }
}
//---------------------------------------------------------------------------
void TChangePassFrame::changedGood()
{
    ErrorLabel->Font->Color = GOOD_COLOR;
    ErrorLabel->Caption = "Пароль изменен.";
}
//---------------------------------------------------------------------------
void TChangePassFrame::changedError( const std::string &msg )
{
    error( NULL, msg );
}
//---------------------------------------------------------------------------
void TChangePassFrame::createDoneGood( const std::string &msg )
{
    CreateErrorLabel->Font->Color = GOOD_COLOR;
    CreateErrorLabel->Caption = msg.c_str(); 
}
//---------------------------------------------------------------------------
void TChangePassFrame::deleteDoneGood( const std::string &msg )
{
    createDoneGood( msg );
}
//---------------------------------------------------------------------------
void TChangePassFrame::createError( const std::string &msg )
{
    createError( NULL, msg );
}
//---------------------------------------------------------------------------
void TChangePassFrame::deleteError( const std::string &msg )
{
    createError( msg );
}
//---------------------------------------------------------------------------
void __fastcall TChangePassFrame::CreateButtonClick(TObject *)
{
    if ( CreateNameEdit->Text.IsEmpty() )
    {
        createError( CreateNameEdit, "Введите имя пользователя." );
        return;
    }

    if ( CreatePassEdit->Text != CreatePassConfirmEdit->Text )
    {
        createError( CreatePassEdit, "" );
        createError( CreatePassConfirmEdit, "Введенные пароли не совпадают. Повторите ввод нового пароля и его подтверждения." );
        return;
    }

    if ( onCreateUser )
    {
        onCreateUser(
            CreateNameEdit->Text.c_str(),
             CreatePassEdit->Text.c_str(),
             ComboBox1->ItemIndex==-1 ? -1 : ComboBox1->ItemIndex + 1  

        );
    }

}
//---------------------------------------------------------------------------
void __fastcall TChangePassFrame::DelButtonClick(TObject *)
{
    if ( CreateNameEdit->Text.IsEmpty() )
    {
        createError( CreateNameEdit, "Введите имя пользователя." );
        return;
    }

    AnsiString userName;
    if ( InputQuery(
            "Подтверждение удаления",
            "Подтвердите имя пользователя.",
            userName ) )
    {
        if ( userName == CreateNameEdit->Text )
        {
            if ( onDeleteUser )
                onDeleteUser( userName.c_str() );
        }
        else
        {
            createError( NULL, "Подтвержденное имя не совпадает с исходным именем." );    
        }
    }
}
//---------------------------------------------------------------------------

