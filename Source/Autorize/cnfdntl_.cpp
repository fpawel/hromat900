//---------------------------------------------------------------------------
#include <fstream>

#pragma hdrstop

#include "cnfdntl_.h"

//------------------------------------------------------------------------------

//vcl
#include <Dialogs.hpp>
#include <forms.hpp>

// loki
#include "..\my_include\MyNoDestroySingleton.hpp"

#include <tinyxml.h>
#include <functors.h>
//#include "MyFileCtrl.h"

#include "guicon.h"
#include "AnsiStringUtils_.h"
//#include "MyAppProfile.h"
#include "MyAppWinPath.hpp"
#include "xmlUtils_.h"

#include "AppEvts.h"
#include "MySynchronize.h"
#include "HromatSoftID.h"
#include "autorize_system.h"
#include "MyFileCtrl.h"

#pragma package(smart_init)

const char* const USERS = "users";

const char* const NAME = "name";
const char* const PASSWORD = "password";
const char* const VISIBLE = "visible";
//const char* const CURRENT = "current";

const char* const TRUE_STR = "true";
const char* const FALSE_STR = "false";


#define ADMIN_USER_NAME "�������������"
#define ADMIN_PASSWORD "masterkey"
//------------------------------------------------------------------------------
/// ���� � ��������������
const AnsiString USERS_FILE_NAME = ".users";
//------------------------------------------------------------------------------
/* ������

<confidential>
	<users>
    	<admin>
        	<name>root</name>
        	<password>��� ������� ������</password>
        	<visible>true</visible>
    	</admin>
		<user>
			<name>���� ������</name>
			<password>������ �������</password>
			<visible>true</visible>
		</user>
	</users>
</confidential>
*/

struct InputHex
    {
        unsigned short Do( const AnsiString &msg, unsigned short init ) const
        {
            const AnsiString ttl = MYSPRINTF_("������� %s.\n", msg);
            go_there:

            AnsiString s;
            s = IntToHex( init, 4 );
            if( !InputQuery("������� ����������������� �����", ttl+"\n����������������� ����� �� 0001 �� FFFF.\n", s) )
                return 0;
            const MyInt getVal = MyStrToInt( s, 16 );
            const long v = getVal.first;
            if( !getVal.second || v<1 || v>0xFFFF )
            {
                ShowMessage( "������������ ����: \""+s+"\"!");
                goto go_there;
            }
            return v;
        }
    };
