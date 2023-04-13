// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#include "Utils.hpp"

#include "lizard/symbolic/Index.hpp"
#include "lizard/symbolic/TensorElement.hpp"

#include <libperm/Cycle.hpp>
#include <libperm/ExplicitPermutation.hpp>
#include <libperm/PrimitivePermutationGroup.hpp>
#include <libperm/SpecialGroups.hpp>
#include <libperm/Utils.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <tuple>
#include <vector>


using namespace lizard;


////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// TEST FIXTURES //////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////


struct TensorElementTest
	: ::testing::TestWithParam< std::tuple< Tensor, perm::PrimitivePermutationGroup, std::vector< Index > > > {};



////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// TEST CASES ////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

TEST_P(TensorElementTest, construction_and_equality) {
	const Tensor tensor                            = std::get< 0 >(GetParam());
	const perm::PrimitivePermutationGroup symmetry = std::get< 1 >(GetParam());
	std::vector< Index > indices                   = std::get< 2 >(GetParam());

	std::vector< Index > canonicalIndices = indices;
	int canonicalizationSign              = perm::canonicalize(canonicalIndices, symmetry);

	auto [element1, sign1] = TensorElement::create(tensor, indices, symmetry);
	ASSERT_EQ(sign1, canonicalizationSign);
	ASSERT_THAT(element1.getIndices(), ::testing::ElementsAreArray(canonicalIndices));


	std::vector< IndexSpace > slots(indices.size());
	for (std::size_t i = 0; i < indices.size(); ++i) {
		slots[i] = indices[i].getSpace();
	}

	perm::Permutation slotCanonicalization = perm::computeCanonicalizationPermutation(slots, symmetry);

	auto [block, blockSign] = TensorBlock::create(tensor, slots, symmetry);
	ASSERT_EQ(slotCanonicalization->sign(), blockSign);
	ASSERT_EQ(element1.getBlock(), block);


	// Before we can use the indices to construct the element corresponding to the just generated block, we have
	// to apply the slot-canonicalization to the indices in order to ensure that the index-to-slot correspondence
	// is retained
	perm::applyPermutation(indices, slotCanonicalization);

	auto [element2, sign2] = TensorElement::create(block, indices);
	ASSERT_EQ(element2.getBlock(), block);
	ASSERT_EQ(sign2 * slotCanonicalization->sign(), canonicalizationSign);
	ASSERT_THAT(element2.getIndices(), ::testing::ElementsAreArray(canonicalIndices));

	ASSERT_EQ(element1, element2);
}

TEST(TensorElement, hash) {
	const perm::PrimitivePermutationGroup symmetries = test::generate(
		{ perm::ExplicitPermutation(perm::Cycle({ 0, 1 }), -1), perm::ExplicitPermutation(perm::Cycle({ 2, 3 }), -1) });

	std::vector< Index > indices = test::indexSequence(4);

	// Ensure indices are sorted to make next_permutation work as expected
	std::sort(indices.begin(), indices.end());

	std::vector< std::size_t > uniqueHashes;
	std::vector< TensorElement > uniqueElements;

	do {
		TensorElement element = std::get< 0 >(TensorElement::create(Tensor("Dummy"), indices, symmetries));

		std::size_t hash = std::hash< TensorElement >{}(element);

		if (std::find(uniqueElements.begin(), uniqueElements.end(), element) == uniqueElements.end()) {
			ASSERT_EQ(std::find(uniqueHashes.begin(), uniqueHashes.end(), hash), uniqueHashes.end());

			uniqueElements.push_back(element);
			uniqueHashes.push_back(hash);
		} else {
			ASSERT_EQ(
				std::distance(uniqueElements.begin(), std::find(uniqueElements.begin(), uniqueElements.end(), element)),
				std::distance(uniqueHashes.begin(), std::find(uniqueHashes.begin(), uniqueHashes.end(), hash)));
		}
	} while (std::next_permutation(indices.begin(), indices.end()));

	// There are a total of 24 permutations of a sequence with 4 unique elements and therefore the order
	// of a subgroup of Sym(4) must be a factor of 24
	ASSERT_EQ(24 % symmetries.order(), 0);
	std::size_t expectedUniqueElements = static_cast< std::size_t >(24) / symmetries.order();

	ASSERT_EQ(uniqueElements.size(), expectedUniqueElements);
	ASSERT_EQ(uniqueHashes.size(), expectedUniqueElements);
}


////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// TEST SUITE INSTANTIATIONS ////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

INSTANTIATE_TEST_SUITE_P(
	TensorElement, TensorElementTest,
	::testing::Combine(::testing::Values(Tensor("T")),
					   ::testing::Values(perm::Sym(3), perm::Sym(4),
										 test::generate({ perm::ExplicitPermutation(perm::Cycle({ 0, 1 }), -1),
														  perm::ExplicitPermutation(perm::Cycle({ 2, 3 }), -1) })),
					   ::testing::Values(test::indexSequence(4, 1), test::indexSequence(4, 2))));
