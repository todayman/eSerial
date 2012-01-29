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
//! \file
//! \author Paul O'Neil <redballoon36@gmail.com>
//! \brief Parser.h contains the abstract base classes used to parse serialized objects.

#include <map>
#include <vector>
#include <stack>
#include <string>
#include <fstream>

#include <eos/serialization/Data.h>

#ifndef __EOS_SERIALIZTION_PARSER_H__
#define __EOS_SERIALIZTION_PARSER_H__

namespace eos {
	namespace serialization {

		class Parser;
		class Writable;

		template<typename T> class Data;
		typedef Data<Writable> Object;

		//! \brief A function that acts as a constructor.
		//! The eos::serialization::DefaultFactory uses one of these for each
		//! class to allocate new objects.  Calling this should be roughly
		//! equivalent to calling \code new some_class; \endcode
		typedef Writable * (*ctor_func_t)() ;
		
		//! \brief An object that acts as a constructor.
		//! \details The eos::serialization::DefaultFactory uses this, by calling
		//! newObject, to allocate new objects.  This is roughly comparable to
		//! calling the constructor.  This object will be used for ALL
		//! instantiations of a particular identifer; that is, it cannot change
		//! during parsing.
		class ctor_block_t {
		public:
			//! Generic, empty destructor
			virtual ~ctor_block_t() { };
			//! Alloctes a eos::serialization::Writable and returns it.
			//! \details Perhaps it does something to the object before it gets returned.
			//! \return a newly allocated object
			virtual Writable * newObject() const = 0;
		};

		//! An abstract factory for all of the objects read during parsing.
		//! This class determines how to convert from string identifiers to
		//! class types and allocated objects.
		class Factory {
		public:
			//! Generic, empty virtual destructor
			virtual ~Factory() { }
			//! Allocates a new object, of the kind identified by className
			//! \param className a string indicating what kind of object should
			//!					should be returned.
			//! \return a newly allocated object of the kind indicated by className
			virtual Writable * newObject(const std::string& className) = 0;
		};

		//! \brief A simple implementation of eos::serialization::Factory.
		//! \details It keeps a map from string class identifiers to the
		//! constructors, and allocates new objects by looking up their constructor.
		class DefaultFactory : public Factory {
			std::map<std::string, ctor_block_t*> ctors;
		public:
			//! Destroys the factory, and the map of constrcutors
			virtual ~DefaultFactory();
			
			//! Assigns the a constructor to called for objects certain classes
			//! \param className a string indicating the type of the to-be object
			//! \param ctor a function that allocates the right kind of object
			//! \return a new instance of the class indicated by className
			void registerClass(const std::string& className, ctor_func_t ctor);
			
			//! Assigns the a constructor to called for objects certain classes
			//! \param className a string indicating the type of the to-be object
			//! \param ctor an object that allocates the right kind of object
			//! \return a new instance of the class indicated by className
			void registerClass(const std::string& className, ctor_block_t* ctor);
			
			//! Allocates an object.
			//! \param className the class to instantiate
			//! \return a new object of the given class
			virtual Writable * newObject(const std::string& className) override;
		};

		//! \brief Abstract class that translates from the intermediate structures in
		//! Data.h to real, live objects.
		//! \details Subclasses implement reading from files / other sources to
		//! generate the intermediate structures.
		class Parser {
		protected:
			//! The top-level objects in the stores.
			std::vector<Object*> root_objs;
			
			//! Map from object ID to the metadata for that object.
			std::map<size_t, Object*> metaIDList;
			
			//! Implemented by subclasses to read from disk / other source.
			virtual void firstPass(std::istream& filename) = 0;
			
			//! Map from object ID to the real object.
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
			//! Initializes a new parser.  Doesn't actually do much...
			Parser() : root_objs(), metaIDList(), realIDList(), factory(nullptr), curObj(nullptr) { }
			//! Destorys the parser.
			//! This does NOT free any real life objects generated by parsing.
			//! It assumes that you have already taken ownership of them.
			virtual ~Parser();
			//! Parses objects in the given stream.
			//! \param input the source of the new objects.
			void parseStream(std::istream& input);
			
			//! Helper function to read from a file.  Just opens the file
			//! and calls parseStream with the new stream.
			void parseFile(const std::string& filename) {
				std::ifstream input(filename);
				parseStream(input);
				input.close();
			}
			
			//! \brief Returns the eos::serialization::Factory used by this Parser to
			//! generate real objects.
			//! \details If this parser does not have one, it allocates a new
			//! eos::serialization::DefaultFactory and returns that.
			//! \return the factory of objects for this Parser
			Factory * getFactory() {
				if( nullptr == factory ) {
					factory = new DefaultFactory();
				}
				return factory;
			}
			
			//! \brief Sets the eos::serialization::Factory used to generate real objects
			//! for this parser.
			//! \details If this Parser already has a factory, then it frees the old one.
			//! \param newFactory the new generator of objects.
			void setFactory(Factory * newFactory);
			
			//! \brief Gets the a value from the Parser.
			//! \details This should be called from eos::serialization::Writable::read().
			//! \param name the key identifying this particular value
			//! \param val a pointer to where the new value should go
			template<typename T>
			void read(const std::string& name, T * val) {
				typedef typename is_base_type_thingy<std::is_scalar<T>::value, T>::readType implType;
				read_impl< implType >(name, reinterpret_cast<implType*>(val));
			}
			
			//! \brief Gets an array from the Parser.  Memory will be allocated by
			//! the Parser.
			//! \details This should be called from eos::serialization::Writable::read().
			//! \param name the key identifying this particular array
			//! \param elements a pointer to where the array should go.
			//! \param count if not nullptr, it will contain the number of elements in
			//! the array
			template<typename T>
			void readArray(const std::string& name, T ** elements, size_t * count) {
				static_assert( !std::is_pointer<T>::value || std::is_base_of<Writable, typename std::remove_pointer<T>::type >::value,
											"Only pointers to subclasses of eos::serialization::Writable may be written.");
				typedef typename is_base_type_thingy<std::is_scalar<T>::value, T>::theType implType;
				read_array_impl<implType>(name, reinterpret_cast<implType**>(elements), count);
			}
			
			//! Creates and returns a new Parser that can read objects from XML files.
			//! \return a new Parser that can read objects from XML files.
			static Parser * newXMLParser();
		};

	} // namespace serialization
} // namespace eos

#endif // __EOS_SERIALIZTION_PARSER_H__
