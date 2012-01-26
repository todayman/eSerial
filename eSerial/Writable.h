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
//! \file
//! \brief Contains the interface for all serializable objects
//! \author Paul O'Neil <redballoon36@gmail.com>

#ifndef __EOS_SERIALIZATION_WRITEABLE_H__
#define __EOS_SERIALIZATION_WRITEABLE_H__

namespace eos {
	namespace serialization {

		class Writer;
		class Parser;

		//! An object that can be written to disk.
		class Writable {
		public:
			virtual ~Writable() = default;
			
			//! Writes this object to the given writer. Subclasses
			//! must implement both the read() and write() methods.
			//! \param writer the target to write to
			virtual void write(Writer * writer) = 0;
			
			//! Reads / Initializes this object from the given reader.
			//! \param reader the source for this object
			virtual void read(Parser * reader) = 0;
		};

	} // namespace serialization
} // namespace eos

#endif // __E_WRITEABLE__
