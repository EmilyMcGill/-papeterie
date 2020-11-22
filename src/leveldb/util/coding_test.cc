// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "util/coding.h"

#include "util/testharness.h"

namespace leveldb {

class Coding { };

TEST(Coding, Fixed32) {
  std::string s;
  for (uint32_t v = 0; v < 100000; v++) {
    PutFixed32(&s, v);
  }

  const char* p = s.data();
  for (uint32_t v = 0; v < 100000; v++) {
    uint32_t actual = DecodeFixed32(p);
    ASSERT_EQ(v, actual);
    p += sizeof(uint32_t);
  }
}

TEST(Coding, Fixed64) {
  std::string s;
  for (int power = 0; power <= 63; power++) {
    uint64_t v = static_cast<uint64_t>(1) << power;
    PutFixed64(&s, v - 1);
    PutFixed64(&s, v + 0);
    PutFixed64(&s, v + 1);
  }

  const char* p = s.data();
  for (int power = 0; power <= 63; power++) {
    uint64_t v = static_cast<uint64_t>(1) << power;
    uint64_t actual;
    actual = DecodeFixed64(p);
    ASSERT_EQ(v-1, actual);
    p += sizeof(uint64_t);

    actual = DecodeFixed64(p);
    ASSERT_EQ(v+0, actual);
    p += sizeof(uint64_t);

    actual = DecodeFixed64(p);
    ASSERT_EQ(v+1, actual);
    p += sizeof(uint64_t);
  }
}

// Test that encoding routines generate little-endian encodings
TEST(Coding, EncodingOutput) {
  std::string dst;
  PutFixed32(&dst, 0x04030201);
  ASSERT_EQ(4, dst.size());