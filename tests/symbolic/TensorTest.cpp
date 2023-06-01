// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#include "lizard/symbolic/Tensor.hpp"

#include <gtest/gtest.h>

#include <functional>
#include <string>


using namespace lizard;


////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// TEST FIXTURES //////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////


class TensorTest : public ::testing::TestWithParam< std::tuple< std::string, std::string > > {
public:
	using ParamPack = std::tuple< std::string, std::string >;
};


////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// TEST CASES ////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

TEST_P(TensorTest, equality) {
	const std::string firstName  = std::get< 0 >(GetParam());
	const std::string secondName = std::get< 1 >(GetParam());

	const Tensor first(firstName);
	const Tensor second(secondName);
	const Tensor defaultConstructed;

	// Always expect equality with self
	ASSERT_EQ(first, first);
	ASSERT_EQ(second, second);

	if (firstName == secondName) {
		ASSERT_EQ(first, second);
		ASSERT_EQ(second, first);
	} else {
		ASSERT_NE(first, second);
		ASSERT_NE(second, first);
	}

	ASSERT_FALSE(firstName.empty());
	ASSERT_FALSE(secondName.empty());

	ASSERT_NE(first, defaultConstructed);
	ASSERT_NE(second, defaultConstructed);
	ASSERT_NE(defaultConstructed, first);
	ASSERT_NE(defaultConstructed, second);
}

TEST_P(TensorTest, hash) {
	const std::string firstName  = std::get< 0 >(GetParam());
	const std::string secondName = std::get< 1 >(GetParam());

	const Tensor first(firstName);
	const Tensor second(secondName);
	const Tensor defaultConstructed;

	const std::size_t hash1       = std::hash< Tensor >{}(first);
	const std::size_t hash2       = std::hash< Tensor >{}(second);
	const std::size_t defaultHash = std::hash< Tensor >{}(defaultConstructed);

	if (first == second) {
		ASSERT_EQ(hash1, hash2);
	} else {
		ASSERT_NE(hash1, hash2);
	}

	ASSERT_FALSE(firstName.empty());
	ASSERT_FALSE(secondName.empty());

	ASSERT_NE(hash1, defaultHash);
	ASSERT_NE(hash2, defaultHash);
}


////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// TEST SUITE INSTANTIATIONS ////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

INSTANTIATE_TEST_SUITE_P(Tensor, TensorTest,
						 ::testing::Combine(::testing::Values(std::string{ "A" }, std::string{ "B" }),
											::testing::Values(std::string{ "a" }, std::string{ "Berta" },
															  std::string{ "A" })));
