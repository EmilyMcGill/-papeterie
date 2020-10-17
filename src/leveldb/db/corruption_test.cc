// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "leveldb/db.h"

#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "leveldb/cache.h"
#include "leveldb/env.h"
#include "leveldb/table.h"
#include "leveldb/write_batch.h"
#include "db/db_impl.h"
#include "db/filename.h"
#include "db/log_format.h"
#include "db/version_set.h"
#include "util/logging.h"
#include "util/testharness.h"
#include "util/testutil.h"

namespace leveldb {

static const int kValueSize = 1000;

class CorruptionTest {
 public:
  test::ErrorEnv env_;
  std::string dbname_;
  Cache* tiny_cache_;
  Options options_;
  DB* db_;

  CorruptionTest() {
    tiny_cache_ = NewLRUCache(100);
    options_.env = &env_;
    options_.block_cache = tiny_cache_;
    dbname_ = test::TmpDir() + "/db_test";
    DestroyDB(dbname_, options_);

    db_ = NULL;
    options_.create_if_missing = true;
    Reopen();
    options_.create_if_missing = false;
  }

  ~CorruptionTest() {
     delete db_;
     DestroyDB(dbname_, Options());
     delete tiny_cache_;
  }

  Status TryReopen() {
    delete db_;
    db_ = NULL;
    return DB::Open(options_, dbname_, &db_);
  }

  void Reopen() {
    ASSERT_OK(TryReopen());
  }

  void RepairDB() {
    delete db_;
    db_ = NULL;
    ASSERT_OK(::leveldb::RepairDB(dbname_, options_));
  }

  void Build(int n) {
    std::string key_space, value_space;
    WriteBatch batch;
    for (int i = 0; i < n; i++) {
