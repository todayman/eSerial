//
//  eos/serialization/hints.h
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

#ifndef __EOS_SERIALIZATION_HINTS_H__
#define __EOS_SERIALIZATION_HINTS_H__

namespace eos {
namespace serialization {

typedef uint32_t hint_t;

constexpr hint_t NO_HINT        = 0;
//! Hints that the data should be stored in a human-readable format
constexpr hint_t READABLE_HINT  = 1 << 0;
//! Hints that the data should be stored in a binary format to conserve space
constexpr hint_t BINARY_HINT    = READABLE_HINT << 1;
//! Hints that the array should be copied, not referenced, before serialization
constexpr hint_t COPY_ARRAY_HINT= BINARY_HINT << 1;
//! Hints that the data should be freed with free(), not delete[]
constexpr hint_t FREE_HINT      = COPY_ARRAY_HINT << 1;

} // namespace serialization
} // namespace eos


#endif // __EOS_SERIALIZATION_HINTS_H__
