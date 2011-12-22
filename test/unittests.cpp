//
//  unittests.cpp
//  eSerial
//
//  Created by Paul O'Neil on 12/22/11.
//  Copyright (c) 2011 Paul O'Neil. All rights reserved.
//

#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>

#include "unittests.h"

int main()
{
  if( CUE_SUCCESS != CU_initialize_registry() ) {
    return CU_get_error();
  }
  
  if( add_b64_tests() ) {
    CU_cleanup_registry();
    return CU_get_error();
  }
  
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  CU_cleanup_registry();
  return CU_get_error();
}
