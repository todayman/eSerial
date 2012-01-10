//
//  eos/serialization/XMLWriter_tests.cpp
//  eSerial
//
//  Created by Paul O'Neil on 1/5/12.
//  Copyright (c) 2012 Paul O'Neil. All rights reserved.
//

#include "macros.h"
#include "XMLWriter.h"
#include "b64.h"
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

template<typename TypeParam>
void PrimitiveArrayObject<TypeParam>::write(Writer * writer) {
	writer->writeName("PrimitiveArrayObject");
	writer->writeArray(values, arrayLen, "array", hints);
}
#define PRIMITIVE_ARRAY_WRITE_METHOD(x) template void PrimitiveArrayObject<x>::write(Writer * writer);
PRIMITIVE_TYPES(PRIMITIVE_ARRAY_WRITE_METHOD)

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
	EXPECT_EQ(makeXMLString_##TYPE(), (stream.str())); \
}

PRIMITIVE_TYPE_IDENTIFIERS(WRITE_TEST)


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
	
	EXPECT_EQ(makeXMLObjectArrayString(&objArray, this->idList), stream.str());
}
