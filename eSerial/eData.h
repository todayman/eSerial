//
//  eData.h
//  eSerial
//
//  Created by Paul O'Neil on 3/21/11.
//  Copyright 2011 Paul O'Neil. All rights reserved.
//

#include <map>
#include <string>

#ifndef __E_DATA_H__
#define __E_DATA_H__

class eWritable;

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

typedef uint32_t hint_t;

constexpr hint_t NO_HINT        = 0;
constexpr hint_t READABLE_HINT  = 1 << 0;
constexpr hint_t BINARY_HINT    = READABLE_HINT << 1;

template<typename T> class EOSArrayData : public _EOSData {
public:
	size_t count;
	T * data;
  hint_t hints;
  
  EOSArrayData() : count(0), data(NULL), hints(NO_HINT) { }
  EOSArrayData(size_t c, T * d, hint_t h) : count(c), data(d), hints(h) { }
};

template<> class EOSArrayData<eWritable*> : public _EOSData {
public:
	size_t count;
	size_t * data;
  hint_t hints;
  
  EOSArrayData() : count(0), data(NULL), hints(NO_HINT) { }
  EOSArrayData(size_t c, size_t * d, hint_t h) : count(c), data(d), hints(h) { }
  ~EOSArrayData() {
    delete [] data;
  }
};

struct EOSObject {
	size_t i;
  std::string name;
	std::map<std::string, pEOSData> data;
};


#endif // __E_DATA_H__