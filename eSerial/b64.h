//
//  eos/serialization/b64.h
//  eSerial
//
//  Created by Paul O'Neil on 10/15/11.
//  Copyright (c) 2012 Paul O'Neil. All rights reserved.
//

#ifndef __EOS_SERIALIZATION_B64_H__
#define __EOS_SERIALIZATION_B64_H__

#ifdef __cplusplus
extern "C" {
#endif

  // Implementation of convert_{to,from}_base64,
  // the other functions are C(++) wrappers for these
  // to make the types and linking work
  size_t _convert_to_base64(const char * data, size_t length, char ** encoded);
  size_t _convert_from_base64(const char * data, size_t length, char ** decoded);

#ifdef __cplusplus
}

/**
 * Converts the given binary data to the textual Base64 encoding.
 *
 * \param data the data to be encoded
 * \param length the number of elements in data
 * \param encoded a pointer to the encoded data.  It should be released with free().
 * \return the number of bytes of the encoded data
 */
template<typename T>
static inline size_t convert_to_base64(const T * data, size_t length, char ** encoded) {
  return _convert_to_base64((const char *) data, length * sizeof(T), encoded);
}

/**
 * Converts the given textual Base64 encoding to binary data.
 *
 * \param data the data to be decoded
 * \param length the length of the data in bytes
 * \param encoded a pointer to the decoded data.  It should be released with free().
 * \return the number elements of the decoded data.
 */
template<typename T>
static inline size_t convert_from_base64(const char * data, size_t length, T ** encoded) {
  return _convert_from_base64((const char *) data, length, (char**)encoded) / sizeof(T);
}
#else

/**
 * Converts the given binary data to the textual Base64 encoding.
 *
 * \param data the data to be encoded
 * \param length the length of the data in bytes
 * \param encoded a pointer to the encoded data.  It should be released with free().
 * \return the number of bytes of the encoded data
 */
#define convert_to_base64(data, length, encoded) _convert_to_base64(data, length, encoded)

/**
 * Converts the given textual Base64 encoding to binary data.
 *
 * \param data the data to be decoded
 * \param length the length of the data in bytes
 * \param encoded a pointer to the decoded data.  It should be released with free().
 * \return the length of the decoded data in bytes
 */
#define convert_from_base64(data, length, decoded) _convert_from_base64(data, length, decoded)

#endif // __cplusplus

#endif // __EOS_SERIALIZATION_B64_H__
