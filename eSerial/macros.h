//
//  eos/serialization/macros.h
//
//  Copyright 2012 Paul O'Neil.
//
//  This file is part of eSerial.
//
//  eSerial is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  eSerial is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with eSerial.  If not, see <http://www.gnu.org/licenses/>.
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
