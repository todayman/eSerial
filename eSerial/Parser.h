//
//  eos/serialization/Parser.h
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
public:
	virtual ~Factory() { }
  virtual Writable * newObject(const std::string& className) = 0;
};

class DefaultFactory : public Factory {
	std::map<std::string, ctor_block_t*> ctors;
public:
	virtual ~DefaultFactory();
	void registerClass(const std::string& className, ctor_func_t ctor);
	void registerClass(const std::string& className, ctor_block_t* ctor);
	virtual Writable * newObject(const std::string& className) override;
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
			factory = new DefaultFactory();
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
