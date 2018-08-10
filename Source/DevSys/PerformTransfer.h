#ifndef MY_PERFORM_MODBUS_TRANSFER_HEADER_INCLUDED__________
#define MY_PERFORM_MODBUS_TRANSFER_HEADER_INCLUDED__________

#include "..\my_include\free_types_.h"
#include "boost\noncopyable.hpp"
/**
*		Данная функция выполняет трансфер приёмопередачи.
*		Определяется в файле "hromat_sys.h".
*		Занимает 100% процессорных ресурсов.
*		Может возбуждать исключения
*/
void PerformModbusTransfer
(
	uInt8 commandCode,			// код комманды
	ByteBuffPtr sendBegin,		// ссылка на передаваемые данные
    ByteBuffPtr sendEnd,
    ByteBuffPtr& reciveBegin,
    ByteBuffPtr& reciveEnd
);

/**
*		Максимальный размер куска запрашиваемых данных
*		Определяется в файле "hromat_sys.h".
*/
unsigned GetHromatMaxRxDFrameSize();

/**
*       Декларация функционала FTP
*		Определяется в файле "hromat_sys.h".
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
