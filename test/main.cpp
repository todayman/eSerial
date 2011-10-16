//
//  main.cpp
//  test
//
//  Created by Paul O'Neil on 10/15/11.
//  Copyright (c) 2011 Paul O'Neil. All rights reserved.
//

#include <libxml/tree.h>
#include <iostream>
using namespace std;
#include "b64.h"

int main (int argc, const char * argv[])
{
  char input[2000];
  cin.getline(input, 2000);
  char * dst;
  convert_to_base64(input, strlen(input), &dst);
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

