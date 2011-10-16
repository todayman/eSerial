//
//  b64.h
//  eSerial
//
//  Created by Paul O'Neil on 10/15/11.
//  Copyright (c) 2011 Paul O'Neil. All rights reserved.
//

#ifndef eSerial_b64_h
#define eSerial_b64_h

#ifdef __cplusplus
extern "C" {
#endif

size_t convert_to_base64(const char * data, size_t length, char ** encoded);

#ifdef __cplusplus
}
#endif


#endif
