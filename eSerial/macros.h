//
//  macros.h
//  eSerial
//
//  Created by Paul O'Neil on 3/21/11.
//  Copyright 2011 Paul O'Neil. All rights reserved.
//

#define PRIMITIVE_TYPES(macro) \
	macro( char ) \
	macro( uint8_t ) \
	macro( uint16_t ) \
	macro( uint32_t ) \
	macro( uint64_t ) \
	macro( int8_t ) \
	macro( int16_t ) \
	macro( int32_t ) \
	macro( int64_t ) \
	macro( float ) \
	macro( double ) \
	macro( long double ) \
	macro( bool )

#define ALL_TYPES(macro) \
	PRIMITIVE_TYPES(macro) \
	macro( eWritable* )
