//
//  eos/serialization/XMLWriter.h
//  eSerial
//
//  Created by Paul O'Neil on 1/6/12.
//  Copyright (c) 2012 Paul O'Neil. All rights reserved.
//

#ifndef eSerial_XMLWriter_h
#define eSerial_XMLWriter_h

#include "Writer.h"
#include <libxml/tree.h>
#include <iostream>

namespace eos {
namespace serialization {

class XMLWriter : public Writer {
  xmlNodePtr addToXML(Object * obj, xmlNodePtr parent);
  
  template<typename T>
  xmlNodePtr writeArrayToXML(xmlNodePtr node, ArrayData<T> * data, const char * type);
  
public:
  XMLWriter() = default;
	virtual void writeStream(std::ostream& output) override;
};

}
}
#endif
