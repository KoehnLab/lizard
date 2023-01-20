// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/Krzmbrzl/lizard/blob/main/LICENSE>.

#pragma once

#include <cassert>
#include <cmath>

namespace lizard {

/**
 * @returns The value of the given base raised to the given power
 */
template< typename ResultType = double >
constexpr auto pow(double base, int exponent) -> ResultType { // NOLINT(bugprone-easily-swappable-parameters)
	assert(base != 0);

	ResultType result = 1;
	ResultType factor = exponent > 0 ? base : static_cast< ResultType >(1) / base;

	if (exponent < 0) {
		exponent *= -1;
	}

	for (int i = 0; i < exponent; ++i) {
		result *= factor;
	}

	return result;
}

} // namespace lizard
