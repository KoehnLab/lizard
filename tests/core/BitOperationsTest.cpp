// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#include "lizard/core/BitOperations.hpp"

#include <gtest/gtest.h>

#include <bitset>
#include <cstdint>


using namespace ::lizard;

////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// TEST CASES ////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

TEST(bit_rotate_left, uint8) {
	std::uint8_t value = 0b10011011; // NOLINT(*-magic-numbers)
	const std::bitset< 8 > noRotate(value);
	const std::bitset< 8 > rotateOne(0b00110111);
	const std::bitset< 8 > rotateTwo(0b01101110);
	const std::bitset< 8 > rotateThree(0b11011100);

	{
		std::bitset< 8 > rotated = bit_rotate< std::uint8_t, RotateDirection::Left >(value, 0);
		ASSERT_EQ(rotated, noRotate);
	}
	{
		std::bitset< 8 > rotated = bit_rotate< std::uint8_t, RotateDirection::Left >(value, 1);
		ASSERT_EQ(rotated, rotateOne);
	}
	{
		std::bitset< 8 > rotated = bit_rotate< std::uint8_t, RotateDirection::Left >(value, 2);
		ASSERT_EQ(rotated, rotateTwo);
	}
	{
		std::bitset< 8 > rotated = bit_rotate< std::uint8_t, RotateDirection::Left >(value, 3);
		ASSERT_EQ(rotated, rotateThree);
	}
	{
		std::bitset< 8 > rotated = bit_rotate< std::uint8_t, RotateDirection::Left >(value, 8);
		ASSERT_EQ(rotated, noRotate);
	}
	{
		std::bitset< 8 > rotated = bit_rotate< std::uint8_t, RotateDirection::Left >(value, 9);
		ASSERT_EQ(rotated, rotateOne);
	}
	{
		std::bitset< 8 > rotated = bit_rotate< std::uint8_t, RotateDirection::Left >(value, 10);
		ASSERT_EQ(rotated, rotateTwo);
	}
}

TEST(bit_rotate_right, uint8) {
	const std::uint8_t value = 0b10011011;
	const std::bitset< 8 > noRotate(value);
	const std::bitset< 8 > rotateOne(0b11001101);
	const std::bitset< 8 > rotateTwo(0b11100110);
	const std::bitset< 8 > rotateThree(0b01110011);

	{
		std::bitset< 8 > rotated = bit_rotate< std::uint8_t, RotateDirection::Right >(value, 0);
		ASSERT_EQ(rotated, noRotate);
	}
	{
		std::bitset< 8 > rotated = bit_rotate< std::uint8_t, RotateDirection::Right >(value, 1);
		ASSERT_EQ(rotated, rotateOne);
	}
	{
		std::bitset< 8 > rotated = bit_rotate< std::uint8_t, RotateDirection::Right >(value, 2);
		ASSERT_EQ(rotated, rotateTwo);
	}
	{
		std::bitset< 8 > rotated = bit_rotate< std::uint8_t, RotateDirection::Right >(value, 3);
		ASSERT_EQ(rotated, rotateThree);
	}
	{
		std::bitset< 8 > rotated = bit_rotate< std::uint8_t, RotateDirection::Right >(value, 8);
		ASSERT_EQ(rotated, noRotate);
	}
	{
		std::bitset< 8 > rotated = bit_rotate< std::uint8_t, RotateDirection::Right >(value, 9);
		ASSERT_EQ(rotated, rotateOne);
	}
	{
		std::bitset< 8 > rotated = bit_rotate< std::uint8_t, RotateDirection::Right >(value, 10);
		ASSERT_EQ(rotated, rotateTwo);
	}
}

TEST(bit_rotate_left, uint16) {
	const std::uint16_t value = 0b1001101100001010;
	const std::bitset< 16 > noRotate(value);
	const std::bitset< 16 > rotateOne(0b0011011000010101);
	const std::bitset< 16 > rotateTwo(0b0110110000101010);
	const std::bitset< 16 > rotateThree(0b1101100001010100);

	{
		std::bitset< 16 > rotated = bit_rotate< std::uint16_t, RotateDirection::Left >(value, 0);
		ASSERT_EQ(rotated, noRotate);
	}
	{
		std::bitset< 16 > rotated = bit_rotate< std::uint16_t, RotateDirection::Left >(value, 1);
		ASSERT_EQ(rotated, rotateOne);
	}
	{
		std::bitset< 16 > rotated = bit_rotate< std::uint16_t, RotateDirection::Left >(value, 2);
		ASSERT_EQ(rotated, rotateTwo);
	}
	{
		std::bitset< 16 > rotated = bit_rotate< std::uint16_t, RotateDirection::Left >(value, 3);
		ASSERT_EQ(rotated, rotateThree);
	}
	{
		std::bitset< 16 > rotated = bit_rotate< std::uint16_t, RotateDirection::Left >(value, 16);
		ASSERT_EQ(rotated, noRotate);
	}
	{
		std::bitset< 16 > rotated =
			bit_rotate< std::uint16_t, RotateDirection::Left >(value, 17); // NOLINT(*-magic-numbers)
		ASSERT_EQ(rotated, rotateOne);
	}
	{
		std::bitset< 16 > rotated =
			bit_rotate< std::uint16_t, RotateDirection::Left >(value, 18); // NOLINT(*-magic-numbers)
		ASSERT_EQ(rotated, rotateTwo);
	}
}

TEST(bit_rotate_right, uint16) {
	const std::uint16_t value = 0b1001101100001010;
	const std::bitset< 16 > noRotate(value);
	const std::bitset< 16 > rotateOne(0b0100110110000101);
	const std::bitset< 16 > rotateTwo(0b01010011011000010);
	const std::bitset< 16 > rotateThree(0b00101001101100001);

	{
		std::bitset< 16 > rotated = bit_rotate< std::uint16_t, RotateDirection::Right >(value, 0);
		ASSERT_EQ(rotated, noRotate);
	}
	{
		std::bitset< 16 > rotated = bit_rotate< std::uint16_t, RotateDirection::Right >(value, 1);
		ASSERT_EQ(rotated, rotateOne);
	}
	{
		std::bitset< 16 > rotated = bit_rotate< std::uint16_t, RotateDirection::Right >(value, 2);
		ASSERT_EQ(rotated, std::bitset< 16 >(rotateTwo));
	}
	{
		std::bitset< 16 > rotated = bit_rotate< std::uint16_t, RotateDirection::Right >(value, 3);
		ASSERT_EQ(rotated, std::bitset< 16 >(rotateThree));
	}
	{
		std::bitset< 16 > rotated = bit_rotate< std::uint16_t, RotateDirection::Right >(value, 16);
		ASSERT_EQ(rotated, noRotate);
	}
	{
		std::bitset< 16 > rotated =
			bit_rotate< std::uint16_t, RotateDirection::Right >(value, 17); // NOLINT(*-magic-numbers)
		ASSERT_EQ(rotated, rotateOne);
	}
	{
		std::bitset< 16 > rotated =
			bit_rotate< std::uint16_t, RotateDirection::Right >(value, 18); // NOLINT(*-magic-numbers)
		ASSERT_EQ(rotated, rotateTwo);
	}
}
