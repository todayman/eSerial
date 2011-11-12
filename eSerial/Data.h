//
//  eData.h
//  eSerial
//
//  Created by Paul O'Neil on 3/21/11.
//  Copyright 2011 Paul O'Neil. All rights reserved.
//

#include <map>
#include <string>

#include <hints.h>

#ifndef __E_DATA_H__
#define __E_DATA_H__

namespace eos {
namespace serialization {

class Writable;

class _Data {
public:
  virtual ~_Data() { }
};
typedef _Data * pData;

template<typename T> class Data : public _Data {
public:
	Data(T d = nullptr) { data = d; }
	T data;
};

template<> class Data<Writable*> : public _Data {
public:
	Data(size_t ident = 0) { i = ident; }
	
	size_t i;
};

template<typename T> class ArrayData : public _Data {
public:
	size_t count;
	T * data;
  hint_t hints;
  
  ArrayData() : count(0), data(nullptr), hints(NO_HINT) { }
  ArrayData(size_t c, T * d, hint_t h) : count(c), data(d), hints(h) { }
  ~ArrayData() {
    if( hints & COPY_ARRAY_HINT ) {
      delete[] data;
    }
  }
};

template<> class ArrayData<Writable*> : public _Data {
public:
	size_t count;
	size_t * data;
  hint_t hints;
  
  ArrayData() : count(0), data(nullptr), hints(NO_HINT) { }
  ArrayData(size_t c, size_t * d, hint_t h) : count(c), data(d), hints(h) { }
  ~ArrayData() {
    delete [] data;
  }
};

struct Object {
	size_t i;
  std::string name;
	std::map<std::string, pData> data;
};

} // namespace serialization
} // namespace eos

#endif // __E_DATA_H__
