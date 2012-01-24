//
//  eos/serialization/macros.h
//  eSerial
//
//  Created by Paul O'Neil on 3/21/11.
//  Copyright 2012 Paul O'Neil. All rights reserved.
//

#define CHAR_TYPES(macro) \
	macro( char ) \
	macro( uint8_t ) \
	macro( int8_t )

#define INTEGER_TYPES(macro) \
	macro( uint16_t ) \
	macro( uint32_t ) \
	macro( uint64_t ) \
	macro( int16_t ) \
	macro( int32_t ) \
	macro( int64_t )

#define PRIMITIVE_TYPES(macro) \
	CHAR_TYPES(macro) \
	INTEGER_TYPES(macro) \
	macro( float ) \
	macro( double ) \
	macro( long double ) \
	macro( bool )


#define PRIMITIVE_TYPE_IDENTIFIERS(macro) \
	CHAR_TYPES(macro) \
	INTEGER_TYPES(macro) \
	macro( float ) \
	macro( double ) \
	macro( long_double ) \
	macro( bool )


#define ALL_TYPES(macro) \
	PRIMITIVE_TYPES(macro) \
	macro( eWritable* )
