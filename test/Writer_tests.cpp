//
//  eos/serialization/Writer_tests.cpp
//  eSerial
//
//  Created by Paul O'Neil on 12/24/11.
//  Copyright (c) 2012 Paul O'Neil. All rights reserved.
//

#include "Writer.h"
#include "tests_common.h"
using namespace eos;
using namespace serialization;
#include <cstring>

#include <gtest/gtest.h>

const std::string data_t::xmlName = "data_t";
const std::string ptr_container_t::xmlName = "ptr_container_t";
const std::string stack_container_t::xmlName = "stack_container_t";

template<typename T>
class TypedWriterTest : public ::testing::Test, public Writer {
public:
	virtual void writeStream(std::ostream& file) { }
};

class MockWritable : public Writable {
public:
	unsigned int writeCount;
	MockWritable() : writeCount(0) { }
	virtual ~MockWritable() { }
	virtual void write(Writer * writer) {
		++writeCount;
		writer->writeName("MockWritable");
		writer->write(writeCount, "writeCount");
	}
	virtual void read(Parser * reader) { }
};

typedef TypedWriterTest<int> WriterTest;

typedef ::testing::Types< char, unsigned char,
	short, unsigned short, int, unsigned int,
	long long int, unsigned long long int,
	float, double, long double, bool > primitives;

TYPED_TEST_CASE(TypedWriterTest, primitives);

TEST_F(WriterTest, ConstructorTest) {
	EXPECT_EQ(0, this->idList.size());
	EXPECT_EQ(0, this->root_objs.size());
}

TEST_F(WriterTest, NewObjectTest) {
	// Put an object into the writer
	MockWritable dataObj;
	Object * newObj = this->newObject(&dataObj);
	Object cleanObj;
	cleanObj.id = 0;
	EXPECT_NE(nullptr, newObj);
	EXPECT_EQ(1, this->idList.size());
	EXPECT_EQ(newObj, this->idList[&dataObj]);
	EXPECT_EQ(0, this->root_objs.size());
	EXPECT_EQ(cleanObj, *newObj);
	
	// Make sure that nothing happens when we put in nullptr
	newObj = this->newObject(nullptr);
	EXPECT_EQ(nullptr, newObj);
	EXPECT_EQ(1, this->idList.size());
	EXPECT_EQ(0, this->root_objs.size());
}

TEST_F(WriterTest, AddRootObject) {
	class NoName : public Writable {
		virtual void write(Writer * writer) override { }
		virtual void read(Parser * reader) override { }
	} no_name;
	EXPECT_THROW(this->addRootObject(&no_name), Writer::UnnamedObject);
	
	MockWritable dataObj;
	Object * newObj = this->addRootObject(&dataObj);
	EXPECT_NE(nullptr, newObj);
	EXPECT_EQ(1, this->root_objs.size());
	EXPECT_EQ(newObj, this->root_objs[0]);
	// since newObj is in root_objs, it will get cleaned up in the destructor
	
	// make sure that nothing bad happens when we pass nullptr
	newObj = this->addRootObject(nullptr);
	EXPECT_EQ(nullptr, newObj);
	EXPECT_EQ(1, this->root_objs.size());
}

TYPED_TEST(TypedWriterTest, AddPrimitiveTest) {
	TypeParam local_val1 = static_cast<TypeParam>(val1);
	TypeParam local_val2 = static_cast<TypeParam>(val2);
	TypeParam local_val3 = static_cast<TypeParam>(val3);
	TypeParam local_val4 = static_cast<TypeParam>(val4);
	
	// you cannot call writer->write() just for fun; you must be writing an object
	EXPECT_THROW(this->write(local_val1, "value"), Writer::NoCurrentObject);
	
	Object * newObj = new Object();
	this->curObj = newObj;
	
	this->write(local_val1, "value1");
	this->write(local_val2, "value2");
	this->write(local_val3, "value3");
	this->write(local_val4, "value4");
	EXPECT_EQ(4, newObj->data.size());
	Data<TypeParam> * data_ptr = dynamic_cast<Data<TypeParam>*>(newObj->data[std::string("value1")]);
	ASSERT_NE(nullptr, data_ptr);
	EXPECT_EQ(local_val1, data_ptr->data);
	data_ptr = dynamic_cast<Data<TypeParam>*>(newObj->data[std::string("value2")]);
	ASSERT_NE(nullptr, data_ptr);
	EXPECT_EQ(local_val2, data_ptr->data);
	data_ptr = dynamic_cast<Data<TypeParam>*>(newObj->data[std::string("value3")]);
	ASSERT_NE(nullptr, data_ptr);
	EXPECT_EQ(local_val3, data_ptr->data);
	data_ptr = dynamic_cast<Data<TypeParam>*>(newObj->data[std::string("value4")]);
	ASSERT_NE(nullptr, data_ptr);
	EXPECT_EQ(local_val4, data_ptr->data);
	delete newObj;
}

