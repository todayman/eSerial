//
//  eos/serialization/eXMLWriter.cpp
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

#include <sstream>
using namespace std;
#include <libxml/tree.h>
#include "b64.h"
#include "Data.h"
#include "Writer.h"
#include "XMLWriter.h"
using namespace eos::serialization;

Writer * Writer::newXMLWriter() {
  return new XMLWriter();
}

template <typename T>
static inline std::string toString (const T & t)
{
  stringstream ss;
  ss << t;
  return ss.str();
}

void XMLWriter::writeStream(ostream &output)
{
  xmlDocPtr doc = xmlNewDoc((const xmlChar *)"1.0");
  doc->encoding = xmlStrdup((const xmlChar *)"UTF-8");
  doc->xmlRootNode = xmlNewDocNode(doc, nullptr, (const xmlChar *)"eos.serialization", nullptr);
  
  xmlNodePtr tree = doc->xmlRootNode;
  
  for( auto iter : root_objs ) {
    addToXML(iter, tree);
  }
	
	xmlChar * dump = nullptr;
	int size = -1;
	xmlDocDumpFormatMemory(doc, &dump, &size, true);
	output << dump;
	xmlFree(dump);
	xmlFreeDoc(doc);
}

template<typename T>
static inline std::string toString(const T * data, size_t count)
{
  stringstream ss;
  if( count ) {
    ss << data[0];
  }
  for( size_t i = 1; i < count; i++ ) {
    ss << " " << data[i];
  }
	return ss.str();
}

template<>
inline std::string toString(const char * data, size_t count)
{
  stringstream ss;
  if( count ) {
    ss << (int)data[0];
  }
  for( size_t i = 1; i < count; i++ ) {
    ss << " " << (int)data[i];
  }
	return ss.str();
}

template<>
inline std::string toString(const uint8_t * data, size_t count)
{
  stringstream ss;
  if( count ) {
    ss << (int)data[0];
  }
  for( size_t i = 1; i < count; i++ ) {
    ss << " " << (int)data[i];
  }
	return ss.str();
}

template<>
inline std::string toString(const int8_t * data, size_t count)
{
  stringstream ss;
  if( count ) {
    ss << (int)data[0];
  }
  for( size_t i = 1; i < count; i++ ) {
    ss << " " << (int)data[i];
  }
	return ss.str();
}

template<typename T>
xmlNodePtr XMLWriter::writeArrayToXML(xmlNodePtr node, ArrayData<T> * data, const char * type)
{
	string str;
  if( data->hints & READABLE_HINT ) {
		str = toString(data->data, data->count);
  }
  else {
		char* dst = nullptr;
    convert_to_base64(data->data, data->count, &dst);
		str = dst;
		delete []dst;
  }
  xmlNodePtr field = xmlNewTextChild(node, nullptr, (const xmlChar*)"array", (xmlChar*)str.c_str());
  
  if( data->hints & READABLE_HINT ) {
    xmlNewProp(field, (const xmlChar*)"hints", (const xmlChar*)toString(READABLE_HINT).c_str());
  }
  xmlNewProp(field, (const xmlChar*)"type", (const xmlChar*)type);
  xmlNewProp(field, (const xmlChar*)"count", (const xmlChar*)toString(data->count).c_str());
	return field;
}

#define WRITE_XML(x)\
else if( Data<x>* data##x = dynamic_cast<Data<x>*>(iter.second) ) { \
	field = xmlNewChild(node, nullptr, (const xmlChar*)#x, (const xmlChar*)toString(data##x ->data).c_str()); \
}

#define WRITE_XML_ARRAY(x)\
else if( ArrayData<x>* array_data_##x = dynamic_cast<ArrayData<x>*>(iter.second) ) { \
  field = writeArrayToXML(node, array_data_##x, #x);\
}

typedef long double long_double;
xmlNodePtr XMLWriter::addToXML(Object *obj, xmlNodePtr parent)
{
  xmlNodePtr node = xmlNewChild(parent, nullptr, (const xmlChar *)"object", nullptr);
  xmlNewProp(node, (const xmlChar*)"id", (const xmlChar*)toString(obj->id).c_str());
  xmlNewProp(node, (const xmlChar*)"class", (const xmlChar*)obj->name.c_str());
  
  for( auto iter : obj->data ) {
    xmlNodePtr field = nullptr;
		if( Data<char>* data = dynamic_cast<Data<char>*>(iter.second) ) {
			field = xmlNewChild(node, nullptr, (const xmlChar*)"char", (const xmlChar*)toString((int)data->data).c_str());
		}
    else if( Data<uint8_t>* data_uint8_t = dynamic_cast<Data<uint8_t>*>(iter.second) ) {
      field = xmlNewChild(node, nullptr, (const xmlChar*)"uint8_t", (const xmlChar*)toString((int)data_uint8_t->data).c_str());
    }
    WRITE_XML(uint16_t)
    WRITE_XML(uint32_t)
    WRITE_XML(uint64_t)
    else if( Data<int8_t>* data_int8_t = dynamic_cast<Data<int8_t>*>(iter.second) ) {
      field = xmlNewChild(node, nullptr, (const xmlChar*)"int8_t", (const xmlChar*)toString((int)data_int8_t->data).c_str());
    }
    WRITE_XML(int16_t)
    WRITE_XML(int32_t)
    WRITE_XML(int64_t)
    WRITE_XML(float)
    WRITE_XML(double)
    WRITE_XML(long_double)
    else if( Data<bool>* data_bool = dynamic_cast<Data<bool>*>(iter.second) ) {
      field = xmlNewChild(node, nullptr, (const xmlChar*)"bool", (const xmlChar*)toString((int)data_bool->data).c_str());
    }
    else if( Data<const char*>* data_char_star = dynamic_cast<Data<const char*>*>(iter.second) ) {
      field = xmlNewTextChild(node, nullptr, (const xmlChar*)"char_star", (const xmlChar*)data_char_star->data);
    }
    else if( Data<Writable> * data_writable = dynamic_cast<Data<Writable>*>(iter.second) ) {
      field = addToXML(data_writable, node);
    }
    else if( Data<Writable*> * data_writable_star = dynamic_cast<Data<Writable*>*>(iter.second) ) {
      field = xmlNewChild(node, nullptr, (const xmlChar*)"eos.serialization.Writable", nullptr);
      xmlNewProp(field, (const xmlChar*)"id", (const xmlChar*)toString(data_writable_star->id).c_str());
    }
    WRITE_XML_ARRAY(char)
    WRITE_XML_ARRAY(uint8_t)
    WRITE_XML_ARRAY(uint16_t)
    WRITE_XML_ARRAY(uint32_t)
    WRITE_XML_ARRAY(uint64_t)
    WRITE_XML_ARRAY(int8_t)
    WRITE_XML_ARRAY(int16_t)
    WRITE_XML_ARRAY(int32_t)
    WRITE_XML_ARRAY(int64_t)
    WRITE_XML_ARRAY(float)
    WRITE_XML_ARRAY(double)
    WRITE_XML_ARRAY(long_double)
    WRITE_XML_ARRAY(bool)
    else if( ArrayData<Writable*>* array_data_writable_star = dynamic_cast<ArrayData<Writable*>*>(iter.second) ) {
      field = writeArrayToXML(node, array_data_writable_star, "eos.serialization.Writable_star");
    }
    
    xmlNewProp(field, (const xmlChar*)"name", (const xmlChar*)iter.first.c_str());
  }
	return node;
}
