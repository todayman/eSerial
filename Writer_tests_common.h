//
//  Writer_tests_common.h
//  eSerial
//
//  Created by Paul O'Neil on 1/6/12.
//  Copyright (c) 2012 Paul O'Neil. All rights reserved.
//

#include <string>

#include "Writable.h"

#ifndef eSerial_Writer_tests_common_h
#define eSerial_Writer_tests_common_h

class data_t : public eos::serialization::Writable {
public:
	static const std::string xmlName;
	int32_t data;
	void write(eos::serialization::Writer * writer) override {
		writer->writeName(xmlName);
		writer->write(data, "data");
	}
	void read(eos::serialization::Parser * parser) override {	}
};

struct ptr_container_t : public eos::serialization::Writable {
public:
	static const std::string xmlName;
	data_t * data;
	virtual void write(eos::serialization::Writer * writer) override {
		writer->writeName(xmlName);
		writer->write(data, "data");
	}
	virtual void read(eos::serialization::Parser * parser) override { }
};

struct stack_container_t : public eos::serialization::Writable {
public:
	static const std::string xmlName;
	data_t data;
	virtual void write(eos::serialization::Writer * writer) override {
		writer->writeName(xmlName);
		writer->write(data, "data");
	}
	virtual void read(eos::serialization::Parser * parser) override { }
};

extern char val1;
extern char val2;
extern char val3;
extern float val4;

#endif
