//
//  eos/serialization/tests_common.h
//  eSerial
//
//  Created by Paul O'Neil on 1/6/12.
//  Copyright (c) 2012 Paul O'Neil. All rights reserved.
//

#include <string>
#include <sstream>

#include "Writable.h"
#include "Writer.h"

#include "macros.h"

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
};

struct ptr_container_t : public eos::serialization::Writable {
public:
	static const std::string xmlName;
	data_t * data;
	virtual void write(eos::serialization::Writer * writer) override;
	virtual void read(eos::serialization::Parser * parser) override;
};

struct stack_container_t : public eos::serialization::Writable {
public:
	static const std::string xmlName;
	data_t data;
	virtual void write(eos::serialization::Writer * writer) override;
	virtual void read(eos::serialization::Parser * parser) override;
};

extern char val1;
extern char val2;
extern char val3;
extern float val4;

typedef long double long_double;

template<typename TypeParam>
class PrimitiveObject : public Writable {
public:
	TypeParam val1;
	TypeParam val2;
	TypeParam val3;
	TypeParam val4;
	PrimitiveObject();
	void write(eos::serialization::Writer * writer) override;
	void read(Parser * reader) override;
};

template<typename TypeParam>
class PrimitiveArrayObject : public eos::serialization::Writable {
public:
	size_t arrayLen;
	TypeParam values[4];
	eos::serialization::hint_t hints;
	PrimitiveArrayObject(eos::serialization::hint_t h = eos::serialization::NO_HINT);
	virtual void write(eos::serialization::Writer * writer) override;
	virtual void read(eos::serialization::Parser * reader) override;
};

class ObjectArray : public Writable {
public:
	static const std::string xmlName;
	size_t arrayLen;
	data_t ** values;
	hint_t hints;
	ObjectArray(hint_t h = NO_HINT);
	virtual ~ObjectArray();
	virtual void write(Writer * writer) override;
	virtual void read(Parser * reader) override { }
};

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

std::string makeXMLObjectArrayString(ObjectArray * data, const std::map<const Writable*, Object*>& idList);

}
}
#endif
