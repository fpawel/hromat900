//------------------------------------------------------------------------------
#ifndef CurHtgH
#define CurHtgH
//------------------------------------------------------------------------------
#include <deque>
//------------------------------------------------------------------------------
#include "boost\noncopyable.hpp"
#include "boost\shared_ptr.hpp"
//------------------------------------------------------------------------------
#include "Hromat900.hpp"
#include "htg_.h"
#include "MyNoDestroySingleton.hpp"
//------------------------------------------------------------------------------
class ProcessHTGFile;
typedef boost::shared_ptr<ProcessHTGFile> PProcessHTGFile;
typedef std::deque< PProcessHTGFile > ProcessHTGFilesList;

class ProcessGraduation;
typedef boost::shared_ptr<ProcessGraduation> PProcessGraduation;


class CurrHtgHelperT : public    boost::noncopyable
{
public:
    explicit CurrHtgHelperT(const bool& enebleReadDetectorIndication);
    void Process();
    void DoOnEndTransfer();

private:
    const bool& enebleReadDetectorIndication_;
    unsigned curHtgIndex_;
    bool isCurHtgIndexInitialized_;
    ProcessHTGFilesList procFiles_[2];
    PProcessGraduation processGraduation_;

    ModbusFileInfo curHtgInfo_[2];

    void ReadHtg(bool detector);
};



//------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------
