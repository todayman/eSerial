//
//  eos/serialization/XMLWriter_tests.cpp
//  eSerial
//
//  Created by Paul O'Neil on 1/5/12.
//  Copyright (c) 2012 Paul O'Neil. All rights reserved.
//

#include "macros.h"
#include "XMLWriter.h"
using namespace eos;
using namespace serialization;
#include <string>
using std::string;
#include <sstream>
using std::stringstream;

#include <gtest/gtest.h>

static const string xmlHeader("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<eos.serialization>\n");
static const string xmlFooter("</eos.serialization>\n");

class XMLWriterTest : public ::testing::Test, public XMLWriter {
};

char val1 = 0;
char val2 = 1;
char val3 = 10;
float val4 = 127.345f;

template<typename TypeParam>
class PrimitiveObject : public Writable {
public:
	TypeParam val1 = static_cast<TypeParam>(::val1);
	TypeParam val2 = static_cast<TypeParam>(::val2);
	TypeParam val3 = static_cast<TypeParam>(::val3);
	TypeParam val4 = static_cast<TypeParam>(::val4);
	void write(Writer * writer) override {
		writer->writeName("PrimitiveObject");
		writer->write(val1, "val1");
		writer->write(val2, "val2");
		writer->write(val3, "val3");
		writer->write(val4, "val4");
	}
	void read(Parser * reader) override { }
};

template <typename T>
static inline std::string toString (const T & t)
{
  stringstream ss;
  ss << t;
  return ss.str();
}

#define makeXMLString(TypeParam) \
static string makeXMLString_##TypeParam() {\
	string result = xmlHeader; \
	result += "  <object id=\"0\" class=\"PrimitiveObject\">\n"; \
	result += "    <" #TypeParam " name=\"val1\">" + toString<TypeParam>(static_cast<TypeParam>(val1)) + "</"#TypeParam">\n"; \
	result += "    <" #TypeParam " name=\"val2\">" + toString<TypeParam>(static_cast<TypeParam>(val2)) + "</"#TypeParam">\n"; \
	result += "    <" #TypeParam " name=\"val3\">" + toString<TypeParam>(static_cast<TypeParam>(val3)) + "</"#TypeParam">\n"; \
	result += "    <" #TypeParam " name=\"val4\">" + toString<TypeParam>(static_cast<TypeParam>(val4)) + "</"#TypeParam">\n"; \
	result += "  </object>\n"; \
	result += xmlFooter; \
	return result;\
}

static string makeXMLString_char() {
	string result = xmlHeader;
	result += "  <object id=\"0\" class=\"PrimitiveObject\">\n";
	result += "    <" "char" " name=\"val1\">" + toString<int>(val1) + "</""char"">\n";
	result += "    <" "char" " name=\"val2\">" + toString<int>(val2) + "</""char"">\n";
	result += "    <" "char" " name=\"val3\">" + toString<int>(val3) + "</""char"">\n";
	result += "    <" "char" " name=\"val4\">" + toString<int>(static_cast<char>(val4)) + "</""char"">\n";
	result += "  </object>\n";
	result += xmlFooter;
	return result;
}

static string makeXMLString_int8_t() {
	string result = xmlHeader;
	result += "  <object id=\"0\" class=\"PrimitiveObject\">\n";
	result += "    <" "int8_t" " name=\"val1\">" + toString<int>(val1) + "</""int8_t"">\n";
	result += "    <" "int8_t" " name=\"val2\">" + toString<int>(val2) + "</""int8_t"">\n";
	result += "    <" "int8_t" " name=\"val3\">" + toString<int>(val3) + "</""int8_t"">\n";
	result += "    <" "int8_t" " name=\"val4\">" + toString<int>(static_cast<int8_t>(val4)) + "</""int8_t"">\n";
	result += "  </object>\n";
	result += xmlFooter;
	return result;
}

static string makeXMLString_uint8_t() {
	string result = xmlHeader;
	result += "  <object id=\"0\" class=\"PrimitiveObject\">\n";
	result += "    <" "uint8_t" " name=\"val1\">" + toString<int>(val1) + "</""uint8_t"">\n";
	result += "    <" "uint8_t" " name=\"val2\">" + toString<int>(val2) + "</""uint8_t"">\n";
	result += "    <" "uint8_t" " name=\"val3\">" + toString<int>(val3) + "</""uint8_t"">\n";
	result += "    <" "uint8_t" " name=\"val4\">" + toString<int>(static_cast<uint8_t>(val4)) + "</""uint8_t"">\n";
	result += "  </object>\n";
	result += xmlFooter;
	return result;
}

typedef long double long_double;

INTEGER_TYPES(makeXMLString)
makeXMLString(float)
makeXMLString(double)
makeXMLString(long_double)

static string makeXMLString_bool() {
	string result = xmlHeader;
	result += "  <object id=\"0\" class=\"PrimitiveObject\">\n";
	result += "    <" "bool" " name=\"val1\">" + toString<int>(static_cast<bool>(val1)) + "</""bool"">\n";
	result += "    <" "bool" " name=\"val2\">" + toString<int>(static_cast<bool>(val2)) + "</""bool"">\n";
	result += "    <" "bool" " name=\"val3\">" + toString<int>(static_cast<bool>(val3)) + "</""bool"">\n";
	result += "    <" "bool" " name=\"val4\">" + toString<int>(static_cast<bool>(val4)) + "</""bool"">\n";
	result += "  </object>\n";
	result += xmlFooter;
	return result;
}


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

class data_t : public Writable {
public:
	int data;
	void write(Writer * writer) override {
		
	}
	void read(Parser * parser) override {	}
};

struct ptr_container_t : public Writable {
public:
	static const string xmlName;
	data_t * data;
	virtual void write(Writer * writer) override {
		writer->writeName(xmlName);
		writer->write(data, "data");
	}
	virtual void read(Parser * parser) override { }
};

const string ptr_container_t::xmlName = "ptr_container_t";

struct stack_container_t : public Writable {
public:
	static const string xmlName;
	data_t data;
	virtual void write(Writer * writer) override {
		writer->write(data, "data");
	}
	virtual void read(Parser * parser) override { }
};
const string stack_container_t::xmlName = "stack_container_t";

TEST_F(XMLWriterTest, InternalObjectTest) {
	stack_container_t stack_data;
	stack_data.data.data = 5;
	
	this->addObject(&stack_data);
	stringstream stream;
	this->writeStream(stream);
	
	string expected = xmlHeader;
	expected += "    <object id=\"0\" name=\"" + stack_container_t::xmlName + "\">\n";
	
	EXPECT_EQ(expected, stream.str());
}
