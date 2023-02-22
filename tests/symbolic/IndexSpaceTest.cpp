// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#include "lizard/symbolic/IndexSpace.hpp"
#include "lizard/symbolic/Spin.hpp"

#include <gtest/gtest.h>

#include <functional>

using namespace lizard;


////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// TEST FIXTURES //////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////


class IndexSpaceTest : public ::testing::TestWithParam< std::tuple< IndexSpace::Id, IndexSpace::Id, Spin, Spin > > {
public:
	using ParamPack = std::tuple< IndexSpace::Id, IndexSpace::Id, Spin, Spin >;
};


////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// TEST CASES ////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

TEST_P(IndexSpaceTest, equality) {
	const IndexSpace::Id firstID  = std::get< 0 >(GetParam());
	const IndexSpace::Id secondID = std::get< 1 >(GetParam());
	const Spin firstSpin          = std::get< 2 >(GetParam());
	const Spin secondSpin         = std::get< 3 >(GetParam());

	const bool shouldBeEqual = firstID == secondID && firstSpin == secondSpin;

	const IndexSpace space1(firstID, firstSpin);
	const IndexSpace space2(secondID, secondSpin);
	const IndexSpace defaultSpace;

	if (shouldBeEqual) {
		ASSERT_EQ(space1, space2);
	} else {
		ASSERT_NE(space1, space2);
	}

	ASSERT_NE(space1, defaultSpace);
	ASSERT_NE(space2, defaultSpace);
}

TEST_P(IndexSpaceTest, hash) {
	const IndexSpace::Id firstID  = std::get< 0 >(GetParam());
	const IndexSpace::Id secondID = std::get< 1 >(GetParam());
	const Spin firstSpin          = std::get< 2 >(GetParam());
	const Spin secondSpin         = std::get< 3 >(GetParam());

	const IndexSpace space1(firstID, firstSpin);
	const IndexSpace space2(secondID, secondSpin);
	const IndexSpace defaultSpace;

	const std::size_t hash1       = std::hash< decltype(space1) >{}(space1);
	const std::size_t hash2       = std::hash< decltype(space2) >{}(space2);
	const std::size_t defaultHash = std::hash< decltype(defaultSpace) >{}(defaultSpace);

	if (space1 == space2) {
		ASSERT_EQ(hash1, hash2);
	} else {
		ASSERT_NE(hash1, hash2);
	}

	ASSERT_NE(hash1, defaultHash);
	ASSERT_NE(hash2, defaultHash);
}


////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// TEST SUITE INSTANTIATIONS ////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

INSTANTIATE_TEST_SUITE_P(IndexSpace, IndexSpaceTest,
						 ::testing::Combine(::testing::Values(0, 5), ::testing::Values(5, 128),
											::testing::Values(Spin::Alpha, Spin::Beta, Spin::Both),
											::testing::Values(Spin::Alpha, Spin::None)));
