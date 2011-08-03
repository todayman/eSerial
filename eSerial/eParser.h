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
#include "eData.h"
#include "macros.h"

#ifndef __E_WRITER__
class eWriter;
#endif

class eParser;

#ifndef __E_PARSER__
#define __E_PARSER__

class eFactory {
  std::map<std::string, constructor_t> ctors;
public:
  void registerClass(const std::string& className, constructor_t ctor);
	eWritable * newObject(std::string className);
};

class eParser {
  std::ifstream input;
	
  std::string istring;
  std::string buff; // strings to read in random stuff
                    // instead of a different string in every method
                    // I got sick of declaring it every time
	void addObject();
	void parseClass(EOSClass * c);
	
protected:
  std::map<size_t, EOSObject*> data;
	virtual void firstPass(const char * filename)=0;
	
private:
  std::map<size_t, eWritable*>objects;
	eFactory * factory;
  std::stack<EOSClass*> classes;
	void secondPass();
	void parseObject(EOSObject * curObj);
	
public:
	void parseFile(const char * filename);
	void setFactory(eFactory * newFactory);
  
#define PARSE_TYPE( x )	void read(const char * name, x * val) {												\
       (*val) = (dynamic_cast<EOSData<x>*>(classes.top()->data[std::string(name)]))->data;	\
     }
  PRIMITIVE_TYPES(PARSE_TYPE)
#undef PARSE_TYPE
	void read(const char * name, eWritable ** val);
	
#define PARSE_ARRAY( x )	void readArray(const char * name, x ** elements, uint32_t * count = NULL) {									\
      (*elements) = (dynamic_cast<EOSArrayData<x>*>(classes.top()->data[std::string(name)]))->data;			\
      if(count) (*count) = (dynamic_cast<EOSArrayData<x>*>(classes.top()->data[std::string(name)]))->count;	\
    }
  PRIMITIVE_TYPES(PARSE_ARRAY)
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
  std::ifstream input;
  std::string istring;
  std::string buff;
	
	void addObject();
	void parseClass(EOSClass * c);
	
protected:
	virtual void firstPass(const char * filename);
};

#endif
