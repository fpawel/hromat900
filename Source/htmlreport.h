//------------------------------------------------------------------------------
#ifndef htmlreportH
#define htmlreportH
//------------------------------------------------------------------------------
#include <string>
#include <vector>
#include "tinyxml.h"
//------------------------------------------------------------------------------
namespace HTML
{
//------------------------------------------------------------------------------
struct TextAttr
{

    std::string     color;      //aqua, black, blue, fuchsia, gray, green, lime, maroon, navy, olive, purple, red, silver, teal, white, yellow
    bool            bold;
    unsigned        size;
    bool            editiable;
    std::string     align;      // center, bottom, left, middle, right, top
    std::string     tag;
    std::string     className;

    struct
    {
        bool color, bold, size, editiable, align, tag, className;
    } h;


    TextAttr();
    TextAttr(const TextAttr& lhs);
    TextAttr operator=(const TextAttr& lhs);
    void swap(const TextAttr& lhs);
    TextAttr Color(const std::string& arg);
    TextAttr Bold();
    TextAttr Size(unsigned arg);
    TextAttr Editiable();
    TextAttr Align(const std::string& arg);
    TextAttr Tag(const std::string& arg);
    TextAttr Class(const std::string& arg);
};
//------------------------------------------------------------------------------
typedef std::vector<TiXmlElement*> ElemsT;
typedef std::vector<ElemsT> TableT;
//------------------------------------------------------------------------------
TiXmlElement*   AddTag  ( TiXmlNode* elem, const std::string& tag);
TiXmlText*      AddText ( TiXmlNode* node, const std::string& text,
    const TextAttr &atr = TextAttr() );
TiXmlElement*   AddTable( TiXmlNode* elem, unsigned colCount, unsigned rowCount,
    TableT &tbl, ElemsT *pRows = 0 );
void SetCellsAttr(TableT &tbl, const std::string &attr, const std::string &value);
TiXmlDocument* CreateDocument(const std::string &titleText, TiXmlElement*& body,
    TiXmlElement*& head );
void AddStyleSheet( TiXmlNode* headNd, const std::string& fn);
void TableSetBorder(TableT& tbl );


}; // namespace HTMLReport
//------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------
