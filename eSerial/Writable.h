//
//  eos/serialization/Writable.h
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

#ifndef __EOS_SERIALIZATION_WRITEABLE_H__
#define __EOS_SERIALIZATION_WRITEABLE_H__

namespace eos {
namespace serialization {

class Writer;
class Parser;

class Writable {
public:
	virtual ~Writable() = default;
	virtual void write(Writer * writer) = 0;
	virtual void read(Parser * reader) = 0;
};

} // namespace serialization
} // namespace eos

#endif // __E_WRITEABLE__
