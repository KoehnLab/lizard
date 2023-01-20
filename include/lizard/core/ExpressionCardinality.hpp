// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/Krzmbrzl/lizard/blob/main/LICENSE>.

#pragma once

#include <cstdint>

namespace lizard {

/**
 * Enumeration holding the possible cardinalities of expressions. Here, the term "cardinality" refers
 * to the amount of arguments a given expression requires.
 */
enum class ExpressionCardinality : std::uint8_t {
	/**
	 * Expression requires no arguments
	 */
	Nullary,
	/**
	 * Expression requires exactly one argument
	 */
	Unary,
	/**
	 * Expression requires exactly two arguments
	 */
	Binary,
};

// We require the numeric value of the cardinality to represent the argument count
static_assert(static_cast< int >(ExpressionCardinality::Nullary) == 0);
static_assert(static_cast< int >(ExpressionCardinality::Unary) == 1);
static_assert(static_cast< int >(ExpressionCardinality::Binary) == 2);

} // namespace lizard
