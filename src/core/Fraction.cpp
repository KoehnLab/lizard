// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/Krzmbrzl/lizard/blob/main/LICENSE>.

#include "lizard/core/Fraction.hpp"

#include <iostream>

namespace lizard {

auto operator<<(std::ostream &stream, const Fraction &fraction) -> std::ostream & {
	return stream << fraction.getNumerator() << " / " << fraction.getDenominator();
}

} // namespace lizard
