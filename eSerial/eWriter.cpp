/*
 *  eWriter.cpp
 *  Elib
 *
 *  Created by Paul O'Neil on 4/10/09.
 *  Copyright 2011 Paul O'Neil. All rights reserved.
 *
 */
#include <iostream>
#include <fstream>
#include "eWriter.h"
using namespace std;

eWriter::eWriter() :
  idList(), objs(), curObj(NULL), curClass(NULL)
{

}

void eWriter::addObject(eWritable * object)
{
	curObj = new EOSObject;
	curObj->i = objs.size();
	objs.push_back(curObj);
	idList[object] = curObj->i;
	curClass = NULL;
	object->write(this);
}

void eWriter::write(eWritable * object, const char * name)
{
	bool writeObj = false;
	if(!idList[object]) {
		idList[object] = idList.size();
		writeObj = true;
	}
	
	curClass->data[string(name)] = new EOSData<eWritable*>(idList[object]);
	
	if(writeObj) {
		EOSObject * oldObj = curObj;
		EOSClass * oldClass = curClass;
		addObject(object);
		curObj = oldObj;
		curClass = oldClass;
	}
}

void eWriter::writeName(const char * name)
{
	EOSClass * newClass = new EOSClass();
	newClass->name = string(name);
	if(curClass) {
		curClass->superclasses[string(name)] = newClass;
  }
	else {
		curObj->data = newClass;
    curClass = newClass;
  }
}

#define WRITE_METHOD( x )	void eWriter::write( x val, const char * name ) { curClass->data.insert(pair<string, pEOSData>(string(name), new EOSData< x >(val))); }
WRITE_METHOD( uint8_t )
WRITE_METHOD( uint16_t )
WRITE_METHOD( uint32_t )
WRITE_METHOD( uint64_t )
WRITE_METHOD( int8_t )
WRITE_METHOD( int16_t )
WRITE_METHOD( int32_t )
WRITE_METHOD( int64_t )
WRITE_METHOD( float )
WRITE_METHOD( double )
WRITE_METHOD( long double )
WRITE_METHOD( bool )
WRITE_METHOD( char* )

#define WRITE_ARRAY_METHOD(x) void eWriter::writeArray( x * val, size_t count, const char * name) \
{\
  curClass->data.insert(pair<string, EOSArrayData<x>*>(string(name), new EOSArrayData<x>(count, val))); \
}
WRITE_ARRAY_METHOD( uint16_t )
WRITE_ARRAY_METHOD( uint32_t )
WRITE_ARRAY_METHOD( uint64_t )
WRITE_ARRAY_METHOD( int8_t )
WRITE_ARRAY_METHOD( int16_t )
WRITE_ARRAY_METHOD( int32_t )
WRITE_ARRAY_METHOD( int64_t )
WRITE_ARRAY_METHOD( float )
WRITE_ARRAY_METHOD( double )
WRITE_ARRAY_METHOD( long double )
WRITE_ARRAY_METHOD( bool )

/* XML Writing */

#include <sstream>
#include "b64.h"

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
  xmlNewProp(node, (const xmlChar*)"class", (const xmlChar*)obj->data->name.c_str());
  
  for( auto iter : obj->data->data ) {
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
    else if( EOSData<char*>* data = dynamic_cast<EOSData<char*>*>(iter.second) ) {
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
