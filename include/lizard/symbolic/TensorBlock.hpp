// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#pragma once

#include "lizard/symbolic/IndexSpace.hpp"
#include "lizard/symbolic/Tensor.hpp"

#include <libperm/PrimitivePermutationGroup.hpp>

#include <cstdint>
#include <functional>
#include <iosfwd>
#include <tuple>
#include <vector>

namespace lizard {

/**
 * Symbolic representation of a given block within a tensor. A block is characterized by a set of index slots
 * where each slot indicates which index space it belongs to. Each of these slots can then be indexed by a
 * concrete index.
 * An example of a tensor block would be the block f[H,P] which describes the part of the Fock matrix
 * that is can be indexed by an index in the occupied space followed from an index from the virtual space
 * (unless there is a symmetry between these two index slots, in which case their order doesn't matter)
 */
class TensorBlock {
public:
	using SlotSymmetry = perm::PrimitivePermutationGroup;
	using IndexSlots   = std::vector< IndexSpace >;

private:
	friend class TensorElement;

	TensorBlock(Tensor tensor, IndexSlots indexSlots, SlotSymmetry symmetry = {});

public:
	/**
	 * @param tensor The Tensor the block belongs to
	 * @param indexSlots A sequence of index slots the block shall have
	 * @param symmetry The slot symmetries of the block
	 * @returns A tuple with the new TensorBlock instance using the canonical sequence of
	 * the provided index slots and the sign corresponding to the symmetry used to bring the
	 * sequence into canonical order
	 */
	[[nodiscard]] static auto create(Tensor tensor, IndexSlots indexSlots, SlotSymmetry symmetry = {})
		-> std::tuple< TensorBlock, int >;

	/**
	 * @returns The block's dimension (amount of slots)
	 */
	[[nodiscard]] auto dimension() const -> std::size_t;

	/**
	 * @returns The Tensor this block belongs to
	 */
	[[nodiscard]] auto getTensor() const -> const Tensor &;
	/**
	 * @returns The Tensor this block belongs to
	 */
	[[nodiscard]] auto getTensor() -> Tensor &;
	/**
	 * Set the Tensor this block belongs to
	 */
	void setTensor(Tensor tensor);

	/**
	 * @returns This block's slot symmetries
	 */
	[[nodiscard]] auto getSlotSymmetry() const -> const SlotSymmetry &;

	/**
	 * @returns This block's index slots
	 */
	[[nodiscard]] auto getIndexSlots() const -> const IndexSlots &;

private:
	Tensor m_tensor;
	SlotSymmetry m_symmetry;
	IndexSlots m_slots;
};

[[nodiscard]] auto operator==(const TensorBlock &lhs, const TensorBlock &rhs) -> bool;
[[nodiscard]] auto operator!=(const TensorBlock &lhs, const TensorBlock &rhs) -> bool;

auto operator<<(std::ostream &stream, const TensorBlock &block) -> std::ostream &;

} // namespace lizard

template<> struct std::hash< lizard::TensorBlock > {
	[[nodiscard]] auto operator()(const lizard::TensorBlock &block) const -> std::size_t;
};
