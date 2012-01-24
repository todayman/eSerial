//
//  eos/serialization/tests_common.h
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

#include <cstring>
#include <string>
#include <sstream>

#include <eSerial/Writable.h>
#include <eSerial/hints.h>

#include "../eSerial/macros.h"

#ifndef eSerial_tests_common_h
#define eSerial_tests_common_h

namespace eos {
namespace serialization {

class data_t : public eos::serialization::Writable {
public:
	static const std::string xmlName;
	int32_t data;
	void write(eos::serialization::Writer * writer) override;
	void read(eos::serialization::Parser * parser) override;
	bool operator==(const data_t& other) const {
		return this->data == other.data;
	}
	bool operator!=(const data_t& other) const {
		return this->data != other.data;
	}
};

data_t * new_data_t(void);

struct ptr_container_t : public eos::serialization::Writable {
public:
	static const std::string xmlName;
	data_t * data;
	virtual void write(eos::serialization::Writer * writer) override;
	virtual void read(eos::serialization::Parser * parser) override;
	bool operator==(const ptr_container_t& other) const {
		return (*this->data) == (*other.data);
	}
};

ptr_container_t * new_ptr_container_t(void);

struct stack_container_t : public eos::serialization::Writable {
public:
	static const std::string xmlName;
	data_t data;
	virtual void write(eos::serialization::Writer * writer) override;
	virtual void read(eos::serialization::Parser * parser) override;
	bool operator==(const stack_container_t& other) const {
		return this->data == other.data;
	}
};

stack_container_t * new_stack_container_t(void);

extern char val1;
extern char val2;
extern char val3;
extern float val4;

typedef long double long_double;

template<typename TypeParam>
class PrimitiveObject : public Writable {
public:
	static const std::string xmlName;
	TypeParam val1;
	TypeParam val2;
	TypeParam val3;
	TypeParam val4;
	PrimitiveObject();
	virtual void write(eos::serialization::Writer * writer) override;
	virtual void read(Parser * reader) override;
	bool operator==(const PrimitiveObject<TypeParam>& other) const {
		return	this->val1 == other.val1 &&
						this->val2 == other.val2 &&
						this->val3 == other.val3 &&
						this->val4 == other.val4;
	}
};

template<> bool PrimitiveObject<double>::operator==(const PrimitiveObject<double>& other) const;
template<> bool PrimitiveObject<long double>::operator==(const PrimitiveObject<long double>& other) const;

template<typename TypeParam>
PrimitiveObject<TypeParam> * new_PrimitiveObject(void);

template<typename TypeParam>
class PrimitiveArrayObject : public eos::serialization::Writable {
public:
	static const std::string xmlName;
	size_t arrayLen;
	TypeParam * values;
	eos::serialization::hint_t hints;
	PrimitiveArrayObject(eos::serialization::hint_t h = eos::serialization::NO_HINT);
	virtual ~PrimitiveArrayObject();
	virtual void write(eos::serialization::Writer * writer) override;
	virtual void read(eos::serialization::Parser * reader) override;
	bool operator==(const PrimitiveArrayObject<TypeParam>& other) const {
		if( this->arrayLen != other.arrayLen ) {
			return false;
		}
		return memcmp(this->values, other.values, arrayLen * sizeof(TypeParam)) == 0;
	}
};

template<> bool PrimitiveArrayObject<double>::operator==(const PrimitiveArrayObject<double>& other) const;
template<> bool PrimitiveArrayObject<long double>::operator==(const PrimitiveArrayObject<long double>& other) const;

template<typename TypeParam>
PrimitiveArrayObject<TypeParam> * new_PrimitiveArrayObject(void);

class ObjectArray : public Writable {
public:
	static const std::string xmlName;
	size_t arrayLen;
	data_t ** values;
	hint_t hints;
	ObjectArray(hint_t h = NO_HINT);
	virtual ~ObjectArray();
	virtual void write(Writer * writer) override;
	virtual void read(Parser * reader) override;
	bool operator==(const ObjectArray& other) const {
		if( this->arrayLen != other.arrayLen ) {
			return false;
		}
		for( size_t idx = 0; idx < arrayLen; ++idx ) {
			if( *(this->values[idx]) != *(other.values[idx]) ) {
				return false;
			}
		}
		return true;
	}
};

ObjectArray * new_ObjectArray();

template <typename T>
static inline std::string toString (const T & t)
{
	std::stringstream ss;
  ss << t;
  return ss.str();
}

#define MAKE_XML_STRING_DECL(TYPE)	std::string makeXMLString_##TYPE();
PRIMITIVE_TYPE_IDENTIFIERS(MAKE_XML_STRING_DECL)
std::string makeXMLString_stack_container_t(stack_container_t * stack_data);
std::string makeXMLString_ptr_container_t(ptr_container_t * ptr_data);

#define MAKE_XML_ARRAY_STRING_DECL(TYPE) std::string makeXMLArrayString_##TYPE(PrimitiveArrayObject<TYPE> * arrayObj);
PRIMITIVE_TYPE_IDENTIFIERS(MAKE_XML_ARRAY_STRING_DECL)

#define MAKE_XML_ARRAY_READABLE_STRING_DECL(TYPE) std::string makeXMLArray_ReadableString_##TYPE(PrimitiveArrayObject<TYPE> * arrayObj);
PRIMITIVE_TYPE_IDENTIFIERS(MAKE_XML_ARRAY_READABLE_STRING_DECL)

std::string makeXMLObjectArrayString(ObjectArray * data);

}
}
#endif
