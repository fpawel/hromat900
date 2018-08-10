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
// ��� ����� ������� ��������
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
// 			��������� �������� ������
////////////////////////////////////////////////////////////////////////////////
class TransferManager;
class TiXmlElement;
class TaskList;
class Hromat900T : public boost::noncopyable
{
//---------------------------------------------------------------------------
public:

    // �������� �������� �������� ������900
	AnsiString GetCommandDescription(uInt8 cmd) const;

    // ��������� �����
    AnsiString Number();


    
    /**
    *       ���������� ������
    */
    void SetCicle(unsigned);

    /**
    *       ������ � ������� �������� �����
    */
    void WriteBasePicTable( bool, const HromatBasePicTable& );
    void ReadBasePicTable( bool );

    /**
    *   �������� ������ ������������
    */
    void InitializeDeviceArchive(  );
    void ReadDeviceArchiveFile( const HtgItemsArray& archive );
    unsigned Get_ReadDeviceArchiveFile_TaskCount();


	void SetupSysConfigDialog();
    void SetupApplicationDialog();

    /**
    *       �������� �������� ����������
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

    // ������������ � ������� ����������� ������������� ����� ��������� � ���������
    unsigned DurHtg1() const { return durHtg1_; }

    // ������������ ������������� � �������
    unsigned HtgDuration() const;

    bool GraduateIsEnabled() const;

private:

    PVoid logLifeTime_;
    TiXmlElement *xml_;

    // �������� �������� ������900
    typedef std::map<uInt8, AnsiString> CmdToDescrMap;
    const CmdToDescrMap cmdToDescr_;

    // �������� ����������
	boost::shared_ptr<TransferManager> trnsfrMan_;

    // ��������� ������������
    VDouble hromatSysConfig_;

    // ����, ����������� ���������� ��������� ���������� - ��������� ������������
    bool enebleReadDetectorIndication_;

    bool isWarningSoundEnabled_;

    // ���� � ������� ��������������
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


