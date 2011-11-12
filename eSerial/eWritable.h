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

#ifndef __E_WRITER_H__
class Writer;
#endif

#ifndef __E_PARSER_H__
class Parser;
#endif

class Writable {
public:
	virtual void write(Writer * writer) { }
	virtual void read(Parser * reader) { }
};

} // namespace serialization
} // namespace eos

#endif // __E_WRITEABLE__