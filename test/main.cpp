//
//  main.cpp
//  test
//
//  Created by Paul O'Neil on 10/15/11.
//  Copyright (c) 2011 Paul O'Neil. All rights reserved.
//

#include <iostream>
using namespace std;
#include "Writable.h"
#include "Writer.h"
using namespace eos::serialization;

int theArray[] = {12,34,743,63,2};

class Car : public Writable {
  uint8_t _passengers;
  uint16_t _seats;
  float _mpg;
  
public:
  Car(uint8_t p, uint8_t s, float m) : _passengers(p), _seats(s), _mpg(m) {}
  virtual void write(Writer * writer) const override {
    writer->writeName("Car");
    writer->write(_passengers, "passengers");
    writer->write(_seats, "seats");
    writer->write(_mpg, "mpg");
    writer->writeArray(theArray, sizeof(theArray)/sizeof(theArray[0]), "theArray!", READABLE_HINT);
    writer->write("A message in a bottle!", "msg");
  }
  
  virtual void read(Parser * parser) override {
  }
};

class Car_Container : public Writable {
  Car c;
public:
  Car_Container() : c(1,2,3.0) { }
  virtual void write(Writer * writer) const override {
    writer->writeName("Car_Container");
    writer->write(c, "car");
  }
  
  virtual void read(Parser * reader) override {
  }
};


int main (int argc, const char * argv[])
{
  /*char input[2000];
  cin.getline(input, 2000);
  int * dst1;
  size_t len = convert_from_base64(input, strlen(input), &dst1);
  for( size_t i = 0; i < len; i++ ) 
    cout << dst1[i] << ", ";
  cout << endl;
  delete []dst1;
  
  char * dst2;
  const int intArray[] = {1,2,3,4,5,6,7,8,9,12,11,12,13,14,15,16,367532};
  cout << "size = " << sizeof(intArray)/sizeof(intArray[0]) << endl;
  convert_to_base64(intArray, sizeof(intArray)/sizeof(intArray[0]), &dst2);
  cout << dst2 << endl;
  delete []dst2;*/
  /*xmlDocPtr doc;
  doc = xmlNewDoc((const xmlChar*)"1.0");
  
  doc->encoding = xmlStrdup((const xmlChar*)"UTF-8");
  doc->xmlRootNode = xmlNewDocNode(doc, nullptr, (const xmlChar *)"eSerial", nullptr);
  
  xmlNodePtr tree = xmlNewChild(doc->xmlRootNode, nullptr, (const xmlChar*)"data", (const xmlChar*)"<l&ook!");
  
  xmlSaveFormatFileEnc("test.xml", doc, "UTF-8", 1);
  xmlFreeDoc(doc);*/
  
  // insert code here...
  Car c(1,2,3.0);
  Car_Container cc;
  Writer * writer = Writer::newXMLWriter();
  writer->addObject(&cc);
  writer->writeFile("testFile.xml");
  delete writer;
  return 0;
}

