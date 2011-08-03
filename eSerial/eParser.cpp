/*
 *  eParser.cpp
 *  space
 *
 *  Created by Paul O'Neil on 7/31/09.
 *  Copyright 2011 Paul O'Neil. All rights reserved.
 *
 */

#include "eWriter.h"
#include "eParser.h"
using namespace std;

void eFactory::registerClass(const string &className, constructor_t ctor) {
  ctors.insert( pair<string, constructor_t>(className, ctor) );
}

eWritable * eFactory::newObject(std::string className) {
  return /*dynamic_cast<eWritable*>(*/ctors[className]()/*)*/;
}

void eParser::parseFile(const char * filename)
{
	firstPass(filename);
	secondPass();
}

// go through the the hierarchy of EOS objects, and convert them to real objects
void eParser::secondPass()
{
	map<size_t, EOSObject*>::iterator end = data.end();
	for(map<size_t, EOSObject*>::iterator iter = data.begin(); iter != end; iter ++ ) {
		if(!objects[iter->first])
			parseObject(iter->second);
	}
}

void eParser::parseObject(EOSObject * curObj)
{
	eWritable * obj = factory->newObject(curObj->data->name);
	objects[curObj->i] = obj;
	classes.push(curObj->data);
	obj->read(this);
	classes.pop();
}

void eParser::read(const char * name, eWritable ** val) {
	EOSData<eWritable*>* newObj = dynamic_cast<EOSData<eWritable*>*> ( classes.top()->data[string(name)] );
	if(!objects[newObj->i])
		parseObject(data[newObj->i]);
	(*val) = objects[newObj->i];
}

void eParser::readArray(const char * name, eWritable *** elements, uint32_t * count)
{
	EOSArrayData<eWritable*>* newObj = dynamic_cast<EOSArrayData<eWritable*>*> ( classes.top()->data[string(name)] );
	if(count)
		(*count) = newObj->count;
	(*elements) = new eWritable*[newObj->count];
	for(uint32_t i=0; i<newObj->count; i++) {
		if(!objects[newObj->data[i]])
			parseObject(data[newObj->data[i]]);
		(*elements)[i] = objects[newObj->data[i]];
	}
}

// convert a text based file to a tree of EOS objects
void eTextParser::firstPass(const char * filename) {
	input.open(filename);
	
	for(input >> istring; istring == "<object" && input.good(); input >> istring)
		addObject();
	input.close();
}

void eTextParser::addObject()
{
	EOSObject * obj = new EOSObject;
	input >> obj->i >> buff;
	obj->data = new EOSClass;
	parseClass(obj->data);
	data.insert(pair<size_t, EOSObject*>(obj->i, obj));
}

#define ELIF_TYPE( x )	else if(istring == typePrefix + #x) {							\
							ptr = new EOSData<x>;										\
							input >> buff >> ((EOSData<x>*)ptr)->data >> istring;		\
							c->data.insert(pair<string, pEOSData>(string(buff), ptr));	\
						}
#define ELIF_ARRAY( x )	else if(istring == #x) {																		\
							ptr = new EOSArrayData<x>;																	\
							input >> buff;																				\
							input >> ((EOSArrayData<x>*)ptr)->count;													\
							while(input.get() != '>') ;																	\
							((EOSArrayData<x>*)ptr)->data = new x [((EOSArrayData<x>*)ptr)->count];						\
							input.read((char*)((EOSArrayData<x>*)ptr)->data, ((EOSArrayData<x>*)ptr)->count*sizeof(x));	\
							input >> istring;																			\
							c->data.insert(pair<string, pEOSData>(string(buff), ptr));									\
						}

void eTextParser::parseClass(EOSClass * c)
{
	if(istring != "<class") {
		input >> istring;
		if(istring != "<class")
			throw 1;
	}
	
	input >> c->name >> buff;
	
	string typePrefix="<";
	
	for(input >> istring; istring != "</class>" && input.good(); input >> istring) {
		pEOSData ptr;
		uint32_t len;
		if(istring == "<uint8_t") {
			ptr = new EOSData<uint8_t>;
			input >> buff >> ((EOSData<int>*)ptr)->data >> istring;
			c->data.insert(pair<string, pEOSData>(string(buff), ptr));
		}
		ELIF_TYPE( uint16_t )
		ELIF_TYPE( uint32_t )
		ELIF_TYPE( uint64_t )
		else if(istring == "<int8_t") {
			ptr = new EOSData<int8_t>;
			input >> buff >> ((EOSData<int>*)ptr)->data >> istring;
			c->data.insert(pair<string, pEOSData>(string(buff), ptr));
		}
		ELIF_TYPE( int16_t )
		ELIF_TYPE( int32_t )
		ELIF_TYPE( int64_t )
		ELIF_TYPE( float )
		ELIF_TYPE( double )
		ELIF_TYPE( long double )
		ELIF_TYPE( bool )
		else if(istring == "<objectID") {
			ptr = new EOSData<eWritable*>;
			input >> buff >> ((EOSData<eWritable*>*)ptr)->i >> istring;
			c->data.insert(pair<string, pEOSData>(string(buff), ptr));
		}
		else if(istring == "<class") {
			EOSClass * nc = new EOSClass;
			parseClass(nc);
			c->superclasses.insert(pair<string, EOSClass*>(string(nc->name), nc));
		}
		else if(istring == "<string") {
			ptr = new EOSData<char*>;
			input >> buff >> len;
			((EOSData<char*>*)ptr)->data = new char[len+1];
			while(input.get() != '>');
			input.read(((EOSData<char*>*)ptr)->data, len);
			((EOSData<char*>*)ptr)->data[len] = 0;
			input >> istring;
			c->data.insert(pair<string, pEOSData>(string(buff), ptr));
		}
		else if(istring == "<array") {
			input >> istring;
			if(istring == "uint8_t") {
				ptr = new EOSArrayData<uint8_t>;
				input >> buff;
				input >> ((EOSArrayData<uint8_t>*)ptr)->count;
				while(input.get() != '>') ;
				((EOSArrayData<uint8_t>*)ptr)->data = new uint8_t [((EOSArrayData<uint8_t>*)ptr)->count];
				input.read((char*)((EOSArrayData<uint8_t>*)ptr)->data, ((EOSArrayData<uint8_t>*)ptr)->count*sizeof(uint8_t));
				input >> istring;
				c->data.insert(pair<string, pEOSData>(string(buff), ptr));
			}
			ELIF_ARRAY( uint16_t )
			ELIF_ARRAY( uint32_t )
			ELIF_ARRAY( uint64_t )
			ELIF_ARRAY( int8_t )
			ELIF_ARRAY( int16_t )
			ELIF_ARRAY( int32_t )
			ELIF_ARRAY( int64_t )
			ELIF_ARRAY( float )
			ELIF_ARRAY( double )
			ELIF_ARRAY( long double )
			ELIF_ARRAY( bool )
			else if(istring == "eWritable*") {
				ptr = new EOSArrayData<eWritable>;
				input >> buff;
				input >> ((EOSArrayData<eWritable>*)ptr)->count;
				while(input.get() != '>') ;
				((EOSArrayData<eWritable*>*)ptr)->data = new uint32_t [((EOSArrayData<eWritable*>*)ptr)->count];
				input.read((char*)((EOSArrayData<eWritable*>*)ptr)->data, ((EOSArrayData<eWritable*>*)ptr)->count*sizeof(uint8_t));
				input >> istring;
				c->data.insert(pair<string, pEOSData>(string(buff), ptr));
			}
		}
	}
}
