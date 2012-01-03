//
//  Writer_tests.cpp
//  eSerial
//
//  Created by Paul O'Neil on 12/24/11.
//  Copyright (c) 2011 Paul O'Neil. All rights reserved.
//

#include "Writer.h"
using namespace eos;
using namespace serialization;

#include <gtest/gtest.h>

template<typename T>
class TypedWriterTest : public ::testing::Test, public Writer {
public:
	virtual void writeFile(const std::string& file) { }
};

class MockWritable : public Writable {
public:
	unsigned int writeCount;
	unsigned int readCount;
	virtual ~MockWritable() { }
	virtual void write(Writer * writer) const {}
	virtual void read(Parser * reader) {}
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
	// since newObj is not in the root_objs collection, it does not
	// get cleaned up in the destructor.
	delete newObj;
	this->idList.clear();

	// Make sure that nothing happens when we put in nullptr
	newObj = this->newObject(nullptr);
	EXPECT_EQ(nullptr, newObj);
	EXPECT_EQ(0, this->idList.size());
	EXPECT_EQ(0, this->root_objs.size());
}

TEST_F(WriterTest, AddObjectGetID) {
	MockWritable dataObj;
	Object * newObj = this->addObjectGetID(&dataObj);
	EXPECT_NE(nullptr, newObj);
	EXPECT_EQ(1, this->root_objs.size());
	EXPECT_EQ(newObj, this->root_objs[0]);
	// since newObj is in root_objs, it will get cleaned up in the destructor
	
	// make sure that nothing bad happens when we pass nullptr
	newObj = this->addObjectGetID(nullptr);
	EXPECT_EQ(nullptr, newObj);
	EXPECT_EQ(1, this->root_objs.size());
}

TYPED_TEST(TypedWriterTest, AddPrimitiveTest) {
	// you cannot call writer->write() just for fun; you must be writing an object
	EXPECT_THROW(this->write(static_cast<TypeParam>(0), "value"), Writer::NoCurrentObject);
	
	Object * newObj = new Object();
	this->curObj = newObj;
	
	TypeParam val1 = 0;
	TypeParam val2 = 1;
	TypeParam val3 = 10;
	TypeParam val4 = 127;
	
	this->write(val1, "value1");
	this->write(val2, "value2");
	this->write(val3, "value3");
	this->write(val4, "value4");
	EXPECT_EQ(4, newObj->data.size());
	Data<TypeParam> * data_ptr = dynamic_cast<Data<TypeParam>*>(newObj->data[std::string("value1")]);
	ASSERT_NE(nullptr, data_ptr);
	EXPECT_EQ(val1, data_ptr->data);
	data_ptr = dynamic_cast<Data<TypeParam>*>(newObj->data[std::string("value2")]);
	ASSERT_NE(nullptr, data_ptr);
	EXPECT_EQ(val2, data_ptr->data);
	data_ptr = dynamic_cast<Data<TypeParam>*>(newObj->data[std::string("value3")]);
	ASSERT_NE(nullptr, data_ptr);
	EXPECT_EQ(val3, data_ptr->data);
	data_ptr = dynamic_cast<Data<TypeParam>*>(newObj->data[std::string("value4")]);
	ASSERT_NE(nullptr, data_ptr);
	EXPECT_EQ(val4, data_ptr->data);
	delete newObj;
}

TEST_F(WriterTest, AddStackObjectTest) {
	//EXPECT_THROW(this->write
}

TEST_F(WriterTest, AddPointerTest) {
	
}
