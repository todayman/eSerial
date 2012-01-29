//
//  eos/serialization/XMLWriter_tests.cpp
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

#include "../eSerial/macros.h"
#include "../eSerial/XMLWriter.h"
#include <eSerial/b64.h>
#include "tests_common.h"
using namespace eos;
using namespace serialization;
#include <string>
using std::string;
#include <sstream>
using std::stringstream;

#include <gtest/gtest.h>

namespace eos {
namespace serialization {

void data_t::write(Writer * writer) {
	writer->writeName(xmlName);
	writer->write(data, "data");
}

void ptr_container_t::write(Writer * writer) {
	writer->writeName(xmlName);
	writer->write(data, "data");
}

void stack_container_t::write(Writer * writer) {
	writer->writeName(xmlName);
	writer->write(data, "data");
}

template<typename T>
void PrimitiveObject<T>::write(Writer * writer) {
	writer->writeName("PrimitiveObject");
	writer->write(val1, "val1");
	writer->write(val2, "val2");
	writer->write(val3, "val3");
	writer->write(val4, "val4");
}

#define PRIMITIVE_WRITE_METHOD(x) template void PrimitiveObject<x>::write(Writer * writer);
PRIMITIVE_TYPES(PRIMITIVE_WRITE_METHOD)

void PrimitiveObject<char_star>::write(Writer * writer) {
	writer->writeName("PrimitiveObject");
	writer->write(val1, "val1");
}

template<typename TypeParam>
void PrimitiveArrayObject<TypeParam>::write(Writer * writer) {
	writer->writeName("PrimitiveArrayObject");
	writer->writeArray(values, arrayLen, "array", hints);
}
#define PRIMITIVE_ARRAY_WRITE_METHOD(x) template void PrimitiveArrayObject<x>::write(Writer * writer);
PRIMITIVE_TYPES(PRIMITIVE_ARRAY_WRITE_METHOD)

void ObjectArray::write(Writer * writer) {
	writer->writeName("ObjectArray");
	writer->writeArray(values, arrayLen, "values");
}

}
}
class XMLWriterTest : public ::testing::Test, public XMLWriter {
};

#define WRITE_TEST(TYPE) \
TEST_F(XMLWriterTest, Add##TYPE##Test) { \
	PrimitiveObject<TYPE> primObj; \
	this->addObject(&primObj); \
	\
	stringstream stream; \
	this->writeStream(stream); \
	\
	ASSERT_EQ(makeXMLString_##TYPE(&primObj), (stream.str())); \
}

PRIMITIVE_TYPE_IDENTIFIERS(WRITE_TEST)
WRITE_TEST(char_star)

TEST_F(XMLWriterTest, InternalObjectTest) {
	stack_container_t stack_data;
	stack_data.data.data = 5;
	
	this->addObject(&stack_data);
	stringstream stream;
	this->writeStream(stream);
		
	EXPECT_EQ(makeXMLString_stack_container_t(&stack_data), stream.str());
}

TEST_F(XMLWriterTest, InternalPointerTest) {
	ptr_container_t ptr_data;
	ptr_data.data = new data_t();
	ptr_data.data->data = 5;
	
	this->addObject(&ptr_data);
	stringstream stream;
	this->writeStream(stream);
	
	EXPECT_EQ(makeXMLString_ptr_container_t(&ptr_data), stream.str());
	delete ptr_data.data;
}

#define WRITE_ARRAY_TEST(TYPE) \
TEST_F(XMLWriterTest, PrimitiveArrayTest_##TYPE) { \
	PrimitiveArrayObject<TYPE> arrayObj; \
	this->addObject(&arrayObj); \
	\
	stringstream stream; \
	this->writeStream(stream); \
	\
	EXPECT_EQ(makeXMLArrayString_##TYPE(&arrayObj), (stream.str())); \
}

PRIMITIVE_TYPE_IDENTIFIERS(WRITE_ARRAY_TEST)


#define WRITE_READABLE_ARRAY_TEST(TYPE) \
TEST_F(XMLWriterTest, PrimitiveReadableArrayTest_##TYPE) { \
	PrimitiveArrayObject<TYPE> arrayObj(READABLE_HINT); \
	this->addObject(&arrayObj); \
	\
	stringstream stream; \
	this->writeStream(stream); \
	\
	EXPECT_EQ(makeXMLArray_ReadableString_##TYPE(&arrayObj), (stream.str())); \
}

PRIMITIVE_TYPE_IDENTIFIERS(WRITE_READABLE_ARRAY_TEST)


TEST_F(XMLWriterTest, ObjectArrayTest) {
	ObjectArray objArray;
	this->addObject(&objArray);
	
	stringstream stream;
	this->writeStream(stream);
	
	EXPECT_EQ(makeXMLObjectArrayString(&objArray), stream.str());
}
