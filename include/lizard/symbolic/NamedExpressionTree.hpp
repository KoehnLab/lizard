// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#pragma once

#include "lizard/symbolic/ExpressionTree.hpp"


namespace lizard {

/**
 * This class extends the regular ExpressionTree by allowing it to give a name to the expression represented
 * by the tree. This "name" will be assumed to name the result of whatever the expression is calculating.
 */
template< typename Result, typename Variable > class NamedExpressionTree : public ExpressionTree< Variable > {
public:
	NamedExpressionTree(Result result) : ExpressionTree< Variable >(), m_result(std::move(result)) {}

	/**
	 * @returns The result this expression calculates
	 */
	[[nodiscard]] auto getResult() const -> const Result & { return m_result; }

	/**
	 * @returns The result this expression calculates
	 */
	[[nodiscard]] auto getResult() -> Result & { return m_result; }


	/**
	 * @returns The name of this expression
	 */
	[[nodiscard]] auto getName() const -> const Result & { return m_result; }

	/**
	 * @returns The name of this expression
	 */
	[[nodiscard]] auto getName() -> Result & { return m_result; }

private:
	Result m_result;
};

} // namespace lizard
