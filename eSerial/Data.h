//
//  eos/serialization/Data.h
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
//! \brief Data.h defines the intermediate structures.
//!
//! As the eos::serialization::Writer traverses the object graph, it generates
//! these structures.  Next, the eos::serialization::Writer uses these to write
//! data out.  These store almost all of the information needed to serialize the 
//! object graph.  Knowledge of the root level objects is all that is missing.
//! \author Paul O'Neil <redballoon36@gmail.com>

#include <map>
#include <string>
#include <cstdlib>

#include <eSerial/hints.h>

#ifndef __EOS_SERIALIZATION_DATA_H__
#define __EOS_SERIALIZATION_DATA_H__

namespace eos {
	namespace serialization {

		class Writable;

		//! The superclass of all the metadata helper classes.
		//! This class exists so that they all have RTTI info and
		//! can be stored in the same containers.
		//!
		//! \todo identifiers starting with underscore capital letter are reserved...
		//! This is in a namespace, so the qualified name doesn't start with those,
		//! and it doesn't appear to be a problem (yet).
		class _Data {
		public:
			virtual ~_Data() { }
			//! Returns true if this is the same kind of data with the same value
			//! \return true if this is the same kind of data with the same value
			virtual bool operator==(const _Data& other) const = 0;
			
			//! Whether the owner of this object is responsible for
			//! deleting it, or whether it will be deleted by the Writer/Reader.
			//! \return whether this object is deleted by the owner or the Writer/Reader
			virtual bool shouldBeDeletedByOwner() const = 0;
		};
		//! A pointer to a metadata structure
		typedef _Data * pData;

		//! \brief Contains metadata on a particular value, namely it's type
		//! (by which instantiation is used) and its value.
		template<typename T> class Data : public _Data {
		public:
			//! Creates a new metadata with the given value
			Data(T d = nullptr) : data(d) {}
			//! The value
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

		//! Stores data on a pointer to another serialized object
		template<> class Data<Writable*> : public _Data {
		public:
			//! Creates a new Data pointing to the object given ID ident.
			//! \param ident the ID of the object this pointer points to
			Data(size_t ident = 0) : id(ident) { }
			
			//! \brief The ID number of the object this points to.
			//! \see Data<Writable>::id
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

		//! \brief Stores data for an array of type T
		//! \param T the type of the data in the array
		template<typename T> class ArrayData : public _Data {
		public:
			//! The number of elements in this array
			size_t count;
			//! The contents of the array
			T * data;
			//! Hints as to how the array should be written or how the memory is handled
			hint_t hints;
			
			//! Creates a new, emtpy ArrayData<T>
			ArrayData() : count(0), data(nullptr), hints(NO_HINT) { }
			
			//! \brief Creates a new ArrayData<T> with the given data and hints.
			//! 
			//! The ArrayData does NOT take ownership of the array and will not
			//! delete the array when the ArrayData is destroyed.
			//! \param c the number of elements in the array
			//! \param d the contents of the array
			//! \param h hints to be applied to the array.  Possible flags are
			//! eos::serialization::COPY_ARRAY_HINT, eos::serialization::FREE_HINT,
			//!	and eos::serialization::READABLE_HINT.
			//! \todo make sure that COPY_ARRAY_HINT actually copies...
			ArrayData(size_t c, T * d, hint_t h) : count(c), data(d), hints(h) { }
			
			//! Destroys the ArrayData.  If eos::serialization::COPY_ARRAY_HINT is set,
			//! then this frees the data in the array
			virtual ~ArrayData() {
				if( count && (hints & COPY_ARRAY_HINT) ) {
					delete[] data;
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

		//! Stores an array of pointers to objects.
		template<> class ArrayData<Writable*> : public _Data {
		public:
			//! The number of elements in the array.
			size_t count;
			//! An array of IDs to the objects being pointed to
			size_t * data;
			//! Hints as to how the array should be written or how the memory is handled
			hint_t hints;
			
			ArrayData() : count(0), data(nullptr), hints(NO_HINT) { }
			//! \brief Creates a new ArrayData<Writable>> with the given data and hints.
			//! \param c the number of elements in the array
			//! \param h hints to be applied to the array.  The only possible flag
			//!	is eos::serialization::READABLE_HINT.
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

		//! Stores all the data in an object.
		template<> class Data<Writable> : public _Data {
		public:
			//! The ID number of this object
			size_t id;
			
			//! A string identifying the class / type name of this object
			std::string name;
			
			//! Stores the fields of this object.
			//!
			//! The key is an identifier for the field, while the value is the
			//! value of the field.
			std::map<std::string, pData> data;
			
			//! Creates a new, empty object
			Data() : id(static_cast<size_t>(-1)), name(), data() { }
			
			//! Destroys the Data, and deletes its fields
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
