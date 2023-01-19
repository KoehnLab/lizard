// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/Krzmbrzl/lizard/blob/main/LICENSE>.

#ifndef LIZARD_CORE_EXPRESSION_OPERATOR_HPP_
#define LIZARD_CORE_EXPRESSION_OPERATOR_HPP_

#include <cstdint>
#include <iosfwd>

namespace lizard {

/**
 * The representable mathematical operators
 */
enum class ExpressionOperator : std::uint8_t {
	Plus,
	Times,
};

auto operator<<(std::ostream &stream, const ExpressionOperator &operatorType) -> std::ostream &;

} // namespace lizard

#endif // LIZARD_CORE_EXPRESSION_OPERATOR_HPP_
