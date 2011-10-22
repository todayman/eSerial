/*
 *  eParser.h
 *  space
 *
 *  Created by Paul O'Neil on 7/31/09.
 *  Copyright 2011 Paul O'Neil. All rights reserved.
 *
 */

#include <map>
#include <stack>

class eWriter;
class eParser;
class eWritable;
class EOSObject;

#ifndef __E_PARSER__
#define __E_PARSER__

typedef eWritable * (*ctor_func_t)(/*EOSClass * data*/) ;

class ctor_block_t {
public:
  virtual eWritable * newObject() const = 0;
};

class eFactory {
  std::map<std::string, ctor_block_t*> ctors;
public:
  void registerClass(const std::string& className, ctor_func_t ctor);
  void registerClass(const std::string& className, ctor_block_t* ctor);
	eWritable * newObject(std::string className);
};

class eParser {
protected:
  std::map<size_t, EOSObject*> data;
	virtual void firstPass(const char * filename) = 0;
	
private:
  std::map<size_t, eWritable*> objects;
	eFactory * factory;
  std::stack<EOSObject*> objStack;
  EOSObject * curObj;
	void secondPass();
	void parseObject(EOSObject * curObj);
	
public:
	void parseFile(const char * filename);
	void setFactory(eFactory * newFactory);
  
  template<typename T>
  void read(const char * name, T * val);
	
  template<typename T>
  void readArray(const char * name, T ** elements, size_t * count);
  
  static eParser * newXMLParser();
};

#endif
