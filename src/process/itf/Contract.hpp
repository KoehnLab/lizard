// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#pragma once

#include "lizard/core/Fraction.hpp"
#include "lizard/symbolic/TensorElement.hpp"

#include "Multiplyable.hpp"
#include "TensorExpression.hpp"

namespace lizard::itf {

/**
 * Represents a single, binary contract operation in ITF
 */
class Contract : public Multiplyable {
public:
	Contract(TensorElement result, TensorExpression lhs, TensorExpression rhs, Fraction factor = 1);

	/**
	 * Sets the result tensor that this contraction produces
	 */
	void setResult(TensorElement result);

	/**
	 * @returns The result tensor of this contraction
	 */
	[[nodiscard]] auto getResult() const -> const TensorElement &;

	/**
	 * @returns The left-hand side of the contraction
	 */
	[[nodiscard]] auto getLhs() const -> const TensorExpression &;

	/**
	 * @returns The right-hand side of the contraction
	 */
	[[nodiscard]] auto getRhs() const -> const TensorExpression &;

	[[nodiscard]] auto stringify(const IndexSpaceManager &manager) const -> std::string override;

private:
	TensorElement m_result;
	TensorExpression m_lhs;
	TensorExpression m_rhs;
};

} // namespace lizard::itf
