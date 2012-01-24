//
//  eos/serialization/XMLReader_test.cpp
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

#include "tests_common.h"
#include <eSerial/Parser.h>
#include "../eSerial/XMLParser.h"
using namespace eos;
using namespace serialization;
#include <sstream>
using std::stringstream;
#include <iostream>
using namespace std;

#include <gtest/gtest.h>

namespace eos {
namespace serialization {

void data_t::read(Parser * reader) {
	reader->read("data", &data);
}

void ptr_container_t::read(Parser * reader) {
	reader->read("data", &data);
}

void stack_container_t::read(Parser * reader) {
	reader->read("data", &data);
}

template<typename TypeParam>
void PrimitiveObject<TypeParam>::read(Parser * reader) {
	reader->read("val1", &val1);
	reader->read("val2", &val2);
	reader->read("val3", &val3);
	reader->read("val4", &val4);
}

#define PRIMITIVE_READ_METHOD(x) template void PrimitiveObject<x>::read(Parser * reader);
PRIMITIVE_TYPES(PRIMITIVE_READ_METHOD)

template<typename TypeParam>
void PrimitiveArrayObject<TypeParam>::read(Parser * reader) {
	delete []values;
	reader->readArray<TypeParam>("array", &values, &arrayLen);
}
#define PRIMITIVE_ARRAY_READ_METHOD(x) template void PrimitiveArrayObject<x>::read(Parser * reader);
PRIMITIVE_TYPES(PRIMITIVE_ARRAY_READ_METHOD)

void ObjectArray::read(Parser * reader) {
	for( size_t i = 0; i < arrayLen; ++i ) {
		delete values[i];
	}
	delete []values;
	reader->readArray("values", &values, &arrayLen);
}

}
}

class XMLReaderTest : public ::testing::Test, public XMLParser {
};

#define READ_TEST(TYPE)\
TEST_F(XMLReaderTest, Read##TYPE##Test) { \
	PrimitiveObject<TYPE> primObj; \
	stringstream stream(makeXMLString_##TYPE()); \
	this->getFactory()->registerClass(PrimitiveObject<TYPE>::xmlName, (ctor_func_t)new_PrimitiveObject<TYPE>); \
	this->parseStream(stream); \
	\
	ASSERT_EQ(1, this->realIDList.size()); \
	EXPECT_EQ(primObj, dynamic_cast<const PrimitiveObject<TYPE>&>(*(this->realIDList[0]))); \
	delete this->realIDList[0]; \
}

PRIMITIVE_TYPE_IDENTIFIERS(READ_TEST)

TEST_F(XMLReaderTest, InternalObjectTest) {
	stack_container_t stack_data;
	stack_data.data.data = 5;
	
	this->getFactory()->registerClass(stack_container_t::xmlName, (ctor_func_t)new_stack_container_t);
	
	stringstream stream(makeXMLString_stack_container_t(&stack_data));
	this->parseStream(stream);
	
	ASSERT_EQ(2, this->metaIDList.size());
	ASSERT_EQ(2, this->realIDList.size());
	ASSERT_EQ(1, this->root_objs.size());
	EXPECT_EQ(stack_data, dynamic_cast<stack_container_t&>(*(this->realIDList[0])));
	delete this->realIDList[0];
	this->realIDList.erase(0);
}

TEST_F(XMLReaderTest, InternalPointerTest) {
	ptr_container_t ptr_data;
	ptr_data.data = new data_t();
	ptr_data.data->data = 5;
	
	this->getFactory()->registerClass(ptr_container_t::xmlName, (ctor_func_t)new_ptr_container_t);
	this->getFactory()->registerClass(data_t::xmlName, (ctor_func_t)new_data_t);
	
	stringstream stream(makeXMLString_ptr_container_t(&ptr_data));
	this->parseStream(stream);
	
	ASSERT_EQ(2, this->metaIDList.size());
	ASSERT_EQ(2, this->realIDList.size());
	ASSERT_EQ(2, this->root_objs.size());
	EXPECT_EQ(ptr_data, dynamic_cast<ptr_container_t&>(*(this->realIDList[0])));
	delete this->realIDList[0];
	delete this->realIDList[1];
	this->realIDList.erase(0);
	this->realIDList.erase(1);
	delete ptr_data.data;
}

#define READ_ARRAY_TEST(TYPE) \
TEST_F(XMLReaderTest, PrimitiveArrayTest_##TYPE) { \
	PrimitiveArrayObject<TYPE> arrayObj; \
	\
	this->getFactory()->registerClass(PrimitiveArrayObject<TYPE>::xmlName, (ctor_func_t)new_PrimitiveArrayObject<TYPE>); \
	\
	stringstream stream(makeXMLArrayString_##TYPE(&arrayObj)); \
	this->parseStream(stream); \
	\
	ASSERT_EQ(1, this->metaIDList.size()); \
	ASSERT_EQ(1, this->realIDList.size()); \
	ASSERT_EQ(1, this->root_objs.size()); \
	EXPECT_EQ(arrayObj, dynamic_cast<PrimitiveArrayObject<TYPE>&>(*(this->realIDList[0]))); \
	delete this->realIDList[0]; \
	this->realIDList.erase(0); \
}
PRIMITIVE_TYPE_IDENTIFIERS(READ_ARRAY_TEST)

#define READ_READABLE_ARRAY_TEST(TYPE) \
TEST_F(XMLReaderTest, PrimitiveReadableArrayTest_##TYPE) { \
	PrimitiveArrayObject<TYPE> arrayObj; \
	\
	this->getFactory()->registerClass(PrimitiveArrayObject<TYPE>::xmlName, (ctor_func_t)new_PrimitiveArrayObject<TYPE>); \
	\
	stringstream stream(makeXMLArray_ReadableString_##TYPE(&arrayObj)); \
	this->parseStream(stream); \
	\
	ASSERT_EQ(1, this->metaIDList.size()); \
	ASSERT_EQ(1, this->realIDList.size()); \
	ASSERT_EQ(1, this->root_objs.size()); \
	EXPECT_EQ(arrayObj, dynamic_cast<PrimitiveArrayObject<TYPE>&>(*(this->realIDList[0]))); \
	delete this->realIDList[0]; \
}
PRIMITIVE_TYPE_IDENTIFIERS(READ_READABLE_ARRAY_TEST)

TEST_F(XMLReaderTest, ObjectArrayTest) {
	ObjectArray objArray;
	
	this->getFactory()->registerClass(data_t::xmlName, (ctor_func_t)new_data_t);
	this->getFactory()->registerClass(ObjectArray::xmlName, (ctor_func_t)new_ObjectArray);
	
	stringstream stream(makeXMLObjectArrayString(&objArray));
	this->parseStream(stream);
	
	ASSERT_EQ(objArray.arrayLen + 1, this->metaIDList.size());
	ASSERT_EQ(objArray.arrayLen + 1, this->realIDList.size());
	ASSERT_EQ(objArray.arrayLen + 1, this->root_objs.size());
	EXPECT_EQ(objArray, dynamic_cast<ObjectArray&>(*(this->realIDList[0])));
	delete this->realIDList[0];
}
