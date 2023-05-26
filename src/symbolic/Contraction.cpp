// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#include "lizard/symbolic/Contraction.hpp"
#include "lizard/symbolic/Tensor.hpp"
#include "lizard/symbolic/TensorBlock.hpp"

#include <libperm/PrimitivePermutationGroup.hpp>
#include <libperm/Utils.hpp>

#include <algorithm>
#include <string>

namespace lizard {

[[nodiscard]] auto contractIndices(const std::vector< Index > &lhs, const std::vector< Index > &rhs)
	-> std::vector< Index > {
	std::vector< Index > uniqueIndices;

	contractIndices(lhs, rhs, uniqueIndices);

	return uniqueIndices;
}

void contractIndices(const std::vector< Index > &lhs, const std::vector< Index > &rhs,
					 std::vector< Index > &uniqueIndices) {
	uniqueIndices.clear();
	// Reserve for "worst" case
	uniqueIndices.reserve(lhs.size() + rhs.size());

	for (const Index &current : lhs) {
		auto iter = std::find_if(rhs.begin(), rhs.end(), Index::FindByName{ current });

		if (iter == rhs.end()) {
			uniqueIndices.push_back(current);
		}
	}

	for (const Index &current : rhs) {
		auto iter = std::find_if(lhs.begin(), lhs.end(), Index::FindByName{ current });

		if (iter == lhs.end()) {
			uniqueIndices.push_back(current);
		}
	}
}

auto contract(const TensorElement &lhs, const TensorElement &rhs, std::string_view resultName) -> TensorElement {
	Tensor resultTensor(std::string{ resultName });

	std::vector< std::size_t > lhsExcludes;
	std::vector< std::size_t > rhsExcludes;

	std::vector< Index > resultIndices;

	for (std::size_t i = 0; i < lhs.getIndices().size(); ++i) {
		const auto *iter =
			std::find_if(rhs.getIndices().begin(), rhs.getIndices().end(), Index::FindByName{ lhs.getIndices()[i] });

		if (iter != rhs.getIndices().end()) {
			// This index appears in the lhs AND the rhs expression -> this is a contraction index
			lhsExcludes.push_back(i);
			rhsExcludes.push_back(static_cast< std::size_t >(std::distance(rhs.getIndices().begin(), iter)));
		} else {
			resultIndices.push_back(lhs.getIndices()[i]);
		}
	}

	for (std::size_t i = 0; i < rhs.getIndices().size(); ++i) {
		auto iter = std::find(rhsExcludes.begin(), rhsExcludes.end(), i);

		if (iter == rhsExcludes.end()) {
			resultIndices.push_back(rhs.getIndices()[i]);
		}
	}

	TensorBlock::SlotSymmetry resultSymmetry = perm::concatenate< TensorBlock::SlotSymmetry >(
		lhs.getBlock().getSlotSymmetry(), lhs.getIndices().size() - lhsExcludes.size(),
		rhs.getBlock().getSlotSymmetry(), lhsExcludes, rhsExcludes);

	auto [resultElement, sign] =
		TensorElement::create(resultTensor, std::move(resultIndices), std::move(resultSymmetry));

	// Since we don't change the relative order of the indices from the lhs and rhs element and there can't be any
	// symmetries that mix indices from the lhs and rhs elements, the indices should remain in canonical order.
	assert(sign == 1); // NOLINT

	return resultElement;
}

} // namespace lizard
