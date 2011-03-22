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
#include "eWriter.h"
using namespace std;

void eWriter::addObject(eWritable * object)
{
	curObj = new EOSObject;
	curObj->i = objs.size();
	objs.push_back(curObj);
	idList[object] = curObj->i;
	curClass = NULL;
	object->write(this);
}

void eWriter::write(eWritable * object, const char * name)
{
	bool writeObj = false;
	if(!idList[object]) {
		idList[object] = idList.size();
		writeObj = true;
	}
	
	curClass->data[string(name)] = new EOSData<eWritable*>(idList[object]);
	
	if(writeObj) {
		EOSObject * oldObj = curObj;
		EOSClass * oldClass = curClass;
		addObject(object);
		curObj = oldObj;
		curClass = oldClass;
	}
}

void eWriter::writeName(const char * name)
{
	EOSClass * newClass = new EOSClass;
	newClass->name = string(name);
	if(curClass)
		curClass->superclasses[string(name)] = newClass;
	else
		curObj->data = newClass;
}

#define WRITE_METHOD( x )	void eWriter::write( x val, const char * name ) { curClass->data[string(name)] = new EOSData< x >(val); }
WRITE_METHOD( uint8_t )
WRITE_METHOD( uint16_t )
WRITE_METHOD( uint32_t )
WRITE_METHOD( uint64_t )
WRITE_METHOD( int8_t )
WRITE_METHOD( int16_t )
WRITE_METHOD( int32_t )
WRITE_METHOD( int64_t )
WRITE_METHOD( float )
WRITE_METHOD( double )
WRITE_METHOD( long double )
WRITE_METHOD( bool )

/* Text writing */
#define INDENT                          for(uint8_t i=0; i<indentLevel; i++) str << '\t';

void eTextWriter::writeFile(const char * pathname)
{
	ofstream output(pathname);
	for(map<eWritable*,string>::iterator iter=contents.begin();	iter != contents.end();	iter ++)
		output << (*iter).second << endl;
	output.close();
}

void eTextWriter::write(eWritable * object, const char * name)
{
	bool writeObj = false;
	if(!idList[object]) {
		idList[object] = idList.size();
		writeObj = true;
	}
	INDENT;
	str << "<objectID " << name << " " << idList[object] << " />\n";
	if(writeObj) {
		string astring = str.str();
		str.str(string(""));
		uint32_t oldLevel = indentLevel;
		addObject(object);
		str.str(astring);
		str.seekp(0, ios::end);
		indentLevel = oldLevel;
	}
}

void eTextWriter::writeID(eWritable * object)
{
	bool writeObj = false;
	if(!idList[object]) {
		idList[object] = idList.size();
		writeObj = true;
	}
	INDENT;
	str.write((char*)&idList[object], sizeof(uint32_t));
	if(writeObj) {
		string astring = str.str();
		str.str(string(""));
		uint32_t oldLevel = indentLevel;
		addObject(object);
		str.str(astring);
		str.seekp(0, ios::end);
		indentLevel = oldLevel;
	}
}

void eTextWriter::addObject(eWritable * object)
{
	if(!idList[object]) {
		idList[object] = idList.size();
	}

	indentLevel=1;
	str << "<object " << idList[object] << " >\n";
	object->write(this);
	str << "</object>\n";
	contents[object] = str.str();
}

void eTextWriter::finishObject()
{
	indentLevel--;
	INDENT;
	str << "</class>\n";
}

void eTextWriter::writeName(const char * name)
{
	INDENT;
	str << "<class " << name << " >\n";
	indentLevel++;
}

#define WRITE_TEXT_METHOD( x )	void eTextWriter::write( x val, const char * name ) { INDENT; str << "<" << #x << " " << name << " " << val << " />\n"; }
void eTextWriter::write( uint8_t val, const char * name ) { INDENT; str << "<uint8_t " << name << " " << (int)val << " />\n"; }
WRITE_TEXT_METHOD(uint16_t)
WRITE_TEXT_METHOD(uint32_t)
WRITE_TEXT_METHOD(uint64_t)
void eTextWriter::write( int8_t val, const char * name ) { INDENT; str << "<int8_t " << name << " " << (int)val << " />\n"; }
WRITE_TEXT_METHOD(int16_t)
WRITE_TEXT_METHOD(int32_t)
WRITE_TEXT_METHOD(int64_t)
WRITE_TEXT_METHOD(float)
WRITE_TEXT_METHOD(double)
WRITE_TEXT_METHOD(long double)
WRITE_TEXT_METHOD(bool)

void eTextWriter::write(const char * astring, const char * name)
{
	INDENT;
	str << "<string " << name << " " << strlen(astring) << ">" << astring << "</string>\n";
}

#define WRITE_TEXT_ARRAY( x )	void eTextWriter::writeArray ( x * elements, uint32_t count, const char * name) {		\
									INDENT;														\
									str << "<array " << name << " " << #x << " " << count << " >";				\
									str.write((char*)elements, count*sizeof(x));				\
									str << "</array>\n";										\
								}

WRITE_TEXT_ARRAY(uint8_t)
WRITE_TEXT_ARRAY(uint16_t)
WRITE_TEXT_ARRAY(uint32_t)
WRITE_TEXT_ARRAY(uint64_t)
WRITE_TEXT_ARRAY(int8_t)
WRITE_TEXT_ARRAY(int16_t)
WRITE_TEXT_ARRAY(int32_t)
WRITE_TEXT_ARRAY(int64_t)
WRITE_TEXT_ARRAY(float)
WRITE_TEXT_ARRAY(double)
WRITE_TEXT_ARRAY(long double)
WRITE_TEXT_ARRAY(bool)
void eTextWriter::writeArray ( eWritable ** elements, uint32_t count, const char * name) {
	INDENT;
	str << "<array " << name << " eWritable* " << count << " >";
	for(uint32_t i=0; i<count; i++)
		writeID(elements[i]);
	str << "</array>\n";
}
