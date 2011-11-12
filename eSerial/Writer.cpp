/*
 *  eos/serialization/Writer.cpp
 *
 *  Created by Paul O'Neil on 4/10/09.
 *  Copyright 2011 Paul O'Neil. All rights reserved.
 *
 */
#include <iostream>
#include <fstream>
using namespace std;
#include "Writer.h"
#include "Data.h"
#include "macros.h"
using namespace eos::serialization;

Writer::~Writer() {
  for( Object * o : objs ) {
    delete o;
  }
}

size_t Writer::addObjectGetID(Writable * object)
{
  size_t id = objs.size();
	curObj = new Object();
	curObj->i = id;
	objs.push_back(curObj);
	idList.insert(make_pair(object, id));
	object->write(this);
  curObj = nullptr;
  return id;
}

void Writer::writeName(const char * name)
{
  if( curObj->name.size() == 0 ) {
    curObj->name = string(name);
  }
}

template<typename T>
void Writer::write( T val, const char * name ) {
  curObj->data.insert(make_pair(string(name), new Data< T >(val)));
}

#define WRITE(x) \
template void Writer::write(x, const char *);

PRIMITIVE_TYPES(WRITE)
template void Writer::write(const char*, const char *);

template<>
void Writer::write(Writable * object, const char * name)
{
	bool writeObj = false;
	if( !idList.count(object)  ) {
		idList.insert(make_pair(object, idList.size()));
		writeObj = true;
	}
	
	curObj->data.insert(make_pair(string(name), new Data<Writable*>(idList[object])));
	
	if(writeObj) {
		Object * oldObj = curObj;
		addObject(object);
		curObj = oldObj;
	}
}

template<typename T>
void Writer::writeArray( T * val, size_t count, const char * name, hint_t hint)
{
  T * array = nullptr;
  if( hint & COPY_ARRAY_HINT ) {
    array = new T[count];
    memcpy(array, val, count*sizeof(T));
  }
  else {
    array = val;
  }
  curObj->data.insert(make_pair(string(name), new ArrayData<T>(count, array, hint)));
}

#define WRITE_ARRAY(x) \
template void Writer::writeArray(x *, size_t, const char *, hint_t);

PRIMITIVE_TYPES(WRITE_ARRAY)
template<> void Writer::writeArray(Writable ** val, size_t count, const char * name, hint_t hint) {
  ArrayData<Writable*> * data = new ArrayData<Writable*>(count, nullptr, hint);
  
  for( size_t i = 0; i < count; i++ ) {
    data->data[i] = addObjectGetID(val[i]);
  }
  
  curObj->data.insert(make_pair(string(name), data));
}
