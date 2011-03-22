//
//  eData.h
//  eSerial
//
//  Created by Paul O'Neil on 3/21/11.
//  Copyright 2011 Paul O'Neil. All rights reserved.
//

#include <map>

#ifndef __E_DATA_H__
#define __E_DATA_H__

class _EOSData {
protected:
	virtual size_t getData()=0;// to enable polymorphism
};
typedef _EOSData * pEOSData;

template<typename T> class EOSData : public _EOSData {
protected:
	virtual size_t getData() { return 0; } // to enable polymorphism
public:
	EOSData(T d=NULL) { data = d; }
	T data;
};

template<> class EOSData<eWritable*> : public _EOSData {
protected:
	virtual size_t getData() { return i; } // to enable polymorphism
public:
	EOSData(size_t ident=0) {i = ident; }
	
	size_t i;
};

template<typename T> class EOSArrayData : public _EOSData {
protected:
	virtual size_t getData() { return 0; } // to enable polymorphism
public:
	uint32_t count;
	T * data;
};

template<> class EOSArrayData<eWritable*> : public _EOSData {
protected:
	virtual size_t getData() { return 0; } // to enable polymorphism
public:
	uint32_t count;
	uint32_t * data;
};

struct EOSClass {
	std::string name;
	std::map<std::string, pEOSData> data;
	std::map<std::string, EOSClass*> superclasses;
};

struct EOSObject {
	size_t i;
	EOSClass * data;
};

#endif // __E_DATA_H__