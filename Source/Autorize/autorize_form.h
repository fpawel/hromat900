//---------------------------------------------------------------------------

#ifndef autorize_formH
#define autorize_formH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <ImgList.hpp>

#include <string>
#include "express_bar.h"
//---------------------------------------------------------------------------
/*!
 * ���� ����������� ������������.
 * ���������� ������ ���� ��������� �������������
 */
class TAutorizeForm : public TForm
{
__published:	// IDE-managed Components
    TPanel *UsersBarPanel;
    TPanel *Panel2;
    TLabel *Label1;
    TLabel *Label2;
    TEdit *NameEdit;
    TEdit *PasswordEdit;
    TLabel *ErrorLabel;
    TButton *OkButton;
    TButton *CancelButton;
    TLabel *Label3;
    TImageList *ImageList;
    TLabel *labelActionLabel;
    TLabel *labelActionContent;
    TBevel *Bevel1;
    void __fastcall NameEditChange(TObject *Sender);
    void __fastcall OkButtonClick(TObject *Sender);
    void __fastcall CancelButtonClick(TObject *Sender);
  void __fastcall FormShow(TObject *Sender);
private:	// User declarations
public:		// User declarations
    __fastcall TAutorizeForm(TComponent* Owner);
    __fastcall ~TAutorizeForm();

    enum IconType
    {
        IconType_User = 0,
        IconType_Admin,

        IconType_FORCEDWORD = 0x7fffffff
    };

    //! ��������� ������������ � ���� �����������
    void addUser( const std::string &name, IconType iconType );

    //! ���������� ������������ ��� ����������
    void setCurrent( const std::string &name );
    std::string current();

    //! ����������� ����. ���������� false ����� ����������� ����� �� �����
    bool show();

    //! �������� ����
    void close();

    //! ����� ������ �����������
    void error( const char* msg );

    //! ������� ����������� (signal)
    bool (__closure *mayIComing)( const std::string &name, const std::string &password );

private:
  std::string m_current;
	ExpressBar *m_usersBar;
    ExpressBarItem *m_selectedItem;

    void onUserSelected( ExpressBarItem *item );
    void clearError();
};
//---------------------------------------------------------------------------
extern PACKAGE TAutorizeForm *AutorizeForm;
//---------------------------------------------------------------------------
#endif
