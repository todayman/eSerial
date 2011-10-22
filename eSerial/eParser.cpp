/*
 *  eParser.cpp
 *  space
 *
 *  Created by Paul O'Neil on 7/31/09.
 *  Copyright 2011 Paul O'Neil. All rights reserved.
 *
 */

#include "eWritable.h"
#include "eParser.h"
#include "eData.h"
#include "macros.h"
using namespace std;

class ctor_func_block {
  ctor_func_t func;
public:
  ctor_func_block(ctor_func_t f) : func(f) { }
  virtual eWritable * newObject() const {
    return func();
  }
};

void eFactory::registerClass(const string &className, ctor_func_t ctor) {
  ctors.insert( make_pair(className, (ctor_block_t*)new ctor_func_block(ctor)) );
}

eWritable * eFactory::newObject(std::string className) {
  return /*dynamic_cast<eWritable*>(*/ctors[className]->newObject()/*)*/;
}

void eParser::parseFile(const char * filename)
{
	firstPass(filename);
	secondPass();
}

// go through the the hierarchy of EOS objects, and convert them to real objects
void eParser::secondPass()
{
	auto end = data.end();
	for(auto iter = data.begin(); iter != end; iter ++ ) {
		if( !objects.count(iter->first) ) {
			parseObject(iter->second);
    }
	}
}

void eParser::parseObject(EOSObject * c)
{
	if( curObj ) {
    objStack.push(curObj);
  }
  curObj = c;
  eWritable * obj = factory->newObject(curObj->name);
  objects.insert(make_pair(curObj->i, obj));
	obj->read(this);
  
  if( !objStack.empty() ) {
    curObj = objStack.top();
    objStack.pop();
  }
  else {
    curObj = NULL;
  }
}

template<typename T>
void eParser::read(const char * name, T * val)
{
  (*val) = (dynamic_cast<EOSData<T>*>(curObj->data[string(name)]))->data;
}

#define READ(x) \
template void eParser::read(const char *, x *);

PRIMITIVE_TYPES(READ)

template<>
void eParser::read(const char * name, eWritable ** val) {
	EOSData<eWritable*>* newObj = dynamic_cast<EOSData<eWritable*>*> ( curObj->data[string(name)] );
	if(!objects.count(newObj->i)) {
		parseObject(data[newObj->i]);
  }
	(*val) = objects[newObj->i];
}

template<typename T>
void eParser::readArray(const char * name, T ** elements, size_t * count) {
  (*elements) = (dynamic_cast<EOSArrayData<T>*>(curObj->data[std::string(name)]))->data;
  if(count) (*count) = (dynamic_cast<EOSArrayData<T>*>(curObj->data[std::string(name)]))->count;
}

#define READ_ARRAY(x)\
template void eParser::readArray(const char * name, x ** elements, size_t * count);
PRIMITIVE_TYPES(READ_ARRAY)

template<>
void eParser::readArray(const char * name, eWritable *** elements, size_t * count)
{
	EOSArrayData<eWritable*>* newObj = dynamic_cast<EOSArrayData<eWritable*>*> ( curObj->data[string(name)] );
	if(count)
		(*count) = newObj->count;
	(*elements) = new eWritable*[newObj->count];
	for(uint32_t i=0; i<newObj->count; i++) {
		if(!objects[newObj->data[i]])
			parseObject(data[newObj->data[i]]);
		(*elements)[i] = objects[newObj->data[i]];
	}
}
