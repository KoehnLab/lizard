// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#include "lizard/symbolic/IndexSpace.hpp"

#include <iostream>

namespace lizard {
IndexSpace::IndexSpace(Id identifier, Spin spin) : m_id(identifier), m_spin(spin) {
}

auto IndexSpace::getID() const -> Id {
	return m_id;
}

void IndexSpace::setID(Id identifier) {
	m_id = identifier;
}

auto IndexSpace::getSpin() const -> Spin {
	return m_spin;
}

void IndexSpace::setSpin(Spin spin) {
	m_spin = spin;
}

auto operator==(const IndexSpace &lhs, const IndexSpace &rhs) -> bool {
	return lhs.getID() == rhs.getID() && lhs.getSpin() == rhs.getSpin();
}

auto operator!=(const IndexSpace &lhs, const IndexSpace &rhs) -> bool {
	return !(lhs == rhs);
}

auto operator<<(std::ostream &stream, const IndexSpace &space) -> std::ostream & {
	stream << "IndexSpace{" << space.getID();

	if (space.getSpin() != Spin::None) {
		stream << ", " << space.getSpin();
	}

	return stream << "}";
}

} // namespace lizard

auto std::hash< const lizard::IndexSpace >::operator()(const lizard::IndexSpace &space) -> std::size_t {
	return std::hash< decltype(space.getID()) >{}(space.getID())
		   ^ (std::hash< decltype(space.getSpin()) >{}(space.getSpin()) << 4);
}
