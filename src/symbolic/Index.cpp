// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#include "lizard/symbolic/Index.hpp"
#include "lizard/core/BitOperations.hpp"
#include "lizard/symbolic/IndexSpace.hpp"

#include <iostream>


namespace lizard {

Index::Index(Id id, IndexSpace space, IndexType type)
	: m_id(std::move(id)), m_space(std::move(space)), m_type(std::move(type)) {
}

auto Index::getID() const -> Id {
	return m_id;
}

void Index::setID(Id id) {
	m_id = std::move(id);
}

auto Index::getSpace() const -> const IndexSpace & {
	return m_space;
}

void Index::setSpace(IndexSpace space) {
	m_space = std::move(space);
}

auto Index::getType() const -> IndexType {
	return m_type;
}

void Index::setType(IndexType type) {
	m_type = std::move(type);
}

auto operator==(const Index &lhs, const Index &rhs) -> bool {
	return lhs.getID() == rhs.getID() && lhs.getType() == rhs.getType() && lhs.getSpace() == rhs.getSpace();
}

auto operator!=(const Index &lhs, const Index &rhs) -> bool {
	return !(lhs == rhs);
}

auto operator<(const Index &lhs, const Index &rhs) -> bool {
	// The first thing that indices have to be compared on is their index space in order to ensure
	// that a canonical sequence of indices is compatible to a canonical sequence of the corresponding
	// index spaces
	if (lhs.getSpace() != rhs.getSpace()) {
		return lhs.getSpace() < rhs.getSpace();
	}

	if (lhs.getID() != rhs.getID()) {
		return lhs.getID() < rhs.getID();
	}

	return lhs.getType() < rhs.getType();
}

auto operator<=(const Index &lhs, const Index &rhs) -> bool {
	return lhs == rhs || lhs < rhs;
}

auto operator>(const Index &lhs, const Index &rhs) -> bool {
	return !(lhs <= rhs);
}

auto operator>=(const Index &lhs, const Index &rhs) -> bool {
	return !(lhs < rhs);
}

auto operator<<(std::ostream &stream, const Index &index) -> std::ostream & {
	stream << "Idx{" << static_cast< unsigned int >(index.getID()) << ", " << index.getSpace() << ", ";

	switch (index.getType()) {
		case IndexType::Annihilator:
			stream << "A";
			break;
		case IndexType::Creator:
			stream << "C";
			break;
		case IndexType::External:
			stream << "E";
			break;
	}

	return stream << "}";
}

} // namespace lizard

auto std::hash< lizard::Index >::operator()(const lizard::Index &index) -> std::size_t {
	return std::hash< lizard::IndexSpace >{}(index.getSpace())
		   ^ lizard::bit_rotate< std::size_t >(std::hash< lizard::Index::Id >{}(index.getID()), 8)
		   ^ lizard::bit_rotate< std::size_t >(std::hash< lizard::IndexType >{}(index.getType()), 16);
}
