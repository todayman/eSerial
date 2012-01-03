//
//  b64_tests.c
//  eSerial
//
//  Created by Paul O'Neil on 12/22/11.
//  Copyright (c) 2011 Paul O'Neil. All rights reserved.
//

#include <gtest/gtest.h>

#include "b64.h"

static const char text[] = "Man is distinguished, not only by his reason, but by this singular passion from other animals, which is a lust of the mind, that by a perseverance of delight in the continued and indefatigable generation of knowledge, exceeds the short vehemence of any carnal pleasure.";
static const char data[] = "TWFuIGlzIGRpc3Rpbmd1aXNoZWQsIG5vdCBvbmx5IGJ5IGhpcyByZWFzb24sIGJ1dCBieSB0aGlzIHNpbmd1bGFyIHBhc3Npb24gZnJvbSBvdGhlciBhbmltYWxzLCB3aGljaCBpcyBhIGx1c3Qgb2YgdGhlIG1pbmQsIHRoYXQgYnkgYSBwZXJzZXZlcmFuY2Ugb2YgZGVsaWdodCBpbiB0aGUgY29udGludWVkIGFuZCBpbmRlZmF0aWdhYmxlIGdlbmVyYXRpb24gb2Yga25vd2xlZGdlLCBleGNlZWRzIHRoZSBzaG9ydCB2ZWhlbWVuY2Ugb2YgYW55IGNhcm5hbCBwbGVhc3VyZS4=";

TEST(Base64Test, Base64EncodingTest) {
	char * encoded;
	// length of the array includes the null terminator, which isn't part of the data
	convert_to_base64(text, sizeof(text) - 1, &encoded);
	EXPECT_EQ(memcmp(encoded, data, sizeof(data)-1), 0);
	free(encoded);
}

TEST(Base64Test, Base64DecodingTest) {
	char * decoded;
	// length of the array includes the null terminator, which isn't part of the data
	convert_from_base64(data, sizeof(data) - 1, &decoded);
	EXPECT_EQ(memcmp(decoded, text, sizeof(text) - 1), 0);
	free(decoded);
}
