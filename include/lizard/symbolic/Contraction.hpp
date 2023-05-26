// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#pragma once

#include "lizard/symbolic/Index.hpp"
#include "lizard/symbolic/TensorElement.hpp"

#include <string_view>
#include <vector>

namespace lizard {

/**
 * @returns The indices that are left over after contracting the two index sequences with one another.
 * Thus, the returned indices are exactly those indices that are not contained in both lists.
 */
[[nodiscard]] auto contractIndices(const std::vector< Index > &lhs, const std::vector< Index > &rhs)
	-> std::vector< Index >;

/**
 * Finds the indices that will remain after contracting lhs with rhs and write them to uniqueIndices
 */
void contractIndices(const std::vector< Index > &lhs, const std::vector< Index > &rhs,
					 std::vector< Index > &uniqueIndices);

/**
 * @param lhs The first tensor to contract
 * @param rhs The second tensor to contract
 * @param resultName The name that the resulting tensor shall have
 * @returns The TensorElement for holding the result of contracting the given tensor elements
 */
[[nodiscard]] auto contract(const TensorElement &lhs, const TensorElement &rhs, std::string_view resultName)
	-> TensorElement;

} // namespace lizard
