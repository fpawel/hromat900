//------------------------------------------------------------------------------
#include <vector>
#include <math>

#pragma hdrstop
//------------------------------------------------------------------------------
#include <string>
//------------------------------------------------------------------------------
#include "htmlreport.h"

//------------------------------------------------------------------------------
#pragma package(smart_init)
//------------------------------------------------------------------------------
namespace HTML
{
//------------------------------------------------------------------------------
TiXmlElement* AddTag( TiXmlNode* elem, const std::string& tag)
{
    return elem->LinkEndChild( new TiXmlElement( tag.c_str() ) )->ToElement();
}
//------------------------------------------------------------------------------

TiXmlText* AddText( TiXmlNode* node, const std::string& text, const TextAttr &atr )
{
    TiXmlElement* elem = node->ToElement();
    assert(elem );
    if(atr.h.tag && !atr.tag.empty())
        elem = AddTag( elem, atr.tag);
    if(atr.h.bold)
        elem = AddTag( elem, "b");
    if(atr.h.align || atr.h.editiable || atr.h.className )  {
        elem = AddTag( elem, "p");
        if(atr.h.editiable)
            elem->SetAttribute("contenteditable","true");
        if(atr.h.align)
            elem->SetAttribute("align", atr.align.c_str() );
        if(atr.h.className)
            elem->SetAttribute("class", atr.className.c_str() );
    }
    if(atr.h.color || atr.h.size)  {
        elem = AddTag( elem, "font");
        if(atr.h.color)
            elem->SetAttribute("color", atr.color.c_str() );
        if(atr.h.size)
            elem->SetAttribute("size", atr.size );
    }

    return elem->LinkEndChild( new TiXmlText( text.c_str() ) )->ToText();
}
//------------------------------------------------------------------------------
void AddMeta( TiXmlNode* elem)
{
    TiXmlElement *nd = AddTag( elem, "meta");
    nd->SetAttribute("http-equiv", "Content-Type");
    nd->SetAttribute("content", "text/html; charset=windows-1251");
}
//------------------------------------------------------------------------------
void AddStyleSheet( TiXmlNode* headNd, const std::string& fn)
{
    TiXmlElement *linkStyleSheet = AddTag( headNd, "link");
    linkStyleSheet->SetAttribute("rel", "stylesheet");
    linkStyleSheet->SetAttribute("type", "text/css");
    linkStyleSheet->SetAttribute("href", fn.c_str());
}
//------------------------------------------------------------------------------
TiXmlElement* AddTable(TiXmlNode* elem, unsigned colCount,
    unsigned rowCount, TableT &tbl, ElemsT *pRows )
{
    tbl.resize( colCount, ElemsT(rowCount, NULL) );
    TiXmlElement *tblElem = AddTag( elem, "table");
    for( unsigned ro=0; ro<rowCount; ++ro)
    {
        TiXmlElement *roElem = AddTag( tblElem, "tr");
        if(pRows)
            pRows->push_back(roElem);
        for( unsigned cl=0; cl<colCount; ++cl)
        {
            tbl[cl][ro] = AddTag( roElem, "td");
        }
    }
    return tblElem;
}
//------------------------------------------------------------------------------
void TableSetBorder(TableT& tbl )
{
    const unsigned colCount = tbl.size(), rowCount = tbl.at(0).size();
    for(int cl=0; cl<colCount; ++cl) for(int ro=0; ro<rowCount; ++ro)
    {
        tbl[cl][ro] = AddTag( tbl[cl][ro], "div");
        tbl[cl][ro]->SetAttribute("class", "layer1");
    }
}
//------------------------------------------------------------------------------
void SetCellsAttr(TableT &tbl, const std::string &attr, const std::string &value)
{
    const unsigned colCount = tbl.size(), rowCount = tbl.at(0).size();
    for(int cl=0; cl<colCount; ++cl) for(int ro=0; ro<rowCount; ++ro)
        tbl[cl][ro]->SetAttribute(attr.c_str(), value.c_str());
}
//------------------------------------------------------------------------------
TiXmlDocument* CreateDocument(const std::string &titleText, TiXmlElement*& body,
    TiXmlElement*& head )
{
    TiXmlDocument* xmlDoc = new TiXmlDocument;
    TiXmlElement
        *htm = AddTag( xmlDoc, "html");
            head = AddTag( htm, "head");
    TiXmlElement
                *title = AddTag( head, "title");
    body = AddTag( htm, "body");

    AddMeta( head );
    AddText( title, titleText );

    return xmlDoc;
}
//------------------------------------------------------------------------------
TextAttr::TextAttr()
{
    h.color=h.bold=h.size=h.editiable=h.align=h.tag=h.className=false;
}
//------------------------------------------------------------------------------
TextAttr::TextAttr(const TextAttr& lhs)
{
    swap(lhs);
}
//------------------------------------------------------------------------------
TextAttr TextAttr::operator=(const TextAttr& lhs)
{
    swap(lhs);
    return *this;
}
//------------------------------------------------------------------------------
void TextAttr::swap(const TextAttr& lhs)
{
    //color, bold, size, editiable, align;
    color           = lhs.color;
    bold            = lhs.bold;
    size            = lhs.size;
    editiable       = lhs.editiable;
    align           = lhs.align;
    tag             = lhs.tag;
    className       = lhs.className;
    h.color         = lhs.h.color;
    h.bold          = lhs.h.bold;
    h.size          = lhs.h.size;
    h.editiable     = lhs.h.editiable;
    h.align         = lhs.h.align;
    h.tag           = lhs.h.tag;
    h.className     = lhs.h.className;
}
//------------------------------------------------------------------------------
TextAttr TextAttr::Color(const std::string& arg)
{

    this->color = arg;
    this->h.color = true;
    return *this;
}
//------------------------------------------------------------------------------
TextAttr TextAttr::Bold()
{

    this->bold = true;
    this->h.bold = true;
    return *this;
}
//------------------------------------------------------------------------------
TextAttr TextAttr::Size(unsigned arg)
{

    this->size = arg;
    this->h.size = true;
    return *this;
}
//------------------------------------------------------------------------------
TextAttr TextAttr::Editiable()
{

    this->editiable = true;
    this->h.editiable = true;
    return *this;
}
//------------------------------------------------------------------------------
TextAttr TextAttr::Align(const std::string& arg)
{

    this->align = arg;
    this->h.align = true;
    return *this;
}
//------------------------------------------------------------------------------
TextAttr TextAttr::Tag(const std::string& arg)
{

    this->tag = arg;
    this->h.tag = true;
    return *this;
}
//------------------------------------------------------------------------------
TextAttr TextAttr::Class(const std::string& arg)
{

    this->className = arg;
    this->h.className = true;
    return *this;
}
//------------------------------------------------------------------------------
}; // namespace HTMLReport
//------------------------------------------------------------------------------

