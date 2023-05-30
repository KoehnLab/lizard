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
#include "lizard/symbolic/TreeNode.hpp"

#include <fmt/core.h>

#include <gtest/gtest.h>

#include <vector>

using namespace lizard;



////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// TEST FIXTURES //////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

struct FormatTest : ::testing::Test {
	FormatTest() : m_tree1(Tensor("R")), m_tree2(Tensor("R")) {
		const IndexSpace occSpace(0, Spin::Both);
		const IndexSpace virtSpace(1, Spin::Both);
		const IndexSpace extSpace(2, Spin::Both);

		const IndexSpaceData occData("occ", 'o', 10, Spin::Both, { 'i', 'j', 'k', 'l' }, '\'');
		const IndexSpaceData virtData("virt", 'v', 100, Spin::Both, { 'a', 'b', 'c', 'd' }, '\'');
		const IndexSpaceData extData("ext", 'e', 150, Spin::None, { 'p', 'q', 'r', 's' }, '\'');

		m_manager.registerSpace(occSpace, occData);
		m_manager.registerSpace(virtSpace, virtData);
		m_manager.registerSpace(extSpace, extData);

		m_spaces  = { occSpace, virtSpace, extSpace };
		m_indices = {
			Index(0, occSpace, IndexType::Creator),
			Index(1, virtSpace, IndexType::Annihilator),
			Index(2, extSpace, IndexType::External),
		};
		m_tensors = { Tensor{ "A" }, Tensor{ "B" }, Tensor{ "C" } };
		m_blocks  = {
            TensorBlock(m_tensors[0]),
            std::get< 0 >(TensorBlock::create(m_tensors[1], { m_spaces[0], m_spaces[1] })),
            std::get< 0 >(TensorBlock::create(m_tensors[2], { m_spaces[2], m_spaces[0], m_spaces[1] })),
		};
		m_elements = {
			TensorElement(m_tensors[0]),
			std::get< 0 >(TensorElement::create(m_tensors[1], { m_indices[1], m_indices[2] }, {})),
			std::get< 0 >(TensorElement::create(m_tensors[2], { m_indices[0], m_indices[2], m_indices[1] }, {})),
		};

		// Construct sample tree for something like
		// R = 1/2 + A * ( -2/3 * C + B * C )
		m_tree1.add(TreeNode(1, 2));
		m_tree1.add(m_elements[0]);
		m_tree1.add(TreeNode(-2, 3));
		m_tree1.add(m_elements[2]);
		m_tree1.add(TreeNode(ExpressionOperator::Times));
		m_tree1.add(m_elements[1]);
		m_tree1.add(m_elements[2]);
		m_tree1.add(TreeNode(ExpressionOperator::Times));
		m_tree1.add(TreeNode(ExpressionOperator::Plus));
		m_tree1.add(TreeNode(ExpressionOperator::Times));
		m_tree1.add(TreeNode(ExpressionOperator::Plus));
		assert(m_tree1.isValid()); // NOLINT

		// Construct another sample tree for something like
		// R = ( A * ( B * C ) ) * (A + B) + ( A + B * -1 ) * C
		m_tree2.add(m_elements[0]);
		m_tree2.add(m_elements[1]);
		m_tree2.add(m_elements[2]);
		m_tree2.add(TreeNode(ExpressionOperator::Times));
		m_tree2.add(TreeNode(ExpressionOperator::Times));
		m_tree2.add(m_elements[0]);
		m_tree2.add(m_elements[1]);
		m_tree2.add(TreeNode(ExpressionOperator::Plus));
		m_tree2.add(TreeNode(ExpressionOperator::Times));
		m_tree2.add(m_elements[0]);
		m_tree2.add(m_elements[1]);
		m_tree2.add(TreeNode(-1));
		m_tree2.add(TreeNode(ExpressionOperator::Times));
		m_tree2.add(TreeNode(ExpressionOperator::Plus));
		m_tree2.add(m_elements[2]);
		m_tree2.add(TreeNode(ExpressionOperator::Times));
		m_tree2.add(TreeNode(ExpressionOperator::Plus));
		assert(m_tree2.isValid()); // NOLINT
	}

