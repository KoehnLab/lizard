// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/Krzmbrzl/lizard/blob/main/LICENSE>.

#ifndef LIZARD_CORE_EXPRESSION_HPP_
#define LIZARD_CORE_EXPRESSION_HPP_

#include "lizard/core/ConstexprMath.hpp"
#include "lizard/core/ExpressionCardinality.hpp"
#include "lizard/core/MultiEnum.hpp"
#include "lizard/core/Numeric.hpp"

#include <cstdint>
#include <limits>

namespace lizard {

/*
 * This class represents a mathematical expression that may involve up to binary operators.
 */
class Expression {
public:
	/**
	 * The different kinds of expressions that can be represented
	 */
	enum class Type : std::uint8_t { Operator, Literal, Variable };
	/**
	 * The representable mathematical operators
	 */
	enum class Operator : std::uint8_t { Plus, Times };

	Expression() noexcept = delete;
	Expression(Type type, Numeric left = Numeric::Invalid, Numeric right = Numeric::Invalid) noexcept;

	[[nodiscard]] auto getCardinality() const -> ExpressionCardinality;
	[[nodiscard]] auto getType() const -> Type;

	[[nodiscard]] auto hasParent() const -> bool;
	[[nodiscard]] auto getParent() const -> Numeric;
	void setParent(Numeric parent);

	[[nodiscard]] auto hasLeftArg() const -> bool;
	[[nodiscard]] auto getLeftArg() const -> Numeric;
	void setLeftArg(Numeric arg);

	[[nodiscard]] auto hasRightArg() const -> bool;
	[[nodiscard]] auto getRightArg() const -> Numeric;
	void setRightArg(Numeric arg);

	[[nodiscard]] auto getOperator() const -> Operator;
	void setOperator(Operator operatorType);

	// The following functions mostly provide semantically (and functionally) more clear
	// access to the stored data (different function names may be suitable for different
	// scenarios).

	/**
	 * If this expression represents is of Variable type, this function returns the index of the
	 * actual variable object inside the variable store.
	 */
	[[nodiscard]] auto getVarIndex() const -> Numeric;

	[[nodiscard]] auto getNumerator() const -> std::int32_t;
	[[nodiscard]] auto getDenominator() const -> std::int32_t;
	[[nodiscard]] auto getLiteral() const -> double;

	void setNumerator(std::int32_t numerator);
	void setDenominator(std::int32_t denominator);
	void setLiteral(std::int32_t literal);
	void setLiteral(double literal, double precision = pow(2, -(std::numeric_limits< double >::digits - 1)));

private:
	MultiEnum< Type, Operator > m_flags;
	Numeric m_parent;
	Numeric m_left;
	Numeric m_right;
};

} // namespace lizard

#endif // LIZARD_CORE_EXPRESSION_HPP_
