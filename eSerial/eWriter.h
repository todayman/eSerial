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
#include "eWritable.h"
#include "eData.h"

#ifndef __E_WRITER_H__
#define __E_WRITER_H__

class eWriter {
protected:
	std::map<eWritable*, size_t> idList;
	std::vector<EOSObject*> objs;
	EOSObject * curObj;
  size_t addObjectGetID(eWritable * object);

public:
  eWriter() : idList(), objs(), curObj(NULL) { }
	virtual void writeFile(const char * pathname)=0;
	void addObject(eWritable * object) {
    addObjectGetID(object);
  }
  void writeName(const char * name);
  
	/* write individual values */
  template<typename T>
  void write(T val, const char * name);
  
	/* write arrays of values */
  template<typename T>
  void writeArray(T * elements, size_t count, const char * name, hint_t hint = NO_HINT);
  
  // Get a writer
  static eWriter * newXMLWriter();
};

#endif // __E_WRITER_H__
