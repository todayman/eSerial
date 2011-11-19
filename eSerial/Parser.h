/*
 *  eos/serialization/Parser.h
 *
 *  Created by Paul O'Neil on 7/31/09.
 *  Copyright 2011 Paul O'Neil. All rights reserved.
 *
 */

#include <map>
#include <stack>

#ifndef __EOS_SERIALIZTION_PARSER_H__
#define __EOS_SERIALIZTION_PARSER_H__

namespace eos {
namespace serialization {

class Parser;
class Writable;

template<typename T> class Data;
typedef Data<Writable> Object;

typedef Writable * (*ctor_func_t)(/*EOSClass * data*/) ;

class ctor_block_t {
public:
  virtual ~ctor_block_t() { };
  virtual Writable * newObject() const = 0;
};

class Factory {
  std::map<std::string, ctor_block_t*> ctors;
public:
  void registerClass(const std::string& className, ctor_func_t ctor);
  void registerClass(const std::string& className, ctor_block_t* ctor);
  Writable * newObject(std::string className);
};

class Parser {
protected:
  std::map<size_t, Object*> data;
  virtual void firstPass(const char * filename) = 0;
  
private:
  std::map<size_t, Writable*> objects;
  Factory * factory;
  std::stack<Object*> objStack;
  Object * curObj;
  void secondPass();
  void parseObject(Object * curObj);
  
public:
  Parser() = default;
  virtual ~Parser();
  void parseFile(const char * filename);
  void setFactory(Factory * newFactory);
  
  template<typename T>
  void read(const char * name, T * val);
  
  template<typename T>
  void readArray(const char * name, T ** elements, size_t * count);
  
  static Parser * newXMLParser();
};

} // namespace serialization
} // namespace eos

#endif // __EOS_SERIALIZTION_PARSER_H__
