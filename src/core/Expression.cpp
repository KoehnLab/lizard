// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/Krzmbrzl/lizard/blob/main/LICENSE>.

#include "lizard/core/Expression.hpp"
#include "lizard/core/ExpressionCardinality.hpp"
#include "lizard/core/Numeric.hpp"
#include "lizard/core/SignedCast.hpp"

#include <cassert>
#include <cmath>
#include <utility>

#include <hedley.h>

namespace lizard {

Expression::Expression(Type type, Numeric left, Numeric right) noexcept
	: m_flags(type), m_left(std::move(left)), m_right(std::move(right)) {
}

auto Expression::getCardinality() const -> ExpressionCardinality {
	switch (m_flags.get< Type >()) {
		case Type::Operator:
			return ExpressionCardinality::Binary;
		case Type::Variable:
			[[fallthrough]];
		case Type::Literal:
			return ExpressionCardinality::Nullary;
	}

	assert(false); // NOLINT
	HEDLEY_UNREACHABLE();
}

auto Expression::getType() const -> Type {
	return m_flags.get< Type >();
}

auto Expression::hasParent() const -> bool {
	return m_parent.isValid();
}

auto Expression::getParent() const -> Numeric {
	return m_parent;
}

void Expression::setParent(Numeric parent) {
	m_parent = std::move(parent);
}

auto Expression::hasLeftArg() const -> bool {
	return m_left.isValid();
}

auto Expression::getLeftArg() const -> Numeric {
	return m_left;
}

void Expression::setLeftArg(Numeric arg) {
	m_left = arg;
}

auto Expression::hasRightArg() const -> bool {
	return m_right.isValid();
}

auto Expression::getRightArg() const -> Numeric {
	return m_right;
}

void Expression::setRightArg(Numeric arg) {
	m_right = arg;
}

auto Expression::getOperator() const -> Operator {
	assert(getType() == Type::Operator); // NOLINT

	return m_flags.get< Operator >();
}

void Expression::setOperator(Operator operatorType) {
	assert(getType() == Type::Operator); // NOLINT

	m_flags.set(operatorType);
}

auto Expression::getVarIndex() const -> Numeric {
	assert(getType() == Type::Variable); // NOLINT

	return getLeftArg();
}

auto Expression::getNumerator() const -> std::int32_t {
	assert(getType() == Type::Literal); // NOLINT

	return signed_cast< std::int32_t >(getLeftArg().getValue());
}

auto Expression::getDenominator() const -> std::int32_t {
	assert(getType() == Type::Literal); // NOLINT

	return signed_cast< std::int32_t >(getRightArg().getValue());
}

auto Expression::getLiteral() const -> double {
	assert(getType() == Type::Literal); // NOLINT

	return static_cast< double >(getNumerator()) / getDenominator();
}

void Expression::setNumerator(std::int32_t numerator) {
	assert(getType() == Type::Literal); // NOLINT

	setLeftArg(signed_cast< Numeric::numeric_type >(numerator));
}

void Expression::setDenominator(std::int32_t denominator) {
	assert(getType() == Type::Literal); // NOLINT

	setRightArg(signed_cast< Numeric::numeric_type >(denominator));
}

void Expression::setLiteral(std::int32_t literal) {
	assert(getType() == Type::Literal); // NOLINT

	setNumerator(literal);
	setDenominator(1);
}

void Expression::setLiteral(double literal, double precision) {
	// We perform the conversion double -> symbolic fraction by walking the "Stern-Brocot" tree
	// until we have found a fraction that represents the given literal to a given precision
	// Taken from https://stackoverflow.com/a/5128558

	int sign = literal < 0 ? -1 : 1;
	literal *= sign; // Make literal positive
	std::int32_t numerator   = static_cast< int >(literal);
	std::int32_t denominator = 1;
	double fractionalPart    = literal - numerator;

	if (fractionalPart > precision) {
		if (1 - precision < fractionalPart) {
			numerator++;
		} else {
			std::int32_t upperNum   = 1;
			std::int32_t lowerNum   = 0;
			std::int32_t upperDenom = 1;
			std::int32_t lowerDenom = 1;

			// In essence, we are performing a binary search to find the best representation
			// of our fractional part as a symbolic fraction
			while (true) {
				// The mediant our fractions is
				// mediant = (upperNum + lowerNum) / (upperDenom + lowerDenom)
				std::int32_t mediantNum   = upperNum + lowerNum;
				std::int32_t mediantDenom = upperDenom + lowerDenom;

				const double upperBound = (fractionalPart + precision) * mediantDenom;
				const double lowerBound = (fractionalPart - precision) * mediantDenom;

				// If fractionalPart + precision < middle
				if (upperBound < mediantNum) {
					// middle is new upper bound
					upperNum   = mediantNum;
					upperDenom = mediantDenom;
				} else if (lowerBound > mediantNum) {
					// middle is new lower bound
					lowerNum   = mediantNum;
					lowerDenom = mediantDenom;
				} else {
					// middle is within precision of the fractional value that we have to approximate
					numerator   = numerator * mediantDenom + mediantNum;
					denominator = mediantDenom;
					break;
				}
			}
		}
	}

	setNumerator(sign * numerator);
	setDenominator(denominator);
}

} // namespace lizard
