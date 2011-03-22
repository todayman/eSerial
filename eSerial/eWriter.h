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
#include <string>
#include <sstream>
#include <stdint.h>

class eWriter;

#ifndef __E_PARSER__
class eParser;
#endif

#ifndef __E_WRITEABLE__
#define __E_WRITEABLE__
class eWritable {
	public:
	virtual void write(eWriter * writer){}
	virtual void read(eParser * reader){}
};

class _EOSData {
protected:
	virtual size_t getData()=0;// to enable polymorphism
};
typedef _EOSData * pEOSData;

template<typename T> class EOSData : public _EOSData {
protected:
	virtual size_t getData() { return 0; } // to enable polymorphism
public:
	EOSData(T d=NULL) { data = d; }
	T data;
};

template<> class EOSData<eWritable*> : public _EOSData {
protected:
	virtual size_t getData() { return i; } // to enable polymorphism
public:
	EOSData(size_t ident=0) {i = ident; }
	
	size_t i;
};

template<typename T> class EOSArrayData : public _EOSData {
protected:
	virtual size_t getData() { return 0; } // to enable polymorphism
public:
	uint32_t count;
	T * data;
};

template<> class EOSArrayData<eWritable*> : public _EOSData {
protected:
	virtual size_t getData() { return 0; } // to enable polymorphism
public:
	uint32_t count;
	uint32_t * data;
};

struct EOSClass {
	std::string name;
	std::map<std::string, pEOSData> data;
	std::map<std::string, EOSClass*> superclasses;
};

struct EOSObject {
	size_t i;
	EOSClass * data;
};

#endif

#ifndef __E_WRITER__
#define __E_WRITER__

class eWriter {
	protected:
	std::map<eWritable*, size_t> idList;
	std::vector<EOSObject*> objs;
	EOSObject * curObj;
	EOSClass * curClass;
	public:
	virtual void writeFile(const char * pathname)=0;
	void addObject(eWritable * object);
	void finishObject();
	void writeName(const char * name);

	/* write individual values */
#define WRITE_METHOD( x ) void write( x val, const char * name );
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
	WRITE_METHOD( eWritable* )
#undef WRITE_METHOD
	
	/* write arrays of values */
#define WRITE_ARRAY( x ) void writeArray(x * elements, uint32_t count, const char * name);
	WRITE_ARRAY( uint8_t )
	WRITE_ARRAY( uint16_t )
	WRITE_ARRAY( uint32_t )
	WRITE_ARRAY( uint64_t )
	WRITE_ARRAY( int8_t )
	WRITE_ARRAY( int16_t )
	WRITE_ARRAY( int32_t )
	WRITE_ARRAY( int64_t )
	WRITE_ARRAY( float )
	WRITE_ARRAY( double )
	WRITE_ARRAY( long double )
	WRITE_ARRAY( bool )
	WRITE_ARRAY( eWritable* )
#undef WRITE_ARRAY
	
	virtual void write(const char * astring, const char * name)=0;
};
	
class eTextWriter : public eWriter {
	std::map<eWritable*,std::string> contents;
	std::stringstream str;
	uint32_t indentLevel;
	
	void writeID(eWritable * element);
	
public:
	eTextWriter() { str.clear(); }
	
	void writeFile(const char * pathname);
	void addObject(eWritable * object);
	void finishObject();
	void writeName(const char * name);
	
	/* write individual values */
#define WRITE_METHOD( x ) virtual void write( x val, const char * name );
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
	WRITE_METHOD( eWritable* )
#undef WRITE_METHOD
	
	/* write arrays of values */
#define WRITE_ARRAY( x ) virtual void writeArray(x * elements, uint32_t count, const char * name);
	WRITE_ARRAY( uint8_t )
	WRITE_ARRAY( uint16_t )
	WRITE_ARRAY( uint32_t )
	WRITE_ARRAY( uint64_t )
	WRITE_ARRAY( int8_t )
	WRITE_ARRAY( int16_t )
	WRITE_ARRAY( int32_t )
	WRITE_ARRAY( int64_t )
	WRITE_ARRAY( float )
	WRITE_ARRAY( double )
	WRITE_ARRAY( long double )
	WRITE_ARRAY( bool )
	WRITE_ARRAY( eWritable* )
#undef WRITE_ARRAY
	
	void write(const char * astring, const char * name);
};

#endif

#include "eParser.h"