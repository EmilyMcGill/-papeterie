
// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "helpers/memenv/memenv.h"

#include "leveldb/env.h"
#include "leveldb/status.h"
#include "port/port.h"
#include "util/mutexlock.h"
#include <map>
#include <string.h>
#include <string>
#include <vector>

namespace leveldb {

namespace {

class FileState {
 public:
  // FileStates are reference counted. The initial reference count is zero
  // and the caller must call Ref() at least once.
  FileState() : refs_(0), size_(0) {}

  // Increase the reference count.
  void Ref() {
    MutexLock lock(&refs_mutex_);
    ++refs_;
  }

  // Decrease the reference count. Delete if this is the last reference.
  void Unref() {
    bool do_delete = false;

    {
      MutexLock lock(&refs_mutex_);
      --refs_;
      assert(refs_ >= 0);
      if (refs_ <= 0) {
        do_delete = true;
      }
    }

    if (do_delete) {
      delete this;
    }
  }

  uint64_t Size() const { return size_; }

  Status Read(uint64_t offset, size_t n, Slice* result, char* scratch) const {
    if (offset > size_) {
      return Status::IOError("Offset greater than file size.");
    }
    const uint64_t available = size_ - offset;
    if (n > available) {
      n = available;
    }
    if (n == 0) {
      *result = Slice();
      return Status::OK();
    }

    size_t block = offset / kBlockSize;
    size_t block_offset = offset % kBlockSize;

    if (n <= kBlockSize - block_offset) {
      // The requested bytes are all in the first block.
      *result = Slice(blocks_[block] + block_offset, n);
      return Status::OK();
    }

    size_t bytes_to_copy = n;
    char* dst = scratch;

    while (bytes_to_copy > 0) {
      size_t avail = kBlockSize - block_offset;
      if (avail > bytes_to_copy) {
        avail = bytes_to_copy;
      }
      memcpy(dst, blocks_[block] + block_offset, avail);

      bytes_to_copy -= avail;
      dst += avail;
      block++;
      block_offset = 0;
    }

    *result = Slice(scratch, n);
    return Status::OK();
  }

  Status Append(const Slice& data) {
    const char* src = data.data();
    size_t src_len = data.size();

    while (src_len > 0) {
      size_t avail;
      size_t offset = size_ % kBlockSize;

      if (offset != 0) {
        // There is some room in the last block.
        avail = kBlockSize - offset;
      } else {
        // No room in the last block; push new one.
        blocks_.push_back(new char[kBlockSize]);
        avail = kBlockSize;
      }