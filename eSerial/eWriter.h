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
#include <string>
#include <sstream>
#include <stdint.h>
#include <libxml/tree.h>
#include "eWritable.h"
#include "eData.h"
#include "macros.h"

#ifndef __E_WRITER_H__
#define __E_WRITER_H__

class eWriter {
protected:
	std::map<eWritable*, size_t> idList;
	std::vector<EOSObject*> objs;
	EOSObject * curObj;
	EOSClass * curClass;
public:
  eWriter();
	virtual void writeFile(const char * pathname)=0;
	void addObject(eWritable * object);
	void finishObject();
	void writeName(const char * name);
  
	/* write individual values */
#define WRITE_METHOD( x ) void write( x val, const char * name );
  ALL_TYPES(WRITE_METHOD)
  WRITE_METHOD(char*)
#undef WRITE_METHOD
	
	/* write arrays of values */
#define WRITE_ARRAY( x ) void writeArray(x * elements, size_t count, const char * name);
  ALL_TYPES(WRITE_ARRAY)
#undef WRITE_ARRAY
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
