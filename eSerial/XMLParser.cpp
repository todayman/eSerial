//
//  eos/serialization/eXMLParser.cpp
//
//  Created by Paul O'Neil on 10/21/11.
//  Copyright (c) 2011 Paul O'Neil. All rights reserved.
//

#include <string>
#include <cstring>
#include <sstream>
using namespace std;
#include <libxml/tree.h>
#include "Parser.h"
#include "XMLParser.h"
#include "Data.h"
#include "b64.h"
using namespace eos::serialization;

typedef long double long_double;

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
  size_t len = (size_t)xmlStrlen(text);
  char * result = new char[len + 1];
  strncpy(result, (const char*)text, len);
  result[len] = 0;
  return result;
}

Parser * Parser::newXMLParser()
{
  return new XMLParser();
}

void XMLParser::firstPass(const string& filename)
{
  doc = xmlParseFile(filename.c_str());
  root = xmlDocGetRootElement(doc);
  for ( node = xmlFirstElementChild(root);
       node != nullptr;
       node = xmlNextElementSibling(node)
       )
  {
    parseXMLObject(node);
  }
  
  xmlFreeDoc(doc);
  root = nullptr;
  doc = nullptr;
}

void XMLParser::parseXMLObject(xmlNodePtr node)
{
  Object * obj = new Object();
  
  xmlChar * xmlPropText = xmlGetProp(node, (const xmlChar*)"id");
  obj->id = parse<size_t>(xmlPropText);
  xmlFree(xmlPropText);
  
  xmlPropText = xmlGetProp(node, (const xmlChar *)"class");
  obj->name = string((const char*)xmlPropText);
  xmlFree(xmlPropText);
  
  for ( xmlNodePtr field = xmlFirstElementChild(node);
       field != nullptr;
       field = xmlNextElementSibling(field)
       )
  {
    parseXMLField(field, obj);
  }
}

template<typename T>
static inline void parseString(T * data, const char * str, size_t count)
{
	stringstream ss(str);
	if( count ) {
		ss >> data[0];
	}
	for( size_t i = 1; i < count; i++ ) {
		ss >> data[i];
	}
}

template<>
inline void parseString(uint8_t * data, const char * str, size_t count)
{
	stringstream ss(str);
	int buf;
	if( count ) {
		ss >> buf;
		data[0] = (uint8_t)buf;
	}
	for( size_t i = 1; i < count; i++ ) {
		ss >> buf;
		data[i] = (uint8_t)buf;
	}
}

template<>
inline void parseString(int8_t * data, const char * str, size_t count)
{
	stringstream ss(str);
	int buf;
	if( count ) {
		ss >> buf;
		data[0] = (int8_t)buf;
	}
	for( size_t i = 1; i < count; i++ ) {
		ss >> buf;
		data[i] = (int8_t)buf;
	}
}

#define PARSE_TYPE( x ) \
else if( type == #x ) {\
result = new Data<x>(parse<x>(content)); \
}


#define PARSE_ARRAY_TYPE( x ) \
else if( type == #x ) { \
  ArrayData<x> * arrData = new ArrayData<x>(); \
  if( readable_hint ) { \
    arrData->data = new x[count]; \
    parseString(arrData->data, reinterpret_cast<const char*>(content), count); \
  } \
  else { \
    arrData->hints |= FREE_HINT; \
    convert_from_base64(reinterpret_cast<const char *>(content), (size_t)xmlStrlen(content), &arrData->data); \
  } \
  arrData->count = count; \
  result = arrData; \
}

void XMLParser::parseXMLField(xmlNodePtr field, Object * obj)
{
  string type(reinterpret_cast<const char*>(field->name));
  pData result = nullptr;
  xmlChar* content = xmlNodeGetContent(field);
  if( nullptr == content ) {
    // TODO error
    return;
  }
  
  if( type == "uint8_t" ) {
    result = new Data<uint8_t>((uint8_t)parse<int>(content));
  }
  PARSE_TYPE(uint16_t)
  PARSE_TYPE(uint32_t)
  PARSE_TYPE(uint64_t)
  else if( type == "int8_t" ) {
    result = new Data<int8_t>((int8_t)parse<int>(content));
  }
  PARSE_TYPE(int16_t)
  PARSE_TYPE(int32_t)
  PARSE_TYPE(int64_t)
  PARSE_TYPE(float)
  PARSE_TYPE(double)
  PARSE_TYPE(long double)
  else if( type == "bool" ) {
    result = new Data<bool>((bool)parse<int>(content));
  }
  else if( type == "char_star" ) {
    result = new Data<char*>(parse<char*>(content));
  }
  else if( type == "eos.serialization.Writable*" ) {
    result = new Data<Writable*>(parse<size_t>(content));
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
    
    // see if the array is human readable
    bool readable_hint = false;
    propText = xmlGetProp(field, (const xmlChar*)"hints");
    if( propText ) {
      hint_t hints;
      parseString(&hints, reinterpret_cast<const char*>(propText), 1);
      if( hints & READABLE_HINT ) {
        readable_hint = true;
      }
    }
    
    if( type == "uint8_t" ) {
      ArrayData<uint8_t> * arrData = new ArrayData<uint8_t>();
      if( readable_hint ) {
        arrData->data = new uint8_t[count];
        parseString(arrData->data, reinterpret_cast<const char*>(content), count);
      }
      else {
        arrData->hints |= FREE_HINT;
        convert_from_base64(reinterpret_cast<const char *>(content), (size_t)xmlStrlen(content), &arrData->data);
      }
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
    PARSE_ARRAY_TYPE(bool)
    else if( type == "eos.serialization.Writable*" ) {
      ArrayData<Writable*> * arrData = new ArrayData<Writable*>();
      if( readable_hint ) {
        arrData->data = new size_t[count];
        parseString(arrData->data, reinterpret_cast<const char*>(content), count);
      }
      else {
        arrData->hints |= FREE_HINT;
        convert_from_base64(reinterpret_cast<const char *>(content), (size_t)xmlStrlen(content), &arrData->data);
      }
      arrData->count = count;
      result = arrData;
    }
  }
  xmlFree(content);
  content = nullptr;
  
  if( result ) {
    content = xmlGetProp(field, (const xmlChar*)"name");
    if( nullptr == content ) {
      // TODO error;
      delete result;
      return;
    }
    obj->data.insert(make_pair(string((const char*)content), result));
    xmlFree(content);
    content = nullptr;
  }
}
