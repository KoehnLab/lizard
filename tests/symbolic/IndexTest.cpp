// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#include "lizard/symbolic/Index.hpp"

#include <gtest/gtest.h>

#include <algorithm>
#include <limits>
#include <set>
#include <tuple>
#include <unordered_set>
#include <vector>


using namespace lizard;


////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// TEST FIXTURES //////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////


struct IndexTest : ::testing::TestWithParam< IndexSpace > {};


////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// TEST CASES ////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

TEST_P(IndexTest, equality_and_hash) {
	static_assert(std::numeric_limits< std::size_t >::max() > std::numeric_limits< Index::Id >::max(),
				  "This will produce an endless loop");

	const IndexSpace space = GetParam();

	std::vector< Index > indices;

	// Create all possible indices with the given space
	for (std::size_t id = 0; id < std::numeric_limits< Index::Id >::max(); ++id) {
		for (IndexType type : { IndexType::Annihilator, IndexType::Creator, IndexType::External }) {
			indices.emplace_back(static_cast< Index::Id >(id), space, type);
		}
	}

	ASSERT_EQ(indices.size(), std::set< Index >(indices.begin(), indices.end()).size())
		<< "Wrong duplicates detected via operator<";

	std::sort(indices.begin(), indices.end());
	ASSERT_EQ(std::adjacent_find(indices.begin(), indices.end()), indices.end())
		<< "Wrong duplicates detected via operator== (adjacent_find)";

	std::vector< std::size_t > hashes;
	hashes.reserve(indices.size());
	for (const Index &index : indices) {
		hashes.push_back(std::hash< Index >{}(index));
	}

	ASSERT_EQ(hashes.size(), std::set< std::size_t >(hashes.begin(), hashes.end()).size()) << "Hash collision detected";
}


////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// TEST SUITE INSTANTIATIONS ////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

INSTANTIATE_TEST_SUITE_P(Index, IndexTest,
						 ::testing::Values(IndexSpace(42, Spin::Alpha), IndexSpace(0, Spin::Beta),
										   IndexSpace(255, Spin::None)));
