//
//  eos/serialization/Writer.h
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

#include <iostream>
#include <fstream>
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
	
	class UnnamedObject : public std::exception {
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
	Object * addRootObject(Writable * object);
	
	template<typename T>
	void write_impl(T val, const std::string& name);
	template<typename T>
	void write_array_impl(T * elements, size_t count, const std::string& name, hint_t hint);
	
public:
	Writer() : idList(), root_objs(), curObj(nullptr) { }
	virtual ~Writer();
	void writeFile(const std::string& pathname) {
		std::ofstream output(pathname);
		writeStream(output);
		output.close();
	}
	virtual void writeStream(std::ostream &outputStream) = 0;
	void addObject(Writable * object) {
		addRootObject(object);
	}
	void writeName(const std::string& name) throw(NoCurrentObject);
	
	/* write individual values */
	template<typename T>
	void write(T& val, const std::string& name) {
		static_assert( !std::is_pointer<T>::value || std::is_base_of<Writable, typename std::remove_pointer<T>::type >::value,
									"Only pointers to subclasses of eos::serialization::Writable may be written.");
		write_impl< typename is_base_type_thingy<std::is_scalar<T>::value, T>::theType >(val, name);
	}
	
	/* write arrays of values */
	template<typename T>
	void writeArray(T * elements, size_t count, const std::string& name, hint_t hint = NO_HINT) {
		static_assert( !std::is_pointer<T>::value || std::is_base_of<Writable, typename std::remove_pointer<T>::type >::value,
									"Only pointers to subclasses of eos::serialization::Writable may be written.");
		typedef typename is_base_type_thingy<std::is_scalar<T>::value, T>::theType implType;
		write_array_impl< implType  >(reinterpret_cast<implType*>(elements), count, name, hint);
	}
	
	// Get a writer
	static Writer * newXMLWriter();
};

template<> inline void Writer::write<const char*>(const char *& val, const std::string& name) {
	write_impl<const char *>(val, name);
}

} // namespace serialization
} // namespace eos
#endif // __EOS_SERIALIZATION_WRITER_H__
