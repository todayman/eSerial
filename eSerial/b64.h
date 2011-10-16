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
  size_t _convert_to_base64(const char * data, size_t length, char ** encoded);
  size_t _convert_from_base64(const char * data, size_t length, char ** decoded);
#ifdef __cplusplus
}

template<typename T>
static inline size_t convert_to_base64(const T * data, size_t length, char ** encoded) {
  return _convert_to_base64((const char *) data, length * sizeof(T), encoded);
}

template<typename T>
static inline size_t convert_from_base64(const char * data, size_t length, T ** encoded) {
  return _convert_from_base64((const char *) data, length, (char**)encoded) / sizeof(T);
}
#else

#define convert_to_base64(data, length, encoded) _convert_to_base64(data, length, encoded)
#define convert_from_base64(data, length, decoded) _convert_to_base64(data, length, decoded)

#endif

#endif
