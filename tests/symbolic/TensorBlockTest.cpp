// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#include "Utils.hpp"

#include "lizard/symbolic/IndexSpace.hpp"
#include "lizard/symbolic/Tensor.hpp"
#include "lizard/symbolic/TensorBlock.hpp"

#include <libperm/ExplicitPermutation.hpp>
#include <libperm/PrimitivePermutationGroup.hpp>
#include <libperm/SpecialGroups.hpp>
#include <libperm/Utils.hpp>

#include <gtest/gtest.h>

#include <algorithm>
#include <functional>
#include <string>
#include <vector>


using namespace lizard;


////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// TEST FIXTURES //////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////


struct TensorBlockTest
	: ::testing::TestWithParam<
		  std::tuple< std::tuple< Tensor, perm::PrimitivePermutationGroup >, std::vector< IndexSpace > > > {};



////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// TEST CASES ////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

TEST_P(TensorBlockTest, construction_and_equality) {
	const auto [tensor, symmetry]         = std::get< 0 >(GetParam());
	const std::vector< IndexSpace > slots = std::get< 1 >(GetParam());

	std::vector< IndexSpace > canonicalSlots = slots;
	int canonicalizationSign                 = perm::canonicalize(canonicalSlots, symmetry);

	auto [block, sign] = TensorBlock::create(tensor, slots, symmetry);

	ASSERT_EQ(sign, canonicalizationSign);
	ASSERT_EQ(block.getIndexSlots(), canonicalSlots);
	ASSERT_EQ(block.getSlotSymmetry(), symmetry);
	ASSERT_EQ(block.getTensor(), tensor);

	auto [canonicalBlock, canonicalSign] = TensorBlock::create(tensor, canonicalSlots, symmetry);
	ASSERT_EQ(canonicalSign, 1);
	ASSERT_EQ(canonicalBlock.getIndexSlots(), canonicalSlots);
	ASSERT_EQ(canonicalBlock.getSlotSymmetry(), symmetry);
	ASSERT_EQ(canonicalBlock.getTensor(), tensor);

	ASSERT_EQ(block, canonicalBlock);
}

TEST(TensorBlock, hash) {
	const perm::PrimitivePermutationGroup symmetries =
		test::generate({ perm::ExplicitPermutation({ 1, 0 }, -1), perm::ExplicitPermutation({ 0, 1, 3, 2 }, -1) });

	std::vector< IndexSpace > slots = { IndexSpace(0, Spin::None), IndexSpace(1, Spin::None), IndexSpace(2, Spin::None),
										IndexSpace(3, Spin::None) };
	ASSERT_TRUE(std::is_sorted(slots.begin(), slots.end()));

	std::vector< std::size_t > uniqueHashes;
	std::vector< TensorBlock > uniqueBlocks;

	do {
		TensorBlock block = std::get< 0 >(TensorBlock::create(Tensor("Dummy"), slots, symmetries));

		std::size_t hash = std::hash< TensorBlock >{}(block);

		if (std::find(uniqueBlocks.begin(), uniqueBlocks.end(), block) == uniqueBlocks.end()) {
			ASSERT_EQ(std::find(uniqueHashes.begin(), uniqueHashes.end(), hash), uniqueHashes.end());

			uniqueBlocks.push_back(block);
			uniqueHashes.push_back(hash);
		} else {
			ASSERT_EQ(std::distance(uniqueBlocks.begin(), std::find(uniqueBlocks.begin(), uniqueBlocks.end(), block)),
					  std::distance(uniqueHashes.begin(), std::find(uniqueHashes.begin(), uniqueHashes.end(), hash)));
		}
	} while (std::next_permutation(slots.begin(), slots.end()));

	// There are a total of 24 permutations of a sequence with 4 unique elements and therefore the order
	// of a subgroup of Sym(4) must be a factor of 24
	ASSERT_EQ(24 % symmetries.order(), 0);
	std::size_t expectedUniqueElements = static_cast< std::size_t >(24) / symmetries.order();

	ASSERT_EQ(uniqueBlocks.size(), expectedUniqueElements);
	ASSERT_EQ(uniqueHashes.size(), expectedUniqueElements);
}


////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// TEST SUITE INSTANTIATIONS ////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

auto generateTensors() -> std::vector< std::tuple< Tensor, perm::PrimitivePermutationGroup > > {
	std::vector< std::tuple< Tensor, perm::PrimitivePermutationGroup > > tensors;

	int tensorIndex = 1;
	for (perm::PrimitivePermutationGroup symmetry : {
			 {},
			 test::generate({ perm::ExplicitPermutation({ 1, 0 }) }),
			 test::generate({ perm::ExplicitPermutation({ 1, 0 }, -1), perm::ExplicitPermutation({ 0, 1, 3, 2 }, -1) }),
		 }) {
		Tensor tensor("T" + std::to_string(tensorIndex++));

		tensors.emplace_back(tensor, symmetry);
	}

	return tensors;
}

auto generateSlots() -> std::vector< std::vector< IndexSpace > > {
	std::vector< std::vector< IndexSpace > > spacesList;

	const std::vector< IndexSpace > sequence = { IndexSpace{ 0, Spin::Alpha }, IndexSpace{ 0, Spin::Beta },
												 IndexSpace{ 2, Spin::None }, IndexSpace{ 2, Spin::Both } };

	return test::equivalentSequences(sequence, perm::Sym(4));
}

INSTANTIATE_TEST_SUITE_P(TensorBlock, TensorBlockTest,
						 ::testing::Combine(::testing::ValuesIn(generateTensors()),
											::testing::ValuesIn(generateSlots())));
