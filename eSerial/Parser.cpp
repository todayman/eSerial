//
//  eos/serialization/eParser.cpp
//
//  Copyright 2012 Paul O'Neil.
//
//  This file is part of eSerial.
//
//  eSerial is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  eSerial is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with eSerial.  If not, see <http://www.gnu.org/licenses/>.
//

#include "Writable.h"
#include "Parser.h"
#include "Data.h"
#include "macros.h"
using namespace std;
using namespace eos::serialization;

DefaultFactory::~DefaultFactory() {
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

void DefaultFactory::registerClass(const string &className, ctor_func_t ctor) {
  ctors.insert( make_pair(className, new ctor_func_block(ctor)) );
}

Writable * DefaultFactory::newObject(const std::string& className) {
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
