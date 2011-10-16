//
//  eData.h
//  eSerial
//
//  Created by Paul O'Neil on 3/21/11.
//  Copyright 2011 Paul O'Neil. All rights reserved.
//

#include <map>

#ifndef __E_WRITEABLE_H__
class eWritable;
#endif

#ifndef __E_DATA_H__
#define __E_DATA_H__

class _EOSData {
public:
  virtual ~_EOSData() { }
};
typedef _EOSData * pEOSData;

template<typename T> class EOSData : public _EOSData {
public:
	EOSData(T d=NULL) { data = d; }
	T data;
};

template<> class EOSData<eWritable*> : public _EOSData {
public:
	EOSData(size_t ident=0) {i = ident; }
	
	size_t i;
};

template<typename T> class EOSArrayData : public _EOSData {
public:
	size_t count;
	T * data;
  
  EOSArrayData() : count(0), data(NULL) { }
  EOSArrayData(size_t c, T * d) : count(c), data(d) { }
};

template<> class EOSArrayData<eWritable*> : public _EOSData {
public:
	size_t count;
	size_t * data;
};

struct EOSObject {
	size_t i;
  std::string name;
	std::map<std::string, pEOSData> data;
};

typedef eWritable * (^constructor_t)(/*EOSClass * data*/) ;

#endif // __E_DATA_H__