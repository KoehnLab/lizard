// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#include "SpinLSE.hpp"

#include "Utils.hpp"

#include "lizard/symbolic/Index.hpp"

#include <gtest/gtest.h>

#include <tuple>
#include <utility>
#include <vector>


using namespace lizard;



////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// TEST FIXTURES //////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

struct SpinLSETest
	: ::testing::TestWithParam< std::tuple< std::vector< std::pair< std::vector< Index >, int > >, std::size_t > > {
	using Params = std::tuple< std::vector< std::pair< std::vector< Index >, int > >, std::size_t >;
};


////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// TEST CASES ////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////


TEST_P(SpinLSETest, solve) {
	const std::vector< std::pair< std::vector< Index >, int > > indexSequences = std::get< 0 >(GetParam());
	const std::size_t expectedSolutionCount                                    = std::get< 1 >(GetParam());

	SpinLSE system;

	// Setup LSE
	for (auto [currentIndices, spinProjection] : indexSequences) {
		system.beginEquation();

		for (const Index &index : currentIndices) {
			system.addTerm(index, static_cast< int >(index.getType()));
		}

		system.endEquation(spinProjection);
	}

	std::vector< std::vector< Spin > > solutions = system.solve();

	ASSERT_EQ(solutions.size(), expectedSolutionCount);

	for (const std::vector< Spin > &currentSolution : solutions) {
		for (auto [currentIndices, spinProjection] : indexSequences) {
			int overallSpinChange = 0;
			for (Index currentIndex : currentIndices) {
				auto iter = std::find_if(system.getVariables().begin(), system.getVariables().end(),
										 Index::FindByName{ currentIndex });
				if (iter == system.getVariables().end()) {
					continue;
				}
				std::size_t pos = static_cast< std::size_t >(std::distance(system.getVariables().begin(), iter));

				static_assert(static_cast< int >(Spin::Alpha) == 1);
				static_assert(static_cast< int >(Spin::Beta) == -1);
				static_assert(static_cast< int >(IndexType::Creator) == 1);
				static_assert(static_cast< int >(IndexType::Annihilator) == -1);
				overallSpinChange +=
					static_cast< int >(currentSolution.at(pos)) * static_cast< int >(currentIndex.getType());
			}

			ASSERT_EQ(overallSpinChange, spinProjection);
		}
	}
}


////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// TEST SUITE INSTANTIATIONS ////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

INSTANTIATE_TEST_SUITE_P(
	SpinLSE, SpinLSETest,
	::testing::Values(SpinLSETest::Params({ { { test::createIndexSequence("[]"), 0 } }, 0 }),
					  SpinLSETest::Params({ { { test::createIndexSequence("[a+, i-]"), 0 } }, 2 }),
					  SpinLSETest::Params({ { { test::createIndexSequence("[a-, i+]"), 0 } }, 2 }),
					  SpinLSETest::Params({ { { test::createIndexSequence("[a+, b+, i-, j-]"), 0 } }, 6 })));
