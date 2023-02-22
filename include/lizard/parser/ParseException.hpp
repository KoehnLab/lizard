// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#pragma once

#include "lizard/core/Exception.hpp"

namespace lizard::parser {

class ParseException : public ::lizard::Exception {
public:
	using ::lizard::Exception::Exception;
};

} // namespace lizard::parser
