// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/Krzmbrzl/lizard/blob/main/LICENSE>.

#include "lizard/core/MultiEnum.hpp"

#include <gtest/gtest.h>

#include <cstddef>
#include <cstdint>

using namespace ::lizard;

enum UnscopedEnum {
	UNSCOPED_A = 256,
	UNSCOPED_B,
	UNSCOPED_C,
};

enum class E1 : std::uint8_t {
	A,
	B,
	C,
};

enum class E2 : std::uint16_t {
	A = 5,
	B,
	C,
};

TEST(Core, MultiEnum) {
	MultiEnum< E1, UnscopedEnum, E2 > test;

	ASSERT_GE(sizeof(decltype(test)::underlying_type), sizeof(UnscopedEnum) + sizeof(E1) + sizeof(E2));

	test.set(E1::A);
	test.set(E2::B);
	test.set(UNSCOPED_C);

	ASSERT_EQ(test.get< E1 >(), E1::A);
	ASSERT_EQ(test.get< E2 >(), E2::B);
	ASSERT_EQ(test.get< UnscopedEnum >(), UNSCOPED_C);

	test.set(E2::C);

	ASSERT_EQ(test.get< E1 >(), E1::A);
	ASSERT_EQ(test.get< E2 >(), E2::C);
	ASSERT_EQ(test.get< UnscopedEnum >(), UNSCOPED_C);

	decltype(test) copy = test;

	ASSERT_EQ(test, copy);

	copy.set(E1::B);

	ASSERT_NE(test, copy);

	decltype(test) multiTest(UNSCOPED_C, E1::B);
	ASSERT_EQ(multiTest.get< UnscopedEnum >(), UNSCOPED_C);
	ASSERT_EQ(multiTest.get< E1 >(), E1::B);
}
