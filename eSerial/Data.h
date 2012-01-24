//
//  eos/serialization/Data.h
//
//  Created by Paul O'Neil on 3/21/11.
//  Copyright 2011 Paul O'Neil. All rights reserved.
//

#include <map>
#include <string>
#include <cstdlib>

#include <eSerial/hints.h>

#ifndef __EOS_SERIALIZATION_DATA_H__
#define __EOS_SERIALIZATION_DATA_H__

namespace eos {
namespace serialization {

class Writable;

class _Data {
public:
  virtual ~_Data() { }
	virtual bool operator==(const _Data& other) const = 0;
	virtual bool shouldBeDeletedByOwner() const = 0;
};
typedef _Data * pData;

template<typename T> class Data : public _Data {
public:
	Data(T d = nullptr) : data(d) {}
	T data;
	
	virtual bool operator==(const _Data& other) const {
		try {
			const Data<T>& typed_data = dynamic_cast<const Data<T>&>(other);
			return this->data == typed_data.data;
		}
		catch(const std::bad_cast& e) {
			return false;
		}
	}
	virtual bool shouldBeDeletedByOwner() const override {
		return true;
	}
};

template<> class Data<Writable*> : public _Data {
public:
	Data(size_t ident = 0) : id(ident) { }
	
	size_t id;
	
	virtual bool operator==(const _Data& other) const {
		try {
			const Data<Writable*>& typed_data = dynamic_cast<const Data<Writable*>&>(other);
			return this->id == typed_data.id;
		}
		catch(const std::bad_cast& e) {
			return false;
		}
	}
	
	virtual bool shouldBeDeletedByOwner() const override {
		return true;
	}
};

template<typename T> class ArrayData : public _Data {
public:
	size_t count;
	T * data;
  hint_t hints;
  
  ArrayData() : count(0), data(nullptr), hints(NO_HINT) { }
  ArrayData(size_t c, T * d, hint_t h) : count(c), data(d), hints(h) { }
  ~ArrayData() {
    if( count && (hints & COPY_ARRAY_HINT) ) {
      if( hints & FREE_HINT ) {
        free(data);
      }
      else {
        delete[] data;
      }
    }
  }
	
	virtual bool operator==(const _Data& other) const {
		try {
			const ArrayData<T>& typed_data = dynamic_cast<const ArrayData<T>&>(other);
			if( this->hints == typed_data.hints && this->count == typed_data.count ) {
				return memcmp(this->data, typed_data.data, count * sizeof(T)) == 0;
			}
			return false;
		}
		catch(const std::bad_cast& e) {
			return false;
		}
	}
	
	virtual bool shouldBeDeletedByOwner() const override {
		return true;
	}
};

template<> class ArrayData<Writable*> : public _Data {
public:
	size_t count;
	size_t * data;
  hint_t hints;
  
  ArrayData() : count(0), data(nullptr), hints(NO_HINT) { }
	ArrayData(size_t c, hint_t h) : count(c), data(new size_t[count]), hints(h) { }
  ~ArrayData() {
		if( nullptr != data ) {
			delete [] data;
		}
  }
	
	virtual bool operator==(const _Data& other) const {
		try {
			const ArrayData<Writable*>& typed_data = dynamic_cast<const ArrayData<Writable*>&>(other);
			if( this->hints == typed_data.hints && this->count == typed_data.count ) {
				return memcmp(this->data, typed_data.data, count * sizeof(size_t)) == 0;
			}
			return false;
		}
		catch(const std::bad_cast& e) {
			return false;
		}
	}
	
	virtual bool shouldBeDeletedByOwner() const override {
		return true;
	}
};

template<> class Data<Writable> : public _Data {
public:
  size_t id;
  std::string name;
  std::map<std::string, pData> data;
	
	Data() : id(static_cast<size_t>(-1)), name(), data() { }
	virtual ~Data() {
		for( auto kv_pair : data ) {
			if( kv_pair.second->shouldBeDeletedByOwner() ) {
				delete kv_pair.second;
			}
		}
	}
	
	virtual bool operator==(const _Data& other) const noexcept {
		try {
			const Data<Writable>& typed_data = dynamic_cast<const Data<Writable>&>(other);
			return this->id == typed_data.id
					&& this->name == typed_data.name
					&& this->data == typed_data.data;
		}
		catch(const std::bad_cast& e) {
			return false;
		}
	}
	
	virtual bool shouldBeDeletedByOwner() const override {
		return false;
	}
};

typedef Data<Writable> Object;
	
// these structs are here to route instantiations of Writer.write()
// and Parser.read() to the correct implementation
template<bool ptr_type, typename T>
struct handle_pointer { };

template<typename T>
struct handle_pointer<false, T> {
	typedef T theType;
	typedef T readType;
};

template<typename T>
struct handle_pointer<true, T> {
	static_assert(std::is_base_of<Writable, typename std::remove_pointer<T>::type >::value,
								"You must inherit from eos::serializable::Writable to be write an object.");
	typedef Writable* theType;
	typedef Writable* readType;
};

template<bool primitve, typename T>
struct is_base_type_thingy {
};

template<typename T>
struct is_base_type_thingy<false, T> {
	static_assert(std::is_base_of<Writable, T>::value,
								"You must inherit from eos::serializable::Writable to be write an object.");
	typedef Writable& theType;
	typedef Writable readType;
};

template<typename T>
struct is_base_type_thingy<true, T> {
	typedef typename handle_pointer<std::is_pointer<T>::value, T>::theType theType;
	typedef typename handle_pointer<std::is_pointer<T>::value, T>::readType readType;
};

} // namespace serialization
} // namespace eos

#endif // __EOS_SERIALIZATION_DATA_H__