TEST_F(WriterTest, AddStackObjectTest) {
	MockWritable toWrite;
	EXPECT_THROW(this->write(toWrite, "toWrite"), Writer::NoCurrentObject);
	
	Object * newObj = new Object();
	this->curObj = newObj;
	
	this->write(toWrite, "toWrite");
	EXPECT_EQ(1, newObj->data.size());
	Data<Writable> * data_ptr = dynamic_cast<Data<Writable>*>(newObj->data[std::string("toWrite")]);
	ASSERT_NE(nullptr, data_ptr);
	ASSERT_EQ(1, data_ptr->data.size());
	
	Data<unsigned int> * field_ptr = dynamic_cast<Data<unsigned int>*>(data_ptr->data[std::string("writeCount")]);
	ASSERT_NE(nullptr, field_ptr);
	EXPECT_EQ(1, field_ptr->data);
	
	EXPECT_EQ(0, this->root_objs.size());
	ASSERT_EQ(1, this->idList.size());
	EXPECT_EQ(data_ptr, this->idList.at(&toWrite));
	
	delete newObj;
}

TEST_F(WriterTest, AddPointerTest) {
	MockWritable * toWrite = new MockWritable();
	EXPECT_THROW(this->write(toWrite, "toWrite"), Writer::NoCurrentObject);
	
	Object * newObj = new Object();
	this->curObj = newObj;
	
	this->write(toWrite, "toWrite");
	EXPECT_EQ(1, newObj->data.size());
	Data<Writable*> * data_ptr = dynamic_cast<Data<Writable*>*>(newObj->data[std::string("toWrite")]);
	ASSERT_NE(nullptr, data_ptr);
	ASSERT_EQ(1, root_objs.size());
	ASSERT_EQ(1, this->idList.size());
	
	Data<unsigned int> * field_ptr = dynamic_cast<Data<unsigned int>*>(this->root_objs[data_ptr->id]->data[std::string("writeCount")]);
	ASSERT_NE(nullptr, field_ptr);
	EXPECT_EQ(1, field_ptr->data);
	
	EXPECT_EQ(data_ptr->id, this->idList.at(toWrite)->id);
	delete newObj;
	delete toWrite;
}

TEST_F(WriterTest, CharStarTest) {
	const char * toWrite = "a string to write to disk";
	EXPECT_THROW(this->write(toWrite, "toWrite"), Writer::NoCurrentObject);
	
	Object * newObj = new Object();
	this->curObj = newObj;
	
	this->write(toWrite, "toWrite");
	EXPECT_EQ(1, newObj->data.size());
	Data<const char *> * data_ptr = dynamic_cast<Data<const char*>*>(newObj->data[std::string("toWrite")]);
	ASSERT_NE(nullptr, data_ptr);
	EXPECT_EQ(toWrite, data_ptr->data);
	
	EXPECT_EQ(0, this->root_objs.size());
	
	delete newObj;
}

TEST_F(WriterTest, SharedPointerTest) {
	data_t data;
	ptr_container_t containerA, containerB;
	
	containerA.data = &data;
	containerB.data = &data;
	
	this->addObject(&containerA);
	this->addObject(&containerB);
	
	EXPECT_EQ(3, this->root_objs.size());
	EXPECT_EQ(3, this->idList.size());
	
	Object * meta_A = this->idList[&containerA];
	ASSERT_NE(nullptr, meta_A);
	Object * meta_B = this->idList[&containerB];
	ASSERT_NE(nullptr, meta_B);
	
	Data<Writable*> * meta_data_A = dynamic_cast<Data<Writable*>*>(meta_A->data["data"]);
	ASSERT_NE(nullptr, meta_data_A);
	Data<Writable*> * meta_data_B = dynamic_cast<Data<Writable*>*>(meta_B->data["data"]);
	ASSERT_NE(nullptr, meta_data_B);
	EXPECT_EQ(meta_data_A->id, meta_data_B->id);
}

