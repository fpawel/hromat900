//---------------------------------------------------------------------------
#ifndef autorize_systemH
#define autorize_systemH
//---------------------------------------------------------------------------
#include <system.hpp>

// boost
#include "boost\noncopyable.hpp"
#include "boost\shared_ptr.hpp"

#include "..\DevSys\SoftID.hpp"


class TadminForm;
class AdminSubSystem;

namespace Forms
{
	class TFrame;
};









class AutorizeSystemImpl : public boost::noncopyable
{
public:
	explicit AutorizeSystemImpl(Users& users);

    // вызвать диалог авторизации
    void ExecuteAutorization(const char *actionCaption);

    // вызвать диалог управления правами пользователей
    void ExecuteUserControlDialog();

    bool WasAutorized() const { return wasAutorized_; }
    User GetUser() const { return user_; }

private:
	Users& users_;
    User user_;
    bool wasAutorized_;
    AnsiString userName_;

    TadminForm *adminForm_;
    AdminSubSystem *admin_;
    Forms::TFrame *adminFrame_;
};



#endif
