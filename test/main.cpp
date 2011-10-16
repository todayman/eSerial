//
//  main.cpp
//  test
//
//  Created by Paul O'Neil on 10/15/11.
//  Copyright (c) 2011 Paul O'Neil. All rights reserved.
//

/*
 Man is distinguished, not only by his reason, but by this singular passion from other animals, which is a lust of the mind, that by a perseverance of delight in the continued and indefatigable generation of knowledge, exceeds the short vehemence of any carnal pleasure.
 TWFuIGlzIGRpc3Rpbmd1aXNoZWQsIG5vdCBvbmx5IGJ5IGhpcyByZWFzb24sIGJ1dCBieSB0aGlzIHNpbmd1bGFyIHBhc3Npb24gZnJvbSBvdGhlciBhbmltYWxzLCB3aGljaCBpcyBhIGx1c3Qgb2YgdGhlIG1pbmQsIHRoYXQgYnkgYSBwZXJzZXZlcmFuY2Ugb2YgZGVsaWdodCBpbiB0aGUgY29udGludWVkIGFuZCBpbmRlZmF0aWdhYmxlIGdlbmVyYXRpb24gb2Yga25vd2xlZGdlLCBleGNlZWRzIHRoZSBzaG9ydCB2ZWhlbWVuY2Ugb2YgYW55IGNhcm5hbCBwbGVhc3VyZS4=
*/
#include <libxml/tree.h>
#include <iostream>
using namespace std;
#include "b64.h"

int main (int argc, const char * argv[])
{
  char input[2000];
  cin.getline(input, 2000);
  char * dst;
  convert_from_base64(input, strlen(input), &dst);
  cout << dst << endl;
  /*xmlDocPtr doc;
  doc = xmlNewDoc((const xmlChar*)"1.0");
  
  doc->encoding = xmlStrdup((const xmlChar*)"UTF-8");
  doc->xmlRootNode = xmlNewDocNode(doc, NULL, (const xmlChar *)"eSerial", NULL);
  
  xmlNodePtr tree = xmlNewChild(doc->xmlRootNode, NULL, (const xmlChar*)"data", (const xmlChar*)"<l&ook!");
  
  xmlSaveFormatFileEnc("test.xml", doc, "UTF-8", 1);
  xmlFreeDoc(doc);
  return 0;*/
}

