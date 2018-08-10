//---------------------------------------------------------------------------


#ifndef change_pass_frameH
#define change_pass_frameH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>

#include <express_bar.h>
#include <ImgList.hpp>
#include <string>
#include <vector> 
//---------------------------------------------------------------------------
class TChangePassFrame : public TFrame
{
__published:	// IDE-managed Components
    TPanel *UsersPanel;
    TLabel *Label1;
    TLabel *Label2;
    TLabel *Label3;
    TLabel *Label4;
    TEdit *NameEdit;
    TEdit *OldPassEdit;
    TEdit *NewPassEdit;
    TEdit *NewPassConfirmEdit;
    TButton *ChangeButton;
    TLabel *ErrorLabel;
    TImageList *ImageList;
    TLabel *Label5;
    TLabel *Label6;
    TLabel *Label7;
    TEdit *CreateNameEdit;
    TLabel *Label8;
    TEdit *CreatePassEdit;
    TEdit *CreatePassConfirmEdit;
    TLabel *Label9;
    TButton *CreateButton;
    TLabel *CreateErrorLabel;
    TButton *DelButton;
    TBevel *Bevel1;
    TComboBox *ComboBox1;
    TLabel *Label10;
    void __fastcall NameEditChange(TObject *Sender);
    void __fastcall ChangeButtonClick(TObject *Sender);
    void __fastcall CreateButtonClick(TObject *Sender);
    void __fastcall DelButtonClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
    __fastcall TChangePassFrame(TComponent* Owner);

    enum IconType
    {
        IconType_User = 0,
        IconType_Admin,

        IconType_FORCEDWORD = 0x7fffffff
    };

    void addUser(
        const std::string &name,
        const std::string &pass,
        int accsess,
        IconType iconType
    );
    
    void changePassword( const std::string &name, const std::string &pass );

    void changedGood();
    void changedError( const std::string &msg );

    void createDoneGood( const std::string &msg );
    void deleteDoneGood( const std::string &msg );

    void createError( const std::string &msg );
    void deleteError( const std::string &msg );  
    
    void removeUser( const std::string &name );

    void (__closure *onPasswordChanged)( const std::string &name, const std::string &pass );
    void (__closure *onCreateUser)( const std::string &name, const std::string &pass, int );
    void (__closure *onDeleteUser)( const std::string &name );

private:
    struct User_
    {
        std::string name;
        std::string password;
        int accsess;
        ExpressBarItem *barItem;
    };
    typedef std::vector<User_> Users_;


    void onUserSelected( ExpressBarItem *item );

    void error( TEdit *errorField, const std::string &msg );
    void createError(  TEdit *errorField, const std::string &msg );
    void clearError();

private:
    Users_ m_users;

    ExpressBar *m_usersBar;
    ExpressBarItem *m_selectedItem;

};
//---------------------------------------------------------------------------
extern PACKAGE TChangePassFrame *ChangePassFrame;
//---------------------------------------------------------------------------
#endif
