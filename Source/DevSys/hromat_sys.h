//------------------------------------------------------------------------------
#ifndef hromat_sysH
#define hromat_sysH
//------------------------------------------------------------------------------
#include "boost\noncopyable.hpp"
#include "boost\shared_ptr.hpp"
//------------------------------------------------------------------------------
#include <vector>
#include <map>
//------------------------------------------------------------------------------
#include <SysUtils.hpp>
//------------------------------------------------------------------------------
#include "..\Application\MyIostream.h"
#include "..\my_include\MyNoDestroySingleton.hpp"
#include "Hromat900.hpp"

#include "MyAbstructTransfer.hpp"
#include "..\my_include\free_types_.h"
#include "..\VCLUtils\VCLSignal.h"
//------------------------------------------------------------------------------
// тип формы диалога настроек
class TFormCtrlPrgrmmDlg;
class CurrHtgHelperT;
namespace System
{
    class TObject;
};
namespace Forms
{
    class TForm;
};



////////////////////////////////////////////////////////////////////////////////
// 			Интерфейс даннного модуля
////////////////////////////////////////////////////////////////////////////////
class TransferManager;
class TiXmlElement;
class TaskList;
class Hromat900T : public boost::noncopyable
{
//---------------------------------------------------------------------------
public:

    // получить описание комманды Хромат900
	AnsiString GetCommandDescription(uInt8 cmd) const;

    // заводской номер
    AnsiString Number();


    
    /**
    *       Управление циклом
    */
    void SetCicle(unsigned);

    /**
    *       Работа с опорной таблицей пиков
    */
    void WriteBasePicTable( bool, const HromatBasePicTable& );
    void ReadBasePicTable( bool );

    /**
    *   Просмотр архива хроматографа
    */
    void InitializeDeviceArchive(  );
    void ReadDeviceArchiveFile( const HtgItemsArray& archive );
    unsigned Get_ReadDeviceArchiveFile_TaskCount();


	void SetupSysConfigDialog();
    void SetupApplicationDialog();

    /**
    *       Получить менеджер трансферов
    */
    TransferManager& GetTransferManager();

    class StopThreadInThisFoo : public boost::noncopyable
    {
    public:
    	explicit StopThreadInThisFoo();
        ~StopThreadInThisFoo();
    private:
    	const bool isStarted_;
    };

    explicit Hromat900T();
    ~Hromat900T();

    void RequestMeasurementArchives();

    const double* GetSysConfig() const { return &hromatSysConfig_.at(0); }

    void CancelReadDeviceArchive();
    unsigned GetUserDiskretTimeMSec() const { return userDiskretTimeMSec_; }

    AnsiString GetHtgArchivePath() const { return htgArchivePath_; }

    bool IsRoundingGOST() const;

    double FlowRateCfg(bool dtctr);
    double ColumnTemperatureCfg();

    double DetectorsCount();

    double GetKoefficient(unsigned n) const;

    bool IsWarningSoundEnabled() const { return isWarningSoundEnabled_; }

    AnsiString MestoOtboraProby() const { return mestoOtboraProby_; }
    AnsiString HromatType() const { return hromatType_; }

    // длительность в минутах отображения хроматограммы перед переходом к следующей
    unsigned DurHtg1() const { return durHtg1_; }

    // длительность хроматограммы в минутах
    unsigned HtgDuration() const;

    bool GraduateIsEnabled() const;

private:

    PVoid logLifeTime_;
    TiXmlElement *xml_;

    // описание комманды Хромат900
    typedef std::map<uInt8, AnsiString> CmdToDescrMap;
    const CmdToDescrMap cmdToDescr_;

    // менеджер трансферов
	boost::shared_ptr<TransferManager> trnsfrMan_;

    // параметры хроматографа
    VDouble hromatSysConfig_;

    // флаг, разрешающий считывание показаний детекторов - настройка пользователя
    bool enebleReadDetectorIndication_;

    bool isWarningSoundEnabled_;

    // инфа о текущих хроматограммах
    boost::shared_ptr<CurrHtgHelperT> currHtgHelper_;
    
    unsigned currentHromatCicleType_;
    unsigned userDiskretTimeMSec_;

    unsigned durHtg1_;
    int detectorsCount_;

    AnsiString htgArchivePath_;

    AnsiString mestoOtboraProby_, hromatType_;

    void HandleChnslReaded_(const double*, const double*);
    void MakeNewHTGFileProccesing( bool dtctr );

    void HandleReadedHromatSysConfig(const VDouble&);
    void __fastcall ClickWriteHromatSysConfig( TObject* );
    void __fastcall ClickReadHromatSysConfig( TObject* );

    void HandleDeviceStateReaded( unsigned deviceStatecode, unsigned cicleType);

    void InitializeTaskList();
    void CheckAndCorrectTaskList(TaskList& taskList);
};

struct Hromat900TagT {};
typedef MyNoDestroySingletonHolder<Hromat900T, Hromat900TagT> Hromat900;


//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------


