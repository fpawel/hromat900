//------------------------------------------------------------------------------
#ifndef MY_HROMAT_SOFT_ID_DEFINITION_HEADER_INCLUDED____
#define MY_HROMAT_SOFT_ID_DEFINITION_HEADER_INCLUDED____
//------------------------------------------------------------------------------
#include <system.hpp>
//------------------------------------------------------------------------------
#include <vector>
#include <string>
//------------------------------------------------------------------------------
struct User
{

    enum  AccessLevel {
        accessAdmin,
        accessLabour,
        accessOperator
    } ;

    static AnsiString AccessToStr(AccessLevel accessLevel) {
        switch (accessLevel) {
        case accessAdmin: return "Администратор";

        case accessLabour: return "Лаборант";

        default: return "Оператор" ;
        }
    }

    static AccessLevel StrToAccess( AnsiString s) {
        if(s=="Администратор")
            return accessAdmin;
        if(s=="Лаборант")
            return accessLabour;
        return accessOperator;
    }
	std::string name;
	std::string password;
    AccessLevel accessLevel;
	bool visible;

    bool IsAdmin() const { return accessLevel==accessAdmin;}

    AnsiString Access() const
    {
        return AccessToStr(accessLevel);

    }

    User()
    :   accessLevel( accessAdmin ),
        visible( true )
    {}
};

void ShowLabourMsg();

void ShowAdminMsg()  ;

//------------------------------------------------------------------------------
typedef std::vector<User> Users;
//------------------------------------------------------------------------------
typedef struct HromatSoftIDTag
{
	typedef struct ItemTag
    {
    	unsigned version, crc16;
    } Item;
	Item hromat900;
    Item hrmtlchk;
    Item h9001;
    Item ucint;
    Item calculator;
} HromatSoftID;
//------------------------------------------------------------------------------
HromatSoftID NullHromatSoftID()
{
	HromatSoftID ret = {0};
    return ret;
}
//------------------------------------------------------------------------------
bool operator==( const HromatSoftID::Item& lhs, const HromatSoftID::Item& rhs)
{
	return lhs.version == rhs.version && lhs.crc16 == rhs.crc16;
}
//------------------------------------------------------------------------------
bool operator==( const HromatSoftID& lhs, const HromatSoftID& rhs)
{
	return
    	lhs.hromat900 == rhs.hromat900 &&
        lhs.hrmtlchk == rhs.hrmtlchk &&
        lhs.h9001 == rhs.h9001 &&
        lhs.ucint == rhs.ucint &&
        lhs.calculator == rhs.calculator;
}
//------------------------------------------------------------------------------
bool operator!=( const HromatSoftID& lhs, const HromatSoftID& rhs)
{
	return !(lhs==rhs);
}
//------------------------------------------------------------------------------
bool StringToHromatSoftIDItem(const AnsiString& ss, HromatSoftID::Item& idItem);
bool StringToHromatSoftID(const AnsiString& ss, HromatSoftID& destId);
//------------------------------------------------------------------------------
AnsiString HromatSoftIDToString(const HromatSoftID& id);
AnsiString HromatSoftIDItemToString(const HromatSoftID::Item& id);
//------------------------------------------------------------------------------



#endif