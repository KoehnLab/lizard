// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#include "lizard/core/Fraction.hpp"

#include <gtest/gtest.h>

#include <tuple>

using namespace ::lizard;

////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// TEST FIXTURES //////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

class ConversionTest : public ::testing::TestWithParam< std::tuple< int, int > > {};


////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// TEST CASES ////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

TEST(Fraction, comparison) {
	ASSERT_EQ(Fraction(1, 2), Fraction(1, 2));
	ASSERT_EQ(Fraction(1), Fraction(4, 4));

	ASSERT_LT(Fraction(1), Fraction(4, 2));
	ASSERT_LT(Fraction(2, 3), Fraction(3, 4));

	ASSERT_GT(Fraction(1), Fraction(1, 2));
	ASSERT_GT(Fraction(5, 8), Fraction(3, 7));
}

TEST_P(ConversionTest, fromDecimal) {
	constexpr const float maxError = 1e-5F; // NOLINT(*-magic-numbers)

	const int numerator   = std::get< 0 >(GetParam());
	const int denominator = std::get< 1 >(GetParam());

	const float numericValue = static_cast< float >(numerator) / static_cast< float >(denominator);

	const Fraction frac = Fraction::fromDecimal(numericValue, maxError);

	EXPECT_NEAR(frac.getValue< float >(), numericValue, maxError);
}

TEST(ConversionTest, simplify) {
	ASSERT_EQ(Fraction(4, 2), Fraction(2));
	ASSERT_EQ(Fraction(3, -6), Fraction(-1, 2));
}

TEST(ConversionTest, multiply) {
	ASSERT_EQ(Fraction(3, 4) * Fraction(7, 8), Fraction(21, 32));
	ASSERT_EQ(Fraction(4, 7) * Fraction(1, 4), Fraction(1, 7));

	ASSERT_EQ(Fraction(1, 3) * 4, Fraction(4, 3));
	ASSERT_EQ(5 * Fraction(2, 5), Fraction(2));
}

TEST(ConversionTest, divide) {
	ASSERT_EQ(Fraction(1, 2) / Fraction(3, 4), Fraction(2, 3));
	ASSERT_EQ(Fraction(3, 2) / Fraction(5, 7), Fraction(21, 10));

	ASSERT_EQ(Fraction(1, 2) / 2, Fraction(1, 4));
	ASSERT_EQ(4 / Fraction(1, 2), Fraction(8));
}


////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// TEST SUITE INSTANTIATIONS ////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

INSTANTIATE_TEST_SUITE_P(Fraction, ConversionTest,
						 ::testing::Combine(::testing::Values(-128, -56, 0, 23, 265),
											::testing::Values(-42, -7, 1, 3, 12, 54, 785)));
