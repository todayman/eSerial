//
//  eos/serialization/Parser.h
//  eSerial
//
//	Created by Paul O'Neil on 7/31/09.
//  Copyright 2012 Paul O'Neil. All rights reserved.
//

#include <map>
#include <vector>
#include <stack>
#include <string>
#include <fstream>

#include "Data.h"

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
	~Factory();
  void registerClass(const std::string& className, ctor_func_t ctor);
  void registerClass(const std::string& className, ctor_block_t* ctor);
  Writable * newObject(std::string className);
};

class Parser {
protected:
	std::vector<Object*> root_objs;
  std::map<size_t, Object*> metaIDList;
  virtual void firstPass(std::istream& filename) = 0;
  
  std::map<size_t, Writable*> realIDList;
private:
  Factory * factory;
  Object * curObj;
  void secondPass();
  void parseObject(Object * curObj);
  
	template<typename T>
	void read_impl(const std::string& name, T * val);
	template<typename T>
	void read_array_impl(const std::string& name, T ** elements, size_t * count);
	
public:
  Parser() : root_objs(), metaIDList(), realIDList(), factory(nullptr), curObj(nullptr) { }
  virtual ~Parser();
	void parseStream(std::istream& input);
  void parseFile(const std::string& filename) {
		std::ifstream input(filename);
		parseStream(input);
	}
	Factory * getFactory() {
		if( nullptr == factory ) {
			factory = new Factory();
		}
		return factory;
	}
  void setFactory(Factory * newFactory);
  
  template<typename T>
  void read(const std::string& name, T * val) {
		static_assert( !std::is_pointer<T>::value || std::is_base_of<Writable, typename std::remove_pointer<T>::type >::value,
									"Only pointers to subclasses of eos::serialization::Writable may be written.");
		typedef typename is_base_type_thingy<std::is_scalar<T>::value, T>::readType implType;
		read_impl< implType >(name, reinterpret_cast<implType*>(val));
	}
  
  template<typename T>
  void readArray(const std::string& name, T ** elements, size_t * count) {
		static_assert( !std::is_pointer<T>::value || std::is_base_of<Writable, typename std::remove_pointer<T>::type >::value,
									"Only pointers to subclasses of eos::serialization::Writable may be written.");
		typedef typename is_base_type_thingy<std::is_scalar<T>::value, T>::theType implType;
		read_array_impl<implType>(name, reinterpret_cast<implType**>(elements), count);
	}
  
  static Parser * newXMLParser();
};

} // namespace serialization
} // namespace eos

#endif // __EOS_SERIALIZTION_PARSER_H__
