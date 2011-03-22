//
//  eWritable.h
//  eSerial
//
//  Created by Paul O'Neil on 3/21/11.
//  Copyright 2011 Paul O'Neil. All rights reserved.
//

#ifndef __E_WRITEABLE_H__
#define __E_WRITEABLE_H__

#ifndef __E_WRITER_H__
class eWriter;
#endif

#ifndef __E_PARSER_H__
class eParser;
#endif

class eWritable {
public:
	virtual void write(eWriter * writer){}
	virtual void read(eParser * reader){}
};

#endif // __E_WRITEABLE__