#ifndef MY_PERFORM_MODBUS_TRANSFER_HEADER_INCLUDED__________
#define MY_PERFORM_MODBUS_TRANSFER_HEADER_INCLUDED__________

#include "..\my_include\free_types_.h"
#include "boost\noncopyable.hpp"
/**
*		������ ������� ��������� �������� �������������.
*		������������ � ����� "hromat_sys.h".
*		�������� 100% ������������ ��������.
*		����� ���������� ����������
*/
void PerformModbusTransfer
(
	uInt8 commandCode,			// ��� ��������
	ByteBuffPtr sendBegin,		// ������ �� ������������ ������
    ByteBuffPtr sendEnd,
    ByteBuffPtr& reciveBegin,
    ByteBuffPtr& reciveEnd
);

/**
*		������������ ������ ����� ������������� ������
*		������������ � ����� "hromat_sys.h".
*/
unsigned GetHromatMaxRxDFrameSize();

/**
*       ���������� ����������� FTP
*		������������ � ����� "hromat_sys.h".
*/
class FTPHlpr;
FTPHlpr* GetFTP();
bool IsTCPConnection();
void ShowTopInfoPanel(const AnsiString&);
void HideTopInfoPanel();

bool FTPEnable();


class TopmostInfoPanelHelper : public boost::noncopyable
{
public:
    TopmostInfoPanelHelper(const AnsiString& s )
    {
        ShowTopInfoPanel( s );
    }
    ~TopmostInfoPanelHelper()
    {
        HideTopInfoPanel();
    }
};



#endif
