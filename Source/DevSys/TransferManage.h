//---------------------------------------------------------------------------
#ifndef TransferManageH
#define TransferManageH

// boost
#include "boost\noncopyable.hpp"
#include "boost\shared_ptr.hpp"


// my
#include "MyAbstructTransfer.hpp"
#include "..\my_include\free_types_.h"
#include "..\VCLUtils\VCLSignal.h"
#include "TransmitInterfaceSets.h"
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// тип формы диалога настроек
class TFormCtrlPrgrmmDlg;
typedef boost::shared_ptr<TFormCtrlPrgrmmDlg> PFormCtrlPrgrmmDlg;
//---------------------------------------------------------------------------
typedef my::VCLSignal::Signal<TVoid_IntStrMthd> TReportStateSignal;
//---------------------------------------------------------------------------
// список трансферов
class TaskList;
typedef void (__closure *TOnNotifyTaskList)(TaskList& taskList);



//
class FTPHlpr;


class TransferManager : public boost::noncopyable
{
public:
    enum{ ADD_TASK_HEAD = false, ADD_TASK_TAIL = true };
    
	explicit TransferManager();
    
    // упрвление списком - добавить задание
	void AddTask( MyAbstructTransfer *trnsfr, bool addTail = ADD_TASK_TAIL );

	void AddSingleBackgroundTask( void (__closure *aMethod )(),
    	const AnsiString& what,  bool addTail = ADD_TASK_TAIL );

	// управление потоком
	void StartThread();
	void StopThread();
    void KillThread();
    bool WasStopedOrTerminated() const;

	boost::shared_ptr<void> ConnectOnReportState( TVoid_IntStrMthd method );

    void DebugEditList();
    uInt8 ModusAddr() const;

    /**
	*		Выполнить трансфер приёмопередачи.
	*/
	void PerformTransfer
	(
		uInt8 commandCode,			// код комманды
		ByteBuffPtr sendBegin,		// ссылка на передаваемые данные
    	ByteBuffPtr sendEnd,
    	ByteBuffPtr& reciveBegin,
    	ByteBuffPtr& reciveEnd
	);
    
    /**
    *       список трансферов
    */
    TaskList& GetTaskList();

    const TransmitInterfaceSets& GetSets() const;
    void SetSets( const TransmitInterfaceSets& );

    FTPHlpr* GetFTP();
    bool IsTCP() const;

    bool FTPEnable() const;

    AnsiString ActiveInterfaceDescription() const;

    void SetConsoleinputMode(bool);
    void ShowTopPanel(const AnsiString&, bool showProgress=false);
    void HideTopPanel();
    void SetTopPanelProgress( unsigned v, unsigned vMax=-1, unsigned vMin=-1 );

    void SetOnNotifyTaskList(TOnNotifyTaskList);

    
private:
	class Impl;
    boost::shared_ptr<Impl> impl_;
};
//------------------------------------------------------------------------------
#endif
