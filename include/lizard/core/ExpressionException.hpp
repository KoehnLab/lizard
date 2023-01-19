// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/Krzmbrzl/lizard/blob/main/LICENSE>.

#ifndef LIZARD_CORE_EXPRESSIONEXCEPTION_HPP_
#define LIZARD_CORE_EXPRESSIONEXCEPTION_HPP_

#include "lizard/core/Exception.hpp"

namespace lizard {

/**
 * Exception thrown to indicate that there was an error while processing an expression
 */
class ExpressionException : public Exception {
public:
	using Exception::Exception;
};

} // namespace lizard

#endif // LIZARD_CORE_EXPRESSIONEXCEPTION_HPP_
