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

    // вызывается методом void Perform()
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
    // вызывается методом void Perform()
    void DoOnGetData(ByteBuffPtr& rxd, ByteBuffPtr& rxdEnd); // finale - не трогать
	void DoOnEndTransfer(); // finale - не трогать

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
*       Управление циклом
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
// запрос состояния хроматографа
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
*		Запрос инфы о текущей хроматограмме
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
*		Старт обработки архивов
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
*		Трансфер для считывания хроматограммы из архива
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
*		Прочитать и отобразить базовую таблицу пиков
*/
class ReadBasePicTableTask: public HandleHromatTask
{
public:
	explicit ReadBasePicTableTask(bool dtctr);
    virtual ~ReadBasePicTableTask();
private:
	const bool dtctr_;			// детектор
    HromatBasePicTable dt_;
    void DoOnEndTransfer();         VIRTUAL_FINAL_
    void DoOnBeginTransfer();         VIRTUAL_FINAL_
    void Perform();					VIRTUAL_FINAL_
};


/**
*		Записать базовую таблицу пиков
*/
class WriteBasePicTableTask: public HandleHromatTask
{
public:
	explicit WriteBasePicTableTask(bool dtctr, const HromatBasePicTable& dt );
    virtual ~WriteBasePicTableTask();
private:

	const unsigned dtctr_;			// детектор

    const HromatBasePicTable dt_;

    void DoOnEndTransfer();         VIRTUAL_FINAL_
    void Perform();					VIRTUAL_FINAL_
};

/**
*		Прочитать параметры конфигурации
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
*		Записать параметры конфигурации
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
*		Установить ключ проверки подлинности
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
*		Прочитать архивы измерений
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

    // Файл "Анализ"
    Jurnal_Analize jurnalAnalize_;

    // Файл "Архив анализов"(12k)
    Jurnal_AnalizeArchive jurnalAnalizeArchive_;

    // Файл "Архив градуировок"
    // Jurnal_GraduateArchive jurnalGraduateArchive_;
};
//------------------------------------------------------------------------------



//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------

