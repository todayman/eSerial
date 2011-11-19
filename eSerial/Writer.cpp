/*
 *  eos/serialization/Writer.cpp
 *
 *  Created by Paul O'Neil on 4/10/09.
 *  Copyright 2011 Paul O'Neil. All rights reserved.
 *
 */
#include "Writer.h"
#include "Data.h"
#include "macros.h"
using namespace std;
using namespace eos::serialization;

Writer::~Writer() {
  for( Object * o : root_objs ) {
    delete o;
  }
}

Object * Writer::newObject(const Writable * object) {
  size_t id = obj_count;
	Object *obj = new Object();
  obj_count += 1;
	obj->id = id;
	idList.insert(make_pair(object, obj));
  return obj;
}

Object * Writer::addObjectGetID(const Writable * object)
{
  Object * meta = newObject(object);
  curObj = meta;
	root_objs.push_back(curObj);
	object->write(this);
  curObj = nullptr;
  return meta;
}

void Writer::writeName(const string& name)
{
  if( curObj->name.size() == 0 ) {
    curObj->name = name;
  }
}

template<typename T>
void Writer::write_impl( T val, const string& name ) {
  curObj->data.insert(make_pair(string(name), new Data< T >(val)));
}

#define SPECIALIZATION(x) \
template void Writer::write_impl( x val, const string& name);
PRIMITIVE_TYPES(SPECIALIZATION)
#undef SPECIALIZATION
template void Writer::write_impl(const char* val, const string& name);

template<> void Writer::write_impl(const Writable& val, const string& name) {
  Object * oldObj = curObj;
  curObj = newObject(&val);
  val.write(this);
  oldObj->data.insert(make_pair(name, curObj));
  curObj = oldObj;
}

template<typename T>
void Writer::writeArray( T * val, size_t count, const string& name, hint_t hint)
{
  T * array = nullptr;
  if( hint & COPY_ARRAY_HINT ) {
    array = new T[count];
    memcpy(array, val, count*sizeof(T));
  }
  else {
    array = val;
  }
  curObj->data.insert(make_pair(name, new ArrayData<T>(count, array, hint)));
}

#define WRITE_ARRAY(x) \
template void Writer::writeArray(x * elements, size_t count, const string& name, hint_t hints);

PRIMITIVE_TYPES(WRITE_ARRAY)
template<> void Writer::writeArray(Writable ** val, size_t count, const string& name, hint_t hint) {
  ArrayData<Writable*> * data = new ArrayData<Writable*>(count, nullptr, hint);
  
  for( size_t i = 0; i < count; i++ ) {
    data->data[i] = addObjectGetID(val[i])->id;
  }
  
  curObj->data.insert(make_pair(name, data));
}
