// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#include "PrettyPrint.hpp"
#include "Utils.hpp"

#include "lizard/process/SkeletonQuantityMapper.hpp"
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

struct SkeletonMapperTest : ::testing::TestWithParam< std::tuple< std::string, std::string > > {
	using Params = std::tuple< std::string, std::string >;
};


////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// TEST CASES ////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////


TEST_P(SkeletonMapperTest, process) {
	const std::string &inputTreeSpec    = std::get< 0 >(GetParam());
	const std::string &expectedTreeSpec = std::get< 1 >(GetParam());

	std::vector< NamedTensorExprTree > actual         = { test::createTree< NamedTensorExprTree >(inputTreeSpec) };
	const std::vector< NamedTensorExprTree > expected = { test::createTree< NamedTensorExprTree >(expectedTreeSpec,
																								  true) };

	SkeletonQuantityMapper mapper;
	mapper.process(actual, test::getIndexSpaceManager());

	ASSERT_EQ(actual.size(), 1);
	ASSERT_EQ(expected.size(), 1);

	ASSERT_EQ(actual[0], expected[0]);
}


////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// TEST SUITE INSTANTIATIONS ////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

INSTANTIATE_TEST_SUITE_P(
	SkeletonQuantityMapper, SkeletonMapperTest,
	::testing::Values(SkeletonMapperTest::Params{ "R[] = H[]", "R[] = H[]" },
					  SkeletonMapperTest::Params{ "R[] = A[i+,a-](//) * B[a+,i-](//)", "R[] = A[i+,a-] * B[a+,i-]" },
					  SkeletonMapperTest::Params{ R"(R[] = H[i+,j+,a-,b-](\/\/) * T[a+,b+,i-,j-](\/\/))",
												  "R[] = H[i+,j+,a-,b-] * T[a+,b+,i-,j-]" },
					  SkeletonMapperTest::Params{
						  R"(R[] = H[i+,j+,a-,b-](////) * T[a+,b+,i-,j-](////))",
						  "R[] = (H[i+,j+,a-,b-] + H[j+,i+,a-b-] * -1) * (T[a+,b+,i-,j-] + T[b+,a+,i-,j-] * -1)" }));
