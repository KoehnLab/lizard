// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/Krzmbrzl/lizard/blob/main/LICENSE>.

#include "lizard/core/CumulativeSize.hpp"
#include "lizard/core/SizedStorage.hpp"

#include <gtest/gtest.h>

#include <cstdint>
#include <type_traits>

namespace lizard::test::parser {

// Helper function to implement a constexpr for loop over interval [Start, End]
// Taken from https://artificial-mind.net/blog/2020/10/31/constexpr-for
template< std::size_t Start, std::size_t End, std::size_t Increment = 1, typename Functor >
constexpr void constexpr_for(Functor &&loopBody) {
	if constexpr (Start <= End) {
		// Call function containing loop body
		loopBody(std::integral_constant< decltype(Start), Start >());
		// Step to the next loop iteration where Start += Increment
		constexpr_for< Start + Increment, End, Increment >(std::forward< Functor >(loopBody));
	}
}

using test_until = std::uint64_t;

TEST(Core, sized_storage) {
	constexpr_for< 1, sizeof(test_until) * 8 >([](auto requestedMinBitSize) {
		ASSERT_GE(sizeof(::lizard::core::sized_storage_t< requestedMinBitSize >) * 8, requestedMinBitSize);
	});
}

TEST(Core, sized_octet_storage) {
	constexpr_for< 1, sizeof(test_until) >([](auto requestedMinOctetSize) {
		ASSERT_GE(sizeof(::lizard::core::sized_octet_storage_t< requestedMinOctetSize >), requestedMinOctetSize);
	});
}

enum class TestEnum : std::uint64_t { A, B };

TEST(Core, cumulative_byte_size) {
	ASSERT_EQ(::lizard::core::cumulative_byte_size_v<>, 0);
	ASSERT_EQ(::lizard::core::cumulative_byte_size_v< std::uint8_t >, 1);
	ASSERT_EQ((::lizard::core::cumulative_byte_size_v< std::uint8_t, std::uint16_t >), 3);
	ASSERT_EQ((::lizard::core::cumulative_byte_size_v< std::uint8_t, std::uint16_t, TestEnum >), 11);
}

TEST(Core, cumulative_bit_size) {
	ASSERT_EQ(::lizard::core::cumulative_bit_size_v<>, 0);
	ASSERT_EQ(::lizard::core::cumulative_bit_size_v< std::uint8_t >, 1 * 8);
	ASSERT_EQ((::lizard::core::cumulative_bit_size_v< std::uint8_t, std::uint16_t >), 3 * 8);
	ASSERT_EQ((::lizard::core::cumulative_bit_size_v< std::uint8_t, std::uint16_t, TestEnum >), 11 * 8);
}

} // namespace lizard::test::parser
