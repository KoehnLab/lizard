// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#pragma once

#include "lizard/core/Fraction.hpp"

#include "Operation.hpp"

namespace lizard::itf {

/**
 * Base class for operations that support multiplication with a scalar factor
 */
class Multiplyable : public Operation {
public:
	Multiplyable(Fraction factor = 1);

	/**
	 * @returns The scalar factor associated with this object
	 */
	[[nodiscard]] auto getFactor() const -> const Fraction &;

	/**
	 * Sets the scalar factor associated with this object
	 */
	void setFactor(Fraction factor);

	/**
	 * Multiply this object by the given scalar factor
	 */
	void multiplyBy(const Fraction &factor);

	auto operator*=(const Fraction &rhs) -> Multiplyable &;

private:
	Fraction m_factor;
};

} // namespace lizard::itf