	[[nodiscard]] auto getManager() const -> const IndexSpaceManager & { return m_manager; }
	[[nodiscard]] auto getSpaces() const -> const std::vector< IndexSpace > & { return m_spaces; }
	[[nodiscard]] auto getIndices() const -> const std::vector< Index > & { return m_indices; }
	[[nodiscard]] auto getTensors() const -> const std::vector< Tensor > & { return m_tensors; }
	[[nodiscard]] auto getBlocks() const -> const std::vector< TensorBlock > & { return m_blocks; }
	[[nodiscard]] auto getElements() const -> const std::vector< TensorElement > & { return m_elements; }
	[[nodiscard]] auto getTree1() const -> const NamedTensorExprTree & { return m_tree1; }
	[[nodiscard]] auto getTree2() const -> const NamedTensorExprTree & { return m_tree2; }

private:
	IndexSpaceManager m_manager;
	std::vector< IndexSpace > m_spaces;
	std::vector< Index > m_indices;
	std::vector< Tensor > m_tensors;
	std::vector< TensorBlock > m_blocks;
	std::vector< TensorElement > m_elements;
	NamedTensorExprTree m_tree1;
	NamedTensorExprTree m_tree2;
};


////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// TEST CASES ////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

TEST_F(FormatTest, Tensor) {
	for (const Tensor &current : getTensors()) {
		ASSERT_EQ(fmt::format("{}", current), current.getName());
	}
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
	// TODO: Indicate spin
	const Tensor T("T");
	const TensorBlock scalarBlock(T);

	ASSERT_EQ(fmt::format("{}", TensorBlockFormatter(scalarBlock, getManager())), "T{}");

	const IndexSpace occ  = getManager().createFromName("occ");
	const IndexSpace virt = getManager().createFromName("virt");

	const TensorBlock block = std::get< 0 >(TensorBlock::create(T, { occ, virt }, {}));

	ASSERT_EQ(fmt::format("{}", TensorBlockFormatter(block, getManager())), "T{occ virt}");
}

TEST_F(FormatTest, TensorElement) {
	// TODO: Indicate spin
	const Tensor H("H");
	const TensorElement scalarElement(H);

	ASSERT_EQ(fmt::format("{}", TensorElementFormatter(scalarElement, getManager())), "H[]");

	IndexSpace alphaOccSpace = getManager().createFromName("occ");
	alphaOccSpace.setSpin(Spin::Alpha);
	IndexSpace alphaVirtSpace = getManager().createFromName("virt");
	alphaVirtSpace.setSpin(Spin::Alpha);


	const Index occIdx1(3, getManager().createFromName("occ"), IndexType::Creator);
	const Index occIdx2(2, alphaOccSpace, IndexType::Creator);
	const Index virtIdx1(6, getManager().createFromName("virt"), IndexType::Annihilator);
	const Index virtIdx2(7, alphaVirtSpace, IndexType::Annihilator);

	TensorElement element = std::get< 0 >(TensorElement::create(H, { occIdx1, virtIdx1 }, {}));

	ASSERT_EQ(fmt::format("{}", TensorElementFormatter(element, getManager())), "H[l c']");

	element = std::get< 0 >(TensorElement::create(H, { occIdx1, occIdx2, virtIdx1, virtIdx2 }, {}));

	ASSERT_EQ(fmt::format("{}", TensorElementFormatter(element, getManager())), "H[l k c' d'](././)");
}

TEST_F(FormatTest, TensorExpr) {
	ASSERT_EQ(fmt::format("{}", TensorExprFormatter(getTree1().getRoot(), getManager())),
			  "1/2 + A[] * ( -2/3 * C[i r b] + B[b r] * C[i r b] )");

	ASSERT_EQ(fmt::format("{}", TensorExprFormatter(getTree2().getRoot(), getManager())),
			  "( A[] * ( B[b r] * C[i r b] ) ) * ( A[] + B[b r] ) + ( A[] + B[b r] * -1 ) * C[i r b]");
}

TEST_F(FormatTest, TensorExprTree) {
	ASSERT_EQ(fmt::format("{}", TensorExprTreeFormatter(getTree1(), getManager())),
			  "1/2 + A[] * ( -2/3 * C[i r b] + B[b r] * C[i r b] )");
	ASSERT_EQ(fmt::format("{}", TensorExprTreeFormatter(getTree2(), getManager())),
			  "( A[] * ( B[b r] * C[i r b] ) ) * ( A[] + B[b r] ) + ( A[] + B[b r] * -1 ) * C[i r b]");
}

TEST_F(FormatTest, NamedTensorExprTree) {
	ASSERT_EQ(fmt::format("{}", NamedTensorExprTreeFormatter(getTree1(), getManager())),
			  "R[] = 1/2 + A[] * ( -2/3 * C[i r b] + B[b r] * C[i r b] )");
	ASSERT_EQ(fmt::format("{}", NamedTensorExprTreeFormatter(getTree2(), getManager())),
			  "R[] = ( A[] * ( B[b r] * C[i r b] ) ) * ( A[] + B[b r] ) + ( A[] + B[b r] * -1 ) * C[i r b]");
}
