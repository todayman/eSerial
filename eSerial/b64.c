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

size_t convert_to_base64(const char * data, size_t length, char ** encoded)
{
  if( NULL == data || NULL == encoded ) {
    return 0;
  }
  
  size_t mod_3_length = length - (length % 3); // max multiple of 3 < length
  size_t extra = length - mod_3_length;
  size_t padding_size = 3 - (length % 3); // how many bytes left until a multiple of 3?
  padding_size = (padding_size == 3 ? 0 : padding_size);
  size_t product_length = (length + extra) * 4 / 3 + padding_size;
  
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
  for( i = 0; i < extra; i++ ) {
    buffer.bytes[i] = data[src_idx + i];
  }
  for( ; i < 3; i++ ) {
    buffer.bytes[i] = 0;
  }
  
  if( extra > 0 ) { // extra = 1,2
    result[dst_idx] = get_char(buffer, 0);
    result[dst_idx + 1] = get_char(buffer, 1);
    dst_idx += 2;
  }
  if( extra > 1) { // extra = 2
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