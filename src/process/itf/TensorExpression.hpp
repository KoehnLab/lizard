// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#pragma once

#include "lizard/core/Fraction.hpp"
#include "lizard/symbolic/ExpressionCardinality.hpp"
#include "lizard/symbolic/TensorElement.hpp"

#include "Multiplyable.hpp"

#include <variant>

namespace lizard::itf {

/**
 * Represents a tensor expression in ITF, that can appear as a sub-expression of contractions. Note
 * that such expressions can't appear on their own (outside of contractions) in ITF.
 */
class TensorExpression : public Multiplyable {
public:
	/**
	 * Creates an expression in which the given element is contained and which is optionally
	 * multiplied by the given scalar factor
	 */
	TensorExpression(TensorElement element, Fraction factor = 1);

	/**
	 * @returns The cardinality of this expression. Can only be nullary or binary
	 */
	[[nodiscard]] auto getCardinality() const -> ExpressionCardinality;

	/**
	 * @returns A TensorElement that represents the result of this expression
	 */
	[[nodiscard]] auto getResult() const -> const TensorElement &;

	[[nodiscard]] auto stringify(const IndexSpaceManager &manager) const -> std::string override;

	auto operator+=(const TensorExpression &rhs) -> TensorExpression &;
	[[nodiscard]] auto operator+(const TensorExpression &rhs) -> TensorExpression;


	struct ScaledTensorElement {
		TensorElement tensor;
		Fraction factor;
	};
	struct Addition {
		ScaledTensorElement lhs;
		ScaledTensorElement rhs;
	};

private:
	std::variant< TensorElement, Addition > m_expression;
};

} // namespace lizard::itf
