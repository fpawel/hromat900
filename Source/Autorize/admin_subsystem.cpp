#include "admin_subsystem.h"
#include "change_pass_frame.h"
#include <functors.h>
#include "AppEvts.h"


AdminSubSystem::AdminSubSystem(Users& users)
:   m_changePassFrame( NULL ), m_users(users)
{
    m_changePassFrame = new TChangePassFrame( (TComponent*)NULL );
    m_changePassFrame->onPasswordChanged = onPasswordChanged;
    m_changePassFrame->onCreateUser = onCreateUser;
    m_changePassFrame->onDeleteUser = onDeleteUser;
    
    for ( Users::iterator it = m_users.begin();
        it != m_users.end();
        ++it )
    {
        User &user = *it;
        m_changePassFrame->addUser(
            user.name,
            user.password,
            user.accessLevel,
            user.IsAdmin() ? TChangePassFrame::IconType_Admin : TChangePassFrame::IconType_User
        );
    }
}

AdminSubSystem::~AdminSubSystem()
{   
    if ( m_changePassFrame )
    {
		m_changePassFrame->Parent = NULL;
        delete m_changePassFrame;
        m_changePassFrame = NULL;
    }
}

void AdminSubSystem::ClearEdits()
{
    m_changePassFrame->NameEdit->Text = "";
    m_changePassFrame->OldPassEdit->Text = "";
    m_changePassFrame->NewPassEdit->Text = "";
    m_changePassFrame->NewPassConfirmEdit->Text = "";
    m_changePassFrame->CreateNameEdit->Text = "";
    m_changePassFrame->CreatePassEdit->Text = "";
    m_changePassFrame->CreatePassConfirmEdit->Text = "";
    m_changePassFrame->CreatePassEdit->Text = "";
    m_changePassFrame->ComboBox1->ItemIndex = 0;

}

void AdminSubSystem::onPasswordChanged( const std::string &name, const std::string &pass )
{
    Users::iterator found = std::find_if(
        m_users.begin(),
        m_users.end(),
        EqualByName( name )
    );

    if ( found != m_users.end() )
    {
        User &user = *found;
        user.password = pass;
        m_changePassFrame->changePassword( user.name, user.password );
    }

    m_changePassFrame->changedGood();
    LogMyAppEvt( "Изменён пароль авторизации пользователя", found->name.c_str() );
}

Forms::TFrame* AdminSubSystem::frame()
{
    return m_changePassFrame;
}

void AdminSubSystem::onCreateUser( const std::string &name, const std::string &pass, int access )
{
    Users::iterator found = std::find_if(
        m_users.begin(),
        m_users.end(),
        EqualByName( name )
    );

    if ( found != m_users.end() )
    {
        m_changePassFrame->createError( "Пользователь с таким именем уже существует" );
        return;
    }

    User newUser;
    newUser.name = name;
    newUser.password = pass;
    newUser.accessLevel = access;

    m_users.push_back( newUser );

    m_changePassFrame->addUser(
            newUser.name,
            newUser.password,
            newUser.accessLevel,
            TChangePassFrame::IconType_User
        );

    std::string msg = std::string( "Пользователь '" ) +
            newUser.name +
            std::string( "' создан." );
    m_changePassFrame->createDoneGood( msg );
    LogMyAppEvt( "Создан новый пользователь", newUser.name.c_str() );
}

void AdminSubSystem::onDeleteUser( const std::string &name )
{
    Users::iterator found = std::find_if(
        m_users.begin(),
        m_users.end(),
        EqualByName( name )
    );

    if ( found == m_users.end() )
    {
        std::string msg = "Пользователя с именем '" +
            name + "' не существует.";
        m_changePassFrame->deleteError( msg );
        return;
    }

    if ( (*found).IsAdmin() )
    {
        m_changePassFrame->deleteError( "Удаление Администратора системы запрещено." );
        return;
    }

    m_users.erase( found );
    m_changePassFrame->removeUser( name );

    std::string msg = std::string( "Пользователь '" ) + name + "' удален."; 
    m_changePassFrame->deleteDoneGood( msg );
    LogMyAppEvt( "Удалён пользователь", name.c_str() );
}
