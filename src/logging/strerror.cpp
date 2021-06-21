// Copyright 2021 The RDSS Authors
// Copyright 2020 The XLS Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stdio.h>
#include <string.h>

#include <absl/strings/str_format.h>

#include "strerror.hpp"

namespace rdss {

std::string Strerror(int error_num) {
  // The strerror_r function is available in two versions: XSI-compliant version
  // and a GNU-specific version. The GNU-specific version in glibc returns an
  // char* pointing to the error string. The XSI-compliant version returns an
  // int where 0 indicates success.
  using strerror_r_type = decltype(strerror_r(0, nullptr, 0));
  constexpr bool kXsiCompliant = std::is_same<strerror_r_type, int>::value;
  constexpr bool kGnuSpecific = std::is_same<strerror_r_type, char*>::value;
  static_assert(kXsiCompliant != kGnuSpecific,
                "strerror_r should be either the XSI-compliant version or the "
                "GNU-specific version");

  constexpr int kBufferSize = 512;
  char buffer[kBufferSize] = {};

  if constexpr (kXsiCompliant) {
    if (strerror_r(error_num, buffer, kBufferSize) != 0) {
      return absl::StrFormat(
          "Unknown error, strerror_r failed. error number %d", error_num);
    }
    return buffer;
  } else if constexpr (kGnuSpecific) {
    // Note that without the `reinterpret_cast`, this cannot be compiled with
    // the XSI-compliant version of strerror_r.
    return reinterpret_cast<char*>(strerror_r(error_num, buffer, kBufferSize));
  }
}

}  // namespace rdss
