/*
 *  eSerial.cp
 *  eSerial
 *
 *  Created by Paul O'Neil on 3/21/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include <iostream>
#include "eSerial.h"
#include "eSerialPriv.h"

void eSerial::HelloWorld(const char * s)
{
	 eSerialPriv *theObj = new eSerialPriv;
	 theObj->HelloWorldPriv(s);
	 delete theObj;
};

void eSerialPriv::HelloWorldPriv(const char * s) 
{
	std::cout << s << std::endl;
};

