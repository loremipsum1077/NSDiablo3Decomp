// Protocol Buffers - Google's data interchange format
// Copyright 2008 Google Inc.  All rights reserved.
// http://code.google.com/p/protobuf/
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#line 31 "C:/Projects/D3/Main/Trunk/Contrib/Contrib/BlizzardProtoBuf/2.6.0/src/src/google/protobuf/io/zero_copy_stream_impl_lite.cc"
// Author: kenton@google.com (Kenton Varda)
//  Based on original Protocol Buffers design by
//  Sanjay Ghemawat, Jeff Dean, and others.
#include "zero_copy_stream_impl_lite.h"

#include <algorithm>
#include <limits>

#include "../../../../../../../blz/include/blz/algo_minmax.h"
#include "../stubs/common.h"
#include "../stubs/stl_util.h"


namespace google {
namespace protobuf {
namespace io {

namespace {

// Default block size for Copying{In,Out}putStreamAdaptor.
static const int kDefaultBlockSize = 8192;

}  // namespace

// ===================================================================

ArrayInputStream::ArrayInputStream(const void* data, int size,
                                   int block_size)
  : data_(reinterpret_cast<const uint8*>(data)),
    size_(size),
    block_size_(block_size > 0 ? block_size : size),
    position_(0),
    last_returned_size_(0) {
}

ArrayInputStream::~ArrayInputStream() {
}

bool ArrayInputStream::Next(const void** data, int* size) {
  if (position_ < size_) {
    last_returned_size_ = blz::min(block_size_, size_ - position_);
    *data = data_ + position_;
    *size = last_returned_size_;
    position_ += last_returned_size_;
    return true;
  } else {
    // We're at the end of the array.
    last_returned_size_ = 0;   // Don't let caller back up.
    return false;
  }
}

void ArrayInputStream::BackUp(int count) {
  GOOGLE_CHECK_GT(last_returned_size_, 0)
      << "BackUp() can only be called after a successful Next().";
  GOOGLE_CHECK_LE(count, last_returned_size_);
  GOOGLE_CHECK_GE(count, 0);
  position_ -= count;
  last_returned_size_ = 0;  // Don't let caller back up further.
}

bool ArrayInputStream::Skip(int count) {
  GOOGLE_CHECK_GE(count, 0);
  last_returned_size_ = 0;   // Don't let caller back up.
  if (count > size_ - position_) {
    position_ = size_;
    return false;
  } else {
    position_ += count;
    return true;
  }
}

int64 ArrayInputStream::ByteCount() const {
  return position_;
}


// ===================================================================

ArrayOutputStream::ArrayOutputStream(void* data, int size, int block_size)
  : data_(reinterpret_cast<uint8*>(data)),
    size_(size),
    block_size_(block_size > 0 ? block_size : size),
    position_(0),
    last_returned_size_(0) {
}

ArrayOutputStream::~ArrayOutputStream() {
}

bool ArrayOutputStream::Next(void** data, int* size) {
  if (position_ < size_) {
    last_returned_size_ = blz::min(block_size_, size_ - position_);
    *data = data_ + position_;
    *size = last_returned_size_;
    position_ += last_returned_size_;
    return true;
  } else {
    // We're at the end of the array.
    last_returned_size_ = 0;   // Don't let caller back up.
    return false;
  }
}

void ArrayOutputStream::BackUp(int count) {
  GOOGLE_CHECK_GT(last_returned_size_, 0)
      << "BackUp() can only be called after a successful Next().";
  GOOGLE_CHECK_LE(count, last_returned_size_);
  GOOGLE_CHECK_GE(count, 0);
  position_ -= count;
  last_returned_size_ = 0;  // Don't let caller back up further.
}

int64 ArrayOutputStream::ByteCount() const {
  return position_;
}

// ===================================================================

StringOutputStream::StringOutputStream(blz::string* target)
  : target_(target) {
}

StringOutputStream::~StringOutputStream() {
}

bool StringOutputStream::Next(void** data, int* size) {
  int old_size = target_->size();

  // Grow the string.
  if (old_size < target_->capacity()) {
    // Resize the string to match its capacity, since we can get away
    // without a memory allocation this way.
    STLStringResizeUninitialized(target_, target_->capacity());
  } else {
    // Size has reached capacity, try to double the size.
    if (old_size > std::numeric_limits<int>::max() / 2) {
      // Can not double the size otherwise it is going to cause integer
      // overflow in the expression below: old_size * 2 ";
      GOOGLE_LOG(ERROR) << "Cannot allocate buffer larger than kint32max for "
                 << "StringOutputStream.";
      return false;
    }
    // Double the size, also make sure that the new size is at least
    // kMinimumSize.
    STLStringResizeUninitialized(
      target_,
      blz::max(old_size * 2,
          kMinimumSize + 0));  // "+ 0" works around GCC4 weirdness.
  }

  *data = mutable_string_data(target_) + old_size;
  *size = target_->size() - old_size;
  return true;
}

void StringOutputStream::BackUp(int count) {
  GOOGLE_CHECK_GE(count, 0);
  GOOGLE_CHECK_LE(count, target_->size());
  target_->resize(target_->size() - count);
}

int64 StringOutputStream::ByteCount() const {
  return target_->size();
}

// ===================================================================

}  // namespace io
}  // namespace protobuf
}  // namespace google
