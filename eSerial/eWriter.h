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

namespace eos {
namespace serialization {

class Writer {
protected:
	std::map<Writable*, size_t> idList;
	std::vector<Object*> objs;
	Object * curObj;
  size_t addObjectGetID(Writable * object);

public:
  Writer() : idList(), objs(), curObj(NULL) { }
	virtual void writeFile(const char * pathname)=0;
	void addObject(Writable * object) {
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
  static Writer * newXMLWriter();
};

} // namespace serialization
} // namespace eos
#endif // __E_WRITER_H__
