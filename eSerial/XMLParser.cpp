//
//  eos/serialization/eXMLParser.cpp
//
//  Copyright 2012 Paul O'Neil.
//
//  This file is part of eSerial.
//
//  eSerial is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  eSerial is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with eSerial.  If not, see <http://www.gnu.org/licenses/>.
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

void XMLParser::firstPass(istream& input)
{
	input.seekg(0, ios::end);
	ios::pos_type length = input.tellg();
	char * buffer = new char[length];
	input.seekg(0, ios::beg);
	input.read(buffer, static_cast<std::streamsize>(length));
	
	xmlDocPtr doc = xmlParseMemory(buffer, static_cast<int>(length));
	
  xmlNodePtr root = xmlDocGetRootElement(doc);
  for ( xmlNodePtr node = xmlFirstElementChild(root);
       node != nullptr;
       node = xmlNextElementSibling(node)
       )
  {
		cur_root_obj = nullptr;
		this->root_objs.push_back(parseXMLObject(node));
  }
  
  xmlFreeDoc(doc);
  root = nullptr;
  doc = nullptr;
	delete []buffer;
}

Object * XMLParser::parseXMLObject(xmlNodePtr node)
{
	Object * obj = new Object();
	if( cur_root_obj ) {
		obj->owner = cur_root_obj;
	}
	else {
		obj->owner = obj;
		cur_root_obj = obj;
	}
	
  xmlChar * xmlPropText = xmlGetProp(node, (const xmlChar*)"id");
  obj->id = parse<size_t>(xmlPropText);
  xmlFree(xmlPropText);
  
	this->metaIDList[obj->id] = obj;
	
  xmlPropText = xmlGetProp(node, (const xmlChar *)"class");
  obj->name = string((char*)xmlPropText);
  xmlFree(xmlPropText);
  
  for ( xmlNodePtr field = xmlFirstElementChild(node);
       field != nullptr;
       field = xmlNextElementSibling(field)
       )
  {
    parseXMLField(field, obj);
  }
	return obj;
}

template<typename T>
static inline void parseString(T * data, const char * str, size_t count)
{
	stringstream ss(str);
	for( size_t i = 0; i < count; ++i ) {
		ss >> data[i];
	}
}

template<>
inline void parseString(uint8_t * data, const char * str, size_t count)
{
	stringstream ss(str);
	int buf;
	for( size_t i = 0; i < count; ++i ) {
		ss >> buf;
		data[i] = (uint8_t)buf;
	}
}

template<>
inline void parseString(int8_t * data, const char * str, size_t count)
{
	stringstream ss(str);
	int buf;
	for( size_t i = 0; i < count; ++i ) {
		ss >> buf;
		data[i] = (int8_t)buf;
	}
}

template<>
inline void parseString(char * data, const char * str, size_t count)
{
	stringstream ss(str);
	int buf;
	for( size_t i = 0; i < count; i++ ) {
		ss >> buf;
		data[i] = (char)buf;
	}
}

#define PARSE_TYPE( x ) \
else if( type == #x ) {\
result = new Data<x>(cur_root_obj, parse<x>(content)); \
}

#define PARSE_ARRAY_TYPE( x ) \
else if( type == #x ) { \
  ArrayData<x> * arrData = new ArrayData<x>(cur_root_obj); \
  if( readable_hint ) { \
    arrData->data = new x[count]; \
    parseString(arrData->data, reinterpret_cast<const char*>(content), count); \
  } \
  else { \
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
    result = new Data<uint8_t>(cur_root_obj, (uint8_t)parse<int>(content));
  }
  else if( type == "char" ) {
    result = new Data<char>(cur_root_obj, (char)parse<int>(content));
  }
  PARSE_TYPE(uint16_t)
  PARSE_TYPE(uint32_t)
  PARSE_TYPE(uint64_t)
  else if( type == "int8_t" ) {
    result = new Data<int8_t>(cur_root_obj, (int8_t)parse<int>(content));
  }
  PARSE_TYPE(int16_t)
  PARSE_TYPE(int32_t)
  PARSE_TYPE(int64_t)
  PARSE_TYPE(float)
  PARSE_TYPE(double)
  PARSE_TYPE(long_double)
  else if( type == "bool" ) {
    result = new Data<bool>(cur_root_obj, (bool)parse<int>(content));
  }
  else if( type == "char_star" ) {
    result = new Data<char*>(cur_root_obj, parse<char*>(content));
  }
	else if( type == "object" ) {
		result = parseXMLObject(field);
	}
  else if( type == "eos.serialization.Writable" ) {
		xmlChar* idString = xmlGetProp(field, (const xmlChar*)"id");
    result = new Data<Writable*>(cur_root_obj, parse<size_t>(idString));
		xmlFree(idString);
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
		xmlFree(propText);
    
    if( type == "char" ) {
      ArrayData<char> * arrData = new ArrayData<char>(cur_root_obj);
      if( readable_hint ) {
        arrData->data = new char[count];
        parseString(arrData->data, reinterpret_cast<const char*>(content), count);
      }
      else {
        convert_from_base64(reinterpret_cast<const char *>(content), (size_t)xmlStrlen(content), &arrData->data);
      }
      arrData->count = count;
      result = arrData;
    }
    else if( type == "uint8_t" ) {
      ArrayData<uint8_t> * arrData = new ArrayData<uint8_t>(cur_root_obj);
      if( readable_hint ) {
        arrData->data = new uint8_t[count];
        parseString(arrData->data, reinterpret_cast<const char*>(content), count);
      }
      else {
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
    PARSE_ARRAY_TYPE(long_double)
    PARSE_ARRAY_TYPE(bool)
    else if( type == "eos.serialization.Writable_star" ) {
      ArrayData<Writable*> * arrData = new ArrayData<Writable*>(cur_root_obj);
      if( readable_hint ) {
        arrData->data = new size_t[count];
        parseString(arrData->data, reinterpret_cast<const char*>(content), count);
      }
      else {
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
