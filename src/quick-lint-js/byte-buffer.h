// Copyright (C) 2020  Matthew Glazar
// See end of file for extended copyright information.

#ifndef QUICK_LINT_JS_BYTE_BUFFER_H
#define QUICK_LINT_JS_BYTE_BUFFER_H

#include <cstddef>
#include <memory>
#include <quick-lint-js/assert.h>
#include <quick-lint-js/char8.h>
#include <quick-lint-js/have.h>
#include <quick-lint-js/integer.h>
#include <quick-lint-js/narrow-cast.h>
#include <utility>
#include <vector>

#if QLJS_HAVE_WRITEV
#include <sys/uio.h>
#endif

namespace quick_lint_js {
class byte_buffer_iovec;

class byte_buffer {
 public:
  using size_type = std::size_t;

  static constexpr size_type default_chunk_size = 4096;

  explicit byte_buffer();

  byte_buffer(byte_buffer&&);
  byte_buffer& operator=(byte_buffer&&) = delete;  // Not yet implemented.

  ~byte_buffer();

  void* append(size_type byte_count);

  template <class Func>
  void append(size_type max_byte_count, Func&& f) {
    this->reserve(max_byte_count);
    size_type bytes_written = f(this->cursor_);
    QLJS_ASSERT(bytes_written <= max_byte_count);
    this->cursor_ += bytes_written;
  }

  template <class T>
  void append_decimal_integer(T value) {
    this->append(integer_string_length<T>, [&](void* out) -> size_type {
      char8* begin = reinterpret_cast<char8*>(out);
      char8* end = write_integer<T>(value, begin);
      return narrow_cast<size_type>(end - begin);
    });
  }

  void append_copy(string8_view data);
  void append_copy(char8 data);

  void prepend_copy(string8_view data);

  void clear();

  size_type size() const noexcept;

  bool empty() const noexcept;

  void copy_to(void* raw_out) const;

#if QLJS_HAVE_WRITEV
  byte_buffer_iovec to_iovec() &&;
#endif

 private:
#if QLJS_HAVE_WRITEV
  using chunk = ::iovec;
#else
  struct chunk {
    std::byte* data;
    size_type size;
  };
#endif

  void reserve(size_type extra_byte_count);
  void update_current_chunk_size() noexcept;

  size_type bytes_remaining_in_current_chunk() const noexcept;
  size_type bytes_used_in_current_chunk() const noexcept;

  void add_new_chunk(size_type chunk_size);

  static chunk make_chunk();
  static chunk make_chunk(size_type size);
  static void delete_chunk(chunk&&);

  static const std::byte* chunk_begin(const chunk&) noexcept;
  static std::byte* chunk_begin(chunk&) noexcept;
  static const std::byte* chunk_end(const chunk&) noexcept;
  static std::byte* chunk_end(chunk&) noexcept;
  static size_type chunk_size(const chunk&) noexcept;
  static size_type& chunk_size(chunk&) noexcept;

  std::vector<chunk> chunks_;
  std::byte* cursor_;
  std::byte* current_chunk_end_;

  friend class byte_buffer_iovec;
};

#if QLJS_HAVE_WRITEV
class byte_buffer_iovec {
 public:
  using size_type = byte_buffer::size_type;

  explicit byte_buffer_iovec(std::vector<::iovec>&&);

  ~byte_buffer_iovec();

  const ::iovec* iovec() const noexcept;
  int iovec_count() const noexcept;

  // Remove count bytes from the beginning of this byte_buffer_iovec.
  void remove_front(size_type count);

 private:
  std::vector<::iovec> chunks_;
  std::vector<::iovec>::iterator first_chunk_;
  ::iovec first_chunk_allocation_;
};
#endif
}

#endif

// quick-lint-js finds bugs in JavaScript programs.
// Copyright (C) 2020  Matthew Glazar
//
// This file is part of quick-lint-js.
//
// quick-lint-js is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// quick-lint-js is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with quick-lint-js.  If not, see <https://www.gnu.org/licenses/>.
