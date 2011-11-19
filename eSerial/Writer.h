/*
 *  eos/serialization/Writer.h
 *
 *  Created by Paul O'Neil on 4/10/09.
 *  Copyright 2011 Paul O'Neil. All rights reserved.
 *
 */

#include <vector>
#include <map>
#include <type_traits>
#include "Writable.h"
#include "Data.h"

#ifndef __EOS_SERIALIZATION_WRITER_H__
#define __EOS_SERIALIZATION_WRITER_H__

namespace eos {
namespace serialization {

class Writer {
protected:
	std::map<const Writable*, Object*> idList;
	std::vector<Object*> root_objs;
  size_t obj_count;
	Object * curObj;
  Object * newObject(const Writable * object);
  Object * addObjectGetID(const Writable * object);
  
  template<typename T>
  void write_impl(T val, const char * name);
  
  /* these structs are here to route
   instantiations of write() to the correct implementation */
  template<bool primitve, typename T>
  struct is_base_type_thingy {
  };
  
  template<typename T>
  struct is_base_type_thingy<false, T> {
    static_assert(std::is_base_of<Writable, T>::value,
                  "You must inherit from eos::serializable::Writable to be write an object.");
    typedef const Writable& theType;
  };
  
  template<typename T>
  struct is_base_type_thingy<true, T> {
    typedef T theType;
  };
  
public:
  Writer() : idList(), root_objs(), obj_count(0), curObj(nullptr) { }
  virtual ~Writer();
	virtual void writeFile(const char * pathname) = 0;
	void addObject(Writable * object) {
    addObjectGetID(object);
  }
  void writeName(const char * name);
  
	/* write individual values */
  template<typename T>
  void write(T val, const char * name) {
    static_assert( !is_pointer<T>::value || is_base_of<Writable, T>::value,
                  "Only pointers to subclasses of eos::serialization::Writable may be written.");
    write_impl< typename is_base_type_thingy<std::is_scalar<T>::value, T>::theType >(val, name);
  }
  
	/* write arrays of values */
  template<typename T>
  void writeArray(T * elements, size_t count, const char * name, hint_t hint = NO_HINT);
  
  // Get a writer
  static Writer * newXMLWriter();
};

template<> inline void Writer::write<const char*>(const char * val, const char * name) {
  write_impl<const char *>(val, name);
}

} // namespace serialization
} // namespace eos
#endif // __EOS_SERIALIZATION_WRITER_H__
