// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#include "lizard/symbolic/TensorBlock.hpp"
#include "lizard/core/BitOperations.hpp"

#include <libperm/Utils.hpp>

#include <cassert>
#include <iostream>

namespace lizard {

TensorBlock::TensorBlock(Tensor tensor, IndexSlots indexSlots, SlotSymmetry symmetry)
	: m_tensor(std::move(tensor)), m_symmetry(std::move(symmetry)), m_slots(std::move(indexSlots)) {
	assert(perm::computeCanonicalizationPermutation(m_slots, m_symmetry)->isIdentity()); // NOLINT
}

auto TensorBlock::create(Tensor tensor, IndexSlots indexSlots, SlotSymmetry symmetry)
	-> std::tuple< TensorBlock, int > {
	int sign = perm::canonicalize(indexSlots, symmetry);

	return std::make_tuple(TensorBlock(std::move(tensor), std::move(indexSlots), std::move(symmetry)), sign);
}

auto TensorBlock::dimension() const -> std::size_t {
	return m_slots.size();
}

auto TensorBlock::getTensor() const -> const Tensor & {
	return m_tensor;
}

auto TensorBlock::getTensor() -> Tensor & {
	return m_tensor;
}

void TensorBlock::setTensor(Tensor tensor) {
	m_tensor = std::move(tensor);
}

auto TensorBlock::getSlotSymmetry() const -> const SlotSymmetry & {
	return m_symmetry;
}

auto TensorBlock::getIndexSlots() const -> const IndexSlots & {
	return m_slots;
}

auto operator==(const TensorBlock &lhs, const TensorBlock &rhs) -> bool {
#ifndef NDEBUG
	if (lhs.getTensor() == rhs.getTensor() && lhs.getIndexSlots() == rhs.getIndexSlots()) {
		// We assume that the slot symmetry is an implicit property of a given index block. Therefore,
		// if two blocks are equal before comparing their symmetries, we assume that their symmetry
		// is equal as well.
		// (Note that part of the block's equality is the actual index spaces for its slots, which
		// may differ in spin, which allows to represent the same block in the spin-orbit formalism
		// and with explicit spin labels (after spin integration) even though these blocks will generally
		// have different symmetry.)
		assert(lhs.getSlotSymmetry() == rhs.getSlotSymmetry()); // NOLINT
	}

	// NOLINTNEXTLINE
	assert(perm::computeCanonicalizationPermutation(lhs.getIndexSlots(), lhs.getSlotSymmetry())->isIdentity());
	// NOLINTNEXTLINE
	assert(perm::computeCanonicalizationPermutation(rhs.getIndexSlots(), rhs.getSlotSymmetry())->isIdentity());
#endif
	return lhs.getTensor() == rhs.getTensor() && lhs.getIndexSlots() == rhs.getIndexSlots();
}

auto operator!=(const TensorBlock &lhs, const TensorBlock &rhs) -> bool {
	return !(lhs == rhs);
}

auto operator<<(std::ostream &stream, const TensorBlock &block) -> std::ostream & {
	stream << block.getTensor() << "[";

	for (std::size_t i = 0; i < block.dimension(); ++i) {
		stream << block.getIndexSlots()[i];

		if (i + 1 < block.dimension()) {
			stream << ", ";
		}
	}

	return stream << "]";
}

} // namespace lizard


[[nodiscard]] auto std::hash< lizard::TensorBlock >::operator()(const lizard::TensorBlock &block) const -> std::size_t {
	std::size_t hash = std::hash< lizard::Tensor >{}(block.getTensor());

	// NOLINTNEXTLINE
	assert(perm::computeCanonicalizationPermutation(block.getIndexSlots(), block.getSlotSymmetry())->isIdentity());

	// Note: This hash function is sensitive to the order of index slots, so e.g. [ext., virt.] will produce
	// a different hash than [virt., ext.]. This does not take into consideration that there might be
	// symmetry-equivalent arrangements of these slots. Instead, it is assumed that the slots are already in
	// the canonical order.
	for (std::size_t i = 0; i < block.dimension(); ++i) {
		hash ^= lizard::bit_rotate< std::size_t >(std::hash< lizard::IndexSpace >{}(block.getIndexSlots()[i]),
												  static_cast< unsigned int >(2 * i + 1));
	}

	return hash;
}
