// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#include "Utils.hpp"

#include "lizard/symbolic/Index.hpp"
#include "lizard/symbolic/IndexSpace.hpp"
#include "lizard/symbolic/IndexSpaceManager.hpp"
#include "lizard/symbolic/Spin.hpp"
#include "lizard/symbolic/Tensor.hpp"
#include "lizard/symbolic/TensorBlock.hpp"
#include "lizard/symbolic/TensorElement.hpp"
#include "lizard/symbolic/TensorExpressions.hpp"

#include <gtest/gtest.h>


using namespace lizard;
using namespace lizard::test;


////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// TEST CASES ////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

TEST(Utils, createTensorElement) {
	// Scalar tensors
	TensorElement element = createTensorElement("T[]");

	ASSERT_EQ(element.getBlock().getTensor(), Tensor{ "T" });
	ASSERT_TRUE(element.getIndices().empty());
	ASSERT_EQ(element.getBlock().getSlotSymmetry().order(), 1);

	element = createTensorElement("Blabla[]");

	ASSERT_EQ(element.getBlock().getTensor(), Tensor{ "Blabla" });
	ASSERT_TRUE(element.getIndices().empty());
	ASSERT_EQ(element.getBlock().getSlotSymmetry().order(), 1);


	// Non-scalar tensors
	element = createTensorElement("H[a+, b + , i-,j- ]");
	ASSERT_EQ(element.getBlock().getTensor(), Tensor{ "H" });
	nonstd::span< const Index > indices = element.getIndices();
	ASSERT_EQ(indices.size(), 4);
	IndexSpace virtNoSpin = getIndexSpaceManager().createFromName("virt");
	virtNoSpin.setSpin(Spin::None);
	IndexSpace occNoSpin = getIndexSpaceManager().createFromName("occ");
	occNoSpin.setSpin(Spin::None);
	ASSERT_EQ(indices[0], Index(0, virtNoSpin, IndexType::Creator));
	ASSERT_EQ(indices[1], Index(1, virtNoSpin, IndexType::Creator));
	ASSERT_EQ(indices[2], Index(0, occNoSpin, IndexType::Annihilator));
	ASSERT_EQ(indices[3], Index(1, occNoSpin, IndexType::Annihilator));
	ASSERT_EQ(element.getBlock().getSlotSymmetry().order(), 4);

	element              = createTensorElement(R"(Miau[ a, b + , i-,j- ](/\|.))");
	IndexSpace virtAlpha = getIndexSpaceManager().createFromName("virt");
	virtAlpha.setSpin(Spin::Alpha);
	IndexSpace virtBeta = getIndexSpaceManager().createFromName("virt");
	virtBeta.setSpin(Spin::Beta);
	IndexSpace occBoth = getIndexSpaceManager().createFromName("occ");
	occBoth.setSpin(Spin::Both);
	ASSERT_EQ(element.getBlock().getTensor(), Tensor{ "Miau" });
	indices = element.getIndices();
	ASSERT_EQ(indices.size(), 4);
	ASSERT_EQ(indices[0], Index(0, virtAlpha, IndexType::External));
	ASSERT_EQ(indices[1], Index(1, virtBeta, IndexType::Creator));
	ASSERT_EQ(indices[2], Index(0, occBoth, IndexType::Annihilator));
	ASSERT_EQ(indices[3], Index(1, occNoSpin, IndexType::Annihilator));
	ASSERT_EQ(element.getBlock().getSlotSymmetry().order(), 1);
}

TEST(Utils, createTree) {
	TensorElement A = createTensorElement("A[a+, i-]");
	TensorElement B = createTensorElement("B[]");
	TensorElement C = createTensorElement("C[i+, j+, a-, b-](////)");

	NamedTensorExprTree expectedTree(TensorElement(Tensor{ "Result" }));
	expectedTree.add(A);
	expectedTree.add(B);
	expectedTree.add(A);
	expectedTree.add(TreeNode(ExpressionOperator::Times));
	expectedTree.add(C);
	expectedTree.add(TreeNode(ExpressionOperator::Plus));
	expectedTree.add(TreeNode(ExpressionOperator::Times));
	expectedTree.add(B);
	expectedTree.add(TreeNode(ExpressionOperator::Plus));

	std::string treeSpec       = "A[a+, i-] *(B[]*A[a+,i-] + C[ i +, j +, a-,b- ](////)) + B[]";
	TensorExprTree createdTree = createTree< TensorExprTree >(treeSpec);

	ASSERT_EQ(createdTree, static_cast< TensorExprTree & >(expectedTree));

	treeSpec                             = "Result[] = " + treeSpec;
	NamedTensorExprTree createdNamedTree = createTree< NamedTensorExprTree >(treeSpec);

	ASSERT_EQ(createdTree, expectedTree);
}
