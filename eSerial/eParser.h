/*
 *  eParser.h
 *  space
 *
 *  Created by Paul O'Neil on 7/31/09.
 *  Copyright 2011 Paul O'Neil. All rights reserved.
 *
 */

#include <fstream>
#include <map>
#include <string>
#include <stack>
using namespace std;

#ifndef __E_WRITER__
class eWriter;
#endif

class eParser;

#ifndef __E_PARSER__
#define __E_PARSER__

class eFactory {
public:
	virtual eWritable * newObject(std::string className)=0;
};

class eParser {
	ifstream input;
	
	string istring;
	string buff; // strings to read in random stuff, instead of a different string in every method
	void addObject();
	void parseClass(EOSClass * c);
	
	protected:
	map<size_t, EOSObject*> data;
	virtual void firstPass(const char * filename)=0;
	
	private:
	map<size_t, eWritable*>objects;
	eFactory * factory;
	stack<EOSClass*> classes;
	void secondPass();
	void parseObject(EOSObject * curObj);
	
	public:
	void parseFile(const char * filename);
	void setFactory(eFactory * newFactory);


#define PARSE_TYPE( x )	void read(const char * name, x * val) {												\
							(*val) = (dynamic_cast<EOSData<x>*>(classes.top()->data[string(name)]))->data;	\
						}
	PARSE_TYPE( uint8_t )
	PARSE_TYPE( uint16_t )
	PARSE_TYPE( uint32_t )
	PARSE_TYPE( uint64_t )
	PARSE_TYPE( int8_t )
	PARSE_TYPE( int16_t )
	PARSE_TYPE( int32_t )
	PARSE_TYPE( int64_t )
	PARSE_TYPE( float )
	PARSE_TYPE( double )
	PARSE_TYPE( long double )
	PARSE_TYPE( bool )
	PARSE_TYPE( char* )
#undef PARSE_TYPE
	void read(const char * name, eWritable ** val);
	
#define PARSE_ARRAY( x )	void readArray(const char * name, x ** elements, uint32_t * count = NULL) {									\
								(*elements) = (dynamic_cast<EOSArrayData<x>*>(classes.top()->data[string(name)]))->data;			\
								if(count) (*count) = (dynamic_cast<EOSArrayData<x>*>(classes.top()->data[string(name)]))->count;	\
							}
	PARSE_ARRAY( uint8_t )
	PARSE_ARRAY( uint16_t )
	PARSE_ARRAY( uint32_t )
	PARSE_ARRAY( uint64_t )
	PARSE_ARRAY( int8_t )
	PARSE_ARRAY( int16_t )
	PARSE_ARRAY( int32_t )
	PARSE_ARRAY( int64_t )
	PARSE_ARRAY( float )
	PARSE_ARRAY( double )
	PARSE_ARRAY( long double )
	PARSE_ARRAY( bool )
#undef PARSE_ARRAY
	void readArray(const char * name, eWritable *** elements, uint32_t * count = NULL);
	
	void pushClass(const char * superclass) {
		classes.push(classes.top()->superclasses[superclass]);
	}
	void popClass() {
		classes.pop();
	}
};

#define READ_SUPERCLASS( reader, x )	reader->pushClass(#x);	\
										x::read(reader);		\
										reader->popClass()

class eTextParser : public eParser {
	ifstream input;
	string istring;
	string buff;
	
	void addObject();
	void parseClass(EOSClass * c);
	
	protected:
	virtual void firstPass(const char * filename);
};

#endif
