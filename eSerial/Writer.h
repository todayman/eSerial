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
//! \file
//! \author Paul O'Neil <redballoon36@gmail.com>
//! \brief Writer.h contains the abstract base classes used to serialize objects.

#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <type_traits>
#include <string>
#include <exception>
#include <eos/serialization/Writable.h>
#include <eos/serialization/Data.h>

#ifndef __EOS_SERIALIZATION_WRITER_H__
#define __EOS_SERIALIZATION_WRITER_H__

namespace eos {
	namespace serialization {

		//! \brief Abstract base class that translates from real objects to intermediate
		//! structures.
		//! \details Subclasses implement writing the intermediate structures to
		//! some kind of output (i.e. XML on disk).
		class Writer {
		public:
			
			//! Thrown when an operation requires an object is currently being written
			//! does not have a current object.
			class NoCurrentObject : public std::exception {
				static const char * message;
			public:
				virtual const char * what() const noexcept override {
					return message;
				}
			};
			
			//! When an object is done telling the Writer how to serialize it, it must
			//! give itself an identifier (presumably the name of its class).
			//! When it doesn't, this exception is thrown.
			class UnnamedObject : public std::exception {
				static const char * message;
			public:
				virtual const char * what() const noexcept {
					return message;
				}
			};
			
		protected:
			//! A map from real objects to their metadata
			std::map<const Writable*, Object*> idList;
			//! A list of the top-level objects
			std::vector<Object*> root_objs;
			
			// These really should be private, but their used in the tests,
			// and I don't want to rewrite verything now.  They won't show up
			// in the documentation, so I think it's OK for now...
			//! \private
			Object * curObj;
			
			//! \private
			Object * newObject(Writable * object);
			//! \private
			Object * addRootObject(Writable * object);
			
			//! \private
			template<typename T>
			void write_impl(T val, const std::string& name);
			//! \private
			template<typename T>
			void write_array_impl(T * elements, size_t count, const std::string& name, hint_t hint);
			
		public:
			//! Creates an initializes a new Writer.  Not much to see here.
			Writer() : idList(), root_objs(), curObj(nullptr) { }
			
			//! Destroys all the metadata the Writer has generated, and the Writer.
			virtual ~Writer();
			
			//! \brief Writes the objects that have been "written" to this Writer to disk
			//! \param pathname the name of the file to write to.
			void writeFile(const std::string& pathname) {
				std::ofstream output(pathname);
				writeStream(output);
				output.close();
			}
			
			//! \brief Writes the objects that have been "written" to this Writer to
			//! the stream.
			//! \details Subclasses must implement this method to do the actual writing.
			//! \param outputStream the final destination of the objects.
			virtual void writeStream(std::ostream &outputStream) = 0;
			
			//! \brief Adds a root-level object to be serialized.
			//! \param object the object to be serialized.
			void addObject(Writable * object) {
				addRootObject(object);
			}
			
			//! \brief Sets the name identifying the class of the object
			//! currently being written.
			//! \param name the name of the class
			//! \throws NoCurrentObject if there is not currently an object being written
			void writeName(const std::string& name) throw(NoCurrentObject);
			
			//! \brief Serializes a field of the current object.
			//! \param val the field to be written
			//! \param name the name identifying this field
			//! \throws NoCurrentObject if there is not a current object.
			template<typename T>
			void write(T& val, const std::string& name) {
				write_impl< typename is_base_type_thingy<std::is_scalar<T>::value, T>::theType >(val, name);
			}
			
			//! \brief Serializes a field of the current object.
			//! The field must be an array / pointer to the beginning of an array.
			//! \param elements the contents of the array
			//! \param count the number of elements in the array
			//! \param name a name identifying this field.
			//! \param hint hints how to serialize the array.
			//! \see hints.h
			template<typename T>
			void writeArray(T * elements, size_t count, const std::string& name, hint_t hint = NO_HINT) {
				static_assert( !std::is_pointer<T>::value || std::is_base_of<Writable, typename std::remove_pointer<T>::type >::value,
											"Only pointers to subclasses of eos::serialization::Writable may be written.");
				typedef typename is_base_type_thingy<std::is_scalar<T>::value, T>::theType implType;
				write_array_impl< implType >(reinterpret_cast<implType*>(elements), count, name, hint);
			}
			
			//! Creates and returns a new Writer that can write objects to XML files.
			//! \return a new Writer that can write objects to XML files.
			static Writer * newXMLWriter();
		};
	} // namespace serialization
} // namespace eos
#endif // __EOS_SERIALIZATION_WRITER_H__
