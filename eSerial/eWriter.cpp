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
using namespace std;
#include "eWriter.h"
#include "eData.h"
#include "macros.h"

size_t eWriter::addObjectGetID(eWritable * object)
{
  size_t id = objs.size();
	curObj = new EOSObject();
	curObj->i = id;
	objs.push_back(curObj);
	idList.insert(make_pair(object, id));
	object->write(this);
  curObj = NULL;
  return id;
}

void eWriter::writeName(const char * name)
{
  if( curObj->name.size() == 0 ) {
    curObj->name = string(name);
  }
}

template<typename T>
void eWriter::write( T val, const char * name ) {
  curObj->data.insert(make_pair(string(name), new EOSData< T >(val)));
}

#define WRITE(x) \
template void eWriter::write(x, const char *);

PRIMITIVE_TYPES(WRITE)
template void eWriter::write(const char*, const char *);

template<>
void eWriter::write(eWritable * object, const char * name)
{
	bool writeObj = false;
	if( !idList.count(object)  ) {
		idList.insert(make_pair(object, idList.size()));
		writeObj = true;
	}
	
	curObj->data.insert(make_pair(string(name), new EOSData<eWritable*>(idList[object])));
	
	if(writeObj) {
		EOSObject * oldObj = curObj;
		addObject(object);
		curObj = oldObj;
	}
}

template<typename T>
void eWriter::writeArray( T * val, size_t count, const char * name)
{
  curObj->data.insert(make_pair(string(name), new EOSArrayData<T>(count, val)));
}

#define WRITE_ARRAY(x) \
template void eWriter::writeArray(x *, size_t, const char *);

PRIMITIVE_TYPES(WRITE_ARRAY)
template<> void eWriter::writeArray(eWritable ** val, size_t count, const char * name) {
  EOSArrayData<eWritable*> * data = new EOSArrayData<eWritable*>(count, NULL);
  
  for( size_t i = 0; i < count; i++ ) {
    data->data[i] = addObjectGetID(val[i]);
  }
  
  curObj->data.insert(make_pair(string(name), data));
}
