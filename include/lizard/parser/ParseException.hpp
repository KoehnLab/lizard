// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/Krzmbrzl/lizard/blob/main/LICENSE>.

#ifndef LIZARD_PARSER_PARSEEXCEPTION_HPP_
#define LIZARD_PARSER_PARSEEXCEPTION_HPP_

#include "lizard/core/Exception.hpp"

namespace lizard::parser {

class ParseException : public ::lizard::core::Exception {
public:
	using ::lizard::core::Exception::Exception;
};

} // namespace lizard::parser

#endif // LIZARD_PARSER_PARSEEXCEPTION_HPP_