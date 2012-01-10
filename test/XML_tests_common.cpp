//
//  eos/serialization/XML_tests_common.cpp
//  eSerial
//
//  Created by Paul O'Neil on 1/9/12.
//  Copyright (c) 2012 Paul O'Neil. All rights reserved.
//

#include <string>
using namespace std;
#include "b64.h"
#include "tests_common.h"
using namespace eos;
using namespace serialization;

static const string xmlHeader("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<eos.serialization>\n");
static const string xmlFooter("</eos.serialization>\n");

namespace eos {
	namespace serialization {

char val1 = 0;
char val2 = 1;
char val3 = 10;
float val4 = 127.345f;

template<typename TypeParam>
PrimitiveObject<TypeParam>::PrimitiveObject() :
		val1(static_cast<TypeParam>(eos::serialization::val1)),
		val2(static_cast<TypeParam>(eos::serialization::val2)),
		val3(static_cast<TypeParam>(eos::serialization::val3)),
		val4(static_cast<TypeParam>(eos::serialization::val4))
{	
}
#define PRIMITIVE_CTOR(x) template PrimitiveObject<x>::PrimitiveObject();
PRIMITIVE_TYPES(PRIMITIVE_CTOR);


template<typename TypeParam>
PrimitiveArrayObject<TypeParam>::PrimitiveArrayObject(hint_t h) : arrayLen(4), values(), hints(h) {
	values[0] = static_cast<TypeParam>(val1);
	values[1] = static_cast<TypeParam>(val2);
	values[2] = static_cast<TypeParam>(val3);
	values[3] = static_cast<TypeParam>(val4);
}
#define PRIMITIVE_ARRAY_CTOR(x) template PrimitiveArrayObject<x>::PrimitiveArrayObject(hint_t);
PRIMITIVE_TYPES(PRIMITIVE_ARRAY_CTOR);

ObjectArray::ObjectArray(hint_t h)  : arrayLen(4), values(new data_t*[arrayLen]), hints(h) {
	for( size_t i = 0; i < arrayLen; ++i ) {
		values[i] = new data_t();
	}
	values[0]->data = static_cast<int32_t>(val1);
	values[1]->data = static_cast<int32_t>(val2);
	values[2]->data = static_cast<int32_t>(val3);
	values[3]->data = static_cast<int32_t>(val4);
}

ObjectArray::~ObjectArray() {
	for( size_t i = 0; i < arrayLen; ++i ) {
		delete values[i];
	}
	delete []values;
}

void ObjectArray::write(Writer * writer) {
	writer->writeName("ObjectArray");
	writer->writeArray(values, arrayLen, "values");
}

#define makeXMLString(TypeParam) \
string makeXMLString_##TypeParam() {\
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

string makeXMLString_char() {
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

string makeXMLString_int8_t() {
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

string makeXMLString_uint8_t() {
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

string makeXMLString_bool() {
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

string makeXMLString_stack_container_t(stack_container_t * stack_data) {
	string expected = xmlHeader;
	expected += "  <object id=\"0\" class=\"" + stack_container_t::xmlName + "\">\n";
	expected += "    <object id=\"1\" class=\"" + data_t::xmlName + "\" name=\"data\">\n";
	expected += "      <int32_t name=\"data\">" + toString(stack_data->data.data) + "</int32_t>\n";
	expected += "    </object>\n";
	expected += "  </object>\n";
	expected += xmlFooter;
	return expected;
}

string makeXMLString_ptr_container_t(ptr_container_t * ptr_data) {
	string expected = xmlHeader;
	expected += "  <object id=\"0\" class=\"" + ptr_container_t::xmlName + "\">\n";
	expected += "    <eos.serialization.Writable id=\"1\" name=\"data\"/>\n";
	expected += "  </object>\n";
	expected += "  <object id=\"1\" class=\"" + data_t::xmlName + "\">\n";
	expected += "    <int32_t name=\"data\">" + toString(ptr_data->data->data) + "</int32_t>\n";
	expected += "  </object>\n";
	expected += xmlFooter;
	return expected;
}

#define makeXMLArrayString(TYPE) \
string makeXMLArrayString_##TYPE(PrimitiveArrayObject<TYPE> * data) { \
	string result = xmlHeader; \
	result += "  <object id=\"0\" class=\"PrimitiveArrayObject\">\n"; \
	result += "    <array type=\"" #TYPE "\" count=\"" + toString(data->arrayLen) + "\" name=\"array\">"; \
	char * b64_string; \
	convert_to_base64(data->values, data->arrayLen, &b64_string); \
	result += b64_string; \
	free(b64_string); \
	result += "</array>\n"; \
	result += "  </object>\n"; \
	result += xmlFooter; \
	return result; \
}

PRIMITIVE_TYPE_IDENTIFIERS(makeXMLArrayString)

#define makeXMLArray_ReadableString(TYPE) \
string makeXMLArray_ReadableString_##TYPE(PrimitiveArrayObject<TYPE> * data) { \
	string result = xmlHeader; \
	result += "  <object id=\"0\" class=\"PrimitiveArrayObject\">\n"; \
	result += "    <array hints=\"1\" type=\"" #TYPE "\" count=\"" + toString(data->arrayLen) + "\" name=\"array\">"; \
	size_t i; \
	for( i = 0; i < data->arrayLen - 1; ++i ) { \
	result += toString(data->values[i]) + " "; \
	} \
	if( i < data->arrayLen ) { \
	result += toString(data->values[i]); \
	} \
	result += "</array>\n"; \
	result += "  </object>\n"; \
	result += xmlFooter; \
	return result; \
}

INTEGER_TYPES(makeXMLArray_ReadableString)
makeXMLArray_ReadableString(float)
makeXMLArray_ReadableString(double)
makeXMLArray_ReadableString(long_double)
makeXMLArray_ReadableString(bool)

string makeXMLArray_ReadableString_char(PrimitiveArrayObject<char> * data) { \
	string result = xmlHeader;
	result += "  <object id=\"0\" class=\"PrimitiveArrayObject\">\n";
	result += "    <array hints=\"1\" type=\"" "char" "\" count=\"" + toString(data->arrayLen) + "\" name=\"array\">";
	size_t i;
	for( i = 0; i < data->arrayLen - 1; ++i ) {
		result += toString<int>(data->values[i]) + " ";
	}
	if( i < data->arrayLen ) {
		result += toString<int>(data->values[i]);
	}
	result += "</array>\n";
	result += "  </object>\n";
	result += xmlFooter;
	return result;
}

string makeXMLArray_ReadableString_uint8_t(PrimitiveArrayObject<uint8_t> * data) { \
	string result = xmlHeader;
	result += "  <object id=\"0\" class=\"PrimitiveArrayObject\">\n";
	result += "    <array hints=\"1\" type=\"" "uint8_t" "\" count=\"" + toString(data->arrayLen) + "\" name=\"array\">";
	size_t i;
	for( i = 0; i < data->arrayLen - 1; ++i ) {
		result += toString<int>(data->values[i]) + " ";
	}
	if( i < data->arrayLen ) {
		result += toString<int>(data->values[i]);
	}
	result += "</array>\n";
	result += "  </object>\n";
	result += xmlFooter;
	return result;
}

string makeXMLArray_ReadableString_int8_t(PrimitiveArrayObject<int8_t> * data) { \
	string result = xmlHeader;
	result += "  <object id=\"0\" class=\"PrimitiveArrayObject\">\n";
	result += "    <array hints=\"1\" type=\"" "int8_t" "\" count=\"" + toString(data->arrayLen) + "\" name=\"array\">";
	size_t i;
	for( i = 0; i < data->arrayLen - 1; ++i ) {
		result += toString<int>(data->values[i]) + " ";
	}
	if( i < data->arrayLen ) {
		result += toString<int>(data->values[i]);
	}
	result += "</array>\n";
	result += "  </object>\n";
	result += xmlFooter;
	return result;
}

const std::string ObjectArray::xmlName("ObjectArray");

string makeXMLObjectArrayString(ObjectArray * data, const std::map<const Writable*, Object*>& idList) {
	string result = xmlHeader;
	result += "  <object id=\"0\" class=\"" + ObjectArray::xmlName + "\">\n";
	result += "    <array type=\"" "eos.serialization.Writable_star" "\" count=\"" + toString(data->arrayLen) + "\" name=\"values\">";
	std::vector<size_t> mapped_ids;
	for( size_t i = 0; i < data->arrayLen; ++i ) {
		mapped_ids.push_back(idList.at(data->values[i])->id);
	}
	char * b64_string;
	convert_to_base64(mapped_ids.data(), data->arrayLen, &b64_string);
	result += b64_string;
	free(b64_string);
	result += "</array>\n";
	result += "  </object>\n";
	for( size_t idx = 0; idx < data->arrayLen; ++idx ) {
		const data_t& element = *(data->values[idx]);
		result += "  <object id=\"" + toString(idList.at(&element)->id) + "\" class=\"" + data_t::xmlName + "\">\n";
		result += "    <int32_t name=\"data\">" + toString(element.data) + "</int32_t>\n";
		result += "  </object>\n";
	}
	result += xmlFooter;
	return result;
}

	}
}
