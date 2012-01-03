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
#include <string>
#include <exception>
#include "Writable.h"
#include "Data.h"

#ifndef __EOS_SERIALIZATION_WRITER_H__
#define __EOS_SERIALIZATION_WRITER_H__

namespace eos {
namespace serialization {

class Writer {
public:
	class NoCurrentObject : public std::exception {
		static const char * message;
	public:
		virtual const char * what() const noexcept {
			return message;
		}
	};
	
protected:
	std::map<const Writable*, Object*> idList;
	std::vector<Object*> root_objs;
	Object * curObj;
	
	Object * newObject(Writable * object);
	Object * addObjectGetID(Writable * object);
	
	template<typename T>
	void write_impl(T val, const std::string& name);
	
	/* these structs are here to route
	 instantiations of write() to the correct implementation */
	template<bool primitve, typename T>
	struct is_base_type_thingy {
	};
	
	template<typename T>
	struct is_base_type_thingy<false, T> {
		static_assert(std::is_base_of<Writable, T>::value,
									"You must inherit from eos::serializable::Writable to be write an object.");
		typedef Writable& theType;
	};
	
	template<typename T>
	struct is_base_type_thingy<true, T> {
		typedef T theType;
	};
	
public:
	Writer() : idList(), root_objs(), curObj(nullptr) { }
	virtual ~Writer();
	virtual void writeFile(const std::string& pathname) = 0;
	void addObject(Writable * object) {
		addObjectGetID(object);
	}
	void writeName(const std::string& name) throw(NoCurrentObject);
	
	/* write individual values */
	template<typename T>
	void write(T val, const std::string& name) {
		static_assert( !std::is_pointer<T>::value || std::is_base_of<Writable, T>::value,
									"Only pointers to subclasses of eos::serialization::Writable may be written.");
		write_impl< typename is_base_type_thingy<std::is_scalar<T>::value, T>::theType >(val, name);
	}
	
	/* write arrays of values */
	template<typename T>
	void writeArray(T * elements, size_t count, const std::string& name, hint_t hint = NO_HINT) throw(NoCurrentObject);
	
	// Get a writer
	static Writer * newXMLWriter();
};

template<> inline void Writer::write<const char*>(const char * val, const std::string& name) {
	write_impl<const char *>(val, name);
}

} // namespace serialization
} // namespace eos
#endif // __EOS_SERIALIZATION_WRITER_H__
