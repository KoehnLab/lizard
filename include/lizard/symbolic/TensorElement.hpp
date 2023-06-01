// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#pragma once

#include "lizard/symbolic/Index.hpp"
#include "lizard/symbolic/Tensor.hpp"
#include "lizard/symbolic/TensorBlock.hpp"

#include "nonstd/span.hpp"

#include <functional>
#include <iosfwd>
#include <tuple>
#include <vector>


namespace lizard {

/**
 * Symbolic representation of a specific tensor element. A tensor element is characterized
 * by a distribution of concrete indices over the index slots of the TensorBlock the element belongs to.
 */
class TensorElement {
private:
	TensorElement(TensorBlock block, std::vector< Index > indices);

public:
	/**
	 * @param block The TensorBlock that the created element should belong to
	 * @param indices The concrete indexing to use for the given block. Note that the provided
	 * indices have to be compatible to the provided block
	 * @returns A tuple containing the newly created TensorElement and the sign of the permutation
	 * that was applied to the given indexing in order to bring it into canonical order
	 */
	[[nodiscard]] auto static create(TensorBlock block, std::vector< Index > indices)
		-> std::tuple< TensorElement, int >;

	/**
	 * @param tensor The Tensor that the created element should be indexing
	 * @param indices The indices into the Tensor that point to the represented element
	 * @param symmetry A group of index symmetries for this TensorElement (or rather the TensorBlock that
	 * this element belongs to)
	 * @returns A tuple containing the newly created TensorElement and the sign of the permutation
	 * that was applied to the given indices in order to bring it into canonical order
	 */
	[[nodiscard]] auto static create(Tensor tensor, std::vector< Index > indices, TensorBlock::SlotSymmetry symmetry)
		-> std::tuple< TensorElement, int >;

	/**
	 * Constructs a "tensor" element that in reality is only a scalar
	 */
	TensorElement(Tensor tensor);


	/**
	 * @returns The TensorBlock this element belongs to
	 */
	[[nodiscard]] auto getBlock() const -> const TensorBlock &;

	/**
	 * @returns The indices of this element
	 */
	[[nodiscard]] auto getIndices() const -> nonstd::span< const Index >;

private:
	TensorBlock m_block;
	std::vector< Index > m_indices;
};

[[nodiscard]] auto operator==(const TensorElement &lhs, const TensorElement &rhs) -> bool;
[[nodiscard]] auto operator!=(const TensorElement &lhs, const TensorElement &rhs) -> bool;

auto operator<<(std::ostream &stream, const TensorElement &element) -> std::ostream &;

} // namespace lizard

template<> struct std::hash< lizard::TensorElement > {
	[[nodiscard]] auto operator()(const lizard::TensorElement &element) -> std::size_t;
};
