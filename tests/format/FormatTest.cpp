// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#include "lizard/format/FormatSupport.hpp"
#include "lizard/symbolic/Index.hpp"
#include "lizard/symbolic/IndexSpace.hpp"
#include "lizard/symbolic/IndexSpaceData.hpp"
#include "lizard/symbolic/IndexSpaceManager.hpp"
#include "lizard/symbolic/IndexType.hpp"
#include "lizard/symbolic/Spin.hpp"
#include "lizard/symbolic/Tensor.hpp"
#include "lizard/symbolic/TensorBlock.hpp"
#include "lizard/symbolic/TensorElement.hpp"
#include "lizard/symbolic/TensorExpressions.hpp"

#include <fmt/core.h>

#include <gtest/gtest.h>

using namespace lizard;



////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// TEST FIXTURES //////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

struct FormatTest : ::testing::Test {
	FormatTest() {
		IndexSpace occSpace(0, Spin::Both);
		IndexSpace virtSpace(1, Spin::Both);

		IndexSpaceData occData("occ", 10, Spin::Both, { 'i', 'j', 'k', 'l' }, '\'');
		IndexSpaceData virtData("virt", 100, Spin::Both, { 'a', 'b', 'c', 'd' }, '\'');

		m_manager.registerSpace(occSpace, occData);
		m_manager.registerSpace(virtSpace, virtData);
	}

	[[nodiscard]] auto getManager() -> const IndexSpaceManager & { return m_manager; }

private:
	IndexSpaceManager m_manager;
};


////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// TEST CASES ////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

TEST_F(FormatTest, Tensor) {
	ASSERT_EQ(fmt::format("{}", Tensor("Dummy")), "Dummy");
}

TEST_F(FormatTest, IndexSpace) {
	// The space's spin doesn't matter for the formatting
	for (Spin spin : { Spin::Alpha, Spin::Beta, Spin::Both, Spin::None }) {
		IndexSpace occSpace = getManager().createFromName("occ");
		occSpace.setSpin(spin);

		IndexSpace virtSpace = getManager().createFromName("virt");
		virtSpace.setSpin(spin);

		ASSERT_EQ(fmt::format("{}", IndexSpaceFormatter(occSpace, getManager())), "occ");
		ASSERT_EQ(fmt::format("{}", IndexSpaceFormatter(virtSpace, getManager())), "virt");
	}
}

TEST_F(FormatTest, Index) {
	// Index type and the space's spin doesn't matter for the formatting
	for (IndexType type : { IndexType::Annihilator, IndexType::Creator, IndexType::Annihilator }) {
		for (Spin spin : { Spin::Alpha, Spin::Beta, Spin::Both, Spin::None }) {
			IndexSpace occSpace  = getManager().createFromName("occ");
			IndexSpace virtSpace = getManager().createFromName("virt");

			occSpace.setSpin(spin);
			virtSpace.setSpin(spin);

			const Index occIndex1(1, occSpace, type);
			const Index occIndex2(5, occSpace, type);
			const Index virtIndex1(3, virtSpace, type);
			const Index virtIndex2(11, virtSpace, type);

			ASSERT_EQ(fmt::format("{}", IndexFormatter(occIndex1, getManager())), "j");
			ASSERT_EQ(fmt::format("{}", IndexFormatter(occIndex2, getManager())), "j'");
			ASSERT_EQ(fmt::format("{}", IndexFormatter(virtIndex1, getManager())), "d");
			ASSERT_EQ(fmt::format("{}", IndexFormatter(virtIndex2, getManager())), "d''");
		}
	}
}

TEST_F(FormatTest, TensorBlock) {
	const Tensor T("T");
	const TensorBlock scalarBlock(T);

	ASSERT_EQ(fmt::format("{}", TensorBlockFormatter(scalarBlock, getManager())), "T{}");

	const IndexSpace occ  = getManager().createFromName("occ");
	const IndexSpace virt = getManager().createFromName("virt");

	const TensorBlock block = std::get< 0 >(TensorBlock::create(T, { occ, virt }, {}));

	ASSERT_EQ(fmt::format("{}", TensorBlockFormatter(block, getManager())), "T{occ virt}");
}

TEST_F(FormatTest, TensorElement) {
	const Tensor H("H");
	const TensorElement scalarElement(H);

	ASSERT_EQ(fmt::format("{}", TensorElementFormatter(scalarElement, getManager())), "H[]");

	const Index occIdx(3, getManager().createFromName("occ"), IndexType::Creator);
	const Index virtIdx(6, getManager().createFromName("virt"), IndexType::Annihilator);

	const TensorElement element = std::get< 0 >(TensorElement::create(H, { occIdx, virtIdx }, {}));

	ASSERT_EQ(fmt::format("{}", TensorElementFormatter(element, getManager())), "H[l c']");
}
