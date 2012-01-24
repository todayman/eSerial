//
//  eos/serialization/eParser.cpp
// eSerial
//
//  Created by Paul O'Neil on 7/31/09.
//  Copyright 2012 Paul O'Neil. All rights reserved.
//

#include "Writable.h"
#include "Parser.h"
#include "Data.h"
#include "macros.h"
using namespace std;
using namespace eos::serialization;

Factory::~Factory() {
	for( auto ctor : ctors ) {
		delete ctor.second;
	}
}

class ctor_func_block : public ctor_block_t {
  ctor_func_t func;
public:
  ctor_func_block(ctor_func_t f) : func(f) { }
  virtual ~ctor_func_block() { }
  virtual Writable * newObject() const override {
    return func();
  }
};

void Factory::registerClass(const string &className, ctor_func_t ctor) {
  ctors.insert( make_pair(className, new ctor_func_block(ctor)) );
}

Writable * Factory::newObject(std::string className) {
  return /*dynamic_cast<eWritable*>(*/ctors[className]->newObject()/*)*/;
}

Parser::~Parser() {
  for( auto iter : metaIDList ) {
    delete iter.second;
  }
	delete factory;
}

void Parser::setFactory(eos::serialization::Factory *newFactory) {
  factory = newFactory;
}

void Parser::parseStream(istream& input)
{
	firstPass(input);
	secondPass();
}

// go through the the hierarchy of EOS objects, and convert them to real objects
void Parser::secondPass()
{
	for(auto iter : root_objs ) {
		if( !realIDList.count(iter->id) ) {
			parseObject(iter);
    }
	}
}

void Parser::parseObject(Object * c)
{
	Object * oldObj = curObj;
  curObj = c;
  Writable * obj = factory->newObject(curObj->name);
  realIDList.insert(make_pair(curObj->id, obj));
	obj->read(this);
  
	curObj = oldObj;
}

template<typename T>
void Parser::read_impl(const string& name, T * val)
{
  (*val) = (dynamic_cast<Data<T>*>(curObj->data[name]))->data;
}

#define READ(x) \
template void Parser::read_impl(const string& name, x * val);

PRIMITIVE_TYPES(READ)

template<>
void Parser::read_impl(const string& name, Writable * val) {
	Data<Writable>* newObj = dynamic_cast<Data<Writable>*> ( curObj->data[name] );
	if(!realIDList.count(newObj->id)) {
		realIDList.insert(make_pair(newObj->id, val));
		Object * oldObj = curObj;
		curObj = metaIDList[newObj->id];
		val->read(this);
		curObj = oldObj;
  }
}

template<>
void Parser::read_impl(const string& name, Writable ** val) {
	Data<Writable*>* newObj = dynamic_cast<Data<Writable*>*> ( curObj->data[name] );
	if(!realIDList.count(newObj->id)) {
		parseObject(metaIDList[newObj->id]);
  }
	(*val) = realIDList[newObj->id];
}

template<typename T>
void Parser::read_array_impl(const string& name, T ** elements, size_t * count) {
  (*elements) = (dynamic_cast<ArrayData<T>*>(curObj->data[name]))->data;
  if(count) (*count) = (dynamic_cast<ArrayData<T>*>(curObj->data[name]))->count;
}

#define READ_ARRAY(x)\
template void Parser::read_array_impl(const string& name, x ** elements, size_t * count);
PRIMITIVE_TYPES(READ_ARRAY)

template<>
void Parser::read_array_impl(const string& name, Writable *** elements, size_t * count)
{
	ArrayData<Writable*>* newObj = dynamic_cast<ArrayData<Writable*>*> ( curObj->data[name] );
	
  if(count)
		(*count) = newObj->count;
  
	(*elements) = new Writable*[newObj->count];
	for(size_t i = 0; i < newObj->count; ++i) {
		if(!realIDList.count(newObj->data[i])) {
			parseObject(metaIDList[newObj->data[i]]);
    }
		(*elements)[i] = realIDList[newObj->data[i]];
	}
}
