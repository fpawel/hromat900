#ifndef ADMIN_SUBSYSTEM_INCLUDED
#define ADMIN_SUBSYSTEM_INCLUDED

#include <string>
#include "SoftID.hpp"

class TChangePassFrame;

namespace Forms {
    class TFrame;
};

class AdminSubSystem
{
public:
    AdminSubSystem(Users& users);
    ~AdminSubSystem();

    Forms::TFrame* frame();
    void ClearEdits();

    Users& GetUsers() { return m_users; }

private:
    void onPasswordChanged( const std::string &name, const std::string &pass );
    void onCreateUser( const std::string &name, const std::string &pass, int access );
    void onDeleteUser( const std::string &name );

private:
    TChangePassFrame *m_changePassFrame;
    Users& m_users;
};

#endif