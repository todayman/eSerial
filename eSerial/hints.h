//
//  eos/serialization/hints.h
//  eSerial
//
//  Created by Paul O'Neil on 11/12/11.
//  Copyright (c) 2012 Paul O'Neil. All rights reserved.
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
