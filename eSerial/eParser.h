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
#include <libxml/tree.h>
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
	
protected:
  std::map<size_t, EOSObject*> data;
	virtual void firstPass(const char * filename)=0;
	
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
};

class eXMLParser : public eParser
{
private:
  xmlDocPtr doc;
  xmlNodePtr root;
  xmlNodePtr node;
  
  void parseXMLObject(xmlNodePtr node);
  void parseXMLField(xmlNodePtr field, EOSObject * obj);
protected:
  virtual void firstPass(const char * filename);
  
public:
  eXMLParser();
};
#endif
