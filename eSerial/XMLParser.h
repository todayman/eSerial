//
//  eos/serialization/XMLParser.h
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

#ifndef eSerial_XMLParser_h
#define eSerial_XMLParser_h

#include <libxml/tree.h>
#include <string>
#include <eos/serialization/Data.h>

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
