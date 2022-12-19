// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/Krzmbrzl/lizard/blob/main/LICENSE>.

#include "lizard/core/Expression.hpp"

#include <gtest/gtest.h>

#include <cmath>
#include <limits>

using namespace ::lizard;

constexpr auto pi() -> double {
	return std::atan(1) * 4;
}

TEST(Core, Expression_setLiteral) {
	Expression expr(Expression::Type::Literal);

	for (int literal : { -123456, -5, 0, 5, 123456 }) {
		// Set as integer
		expr.setLiteral(literal);
		ASSERT_EQ(expr.getNumerator(), literal);
		ASSERT_EQ(expr.getDenominator(), 1);
		ASSERT_EQ(expr.getLiteral(), literal);

		// Set as floating point variable
		expr.setLiteral(static_cast< double >(literal));
		ASSERT_EQ(expr.getNumerator(), literal);
		ASSERT_EQ(expr.getDenominator(), 1);
		ASSERT_EQ(expr.getLiteral(), literal);
	}

	// Use a relative precision of 0.01%
	constexpr const double relativePrecision = 0.0001;

	for (int numerator : { -1234, -42, 0, 42, 1234 }) {
		for (int denominator : { 1, 12, 42, 246 }) {
			double value = static_cast< double >(numerator) / denominator;

			expr.setLiteral(value, std::abs(relativePrecision * value));
			ASSERT_LE(std::abs(expr.getLiteral() - value), std::abs(value * relativePrecision));
		}
	}
}
