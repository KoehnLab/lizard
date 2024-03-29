// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#pragma once

#include <cstdint>
#include <iosfwd>

namespace lizard {

/**
 * The different kinds of nodes that can be represented
 */
enum class ExpressionType : std::uint8_t {
	Operator,
	Literal,
	Variable,
};

auto operator<<(std::ostream &stream, const ExpressionType &type) -> std::ostream &;

} // namespace lizard
