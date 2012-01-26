//
//  eos/serialization/b64.h
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

#ifndef __EOS_SERIALIZATION_B64_H__
#define __EOS_SERIALIZATION_B64_H__

#ifdef __cplusplus
extern "C" {
#endif

  // Implementation of convert_{to,from}_base64,
  // the other functions are C(++) wrappers for these
  // to make the types and linking work
  size_t _convert_to_base64(const char * data, size_t length, char ** encoded, void*(*allocator)(size_t));
  size_t _convert_from_base64(const char * data, size_t length, char ** decoded, void*(*allocator)(size_t), size_t scale);

#ifdef __cplusplus
}

template<typename T>
static inline void * alloc(size_t count) {
	return new T[count];
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
  return _convert_to_base64((const char *) data, length * sizeof(T), encoded, alloc<char>);
}

/**
 * Converts the given textual Base64 encoding to binary data.
 *
 * \param data the data to be decoded
 * \param length the length of the data in bytes
 * \param decoded a pointer to the decoded data.  It should be released with free().
 * \return the number elements of the decoded data.
 */
template<typename T>
static inline size_t convert_from_base64(const char * data, size_t length, T ** encoded) {
  return _convert_from_base64((const char *) data, length, (char**)encoded, alloc<T>, sizeof(T)) / sizeof(T);
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
#define convert_to_base64(data, length, encoded) _convert_to_base64(data, length, encoded, malloc)

/**
 * Converts the given textual Base64 encoding to binary data.
 *
 * \param data the data to be decoded
 * \param length the length of the data in bytes
 * \param decoded a pointer to the decoded data.  It should be released with free().
 * \return the length of the decoded data in bytes
 */
#define convert_from_base64(data, length, decoded) _convert_from_base64(data, length, decoded, malloc, 1)

#endif // __cplusplus

#endif // __EOS_SERIALIZATION_B64_H__
