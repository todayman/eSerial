//
//  b64_tests.c
//  eSerial
//
//  Created by Paul O'Neil on 12/22/11.
//  Copyright (c) 2011 Paul O'Neil. All rights reserved.
//
#include <stdlib.h>
#include <stdio.h>
#include <CUnit/CUnit.h>

#include "b64.h"
#include "unittests.h"

static const char text[] = "Man is distinguished, not only by his reason, but by this singular passion from other animals, which is a lust of the mind, that by a perseverance of delight in the continued and indefatigable generation of knowledge, exceeds the short vehemence of any carnal pleasure.";
static const char data[] = "TWFuIGlzIGRpc3Rpbmd1aXNoZWQsIG5vdCBvbmx5IGJ5IGhpcyByZWFzb24sIGJ1dCBieSB0aGlzIHNpbmd1bGFyIHBhc3Npb24gZnJvbSBvdGhlciBhbmltYWxzLCB3aGljaCBpcyBhIGx1c3Qgb2YgdGhlIG1pbmQsIHRoYXQgYnkgYSBwZXJzZXZlcmFuY2Ugb2YgZGVsaWdodCBpbiB0aGUgY29udGludWVkIGFuZCBpbmRlZmF0aWdhYmxlIGdlbmVyYXRpb24gb2Yga25vd2xlZGdlLCBleGNlZWRzIHRoZSBzaG9ydCB2ZWhlbWVuY2Ugb2YgYW55IGNhcm5hbCBwbGVhc3VyZS4=";

static void test_encode(void) {
	char * encoded;
	// length of the array includes the null terminator, which isn't part of the data
	convert_to_base64(text, sizeof(text) - 1, &encoded);
	CU_ASSERT_STRING_EQUAL(encoded, data);
	free(encoded);
}

static void test_decode(void) {
	char * decoded;
	// length of the array includes the null terminator, which isn't part of the data
	convert_from_base64(data, sizeof(data) - 1, &decoded);
	CU_ASSERT_STRING_EQUAL(decoded, text);
	free(decoded);
}

int add_b64_tests(void) {
  CU_pSuite suite = CU_add_suite("Base64 Encoding Suite", NULL, NULL);
  if( NULL == suite ) {
    return -1;
  }
  
  if( (NULL == CU_add_test(suite, "To Base64", test_encode)) ||
		 (NULL == CU_add_test(suite, "From Base64", test_decode))
     )
  {
    return -1;
  }
  
  return 0;
}
