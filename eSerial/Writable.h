//
//  eWritable.h
//  eSerial
//
//  Created by Paul O'Neil on 3/21/11.
//  Copyright 2011 Paul O'Neil. All rights reserved.
//

#ifndef __E_WRITEABLE_H__
#define __E_WRITEABLE_H__

namespace eos {
namespace serialization {

class Writer;
class Parser;

class Writable {
public:
	virtual void write(Writer * writer) = 0;
	virtual void read(Parser * reader) = 0;
};

} // namespace serialization
} // namespace eos

#endif // __E_WRITEABLE__