TEST_F(WriterTest, SharedDataTest) {
	stack_container_t containerA;
	ptr_container_t containerB;
	
	containerA.data.data = 5;
	containerB.data = &containerA.data;
	
	this->addObject(&containerA);
	this->addObject(&containerB);
	EXPECT_EQ(2, this->root_objs.size());
	EXPECT_EQ(3, this->idList.size());
	
	Object * meta_A = this->idList[&containerA];
	ASSERT_NE(nullptr, meta_A);
	Object * meta_B = this->idList[&containerB];
	ASSERT_NE(nullptr, meta_B);
	
	Data<Writable> * meta_data_A = dynamic_cast<Data<Writable>*>(meta_A->data["data"]);
	ASSERT_NE(nullptr, meta_data_A);
	Data<Writable*> * meta_data_B = dynamic_cast<Data<Writable*>*>(meta_B->data["data"]);
	ASSERT_NE(nullptr, meta_data_B);
	EXPECT_EQ(meta_data_A->id, meta_data_B->id);
}

TYPED_TEST(TypedWriterTest, AddPrimitiveArrayTest) {
	TypeParam valArray[] = {static_cast<TypeParam>(val1), static_cast<TypeParam>(val2), static_cast<TypeParam>(val3), static_cast<TypeParam>(val4)};
	
	// you cannot call writer->write() just for fun; you must be writing an object
	EXPECT_THROW(this->writeArray(valArray, sizeof(valArray)/sizeof(TypeParam), "value"), Writer::NoCurrentObject);
	
	Object * newObj = new Object();
	this->curObj = newObj;
	
	this->writeArray(valArray, sizeof(valArray)/sizeof(TypeParam), "plain_value");
	EXPECT_EQ(1, newObj->data.size());
	ArrayData<TypeParam> * data_ptr = dynamic_cast<ArrayData<TypeParam>*>(newObj->data[std::string("plain_value")]);
	ASSERT_NE(nullptr, data_ptr);
	ASSERT_EQ(sizeof(valArray)/sizeof(TypeParam), data_ptr->count);
	EXPECT_EQ(NO_HINT, data_ptr->hints);
	EXPECT_EQ(valArray, data_ptr->data);
	EXPECT_EQ(0, memcmp(valArray, data_ptr->data, sizeof(valArray)));
	// The previous line causes valgrind to complain that a conditional/jump
	// depends on an uninitialized variable.  Not sure why that is happening...
	
	this->writeArray(valArray, sizeof(valArray)/sizeof(TypeParam), "copied_value", COPY_ARRAY_HINT);
	data_ptr = dynamic_cast<ArrayData<TypeParam>*>(newObj->data[std::string("copied_value")]);
	ASSERT_NE(nullptr, data_ptr);
	ASSERT_EQ(sizeof(valArray)/sizeof(TypeParam), data_ptr->count);
	EXPECT_EQ(COPY_ARRAY_HINT, data_ptr->hints);
	EXPECT_NE(valArray, data_ptr->data);
	EXPECT_EQ(0, memcmp(valArray, data_ptr->data, sizeof(valArray)));
	
	delete newObj;
}

TEST_F(WriterTest, PointerArrayTest) {
	constexpr int arrayLen = 4;
	data_t *objArray[arrayLen];
	for( size_t i = 0; i < arrayLen; ++i ) {
		objArray[i] = new data_t();
		objArray[i]->data = static_cast<int>(i);
	}
	Object * newObj = new Object();
	this->curObj = newObj;
	
	this->writeArray(objArray, arrayLen, "objArray");
	EXPECT_EQ(1, newObj->data.size());
	EXPECT_EQ(arrayLen, this->root_objs.size());
	EXPECT_EQ(arrayLen, this->idList.size());
	
	ArrayData<Writable*> * array_ptr = dynamic_cast<ArrayData<Writable*>*>(newObj->data["objArray"]);
	EXPECT_EQ(NO_HINT, array_ptr->hints);
	ASSERT_NE(nullptr, array_ptr);
	ASSERT_EQ(arrayLen, array_ptr->count);
	for( int i = 0; i < arrayLen; ++i ) {
		size_t id = array_ptr->data[i];
		Data<Writable> * element_ptr = dynamic_cast<Data<Writable>*>(this->root_objs[id]);
		ASSERT_NE(nullptr, element_ptr);
		Data<int32_t> * data_ptr = dynamic_cast<Data<int32_t>*>(element_ptr->data["data"]);
		ASSERT_NE(nullptr, data_ptr);
		EXPECT_EQ(i, data_ptr->data);
	}
	
	delete newObj;
	// COPY_ARRAY_HINT is a no-op here, since we're copying the data into metadata anyway
	
	for( int i = 0; i < arrayLen; ++i ) {
		delete objArray[i];
	}
}
