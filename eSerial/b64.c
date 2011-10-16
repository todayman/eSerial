//
//  b64.c
//  eSerial
//
//  Created by Paul O'Neil on 10/15/11.
//  Copyright (c) 2011 Paul O'Neil. All rights reserved.
//

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>

#include "b64.h"

static char toB64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static char fromB64[] = {
	['A'] = 0,
	['B'] = 1,
	['C'] = 2,
	['D'] = 3,
	['E'] = 4,
	['F'] = 5,
	['G'] = 6,
	['H'] = 7,
	['I'] = 8,
	['J'] = 9,
	['K'] = 10,
	['L'] = 11,
	['M'] = 12,
	['N'] = 13,
	['O'] = 14,
	['P'] = 15,
	['Q'] = 16,
	['R'] = 17,
	['S'] = 18,
	['T'] = 19,
	['U'] = 20,
	['V'] = 21,
	['W'] = 22,
	['X'] = 23,
	['Y'] = 24,
	['Z'] = 25,
	['a'] = 26,
	['b'] = 27,
	['c'] = 28,
	['d'] = 29,
	['e'] = 30,
	['f'] = 31,
	['g'] = 32,
	['h'] = 33,
	['i'] = 34,
	['j'] = 35,
	['k'] = 36,
	['l'] = 37,
	['m'] = 38,
	['n'] = 39,
	['o'] = 40,
	['p'] = 41,
	['q'] = 42,
	['r'] = 43,
	['s'] = 44,
	['t'] = 45,
	['u'] = 46,
	['v'] = 47,
	['w'] = 48,
	['x'] = 49,
	['y'] = 50,
	['z'] = 51,
	['0'] = 52,
	['1'] = 53,
	['2'] = 54,
	['3'] = 55,
	['4'] = 56,
	['5'] = 57,
	['6'] = 58,
	['7'] = 59,
	['8'] = 60,
	['9'] = 61,
	['+'] = 62,
	['/'] = 63
};

typedef union {
  char bytes[3];
  uint32_t bits;
} base64_conversion_t;

static inline char get_char(base64_conversion_t data, unsigned int idx)
{
  char result;
  int helper1, helper2;
  switch( idx ) {
    case 0:
      result = (data.bits & 0xFC) >> 2;
      break;
    case 1:
      helper1 = (data.bits & 0x0000F000);
      helper1 >>= 12;
      helper2 = data.bits & 0x03;
      helper2 <<= 4;
      result = helper1 | helper2;
      break;
    case 2:
      result = ((((data.bits & 0x00C00000) >> 14) | ((data.bits & 0x0F00) << 2)) >> 8);
      break;
    case 3:
      result = (data.bits & 0x003F0000) >> 16;
      break;
    default:
      return 0;
  }
  return toB64[result];
}

static inline void set_char(const char in[4], char * out)
{
  base64_conversion_t buff;
  buff.bits = 0;
  buff.bits |= (fromB64[in[0]] << 2);
  
  uint32_t val = fromB64[in[1]];
  buff.bits |= ((val & 0x30) >> 4);
  buff.bits |= ((val & 0x0F) << 12);
  
  val = fromB64[in[2]];
  buff.bits |= ((val & 0x3C) << 6);
  buff.bits |= ((val & 0x03) << 22);
  
  val = fromB64[in[3]];
  buff.bits |= (val << 16);
  
  memcpy(out, buff.bytes, 3);
}

size_t _convert_to_base64(const char * data, size_t length, char ** encoded)
{
  if( NULL == data || NULL == encoded ) {
    return 0;
  }
  
  size_t remainder = length % 3;
  size_t mod_3_length = length - remainder; // max multiple of 3 < length
  size_t padding_size = 3 - (length % 3); // how many bytes left until a multiple of 3?
  padding_size = (padding_size == 3 ? 0 : padding_size);
  size_t product_length = (length + remainder) * 4 / 3 + padding_size;
  
  char * result = malloc(product_length * 4 / 3 + 1);
  if( NULL == result ) {
    return 0;
  }
  
  size_t src_idx, dst_idx;
  base64_conversion_t buffer;
  for( src_idx = 0, dst_idx = 0; src_idx < mod_3_length; src_idx += 3, dst_idx += 4 ) {
    memcpy(buffer.bytes, data + src_idx, 3);
    
    for( char i = 0; i < 4; i++ ) {
      result[dst_idx + i] = get_char(buffer, i);
    }
  }
  
  unsigned char i;
  for( i = 0; i < remainder; i++ ) {
    buffer.bytes[i] = data[src_idx + i];
  }
  for( ; i < 3; i++ ) {
    buffer.bytes[i] = 0;
  }
  
  if( remainder > 0 ) { // remainder = 1,2
    result[dst_idx] = get_char(buffer, 0);
    result[dst_idx + 1] = get_char(buffer, 1);
    dst_idx += 2;
  }
  if( remainder > 1) { // remainder = 2
    result[dst_idx] = get_char(buffer, 2);
    dst_idx += 1;
  }
  
  for( i = 0; i < padding_size; i++ ) {
    result[dst_idx] = '=';
    dst_idx += 1;
  }
  
  result[product_length] = 0;
  (*encoded) = result;
  return product_length;
}

size_t _convert_from_base64(const char * data, size_t length, char ** decoded)
{
  if( NULL == data || NULL == decoded ) {
    return 0;
  }
  
  size_t data_length = length;
  // examine the last 1 or 2 bytes to see how long the data actually is...
  for( data_length = length; data[data_length - 1] == '='; data_length -- ) ;
  
  size_t remainder = (data_length % 4);
  size_t mod_4_length = data_length - remainder;
  size_t padding_size = length - data_length;
  size_t product_size = mod_4_length * 3 / 4 + (padding_size ? 3 - padding_size : 0);
  
  char * result = malloc(product_size + 1);
  
  size_t src_idx, dst_idx;
  for( src_idx = dst_idx = 0; src_idx < mod_4_length; src_idx += 4, dst_idx += 3 ) {
    set_char(data + src_idx, result + dst_idx);
  }
  
  char buffer[4];
  char outBuffer[3];
  size_t i;
  for( i = 0; i < remainder; i++ ) {
    buffer[i] = data[src_idx + i];
  }
  for( ; i < 4; i++ ) {
    buffer[i] = toB64[0];
  }
  set_char(buffer, outBuffer);
  for( i = 0; i + dst_idx < product_size; i++ ) {
    result[dst_idx + i] = outBuffer[i];
  }
  
  result[product_size] = 0;
  (*decoded) = result;
  return product_size;
}