//
//  eXMLParser.cpp
//  eSerial
//
//  Created by Paul O'Neil on 10/21/11.
//  Copyright (c) 2011 Paul O'Neil. All rights reserved.
//

#include <string>
#include <cstring>
#include <sstream>
using namespace std;
#include <libxml/tree.h>
#include "eParser.h"
#include "eData.h"
#include "b64.h"

class eXMLParser : public eParser
{
private:
  xmlDocPtr doc;
  xmlNodePtr root;
  xmlNodePtr node;
  
  void parseXMLObject(xmlNodePtr node);
  void parseXMLField(xmlNodePtr field, EOSObject * obj);
protected:
  virtual void firstPass(const char * filename);
  
public:
  eXMLParser() : doc(NULL), root(NULL), node(NULL) { }
};

template<typename T>
static inline T parse(const xmlChar * text) {
  string contents((const char*)text);
  stringstream stream(contents);
  T var;
  stream >> var;
  return var;
}

template<>
inline char * parse(const xmlChar * text) {
  size_t len = xmlStrlen(text);
  char * result = new char[len + 1];
  strncpy(result, (const char*)text, len);
  result[len] = 0;
  return result;
}

eParser * eParser::newXMLParser()
{
  return new eXMLParser();
}

void eXMLParser::firstPass(const char *filename)
{
  doc = xmlParseFile(filename);
  root = xmlDocGetRootElement(doc);
  for ( node = xmlFirstElementChild(root);
       node != NULL;
       node = xmlNextElementSibling(node)
       )
  {
    parseXMLObject(node);
  }
}

void eXMLParser::parseXMLObject(xmlNodePtr node)
{
  EOSObject * obj = new EOSObject();
  
  xmlChar * xmlPropText = xmlGetProp(node, (const xmlChar*)"id");
  obj->i = parse<size_t>(xmlPropText);
  xmlFree(xmlPropText);
  
  xmlPropText = xmlGetProp(node, (const xmlChar *)"class");
  obj->name = string((const char*)xmlPropText);
  xmlFree(xmlPropText);
  
  for ( xmlNodePtr field = xmlFirstElementChild(node);
       field != NULL;
       field = xmlNextElementSibling(field)
       )
  {
    parseXMLField(field, obj);
  }
}

#define PARSE_TYPE( x ) \
else if( type == #x ) {\
  result = new EOSData<x>(parse<x>(content)); \
}

#define PARSE_ARRAY_TYPE( x ) \
else if( type == #x ) { \
  EOSArrayData<x> * arrData = new EOSArrayData<x>(); \
  convert_from_base64(reinterpret_cast<const char*>(content), strlen(reinterpret_cast<const char*>(content)), &arrData->data); \
  arrData->count = count; \
  result = arrData; \
}

void eXMLParser::parseXMLField(xmlNodePtr field, EOSObject * obj)
{
  string type(reinterpret_cast<const char*>(field->name));
  pEOSData result = NULL;
  xmlChar* content = xmlNodeGetContent(field);
  if( NULL == content ) {
    // TODO error
    return;
  }
  
  if( type == "uint8_t" ) {
    result = new EOSData<uint8_t>((uint8_t)parse<int>(content));
  }
  PARSE_TYPE(uint16_t)
  PARSE_TYPE(uint32_t)
  PARSE_TYPE(uint64_t)
  else if( type == "int8_t" ) {
    result = new EOSData<int8_t>((int8_t)parse<int>(content));
  }
  PARSE_TYPE(int16_t)
  PARSE_TYPE(int32_t)
  PARSE_TYPE(int64_t)
  PARSE_TYPE(float)
  PARSE_TYPE(double)
  PARSE_TYPE(long double)
  else if( type == "bool" ) {
    result = new EOSData<bool>((bool)parse<int>(content));
  }
  else if( type == "eWritable" ) {
  else if( type == "char*" ) {
    result = new EOSData<char*>(parse<char*>(content));
  }
    result = new EOSData<eWritable*>(parse<size_t>(content));
  }
  else if( type == "array" ) {
    // read the types of the elemtns in the array
    xmlChar * propText = xmlGetProp(field, (const xmlChar *)"type");
    type = string((const char *)propText);
    xmlFree(propText);
    
    // read the number of elements in the array
    propText = xmlGetProp(field, (const xmlChar *)"count");
    size_t count = parse<size_t>(propText);
    xmlFree(propText);
    
    if( type == "uint8_t" ) {
      EOSArrayData<uint8_t> * arrData = new EOSArrayData<uint8_t>();
      convert_from_base64(reinterpret_cast<const char *>(content), strlen((const char*)content), &arrData->data);
      arrData->count = count;
      result = arrData;
    }
    PARSE_ARRAY_TYPE(uint16_t)
    PARSE_ARRAY_TYPE(uint32_t)
    PARSE_ARRAY_TYPE(uint64_t)
    PARSE_ARRAY_TYPE(int8_t)
    PARSE_ARRAY_TYPE(int16_t)
    PARSE_ARRAY_TYPE(int32_t)
    PARSE_ARRAY_TYPE(int64_t)
    PARSE_ARRAY_TYPE(float)
    PARSE_ARRAY_TYPE(double)
    PARSE_ARRAY_TYPE(long double)
    else if( type == "eWritable*" ) {
      EOSArrayData<eWritable*> * arrData = new EOSArrayData<eWritable*>();
      convert_from_base64(reinterpret_cast<const char *>(content), strlen((const char*)content), &arrData->data);
      arrData->count = count;
      result = arrData;
    }
  }
  xmlFree(content);
  content = NULL;
  
  if( result ) {
    content = xmlGetProp(field, (const xmlChar*)"name");
    if( NULL == content ) {
      // TODO error;
      delete result;
      return;
    }
    obj->data.insert(make_pair(string((const char*)content), result));
    xmlFree(content);
    content = NULL;
  }
}
