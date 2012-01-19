//
//  XMLParser.h
//  eSerial
//
//  Created by Paul O'Neil on 1/9/12.
//  Copyright (c) 2012 Paul O'Neil. All rights reserved.
//

#ifndef eSerial_XMLParser_h
#define eSerial_XMLParser_h

#include <libxml/tree.h>
#include <string>
#include "Data.h"

namespace eos {
namespace serialization {

class XMLParser : public Parser
{
private:
  Object * parseXMLObject(xmlNodePtr node);
  void parseXMLField(xmlNodePtr field, Object * obj);
protected:
  virtual void firstPass(std::istream& input) override;
  
public:
  XMLParser() = default;
  virtual ~XMLParser() { }
};

}
}
#endif
