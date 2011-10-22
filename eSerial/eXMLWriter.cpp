//
//  eXMLWriter.cpp
//  eSerial
//
//  Created by Paul O'Neil on 10/21/11.
//  Copyright (c) 2011 Paul O'Neil. All rights reserved.
//

#include <sstream>
using namespace std;
#include <libxml/tree.h>
#include "b64.h"
#include "eData.h"
#include "eWriter.h"

class eXMLWriter : public eWriter {
  xmlDocPtr doc;
  xmlNodePtr tree;
  xmlNodePtr node;
  
  void addToXML(EOSObject * obj);
public:
  void writeFile(const char * filename);
};

eWriter * eWriter::newXMLWriter() {
  return new eXMLWriter();
}

template <typename T>
static inline std::string toString (const T & t)
{
  stringstream ss;
  ss << t;
  return ss.str();
}

void eXMLWriter::writeFile(const char * filename)
{
  doc = xmlNewDoc((const xmlChar *)"1.0");
  doc->encoding = xmlStrdup((const xmlChar *)"UTF-8");
  doc->xmlRootNode = xmlNewDocNode(doc, NULL, (const xmlChar *)"eSerial", NULL);
  
  tree = doc->xmlRootNode;
  
  for( auto iter = objs.begin(); iter != objs.end(); iter++ ) {
    addToXML(*iter);
  }
  
  xmlSaveFormatFileEnc(filename, doc, "UTF-8", 1);
  xmlFreeDoc(doc);
}


#define WRITE_XML(x)\
else if( EOSData<x>* data = dynamic_cast<EOSData<x>*>(iter.second) ) { \
  field = xmlNewChild(node, NULL, (const xmlChar*)#x, (const xmlChar*)toString(data->data).c_str()); \
}

#define WRITE_XML_ARRAY(x)\
else if( EOSArrayData<x>* data = dynamic_cast<EOSArrayData<x>*>(iter.second) ) { \
  char* dst; \
  convert_to_base64(data->data, data->count, &dst); \
  field = xmlNewTextChild(node, NULL, (const xmlChar*)"array", (xmlChar*)dst); \
  free(dst); \
  xmlNewProp(field, (const xmlChar*)"type", (const xmlChar*)#x); \
  xmlNewProp(field, (const xmlChar*)"count", (const xmlChar*)toString(data->count).c_str()); \
}

void eXMLWriter::addToXML(EOSObject *obj)
{
  node = xmlNewChild(tree, NULL, (const xmlChar *)"object", NULL);
  xmlNewProp(node, (const xmlChar*)"id", (const xmlChar*)toString(obj->i).c_str());
  xmlNewProp(node, (const xmlChar*)"class", (const xmlChar*)obj->name.c_str());
  
  for( auto iter : obj->data ) {
    xmlNodePtr field = NULL;
    if( EOSData<uint8_t>* data = dynamic_cast<EOSData<uint8_t>*>(iter.second) ) {
      field = xmlNewChild(node, NULL, (const xmlChar*)"uint8_t", (const xmlChar*)toString((int)data->data).c_str());
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
    else if( EOSData<bool>* data = dynamic_cast<EOSData<bool>*>(iter.second) ) {
      field = xmlNewChild(node, NULL, (const xmlChar*)"bool", (const xmlChar*)toString((int)data->data).c_str());
    }
    else if( EOSData<const char*>* data = dynamic_cast<EOSData<const char*>*>(iter.second) ) {
      field = xmlNewTextChild(node, NULL, (const xmlChar*)"string", (const xmlChar*)data->data);
    }
    else if( EOSData<eWritable*> * data = dynamic_cast<EOSData<eWritable*>*>(iter.second) ) {
      field = xmlNewChild(node, NULL, (const xmlChar*)"eWritable", NULL);
      xmlNewProp(field, (const xmlChar*)"id", (const xmlChar*)toString(data->i).c_str());
    }
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
    
    xmlNewProp(field, (const xmlChar*)"name", (const xmlChar*)iter.first.c_str());
  }
}
