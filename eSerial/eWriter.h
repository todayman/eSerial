/*
 *  eWriter.h
 *  Elib
 *
 *  Created by Paul O'Neil on 4/10/09.
 *  Copyright 2011 Paul O'Neil. All rights reserved.
 *
 */

#include <vector>
#include <map>
#include <libxml/tree.h>
#include "eWritable.h"

#ifndef __E_WRITER_H__
#define __E_WRITER_H__

#ifndef __E_DATA_H__
class EOSObject;
#endif

class eWriter {
protected:
	std::map<eWritable*, size_t> idList;
	std::vector<EOSObject*> objs;
	EOSObject * curObj;
public:
  eWriter();
	virtual void writeFile(const char * pathname)=0;
	void addObject(eWritable * object);
	void writeName(const char * name);
  
	/* write individual values */
  template<typename T>
  void write(T val, const char * name);
  
	/* write arrays of values */
  template<typename T>
  void writeArray(T * elements, size_t count, const char * name);
};

class eXMLWriter : public eWriter {
  xmlDocPtr doc;
  xmlNodePtr tree;
  xmlNodePtr node;
  
  void addToXML(EOSObject * obj);
public:
  void writeFile(const char * filename);
};

#endif // __E_WRITER_H__
