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
using namespace eos::serialization;

class ctor_func_block {
  ctor_func_t func;
public:
  ctor_func_block(ctor_func_t f) : func(f) { }
  virtual ~ctor_func_block() { }
  virtual Writable * newObject() const {
    return func();
  }
};

void Factory::registerClass(const string &className, ctor_func_t ctor) {
  ctors.insert( make_pair(className, (ctor_block_t*)new ctor_func_block(ctor)) );
}

Writable * Factory::newObject(std::string className) {
  return /*dynamic_cast<eWritable*>(*/ctors[className]->newObject()/*)*/;
}

Parser::~Parser() {
  for( auto iter : data ) {
    delete iter.second;
  }
}

void Parser::parseFile(const char * filename)
{
	firstPass(filename);
	secondPass();
}

// go through the the hierarchy of EOS objects, and convert them to real objects
void Parser::secondPass()
{
	auto end = data.end();
	for(auto iter = data.begin(); iter != end; iter ++ ) {
		if( !objects.count(iter->first) ) {
			parseObject(iter->second);
    }
	}
}

void Parser::parseObject(Object * c)
{
	if( curObj ) {
    objStack.push(curObj);
  }
  curObj = c;
  Writable * obj = factory->newObject(curObj->name);
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
void Parser::read(const char * name, T * val)
{
  (*val) = (dynamic_cast<Data<T>*>(curObj->data[string(name)]))->data;
}

#define READ(x) \
template void Parser::read(const char *, x *);

PRIMITIVE_TYPES(READ)

template<>
void Parser::read(const char * name, Writable ** val) {
	Data<Writable*>* newObj = dynamic_cast<Data<Writable*>*> ( curObj->data[string(name)] );
	if(!objects.count(newObj->i)) {
		parseObject(data[newObj->i]);
  }
	(*val) = objects[newObj->i];
}

template<typename T>
void Parser::readArray(const char * name, T ** elements, size_t * count) {
  (*elements) = (dynamic_cast<ArrayData<T>*>(curObj->data[std::string(name)]))->data;
  if(count) (*count) = (dynamic_cast<ArrayData<T>*>(curObj->data[std::string(name)]))->count;
}

#define READ_ARRAY(x)\
template void Parser::readArray(const char * name, x ** elements, size_t * count);
PRIMITIVE_TYPES(READ_ARRAY)

template<>
void Parser::readArray(const char * name, Writable *** elements, size_t * count)
{
	ArrayData<Writable*>* newObj = dynamic_cast<ArrayData<Writable*>*> ( curObj->data[string(name)] );
	
  if(count)
		(*count) = newObj->count;
  
	(*elements) = new Writable*[newObj->count];
	for(size_t i = 0; i < newObj->count; i++) {
		if(!objects[newObj->data[i]]) {
			parseObject(data[newObj->data[i]]);
    }
		(*elements)[i] = objects[newObj->data[i]];
	}
}
