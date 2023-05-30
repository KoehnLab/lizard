// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#include "Multiplyable.hpp"

namespace lizard::itf {

Multiplyable::Multiplyable(Fraction factor) : m_factor(std::move(factor)) {
}

auto Multiplyable::getFactor() const -> const Fraction & {
	return m_factor;
}

void Multiplyable::setFactor(Fraction factor) {
	m_factor = std::move(factor);
}

void Multiplyable::multiplyBy(const Fraction &factor) {
	m_factor *= factor;
}

auto Multiplyable::operator*=(const Fraction &rhs) -> Multiplyable & {
	multiplyBy(rhs);

	return *this;
}

} // namespace lizard::itf
