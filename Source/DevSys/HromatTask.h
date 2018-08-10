//---------------------------------------------------------------------------
#ifndef HromatTaskH
#define HromatTaskH
//---------------------------------------------------------------------------

//my
#include "MyAbstructTransfer.hpp"
#include "hromat900.hpp"
#include "MshrmtArc.hpp"
#include "Hromatogram.hpp"

#define VIRTUAL_FINAL_ //

#include <map>
#include <vector>

class TransferManager;
class TFrmWait;
typedef boost::shared_ptr<TFrmWait> PFrmWait;


//------------------------------------------------------------------------------
class HandleReadModbusRegistersData : public HandleHromatTask
{
public:
	explicit HandleReadModbusRegistersData( unsigned short regAddr,
    	uInt8 regCount, bool sendOnce, const AnsiString& what );
    virtual ~HandleReadModbusRegistersData(){}
private:
    const unsigned short int regAddr_;
    const uInt8 regCount_;

    void Perform(); VIRTUAL_FINAL_

    // ���������� ������� void Perform()
    virtual void DoOnGetData(ByteBuffPtr& rxd, ByteBuffPtr& rxdEnd) = 0;
};
//------------------------------------------------------------------------------
class HandleReadModbusFloatValues : public HandleReadModbusRegistersData
{
public:
	explicit HandleReadModbusFloatValues( unsigned short regAddr,
    	uInt8 count, bool sendOnce, const AnsiString& what,
        TVoid_PDoublePDoubleMthd mthd = NULL);
    virtual ~HandleReadModbusFloatValues(){}
private:
    std::vector<double> result_;
    TVoid_PDoublePDoubleMthd mthd_;
    // ���������� ������� void Perform()
    void DoOnGetData(ByteBuffPtr& rxd, ByteBuffPtr& rxdEnd); // finale - �� �������
	void DoOnEndTransfer(); // finale - �� �������

};
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
class SetHromatDatetimeTask : public HandleHromatTask
{
public:
    explicit SetHromatDatetimeTask();
    virtual ~SetHromatDatetimeTask(){}
private:
    void Perform(); 			VIRTUAL_FINAL_
    void DoOnEndTransfer() {}
};
//------------------------------------------------------------------------------

/**
*       ���������� ������
*/
class HromatCicleControl : public HandleHromatTask
{
public:
    explicit HromatCicleControl(unsigned cicle);
    virtual ~HromatCicleControl() {}
private:
    const unsigned cicle_;
    void Perform(); 			VIRTUAL_FINAL_
    void DoOnEndTransfer()		VIRTUAL_FINAL_
    {}
};
//------------------------------------------------------------------------------
// ������ ��������� ������������
    class HandleReadDeviceState : public HandleHromatTask
    {
        typedef void (__closure *AfeterReadFooT)( unsigned deviceStatecode, unsigned cicleType);

    public:
        explicit HandleReadDeviceState(AfeterReadFooT foo);
        void Perform();                 VIRTUAL_FINAL_
        void DoOnEndTransfer();         VIRTUAL_FINAL_
    private:
        unsigned deviceStatecode_, cicleType_;
        AfeterReadFooT foo_;
    };
//------------------------------------------------------------------------------
/**
*		������ ���� � ������� �������������
*/
class CurrHtgHelperT;
class ProcessCurrenHromatogramm : public HandleHromatTask
{
public:
    explicit ProcessCurrenHromatogramm(CurrHtgHelperT&);
    virtual ~ProcessCurrenHromatogramm() {}
private:
    CurrHtgHelperT& currHtgHelper_;
    void Perform(); 			VIRTUAL_FINAL_
    void DoOnEndTransfer();		VIRTUAL_FINAL_
};


/**
*		����� ��������� �������
*/
class HandleStartArchiveProcess : public HandleHromatTask
{
public:

    explicit HandleStartArchiveProcess(TransferManager* trmn);
    virtual ~HandleStartArchiveProcess();
private:

    //std::vector<ModbusFileInfo> filesInfo_[2];
    HtgItemsArray items_;
    bool isOk_;
    TransferManager* trmn_;

    void Perform(); 			VIRTUAL_FINAL_
    void DoOnEndTransfer();		VIRTUAL_FINAL_
};

/**
*		�������� ��� ���������� ������������� �� ������
*/

