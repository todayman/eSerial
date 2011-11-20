//
//  eos/serialization/eXMLWriter.cpp
//
//  Created by Paul O'Neil on 10/21/11.
//  Copyright (c) 2011 Paul O'Neil. All rights reserved.
//

#include <sstream>
using namespace std;
#include <libxml/tree.h>
#include "b64.h"
#include "Data.h"
#include "Writer.h"
using namespace eos::serialization;

class XMLWriter : public Writer {
  void addToXML(Object * obj, xmlNodePtr parent);
  
  template<typename T>
  void writeArrayToXML(xmlNodePtr node, xmlNodePtr field, ArrayData<T> * data, const char * type);
  
public:
  XMLWriter() = default;
  virtual void writeFile(const string& filename) override;
};

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

void XMLWriter::writeFile(const string& filename)
{
  xmlDocPtr doc = xmlNewDoc((const xmlChar *)"1.0");
  doc->encoding = xmlStrdup((const xmlChar *)"UTF-8");
  doc->xmlRootNode = xmlNewDocNode(doc, nullptr, (const xmlChar *)"eos.serialization", nullptr);
  
  xmlNodePtr tree = doc->xmlRootNode;
  
  for( auto iter = root_objs.begin(); iter != root_objs.end(); iter++ ) {
    addToXML(*iter, tree);
  }
  
  xmlSaveFormatFileEnc(filename.c_str(), doc, "UTF-8", 1);
  xmlFreeDoc(doc);
}


#define WRITE_XML(x)\
else if( Data<x>* data = dynamic_cast<Data<x>*>(iter.second) ) { \
  field = xmlNewChild(node, nullptr, (const xmlChar*)#x, (const xmlChar*)toString(data->data).c_str()); \
}

template<typename T>
static inline char * toString(const T * data, size_t count)
{
  stringstream ss;
  char * dst;
  if( count ) {
    ss << data[0];
  }
  for( size_t i = 1; i < count; i++ ) {
    ss << " " << data[i];
  }
  dst = new char[ss.str().size() + 1];
  strncpy(dst, ss.str().c_str(), ss.str().size());
  dst[ss.str().size()] = 0;
  return dst;
}

template<>
inline char * toString(const uint8_t * data, size_t count)
{
  stringstream ss;
  char * dst;
  if( count ) {
    ss << (int)data[0];
  }
  for( size_t i = 1; i < count; i++ ) {
    ss << " " << (int)data[i];
  }
  dst = new char[ss.str().size() + 1];
  strncpy(dst, ss.str().c_str(), ss.str().size());
  dst[ss.str().size()] = 0;
  return dst;
}

template<>
inline char * toString(const int8_t * data, size_t count)
{
  stringstream ss;
  char * dst;
  if( count ) {
    ss << (int)data[0];
  }
  for( size_t i = 1; i < count; i++ ) {
    ss << " " << (int)data[i];
  }
  dst = new char[ss.str().size() + 1];
  strncpy(dst, ss.str().c_str(), ss.str().size());
  dst[ss.str().size()] = 0;
  return dst;
}

template<typename T>
void XMLWriter::writeArrayToXML(xmlNodePtr node, xmlNodePtr field, ArrayData<T> * data, const char * type)
{
  char* dst = nullptr;
  if( data->hints & READABLE_HINT ) {
    dst = toString(data->data, data->count);
  }
  else {
    convert_to_base64(data->data, data->count, &dst);
  }
  field = xmlNewTextChild(node, nullptr, (const xmlChar*)"array", (xmlChar*)dst);
  free(dst);
  
  if( data->hints & READABLE_HINT ) {
    xmlNewProp(field, (const xmlChar*)"hints", (const xmlChar*)toString(READABLE_HINT).c_str());
  }
  xmlNewProp(field, (const xmlChar*)"type", (const xmlChar*)type);
  xmlNewProp(field, (const xmlChar*)"count", (const xmlChar*)toString(data->count).c_str());
}

#define WRITE_XML_ARRAY(x)\
else if( ArrayData<x>* data = dynamic_cast<ArrayData<x>*>(iter.second) ) { \
  writeArrayToXML(node, field, data, #x);\
}

void XMLWriter::addToXML(Object *obj, xmlNodePtr parent)
{
  xmlNodePtr node = xmlNewChild(parent, nullptr, (const xmlChar *)"object", nullptr);
  xmlNewProp(node, (const xmlChar*)"id", (const xmlChar*)toString(obj->id).c_str());
  xmlNewProp(node, (const xmlChar*)"class", (const xmlChar*)obj->name.c_str());
  
  for( auto iter : obj->data ) {
    xmlNodePtr field = nullptr;
    if( Data<uint8_t>* data = dynamic_cast<Data<uint8_t>*>(iter.second) ) {
      field = xmlNewChild(node, nullptr, (const xmlChar*)"uint8_t", (const xmlChar*)toString((int)data->data).c_str());
    }
    WRITE_XML(uint16_t)
    WRITE_XML(uint32_t)
    WRITE_XML(uint64_t)
    WRITE_XML(int8_t)
    WRITE_XML(int16_t)
    WRITE_XML(int32_t)
    WRITE_XML(int64_t)
    WRITE_XML(float)
    WRITE_XML(double)
    WRITE_XML(long double)
    else if( Data<bool>* data = dynamic_cast<Data<bool>*>(iter.second) ) {
      field = xmlNewChild(node, nullptr, (const xmlChar*)"bool", (const xmlChar*)toString((int)data->data).c_str());
    }
    else if( Data<const char*>* data = dynamic_cast<Data<const char*>*>(iter.second) ) {
      field = xmlNewTextChild(node, nullptr, (const xmlChar*)"char_star", (const xmlChar*)data->data);
    }
    else if( Data<Writable> * data = dynamic_cast<Data<Writable>*>(iter.second) ) {
      addToXML(data, node);
    }
    else if( Data<Writable*> * data = dynamic_cast<Data<Writable*>*>(iter.second) ) {
      field = xmlNewChild(node, nullptr, (const xmlChar*)"eos.serialization.Writable", nullptr);
      xmlNewProp(field, (const xmlChar*)"id", (const xmlChar*)toString(data->id).c_str());
    }
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
    WRITE_XML_ARRAY(long double)
    WRITE_XML_ARRAY(bool)
    else if( ArrayData<Writable*>* data = dynamic_cast<ArrayData<Writable*>*>(iter.second) ) {
      writeArrayToXML(node, field, data, "eos.serialization.Writable*");
    }
    
    xmlNewProp(field, (const xmlChar*)"name", (const xmlChar*)iter.first.c_str());
  }
}