//------------------------------------------------------------------------------
void SaveUser( TiXmlElement *root, const User &user )
{
    TiXmlElement *nameElement = new TiXmlElement( NAME );
    TiXmlText *nameContent = new TiXmlText( user.name.c_str() );
    nameElement->LinkEndChild( nameContent );
    root->LinkEndChild( nameElement );

    TiXmlElement *passwordElement = new TiXmlElement( PASSWORD );
    TiXmlText *passwordContent = new TiXmlText( user.password.c_str() );
    passwordElement->LinkEndChild( passwordContent );
    root->LinkEndChild( passwordElement );

    TiXmlElement *visibleElement = new TiXmlElement( VISIBLE );
    TiXmlText *visibleContent = new TiXmlText( user.visible ? TRUE_STR : FALSE_STR );
    visibleElement->LinkEndChild( visibleContent );
    root->LinkEndChild( visibleElement );
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void SaveUsersToXMLDocument( TiXmlElement *usersElement, const Users &users )
{   
    usersElement->Clear();
    //int aa = users.size();
	for ( Users::const_iterator it = users.begin();
		  it != users.end();
		  ++it )
	{
        const User &currentUser = *it;
        TiXmlElement *userElement = 0;

        userElement = new TiXmlElement( currentUser.Access().c_str() );

        usersElement->LinkEndChild( userElement );
        SaveUser( userElement, currentUser );
	}
}
//------------------------------------------------------------------------------
bool loadUser( TiXmlElement *userElement, User &dstUser )
{
    dstUser.accessLevel = User::StrToAccess(userElement->Value());

	TiXmlElement *nameElement = userElement->FirstChildElement( NAME );
	if ( !nameElement || !nameElement->GetText() )
		return false;

	TiXmlElement *passwordElement = userElement->FirstChildElement( PASSWORD );
	if ( !passwordElement || !passwordElement->GetText() )
		return false;

	TiXmlElement *visibleElement = userElement->FirstChildElement( VISIBLE );
	if ( !visibleElement || !visibleElement->GetText() )
		return false;

	dstUser.name = nameElement->GetText();
	dstUser.password = passwordElement->GetText();
	std::string visibleStr = visibleElement->GetText();
	dstUser.visible = ( visibleStr == FALSE_STR ) ? false : true;

	return true;
}
//------------------------------------------------------------------------------
Users LoadUsersFromXMLDocument( TiXmlElement *usersElement )
{
    Users dstUsers(0);
    bool foundAdmin = false;



	    TiXmlElement *userElement = usersElement->FirstChildElement();
		while( userElement )
		{
            User user;
			if ( loadUser( userElement, user ) ) {
                dstUsers.push_back( user );
                if( user.IsAdmin() )
                    foundAdmin = true;
            }
            userElement = userElement->NextSiblingElement();
        }

    if ( !foundAdmin ) {
        User user;
        user.name = ADMIN_USER_NAME;
        user.password = ADMIN_PASSWORD;
        user.accessLevel = User::accessAdmin;
        dstUsers.push_back(user);
    }
    MyWCout( "\t---\t������ �������������� �������� �� ������������ �����.\t---\n");

    return dstUsers;
}
//------------------------------------------------------------------------------
class ConfidentialInfoT
{
public:
	explicit ConfidentialInfoT();

	void Save();
    const Users& GetUsers() const;


    // ������� ������ �������������� ��
	bool SoftIDSetupDialog();

    // �������� ������������� ��
    const HromatSoftID& SoftId() const;

    bool VerifySoftId();

    HromatSoftID LoadSoftIDFromXML();

    // ������� ������ ���������� ������� �������������
    void ExecuteUserControlDialog();

    bool WasAutorized();
	User AutorizedUser();

    // ������� ������ �����������
	void ExecuteAutorization(const char *actionCaption);

    unsigned short int GetHromatCRC16Key() const { return crc16key_; }
    void SetupHromatCRC16Key();
    void ChangeHromatCRC16Key();

    bool IsLabourAutorized()
    {
        return autorizeSystemImpl_.GetUser().accessLevel==User::accessAdmin ||
            autorizeSystemImpl_.GetUser().accessLevel==User::accessLabour;
    }

    bool IsAdminAutorized()
    {
	    return autorizeSystemImpl_.GetUser().accessLevel==User::accessAdmin;
    }
    

private:
    const unsigned short CRC16OfHromat900DotExe_;

    TiXmlDocument *doc_;

    TiXmlElement* topElem_, *usersElem_;
    
	// ������
	Users users_;

     // ������������� ��
    HromatSoftID softId_;

    // ��������� ������ �������� ������� ��� �������� CRC16
    unsigned short crc16key_;

    // ���������� ���������� �����������
    AutorizeSystemImpl autorizeSystemImpl_;



    
};
//------------------------------------------------------------------------------
struct ConfidentialInfoTagT {};
typedef MyNoDestroySingletonHolder<ConfidentialInfoT, ConfidentialInfoTagT> ConfidentialInfo;
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
ConfidentialInfoT::ConfidentialInfoT() :
    CRC16OfHromat900DotExe_( CalcFileCRC16( ParamStr(0) ) ),
    doc_( TiXML::DecryptAndLoadDocument( USERS_FILE_NAME) ),
    topElem_( TiXML::GiveNotNulElement( doc_, "confidential") ),
    usersElem_( TiXML::GiveNotNulElement( topElem_, "users") ),
    users_( LoadUsersFromXMLDocument( usersElem_) ),
    softId_( LoadSoftIDFromXML() ),
    crc16key_( TiXML::GetXMLInteger( topElem_, "CRC16KEY", 1, 0xFFFF, 0xFFFF) ),
    autorizeSystemImpl_(users_)
{
}
//------------------------------------------------------------------------------
void ConfidentialInfoT::SetupHromatCRC16Key()
{
    const InputHex inputHex;
    const unsigned short oldCRC12Key = crc16key_;
    this->ChangeHromatCRC16Key();
    const unsigned short v1 = crc16key_;

    if( ::MessageBox( Application->Handle, "���������� ���� �������� ����������� � �����������?",
    		"���� �������� �����������",
            MB_YESNO | MB_ICONWARNING | MB_SYSTEMMODAL )==IDNO )
    return;
    unsigned short v2 = inputHex.Do("������������ ����", oldCRC12Key);
    if( v2==0 ) return;

    AddToTaskList_SetHromatCRC16Key(v1, v2);
}

void ConfidentialInfoT::ChangeHromatCRC16Key()
{
    const InputHex inputHex;
    const unsigned short oldCRC12Key = crc16key_;
    unsigned short v1 = inputHex.Do("���� ��� �������� �����������", oldCRC12Key);
    if( v1==0 ) return;
    if( crc16key_!=v1 )
    {
        crc16key_ = v1;
        LogMyAppEvt("������ ���� �������� �����������.");
    }
}
//------------------------------------------------------------------------------
HromatSoftID ConfidentialInfoT::LoadSoftIDFromXML()
{
	HromatSoftID ret = {0};
	while
    (
    	!StringToHromatSoftID( TiXML::GetAtr(topElem_, "SOFT_ID"), ret) &&
        !HromatSoftIDSetupDialog(ret)
    );
    ret.hromat900.crc16 = CRC16OfHromat900DotExe_;
    ret.hromat900.version = 1;

    ret.hrmtlchk.crc16 = 0x706f;

    return ret;
}
//-----------------------------------------------------------------------------
// �������� ������������� ��
const HromatSoftID& ConfidentialInfoT::SoftId() const
{
    return  softId_;
}
//------------------------------------------------------------------------------
void ConfidentialInfoT::Save()
{
	TiXML::SetIntAtr(topElem_, "CRC16KEY", crc16key_ );
    TiXML::SetAtr(topElem_, "SOFT_ID", HromatSoftIDToString(softId_) );
    SaveUsersToXMLDocument( usersElem_, users_ );
        
    TiXML::EncryptSignAndSaveDocument( USERS_FILE_NAME, *doc_ );
    MyWCout( "\t---\tC����� �������������� ������� � ����������� ����.\t---\n");
}
//------------------------------------------------------------------------------
const Users& ConfidentialInfoT::GetUsers() const
{
	return users_;
}
//------------------------------------------------------------------------------
// ������� ������ �������������� ��
bool ConfidentialInfoT::SoftIDSetupDialog()
{
    const bool ret = HromatSoftIDSetupDialog( softId_ );
    softId_.hromat900.crc16 = CRC16OfHromat900DotExe_;
	return ret;
}
//------------------------------------------------------------------------------
bool ConfidentialInfoT::VerifySoftId()
{
	return ValidateHromatInlineSoftId( softId_, autorizeSystemImpl_.GetUser().accessLevel==User::accessAdmin );
}
//------------------------------------------------------------------------------
// ������� ������ ���������� ������� �������������
void ConfidentialInfoT::ExecuteUserControlDialog( )
{
	autorizeSystemImpl_.ExecuteUserControlDialog( );
}
//------------------------------------------------------------------------------
bool ConfidentialInfoT::WasAutorized()
{
	return autorizeSystemImpl_.WasAutorized();
}
//------------------------------------------------------------------------------
User ConfidentialInfoT::AutorizedUser()
{
	return autorizeSystemImpl_.GetUser();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// ������� ������ �����������
void ConfidentialInfoT::ExecuteAutorization(const char *actionCaption)
{
	autorizeSystemImpl_.ExecuteAutorization(actionCaption);
}
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
void SaveConfidential()
{
	ConfidentialInfo::Instance().Save();
}
//------------------------------------------------------------------------------
const Users& GetRegisteredUsers()
{
	return ConfidentialInfo::Instance().GetUsers();
}
//------------------------------------------------------------------------------
// �������� ������������� ��
const HromatSoftID& GetSoftId()
{
	return ConfidentialInfo::Instance().SoftId();
}
//------------------------------------------------------------------------------
// ������� ������ �������������� ��
bool SoftIDSetupDialog()
{
	return ConfidentialInfo::Instance().SoftIDSetupDialog();
}
//------------------------------------------------------------------------------
bool VerifySoftId()
{
	return ConfidentialInfo::Instance().VerifySoftId();
}
//------------------------------------------------------------------------------
// ������� ������ �����������
void ExecuteAutorization(const char *actionCaption)
{
	ConfidentialInfo::Instance().ExecuteAutorization(actionCaption);
}
//------------------------------------------------------------------------------
// ������� ������ ���������� ������� �������������
void ExecuteUserControlDialog()
{
	return ConfidentialInfo::Instance().ExecuteUserControlDialog();
}
//------------------------------------------------------------------------------
bool WasAutorized()
{
	return ConfidentialInfo::Instance().WasAutorized();
}
//------------------------------------------------------------------------------
User AutorizedUser()
{
	return ConfidentialInfo::Instance().AutorizedUser();
}
//------------------------------------------------------------------------------
bool IsAdminAutorized()
{
	return ConfidentialInfo::Instance().IsAdminAutorized();
}

bool IsLabourAutorized()
{
	return ConfidentialInfo::Instance().IsLabourAutorized();
}

//------------------------------------------------------------------------------
// �������� ��������� ������ �������� ������� ��� �������� CRC16
unsigned short int GetHromatCRC16Key()
{
    return ConfidentialInfo::Instance().GetHromatCRC16Key();
}
//------------------------------------------------------------------------------
void SetupHromatCRC16Key()
{
    ConfidentialInfo::Instance().SetupHromatCRC16Key();
}

void ChangeHromatCRC16Key()
{
    ConfidentialInfo::Instance().ChangeHromatCRC16Key();
}








