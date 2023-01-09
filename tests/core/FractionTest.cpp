// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/Krzmbrzl/lizard/blob/main/LICENSE>.

#include "lizard/core/Fraction.hpp"

#include <gtest/gtest.h>

using namespace ::lizard;

TEST(Core, Fraction_comparison) {
	ASSERT_EQ(Fraction(1, 2), Fraction(1, 2));
	ASSERT_EQ(Fraction(1), Fraction(4, 4));

	ASSERT_LT(Fraction(1), Fraction(4, 2));
	ASSERT_LT(Fraction(2, 3), Fraction(3, 4));

	ASSERT_GT(Fraction(1), Fraction(1, 2));
	ASSERT_GT(Fraction(5, 8), Fraction(3, 7));
}

TEST(Core, Fraction_fromDecimal) {
	float maxError = 1e-5; // NOLINT(*-magic-numbers)
	for (Fraction::field_type numerator : { -128, -56, 0, 23, 265 }) {
		for (Fraction::field_type denominator : { 1, 3, 12, 54, 785 }) {
			Fraction expectedFraction(numerator, denominator);

			Fraction approximatedFraction =
				Fraction::fromDecimal(static_cast< float >(numerator) / static_cast< float >(denominator), maxError);

			ASSERT_TRUE(std::abs(approximatedFraction.getValue() - expectedFraction.getValue()) < maxError)
				<< "Expected: " << expectedFraction << " but got " << approximatedFraction;
		}
	}
}
