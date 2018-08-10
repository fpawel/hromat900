#include <fstream>
//---------------------------------------------------------------------------
#pragma hdrstop

#include "xmlUtils_.h"
#include "MyFileCtrl.h"

#include "guicon.h"
#include "HromatCrypt.h"


//---------------------------------------------------------------------------

#pragma package(smart_init)

namespace TiXML
{

TiXmlDocument* CreateXMLDocument( const AnsiString& fn )
{
	TiXmlDocument* ret = new TiXmlDocument;
    if( !ret->LoadFile( fn.c_str() )  ) ret->Parse(DEFAULT_FIRST_XML_STRING);
    return ret;
}

///////////////////////////////////////////////////////////////////////////////
//				 атрибуты дерева
///////////////////////////////////////////////////////////////////////////////
//         				строка
AnsiString GetAtr( TiXmlElement* elem, CStr id )
{
	return elem ? elem->Attribute( id.c_str() ) : "";
}
void SetAtr( TiXmlElement* elem, CStr id, CStr val )
{
	elem->SetAttribute(id.c_str(), val.c_str() );
}
void SetIntAtr( TiXmlElement* elem, CStr id, int val )
{
	SetAtr( elem, id, IntToStr(val) );
}

void SetFloatAtr( TiXmlElement* elem, CStr id, double val, CStr fmt )
{
	 if( fmt.IsEmpty() )
     	SetAtr( elem, id, FloatToStr(val) );
     else
     	SetAtr( elem, id, FormatFloat(fmt,val) );
}



//---------------------------------------------------------------------------
AnsiString GetStrAtr( TiXmlElement* elem, CStr id, CStr def)
{
	const AnsiString s = GetAtr( elem, id );
    return s.IsEmpty() ? def : s;
}
//---------------------------------------------------------------------------
//						int
MyInt GetIntAtr( TiXmlElement* elem, CStr name)
{
    return MyStrToInt( GetAtr( elem, name ) );;
}
int GetIntAtrDef( TiXmlElement* elem, CStr name, int def)
{
	const MyInt getAtr = GetIntAtr( elem, name);
    return getAtr.second ? getAtr.first : def;
}

int GetXMLInteger( TiXmlElement* elem, CStr name, int valMin, int valMax, int defVal)
{
	assert(valMin<=valMax);

    const MyInt getAtr = GetIntAtr( elem, name);
    int val = getAtr.first;
    if( !getAtr.second ) val = defVal; else
    if( val>valMax ) val = valMax;  else
    if( val<valMin ) val = valMin;
    return val;
}

//---------------------------------------------------------------------------
//						float
MyDouble GetFloatAtr( TiXmlElement* elem, CStr name)
{
	return MyStrToD( GetAtr( elem, name ) );
}
double GetFloatAtrDef( TiXmlElement* elem, CStr name, double def)
{
	const MyDouble getAtr = GetFloatAtr( elem, name);
    return getAtr.second ? getAtr.first : def;
}

std::vector<double> GetFloatArrAtr( TiXmlElement* elem, unsigned count, double def)
{   
	std::vector<double> ret( count );
    for( unsigned i=0; i<count; ++i )
    	ret[i] = GetFloatAtrDef( elem, AnsiString().sprintf("_%d",i),  def);
    return ret;
}
void SetFloatArrAtr( TiXmlElement* elem,  const std::vector<double>& dt)
{
	char id[] = "_0";
    for( unsigned i=0; i<dt.size(); ++i  )
    {
    	id[1] = '0' + i;
        elem->SetDoubleAttribute( id, dt.at(i) );
    }
}


//---------------------------------------------------------------------------




//
//функции для получения элементов дерева
//

TiXmlElement* GiveNotNulElement( TiXmlNode* elem, CStr s)
{
	assert(elem);
    TiXmlElement* ret = elem->FirstChildElement( s.c_str() );
    TiXmlElement* ret1 =  ret ? ret : elem->LinkEndChild( new TiXmlElement( s.c_str() ) )->ToElement();
    const char* v = ret1->Value();

    return ret1;
}



TiXmlElement* GiveNotNullElementByTag(TiXmlNode* elem, CStr inputStr )
{
	assert(elem);
    const char* inputStrTag = inputStr.c_str();
	const unsigned strTagLen = strlen( inputStrTag ) ;
    std::vector<char> tag( inputStrTag, inputStrTag + strTagLen + 1 );
    for( unsigned i=0; i<strTagLen + 1; ++i ) if( tag[i]=='\\' ) tag[i]='\0';

    //TiXmlElement* ret = elem->ToElement();
    for( const char *s=tag.begin(); s!=tag.end(); s += strlen(s) + 1  )
    	elem = GiveNotNulElement(elem, s );
    return elem->ToElement();;
}

TiXmlElement* GetElementByTag( TiXmlNode* elem, CStr inputStr )
{
	const char* inputStrTag = inputStr.c_str();
	const unsigned strTagLen = strlen(inputStrTag) ;
    std::vector<char> tag( inputStrTag, inputStrTag + strTagLen + 1 );
    for( unsigned i=0; i<strTagLen + 1; ++i ) if( tag[i]=='\\' ) tag[i]='\0';
    for( const char* s=tag.begin(); elem && s!=tag.end(); s += strlen(s) + 1  )
    	elem = elem->FirstChildElement(s);
    return elem->ToElement();
}

bool IsElemValueMatch( TiXmlElement* elem, CStr tag )
{
	return AnsiString(elem->Value())==tag;
}

TiXmlElement* GetFirstNameMatchedElement(TiXmlElement* elem, CStr tag)
{
	if( elem==NULL || IsElemValueMatch( elem, tag ) ) return elem;
	for( elem = elem->FirstChildElement(); elem; elem=elem->NextSiblingElement() )
    {
    	TiXmlElement* cild_i_Matched = GetFirstNameMatchedElement(elem, tag);
    	if( cild_i_Matched ) return cild_i_Matched;
    }
    return NULL;
}

TiXmlElement* GetNotNullFirstNameMatchedElement(TiXmlElement* elem, CStr tag)
{
	TiXmlElement* ret = GetFirstNameMatchedElement(elem, tag);
    return ret==NULL ? GiveNotNullElementByTag( elem, tag ) : ret;
}

void SaveWindowPlacement( TiXmlElement* elem, HWND hWnd )
{
    WINDOWPLACEMENT wndpl = {0};
    GetWindowPlacement(hWnd, &wndpl);
    #define SAVE_(nn) elem->SetAttribute(""#nn, wndpl.##nn )
    SAVE_(length);
    SAVE_(flags);
    SAVE_(showCmd);
    SAVE_(ptMinPosition.x);
    SAVE_(ptMinPosition.y);
    SAVE_(ptMaxPosition.x);
    SAVE_(ptMaxPosition.y);
    SAVE_(rcNormalPosition.left);
    SAVE_(rcNormalPosition.top);
    SAVE_(rcNormalPosition.right);
    SAVE_(rcNormalPosition.bottom);
    #undef SAVE_
}


void LoadWindowPlacement( TiXmlElement* elem, HWND hWnd )
{
    WINDOWPLACEMENT wndpl = {0};
    ::GetWindowPlacement(hWnd, &wndpl);
    #define LOAD_(nn) wndpl.##nn = GetIntAtrDef( elem, ""#nn, wndpl.##nn )
    LOAD_(length);
    LOAD_(flags);
    LOAD_(showCmd);
    LOAD_(ptMinPosition.x);
    LOAD_(ptMinPosition.y);
    LOAD_(ptMaxPosition.x);
    LOAD_(ptMaxPosition.y);
    LOAD_(rcNormalPosition.left);
    LOAD_(rcNormalPosition.top);
    LOAD_(rcNormalPosition.right);
    LOAD_(rcNormalPosition.bottom);
    #undef LOAD_
    ::SetWindowPlacement(hWnd, &wndpl);
}

void EncryptSignAndSaveDocument( const AnsiString &filename, const TiXmlDocument &xmlDocument )
{
    MyForceDirectory(ExtractFilePath( filename ) );

    TiXmlPrinter xmlPrinter;
    xmlDocument.Accept( &xmlPrinter );

    const std::string strBuff = xmlPrinter.CStr();
    const unsigned len = strBuff.size();
    if(len==0) return;

    const char *pStrBuff = &strBuff[0];
    std::vector<unsigned char> buff( pStrBuff, pStrBuff+len );

    const unsigned short int crc16 = HromatSignData( &buff.at(0), len );
    buff.push_back( crc16 >> 8 );
    buff.push_back( crc16 );

    assert( HromatSignData( &buff.at(0), len+2 )==0 );

    HromatCryptData( &buff.at(0), len+2, &buff.at(0) );

    std::ofstream ofstrm( filename.c_str(), std::ios::binary );
    ofstrm.write( &buff.at(0), len+2 );
    ofstrm.close();
}

TiXmlDocument* DecryptAndLoadDocument( const AnsiString &filename)
{
	//LOG_MY_CON_QUOTES( MYSPRINTF_("Открываем шифрованный XML документ %s", ExtractFileName(filename) ) );
	TiXmlDocument* pDoc = new TiXmlDocument;

    std::vector<unsigned char> fileBuff, hashBuff, hash;
    LoadFileToBuff( filename, fileBuff  );
    if( fileBuff.empty() )
    {
    	//MyWCout( "Файл не найден или пуст!\n" );
    	return pDoc;
    }

    const unsigned len = fileBuff.size();
    HromatCryptData( &fileBuff.at(0), len, &fileBuff.at(0) );

    if( HromatSignData( &fileBuff.at(0), len )!=0 )
    {
    	//MyWCout( "Подпись по файлу не совпадает!\n" );
    	return pDoc;
    }

    fileBuff.at(len-2) = '\0';
    try
    {
        pDoc->Parse( &fileBuff.at(0), 0, TIXML_ENCODING_LEGACY );
        //pDoc->SaveFile("c:\\loaded.xml");

    }
    catch (std::exception &e)
    {
    	MyWCout( "Не корректный формат. Файл не является шифрованным XML документом.\n" );
    }

    //MyWCout("Ok!\n");
    return pDoc;
}


};//namespace TiXML



