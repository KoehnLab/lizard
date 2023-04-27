// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#include "PrettyPrint.hpp"
#include "Utils.hpp"

#include "lizard/process/SpinIntegration.hpp"
#include "lizard/symbolic/TensorExpressions.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <string>
#include <tuple>
#include <vector>


using namespace lizard;


////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// TEST FIXTURES //////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

struct SpinIntegrationTest : ::testing::TestWithParam< std::tuple< std::string, std::vector< std::string > > > {
	using Params = std::tuple< std::string, std::vector< std::string > >;
};



////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// TEST CASES ////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////


TEST_P(SpinIntegrationTest, process) {
	const std::string &inputTreeSpec                 = std::get< 0 >(GetParam());
	const std::vector< std::string > outputTreeSpecs = std::get< 1 >(GetParam());

	std::vector< NamedTensorExprTree > expectedResults;
	expectedResults.reserve(outputTreeSpecs.size());
	for (const std::string &currentSpec : outputTreeSpecs) {
		expectedResults.push_back(test::createTree< NamedTensorExprTree >(currentSpec));
	}

	SpinIntegration integrator;

	std::vector< NamedTensorExprTree > actualResults = { test::createTree< NamedTensorExprTree >(inputTreeSpec) };
	integrator.process(actualResults, test::getIndexSpaceManager());

	ASSERT_EQ(actualResults[0], expectedResults[0]);
	ASSERT_THAT(actualResults, ::testing::UnorderedElementsAreArray(expectedResults));
}


////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// TEST SUITE INSTANTIATIONS ////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

INSTANTIATE_TEST_SUITE_P(
	SpinIntegration, SpinIntegrationTest,
	::testing::Values(
		SpinIntegrationTest::Params({ "R[] = H[]", { "R[] = H[]" } }),
		SpinIntegrationTest::Params({ "R[] = H[a+, i-](||) * T[i+, a-](||)",
									  { R"(R[] = H[a+, i-](//) * T[i+, a-](//) + H[a+, i-](\\) * T[i+,a-](\\))" } })
		// This test requires us to be able to fold constants in a given addend in order to not introduce superfluous
		// sign factors
		// SpinIntegrationTest::Params({ "E[] = H[] + H[i+,j+,a-,b-](||||) *  T[a+,b+,i-,j-](||||)",
		//							  { R"(E[] = H[] + H[i+,j+,a-b-](////) * T[a+,b+,i-j-](////) + H[i+,j+,a-b-](\/\/) *
		// T[a+,b+,i-j-](\/\/))" 							   R"(+ H[i+,j+,a-b-](/\\/) * T[a+,b+,i-j-](\//\) +
		// H[i+,j+,a-b-](\//\)
		// * T[a+,b+,i-j-](/\\/))" 							   R"(+ H[i+,j+,a-b-](/\/\) * T[a+,b+,i-j-](/\/\) +
		// H[i+,j+,a-b-](\\\\) * T[a+,b+,i-j-](\\\\))" } })
		));
