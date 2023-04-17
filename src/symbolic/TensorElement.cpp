// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#include "lizard/symbolic/TensorElement.hpp"
#include "lizard/core/BitOperations.hpp"

#include <libperm/Utils.hpp>

#include <algorithm>
#include <cassert>
#include <iostream>


namespace lizard {

TensorElement::TensorElement(TensorBlock block, std::vector< Index > indices)
	: m_block(std::move(block)), m_indices(std::move(indices)) {
	assert(perm::computeCanonicalizationPermutation(indices, m_block.getSlotSymmetry())->isIdentity()); // NOLINT
}

TensorElement::TensorElement(Tensor tensor) : TensorElement(TensorBlock{ std::move(tensor), {}, {} }, {}) {
}

auto TensorElement::create(TensorBlock block, std::vector< Index > indices) -> std::tuple< TensorElement, int > {
	// Assert that the index spaces in the block are compatible with the provided index list
	assert(block.dimension() == indices.size()); // NOLINT
#ifndef NDEBUG
	for (std::size_t i = 0; i < indices.size(); ++i) {
		assert(block.getIndexSlots()[i] == indices[i].getSpace()); // NOLINT
	}
#endif

	// Bring indices into canonical order
	int sign = perm::canonicalize(indices, block.getSlotSymmetry());

	// Verify that the slots are still compatible with the indices
#ifndef NDEBUG
	for (std::size_t i = 0; i < indices.size(); ++i) {
		assert(block.getIndexSlots()[i] == indices[i].getSpace()); // NOLINT
	}
#endif

	return { TensorElement(std::move(block), std::move(indices)), sign };
}

auto TensorElement::create(Tensor tensor, std::vector< Index > indices, TensorBlock::SlotSymmetry symmetry)
	-> std::tuple< TensorElement, int > {
	int sign = perm::canonicalize(indices, symmetry);

	TensorBlock::IndexSlots slots(indices.size());
	for (std::size_t i = 0; i < indices.size(); ++i) {
		slots[i] = indices[i].getSpace();
	}

	assert(perm::computeCanonicalizationPermutation(slots, symmetry)->isIdentity()); // NOLINT

	TensorBlock block(std::move(tensor), std::move(slots), std::move(symmetry));

	return { TensorElement(std::move(block), std::move(indices)), sign };
}

auto TensorElement::getBlock() const -> const TensorBlock & {
	return m_block;
}

auto TensorElement::getIndices() const -> nonstd::span< const Index > {
	return m_indices;
}

auto operator==(const TensorElement &lhs, const TensorElement &rhs) -> bool {
	// NOLINTNEXTLINE
	assert(perm::computeCanonicalizationPermutation(lhs.getIndices(), lhs.getBlock().getSlotSymmetry())->isIdentity());
	// NOLINTNEXTLINE
	assert(perm::computeCanonicalizationPermutation(rhs.getIndices(), rhs.getBlock().getSlotSymmetry())->isIdentity());

	return lhs.getBlock() == rhs.getBlock() && lhs.getIndices().size() == rhs.getIndices().size()
		   && std::equal(lhs.getIndices().begin(), lhs.getIndices().end(), rhs.getIndices().begin());
}

auto operator!=(const TensorElement &lhs, const TensorElement &rhs) -> bool {
	return !(lhs == rhs);
}

auto operator<<(std::ostream &stream, const TensorElement &element) -> std::ostream & {
	stream << element.getBlock().getTensor() << "[";

	for (std::size_t i = 0; i < element.getIndices().size(); ++i) {
		stream << element.getIndices()[i];

		if (i + 1 < element.getIndices().size()) {
			stream << ", ";
		}
	}

	return stream << "]";
}

} // namespace lizard

auto std::hash< lizard::TensorElement >::operator()(const lizard::TensorElement &element) -> std::size_t {
	std::size_t hash = std::hash< lizard::TensorBlock >{}(element.getBlock());

	// NOLINTNEXTLINE
	assert(perm::computeCanonicalizationPermutation(element.getIndices(), element.getBlock().getSlotSymmetry())
			   ->isIdentity());

	for (std::size_t i = 0; i < element.getIndices().size(); ++i) {
		hash ^= lizard::bit_rotate< std::size_t >(std::hash< lizard::Index >{}(element.getIndices()[i]),
												  static_cast< unsigned int >(2 * i + 16));
	}

	return hash;
}
