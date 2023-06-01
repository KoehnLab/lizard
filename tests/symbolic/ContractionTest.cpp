// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#include "Utils.hpp"

#include "lizard/symbolic/Contraction.hpp"
#include "lizard/symbolic/Index.hpp"
#include "lizard/symbolic/IndexSpace.hpp"
#include "lizard/symbolic/IndexType.hpp"
#include "lizard/symbolic/Spin.hpp"
#include "lizard/symbolic/Tensor.hpp"
#include "lizard/symbolic/TensorBlock.hpp"
#include "lizard/symbolic/TensorElement.hpp"

#include <libperm/SpecialGroups.hpp>
#include <libperm/Utils.hpp>

#include <gtest/gtest.h>

#include <tuple>
#include <vector>


using namespace lizard;

////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// TEST FIXTURES //////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

class ContractionTest
	: public ::testing::TestWithParam<
		  std::tuple< std::vector< std::string >, std::vector< std::string >, std::vector< std::string > > > {
	using ParamPack = std::tuple< std::vector< std::string >, std::vector< std::string >, std::vector< std::string > >;
};


////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// TEST CASES ////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////


TEST_P(ContractionTest, contractIndices) {
	std::vector< Index > lhs      = test::createIndices(std::get< 0 >(GetParam()));
	std::vector< Index > rhs      = test::createIndices(std::get< 1 >(GetParam()));
	std::vector< Index > expected = test::createIndices(std::get< 2 >(GetParam()));

	std::vector< Index > actual = contractIndices(lhs, rhs);

	ASSERT_EQ(actual, expected);
}

TEST(Contraction, contract) {
	// Note: In order for the resulting symmetry to be checked by this test, the test case has to be built
	// in Debug mode (the symmetry is checked by an assert in TensorBlock::operator==)
	const Tensor lhsTensor("LHS");
	const Tensor rhsTensor("RHS");
	const Tensor resultTensor("R");

	{
		std::vector< Index > lhsIndices       = test::createIndices({ "i+/", "j+/", "a-/", "b-/" });
		TensorBlock::SlotSymmetry lhsSymmetry = perm::antisymmetricRanges({ { 0, 1 }, { 2, 3 } });
		ASSERT_TRUE(perm::computeCanonicalizationPermutation(lhsIndices, lhsSymmetry)->isIdentity());
		TensorElement lhs = std::get< 0 >(TensorElement::create(lhsTensor, lhsIndices, lhsSymmetry));

		std::vector< Index > rhsIndices       = test::createIndices({ "a+/", "b+/", "i-/", "j-/" });
		TensorBlock::SlotSymmetry rhsSymmetry = perm::antisymmetricRanges({ { 0, 1 }, { 2, 3 } });
		ASSERT_TRUE(perm::computeCanonicalizationPermutation(rhsIndices, rhsSymmetry)->isIdentity());
		TensorElement rhs = std::get< 0 >(TensorElement::create(rhsTensor, rhsIndices, rhsSymmetry));

		TensorElement expected = TensorElement(resultTensor);
		TensorElement actual   = contract(lhs, rhs, "R");

		ASSERT_EQ(actual, expected);
	}

	{
		std::vector< Index > lhsIndices       = test::createIndices({ "i+/", "j+/", "a-/", "b-/" });
		TensorBlock::SlotSymmetry lhsSymmetry = perm::antisymmetricRanges({ { 0, 1 }, { 2, 3 } });
		ASSERT_TRUE(perm::computeCanonicalizationPermutation(lhsIndices, lhsSymmetry)->isIdentity());
		TensorElement lhs = std::get< 0 >(TensorElement::create(lhsTensor, lhsIndices, lhsSymmetry));

		std::vector< Index > rhsIndices       = test::createIndices({ "a+/", "c+/", "k-/", "l-/" });
		TensorBlock::SlotSymmetry rhsSymmetry = perm::antisymmetricRanges({ { 0, 1 }, { 2, 3 } });
		ASSERT_TRUE(perm::computeCanonicalizationPermutation(rhsIndices, rhsSymmetry)->isIdentity());
		TensorElement rhs = std::get< 0 >(TensorElement::create(rhsTensor, rhsIndices, rhsSymmetry));

		std::vector< Index > expectedIndices       = test::createIndices({ "i+/", "j+/", "b-/", "c+/", "k-/", "l-/" });
		TensorBlock::SlotSymmetry expectedSymmetry = perm::antisymmetricRanges({ { 0, 1 }, { 4, 5 } });
		ASSERT_TRUE(perm::computeCanonicalizationPermutation(expectedIndices, expectedSymmetry)->isIdentity());
		TensorElement expected = std::get< 0 >(TensorElement::create(resultTensor, expectedIndices, expectedSymmetry));

		TensorElement actual = contract(lhs, rhs, "R");

		ASSERT_EQ(actual, expected);
	}
}


////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// TEST SUITE INSTANTIATIONS ////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

// clang-format off
INSTANTIATE_TEST_SUITE_P(Contraction, ContractionTest, ::testing::Values(
	ContractionTest::ParamType{ {}, {}, {} },
	ContractionTest::ParamType{
		{ "i+/", "j+/" },
		{ "j-/", "i-/" },
		{}
	},
	ContractionTest::ParamType{
		{ "i+/", "b-\\" },
		{ "j-/", "i-/" },
		{ "b-\\", "j-/" }
	},
	ContractionTest::ParamType{
		{ "i-/", "b+\\" },
		{ "a+/", "b-\\" },
		{ "i-/", "a+/" }
	}
));
// clang-format on
