// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/Krzmbrzl/lizard/blob/main/LICENSE>.

#include "lizard/core/CumulativeSize.hpp"
#include "lizard/core/SizedStorage.hpp"
#include "lizard/core/VariadicHelpers.hpp"

#include <gtest/gtest.h>

#include <cstddef>
#include <cstdint>
#include <type_traits>

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

using namespace ::lizard;

using test_until = std::uint64_t;

TEST(Core, sized_storage) {
	constexpr_for< 1, sizeof(test_until) * 8 >([](auto requestedMinBitSize) {
		ASSERT_GE(sizeof(sized_storage_t< requestedMinBitSize >) * 8, requestedMinBitSize);
	});
}

TEST(Core, sized_octet_storage) {
	constexpr_for< 1, sizeof(test_until) >([](auto requestedMinOctetSize) {
		ASSERT_GE(sizeof(sized_octet_storage_t< requestedMinOctetSize >), requestedMinOctetSize);
	});
}

enum class TestEnum : std::uint64_t { A, B };

TEST(Core, cumulative_octet_size) {
	ASSERT_EQ(cumulative_octet_size_v<>, 0);
	ASSERT_EQ(cumulative_octet_size_v< std::uint8_t >, 1);
	ASSERT_EQ((cumulative_octet_size_v< std::uint8_t, std::uint16_t >), 3);
	ASSERT_EQ((cumulative_octet_size_v< std::uint8_t, std::uint16_t, TestEnum >), 11);
}

TEST(Core, cumulative_bit_size) {
	ASSERT_EQ(cumulative_bit_size_v<>, 0);
	ASSERT_EQ(cumulative_bit_size_v< std::uint8_t >, 1 * 8);
	ASSERT_EQ((cumulative_bit_size_v< std::uint8_t, std::uint16_t >), 3 * 8);
	ASSERT_EQ((cumulative_bit_size_v< std::uint8_t, std::uint16_t, TestEnum >), 11 * 8);
}

struct Dummy {};

TEST(Core, nth_element) {
	ASSERT_TRUE((std::is_same_v< nth_element_t< 0, int, double, Dummy >, int >) );
	ASSERT_TRUE((std::is_same_v< nth_element_t< 1, int, double, Dummy >, double >) );
	ASSERT_TRUE((std::is_same_v< nth_element_t< 2, int, double, Dummy >, Dummy >) );
}

TEST(Core, get_first) {
	ASSERT_TRUE((std::is_same_v< get_first_t< int, double, Dummy >, int >) );
	ASSERT_TRUE((std::is_same_v< get_first_t< int >, int >) );
}

TEST(Core, get_last) {
	ASSERT_TRUE((std::is_same_v< get_last_t< int, double, Dummy >, Dummy >) );
	ASSERT_TRUE((std::is_same_v< get_last_t< int >, int >) );
}

TEST(Core, index_of) {
	ASSERT_EQ((index_of_v< int, int, float, Dummy >), 0);
	ASSERT_EQ((index_of_v< float, int, float, Dummy >), 1);
	ASSERT_EQ((index_of_v< Dummy, int, float, Dummy >), 2);

	// Same test but this time with arbitrary CV qualifiers
	ASSERT_EQ((index_of_v< const int, int, float, Dummy >), 0);
	ASSERT_EQ((index_of_v< float &, int, float, Dummy >), 1);
	ASSERT_EQ((index_of_v< Dummy &&, int, float, Dummy >), 2);
}

TEST(Core, is_contained) {
	ASSERT_TRUE((is_contained_v< int, float, const int, double >) );
	ASSERT_FALSE((is_contained_v< int *, float, const int, double >) );
	ASSERT_TRUE((is_contained_v< float, float, const int, double >) );
	ASSERT_TRUE((is_contained_v< double &, float, const int, double >) );
}
