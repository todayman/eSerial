//
//  eos/serialization/XMLReader_test.cpp
//  eSerial
//
//  Created by Paul O'Neil on 1/9/12.
//  Copyright (c) 2012 Paul O'Neil. All rights reserved.
//

#include "tests_common.h"

namespace eos {
namespace serialization {

void data_t::read(Parser * reader) { }

void ptr_container_t::read(Parser * parser) { }

void stack_container_t::read(Parser * parser) { }

template<typename TypeParam>
void PrimitiveObject<TypeParam>::read(Parser * reader) { }
#define PRIMITIVE_READ_METHOD(x) template void PrimitiveObject<x>::read(Parser * reader);
PRIMITIVE_TYPES(PRIMITIVE_READ_METHOD)

template<typename TypeParam>
void PrimitiveArrayObject<TypeParam>::read(Parser * reader) { }
#define PRIMITIVE_ARRAY_READ_METHOD(x) template void PrimitiveArrayObject<x>::read(Parser * reader);
PRIMITIVE_TYPES(PRIMITIVE_ARRAY_READ_METHOD)


}
}