class ReadArchiveHTGTask: public HandleHromatTask
{
public:
	explicit ReadArchiveHTGTask(const HtgItemsArray& items, TransferManager* trmn);
    virtual ~ReadArchiveHTGTask() {}
private:
    const HtgItemsArray items_;
    HtgItemsArray readed_, bad_;
    HromatogrammsList htgs_[2];
    TransferManager* trmn_;

    void DoOnEndTransfer();
    //void DoOnBeginTransfer();       VIRTUAL_FINAL_
    void Perform();					VIRTUAL_FINAL_

    //LRESULT HandleStartRead();
    //LRESULT HandleStopRead();
    //LRESULT HandleBeginReadHTG(  const ModbusFileInfo& );
    //LRESULT HandleEndReadHTG(    const ModbusFileInfo& );
    bool ReadHTG(unsigned i, bool detector);
};

/**
*		��������� � ���������� ������� ������� �����
*/
class ReadBasePicTableTask: public HandleHromatTask
{
public:
	explicit ReadBasePicTableTask(bool dtctr);
    virtual ~ReadBasePicTableTask();
private:
	const bool dtctr_;			// ��������
    HromatBasePicTable dt_;
    void DoOnEndTransfer();         VIRTUAL_FINAL_
    void DoOnBeginTransfer();         VIRTUAL_FINAL_
    void Perform();					VIRTUAL_FINAL_
};


/**
*		�������� ������� ������� �����
*/
class WriteBasePicTableTask: public HandleHromatTask
{
public:
	explicit WriteBasePicTableTask(bool dtctr, const HromatBasePicTable& dt );
    virtual ~WriteBasePicTableTask();
private:

	const unsigned dtctr_;			// ��������

    const HromatBasePicTable dt_;

    void DoOnEndTransfer();         VIRTUAL_FINAL_
    void Perform();					VIRTUAL_FINAL_
};

/**
*		��������� ��������� ������������
*/

class ReadHromatConfigTask: public HandleHromatTask
{
public:
    typedef void (__closure *OnReaded)(const VDouble&);

	explicit ReadHromatConfigTask(OnReaded onReaded, const VDouble&);
    virtual ~ReadHromatConfigTask() {}
private:
	VDouble hromatSysConfig_;
    OnReaded onReaded_;

    void Perform();					VIRTUAL_FINAL_
    void DoOnEndTransfer();         VIRTUAL_FINAL_
};


/**
*		�������� ��������� ������������
*/
class WriteHromatConfigTask: public HandleHromatTask
{
public:
	explicit WriteHromatConfigTask( const VDouble& hromatSysConfig );
    virtual ~WriteHromatConfigTask();
private:

	const VDouble hromatSysConfig_;
    void Perform();					VIRTUAL_FINAL_
    void DoOnEndTransfer();         VIRTUAL_FINAL_
    bool writen_;
};

/**
*		���������� ���� �������� �����������
*/
class ChangeHromatCRC16KeyTask: public HandleHromatTask
{
public:
	explicit ChangeHromatCRC16KeyTask( unsigned short oldV, unsigned short newV );
    virtual ~ChangeHromatCRC16KeyTask();
private:

    void DoOnEndTransfer();          VIRTUAL_FINAL_
    //virtual bool ShowTopmostPanel() const { return true; }
    void Perform();					VIRTUAL_FINAL_

    const unsigned short int old_, new_;
    bool badKeyDetected_;
};

/**
*		��������� ������ ���������
*/
class ReadMeasurmentsJournalsTask: public HandleHromatTask
{
public:
	explicit ReadMeasurmentsJournalsTask();
    virtual ~ReadMeasurmentsJournalsTask();
private:

    void DoOnEndTransfer();          VIRTUAL_FINAL_
    void Perform();					VIRTUAL_FINAL_
    virtual bool ShowTopmostPanel() const { return true; }

    bool hasGotJurnalAnalize_, hasGotJurnalAnalizeArchive_;

    // ���� "������"
    Jurnal_Analize jurnalAnalize_;

    // ���� "����� ��������"(12k)
    Jurnal_AnalizeArchive jurnalAnalizeArchive_;

    // ���� "����� �����������"
    // Jurnal_GraduateArchive jurnalGraduateArchive_;
};
//------------------------------------------------------------------------------



//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------

