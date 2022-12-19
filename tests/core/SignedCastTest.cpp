// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/Krzmbrzl/lizard/blob/main/LICENSE>.

#include "lizard/core/SignedCast.hpp"

#include <gtest/gtest.h>

#include <cstdint>
#include <limits>
#include <type_traits>

template< typename Int, std::make_signed_t< Int > iterations > void test_signed_cast() {
	using SignedInt   = std::make_signed_t< Int >;
	using UnsignedInt = std::make_unsigned_t< Int >;

	constexpr const SignedInt increment = std::numeric_limits< SignedInt >::max() / iterations;
	static_assert(increment > 0);

	SignedInt signedInt     = 0;
	UnsignedInt unsignedInt = 0;

	for (SignedInt i = 0; i <= iterations; ++i) {
		if (i == iterations) {
			// Ensure we test the max. value
			signedInt = std::numeric_limits< SignedInt >::max();
		} else {
			signedInt = i * increment;
		}


		// For positive numbers the signed and unsigned type should be identical
		unsignedInt = lizard::signed_cast< UnsignedInt >(signedInt);
		ASSERT_EQ(static_cast< UnsignedInt >(signedInt), unsignedInt);
		ASSERT_EQ(signedInt, lizard::signed_cast< SignedInt >(unsignedInt));

		if (signedInt > 0) {
			// For negative numbers we require that the corresponding unsigned representation is bigger
			// than any positive number that can be represented with the signed type.
			signedInt *= -1;
			unsignedInt = lizard::signed_cast< UnsignedInt >(signedInt);
			ASSERT_GT(unsignedInt, static_cast< UnsignedInt >(std::numeric_limits< decltype(signedInt) >::max()));
			ASSERT_EQ(signedInt, lizard::signed_cast< SignedInt >(unsignedInt));
		}

		if (i == iterations) {
			// In case iterations == max. representable value, the regular for-loop logic would produce an endless loop
			break;
		}
	}

	for (SignedInt i = 0; i <= iterations; ++i) {
		if (i == iterations) {
			// Ensure we test the max. value
			unsignedInt = std::numeric_limits< UnsignedInt >::max();
		} else if (i == 0) {
			// Ensure we test the value just outside the range of the signed type
			unsignedInt = static_cast< UnsignedInt >(std::numeric_limits< SignedInt >::max()) + 1;
		} else {
			unsignedInt = static_cast< UnsignedInt >(std::numeric_limits< SignedInt >::max()) + i * increment;
		}

		// For unsigned values that are bigger than any positive value representable with the signed
		// correspondent, we expect the conversion to signed values to end up as a negative number (< 0)
		signedInt = lizard::signed_cast< SignedInt >(unsignedInt);

		ASSERT_LT(signedInt, 0);
		ASSERT_EQ(unsignedInt, lizard::signed_cast< UnsignedInt >(signedInt));

		if (i == iterations) {
			// In case iterations == max. representable value, the regular for-loop logic would produce an endless loop
			break;
		}
	}
}

static constexpr const int nIterations = std::numeric_limits< std::int8_t >::max();

TEST(Core, signed_cast_int8) {
	test_signed_cast< std::int8_t, nIterations >();
}

TEST(Core, signed_cast_int16) {
	test_signed_cast< std::int16_t, nIterations >();
}

TEST(Core, signed_cast_int32) {
	test_signed_cast< std::int32_t, nIterations >();
}

TEST(Core, signed_cast_int64) {
	test_signed_cast< std::int64_t, nIterations >();
}