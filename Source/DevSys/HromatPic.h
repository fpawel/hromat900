//---------------------------------------------------------------------------
#ifndef HromatPicH
#define HromatPicH
//---------------------------------------------------------------------------
#include "Hromat900.hpp"
#include <grids.hpp>


//------------------------------------------------------------------------------
AnsiString PicValToStr(float v);
void PicConcToStr( AnsiString& sC, AnsiString& sD, float c, float d );
void ShowHromatPicTable( TStringGrid*, const HromatPicData*, const HromatPicData*);
void ShowHromatBasePicTable( TStringGrid*, const HromatBasePicData*, const HromatBasePicData*);


void InitializePicTableGrd( TStringGrid*);
void InitializeBasePicTableGrd( TStringGrid*);

HromatBasePicTable MakeBasePicTableFromGrd(TStringGrid* grd);
class TiXmlElement;
HromatBasePicTable MakeBasePicTableFromXML(TiXmlElement *xml);

void SaveHromatBasePicTable( TiXmlElement *xml, const HromatBasePicData*, const HromatBasePicData*);





//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
