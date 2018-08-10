//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#pragma hdrstop
//------------------------------------------------------------------------------
#include "autorize_system.h"
#include "autorize_form.h"
#include <tinyxml.h>
//#include <get_md5_hash.h>
#include "AdminFormUnit.h"
#include "admin_subsystem.h"

// my
#include "MyFileCtrl.h"
#include "MyAppProfile.h"
#include "MyProcEntry.h"
#include "guicon.h"

//------------------------------------------------------------------------------
#pragma package(smart_init)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
enum{ WRONG_COMING_TRYS = 3 };
//------------------------------------------------------------------------------

void ShowLabourMsg() {
    ShowMessage("ƒл€ выполнени€ этого действи€ требуютс€ права доступа не ниже лаборанта.");
}

void ShowAdminMsg() {
    ShowMessage("“олько администратор имеет право совершать это действие!");
}

class AutorizeSystemHelper : public boost::noncopyable
{
public:
	AutorizeSystemHelper(Users& users);
	~AutorizeSystemHelper();
    //! запустить диалог авторизации
	void Execute();
    //! прошла ли авторизаци€
    bool IsAutorized() const;

    User GetUser() const { return user_; }

    /// установить заголовок текущего действи€
    void SetActionCaption( const std::string& caption );

private:
	bool mayIComing( const std::string &name, const std::string &password );
	void autorize( User &user );
	void unautorize( User &user );

private:
	Users& m_users;
	std::string m_workPath;
	boost::shared_ptr<TAutorizeForm> form_;

	User user_;
	bool wasAutorized_;
    int m_wrongComingCounter;
};
//------------------------------------------------------------------------------
AutorizeSystemHelper::AutorizeSystemHelper( Users& users ) :
	m_users(users),
    wasAutorized_( false ),
    m_wrongComingCounter( 0 ),
    form_( new TAutorizeForm( NULL ) )
{   
    std::string currentName;
    currentName = "";

	for ( Users::iterator it = m_users.begin();
		  it != m_users.end();
		  ++it )
	{
		User &currentUser = *it;
		if ( currentUser.visible )
		{
            form_->addUser(
                currentUser.name,
                currentUser.IsAdmin() ? TAutorizeForm::IconType_Admin : TAutorizeForm::IconType_User
            );
		}
	}
    form_->setCurrent(currentName);
	form_->mayIComing = mayIComing;

    
}
//------------------------------------------------------------------------------
AutorizeSystemHelper::~AutorizeSystemHelper()
{
  	std::string currentName = form_->current();
  	//saveCurrent( m_usersFilename, currentName );

    if ( wasAutorized_ )
	{
		unautorize( user_ );
	}
}
//------------------------------------------------------------------------------
bool AutorizeSystemHelper::mayIComing( const std::string &name, const std::string &password )
{
	bool autorized = false;
    std::string hashedPassword =  password;
	for ( Users::iterator it = m_users.begin();
		  it != m_users.end();
		  ++it )
	{
		User &currentUser = *it;
		if ( currentUser.name == name && currentUser.password == hashedPassword )
		{
            form_->close();
			autorize( currentUser );
			autorized = true;
            break;
		}
	}
	if ( !autorized )
	{
		form_->error( "Ќеверное им€ пользовател€ или пароль." );
        ++m_wrongComingCounter;
	}

    if ( m_wrongComingCounter == WRONG_COMING_TRYS )
    {
        autorized = true;
    }
    return autorized;
}
//------------------------------------------------------------------------------
void AutorizeSystemHelper::autorize( User &user )
{
	user_ = user;
	wasAutorized_ = true;
}
//------------------------------------------------------------------------------
void AutorizeSystemHelper::unautorize( User & /*user*/ )
{
    wasAutorized_ = false;
}
//------------------------------------------------------------------------------
void AutorizeSystemHelper::Execute()
{
    form_->show();
}
//------------------------------------------------------------------------------
bool AutorizeSystemHelper::IsAutorized() const
{
    return wasAutorized_;
}
//------------------------------------------------------------------------------
void AutorizeSystemHelper::SetActionCaption( const std::string& caption )
{
    form_->labelActionContent->Caption = caption.c_str();
}
//------------------------------------------------------------------------------


AutorizeSystemImpl::AutorizeSystemImpl( Users& users ) :
	users_(users),
        wasAutorized_(0),  
        adminForm_( new TadminForm(NULL) ),
        admin_( new AdminSubSystem(users) ),
        adminFrame_( admin_->frame() )
{   
    adminFrame_->Align = alClient;
    
}
//------------------------------------------------------------------------------

void AutorizeSystemImpl::ExecuteAutorization(const char *actionCaption)
{
    if( MyParamStrHas( "-nologin" ) )
    {
    	wasAutorized_ = true;
    	user_.name = "јдминистратор";
        user_.accessLevel = User::accessAdmin;
        return;
    }

    // запуск системы регистрации пользовател€
    AutorizeSystemHelper login(users_);
    login.SetActionCaption( actionCaption );
    login.Execute();

    wasAutorized_ = login.IsAutorized();
    if(wasAutorized_) {
        user_ = login.GetUser();
    }

    MYWCOUT_( "ƒоступ - %s\n", user_.Access()  ));
}
//---------------------------------------------------------------------------
// вызвать диалог управлени€ правами пользователей
void AutorizeSystemImpl::ExecuteUserControlDialog( )
{
    admin_->ClearEdits();
    adminFrame_->Parent = adminForm_;
    adminForm_->ShowModal();
    adminFrame_->Parent = NULL;
    users_ = admin_->GetUsers();
}
//---------------------------------------------------------------------------


