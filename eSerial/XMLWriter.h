//
//  eos/serialization/XMLWriter.h
//
//  Copyright 2012 Paul O'Neil.
//
//  This file is part of eSerial.
//
//  eSerial is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  eSerial is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with eSerial.  If not, see <http://www.gnu.org/licenses/>.
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
