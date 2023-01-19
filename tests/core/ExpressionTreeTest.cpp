// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/Krzmbrzl/lizard/blob/main/LICENSE>.

#include "lizard/core/ExpressionTree.hpp"
#include "lizard/core/Expression.hpp"
#include "lizard/core/ExpressionException.hpp"
#include "lizard/core/ExpressionOperator.hpp"
#include "lizard/core/Node.hpp"
#include "lizard/core/TreeTraversal.hpp"
#include "lizard/core/type_traits.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <array>
#include <cassert>
#include <sstream>
#include <stack>
#include <string>
#include <unordered_map>
#include <vector>

struct Variable {
	std::string name;
};

auto operator==(const Variable &lhs, const Variable &rhs) -> bool {
	return lhs.name == rhs.name;
}

auto operator!=(const Variable &lhs, const Variable &rhs) -> bool {
	return !(lhs == rhs);
}

using namespace ::lizard;

auto treeFromPostfix(const std::string &postfixExpr) -> ExpressionTree< Variable > {
	ExpressionTree< Variable > tree;

	std::istringstream stream(postfixExpr);
	std::string currentToken;

	while (std::getline(stream, currentToken, ' ')) {
		if (currentToken.empty()) {
			continue;
		}

		if (currentToken == "+") {
			tree.add(Node(ExpressionOperator::Plus));
			continue;
		}
		if (currentToken == "*") {
			tree.add(Node(ExpressionOperator::Times));
			continue;
		}
		if (currentToken == "-") {
			throw std::runtime_error("Operator '-' is not supported");
		}
		try {
			std::size_t processedChars = 0;
			int number                 = std::stoi(currentToken, &processedChars);
			EXPECT_TRUE(processedChars == currentToken.size());

			tree.add(Node(number));
		} catch (const std::invalid_argument &) {
			// currentToken is not a number -> it must be a variable
			tree.add(Variable{ std::move(currentToken) });
		}
	}

	return tree;
}

auto evaluate(const ExpressionTree< Variable > &tree, const std::unordered_map< std::string, int > &variables) -> int {
	EXPECT_TRUE(tree.isValid());

	std::stack< int > resultStack;

	for (auto it = tree.begin< TreeTraversal::DepthFirst_PostOrder >(); it != tree.end(); ++it) {
		const ConstExpression< Variable > &expression = *it;

		switch (expression.getCardinality()) {
			case ExpressionCardinality::Binary: {
				EXPECT_TRUE(expression.getType() == ExpressionType::Operator);
				EXPECT_TRUE(resultStack.size() >= 2);

				int left = resultStack.top();
				resultStack.pop();
				int right = resultStack.top();
				resultStack.pop();
				int result = 0;

				switch (expression.getOperator()) {
					case ExpressionOperator::Plus:
						result = left + right;
						break;
					case ExpressionOperator::Times:
						result = left * right;
						break;
				}

				resultStack.push(result);
				break;
			}
			case ExpressionCardinality::Unary:
				// We don't expect to see those for the time being
				throw "Shouldn't be reached";
				break;
			case ExpressionCardinality::Nullary:
				if (expression.getType() == ExpressionType::Variable) {
					EXPECT_TRUE(variables.find(expression.getVariable().name) != variables.end());
					resultStack.push(variables.at(expression.getVariable().name));
				} else {
					// We expect to work with integers only
					resultStack.push(expression.getNumerator());
				}
				break;
		}
	}

	return resultStack.top();
}


TEST(Core, ExpressionTree_construction) {
	/*    *
	 *   / \
	 *  2   x
	 */
	ExpressionTree< Variable > tree;

	tree.add(Node(2));
	ASSERT_TRUE(tree.isValid());
	tree.add(Variable{ "x" });
	ASSERT_FALSE(tree.isValid());
	tree.add(Node(ExpressionOperator::Times));
	ASSERT_TRUE(tree.isValid());

	// Adding a binary operator when only a single argument is currently available should throw
	ASSERT_THROW(tree.add(Node(ExpressionOperator::Plus)), ExpressionException);

	ExpressionTree< Variable > other = treeFromPostfix("2 x *");

	ASSERT_EQ(tree, other);
}

template< TreeTraversal iteration_order, typename TreeType >
void testTreeIteration(const TreeType &tree,
					   const std::vector< Expression< expression_tree_variable_t< TreeType > > > &expectedSequence) {
	std::vector< ConstExpression< expression_tree_variable_t< TreeType > > > producedSequence;

	for (auto it = tree.template begin< iteration_order >(); it != tree.template end< iteration_order >(); ++it) {
		producedSequence.push_back(*it);
	}

	ASSERT_EQ(producedSequence, expectedSequence);
}

TEST(Core, ExpressionTree_iteration) {
	/*    *
	 *   / \
	 *  2   x
	 */
	ExpressionTree< Variable > tree;
	tree.add(Node(2));
	tree.add(Variable{ "x" });
	tree.add(Node(ExpressionOperator::Times));

	Node literalNode(2);
	literalNode.setParent(Numeric(2));
	Node varNode(ExpressionType::Variable, Numeric(0));
	varNode.setParent(Numeric(2));
	Node timesNode(ExpressionOperator::Times);
	timesNode.setLeftChild(Numeric(0));
	timesNode.setRightChild(Numeric(1));

	Expression< Variable > literalExpr(Numeric(0), literalNode, tree);
	Expression< Variable > varExpr(Numeric(1), varNode, tree);
	Expression< Variable > timesExpr(Numeric(2), timesNode, tree);

	std::vector< Expression< Variable > > postOrderExpressions = { literalExpr, varExpr, timesExpr };
	std::vector< Expression< Variable > > preOrderExpressions  = { timesExpr, literalExpr, varExpr };
	std::vector< Expression< Variable > > inOrderExpressions   = { literalExpr, timesExpr, varExpr };

	testTreeIteration< TreeTraversal::DepthFirst_PostOrder >(tree, postOrderExpressions);
	testTreeIteration< TreeTraversal::DepthFirst_PostOrder >(static_cast< const decltype(tree) & >(tree),
															 postOrderExpressions);

	testTreeIteration< TreeTraversal::DepthFirst_PreOrder >(tree, preOrderExpressions);
	testTreeIteration< TreeTraversal::DepthFirst_PreOrder >(static_cast< const decltype(tree) & >(tree),
															preOrderExpressions);

	testTreeIteration< TreeTraversal::DepthFirst_InOrder >(tree, inOrderExpressions);
	testTreeIteration< TreeTraversal::DepthFirst_InOrder >(static_cast< const decltype(tree) & >(tree),
														   inOrderExpressions);

	// Test that the default iteration order corresponds to one of the explicitly requestable orders (which one exactly
	// is undefined though)
	std::vector< ConstExpression< Variable > > defaultOrderExpressions;
	for (ConstExpression< Variable > currentExpr : tree) {
		defaultOrderExpressions.push_back(currentExpr);
	}
	EXPECT_THAT((std::array{ postOrderExpressions, preOrderExpressions, inOrderExpressions }),
				testing::Contains(defaultOrderExpressions));


	std::unordered_map< std::string, int > variables;
	variables["x"] = 8;

	ASSERT_EQ(evaluate(tree, variables), 16);


	/*
	 * Extend tree to and then repeat the simple tests from above with this (slightly) more complex tree
	 *
	 *      +
	 *     / \
	 *    *   3
	 *   / \
	 *  2   x
	 */
	Node secondLiteralNode(3);
	secondLiteralNode.setParent(Numeric(4));
	Node plusNode(ExpressionOperator::Plus);
	plusNode.setLeftChild(Numeric(2));
	plusNode.setRightChild(Numeric(3));
	timesNode.setParent(Numeric(4));

	tree.add(secondLiteralNode);
	tree.add(plusNode);

	Expression< Variable > secondLiteralExpr(Numeric(3), secondLiteralNode, tree);
	Expression< Variable > plusExpr(Numeric(4), plusNode, tree);

	postOrderExpressions = { literalExpr, varExpr, timesExpr, secondLiteralExpr, plusExpr };
	preOrderExpressions  = { plusExpr, timesExpr, literalExpr, varExpr, secondLiteralExpr };
	inOrderExpressions   = { literalExpr, timesExpr, varExpr, plusExpr, secondLiteralExpr };

	testTreeIteration< TreeTraversal::DepthFirst_PostOrder >(tree, postOrderExpressions);
	testTreeIteration< TreeTraversal::DepthFirst_PostOrder >(static_cast< const decltype(tree) & >(tree),
															 postOrderExpressions);

	testTreeIteration< TreeTraversal::DepthFirst_PreOrder >(tree, preOrderExpressions);
	testTreeIteration< TreeTraversal::DepthFirst_PreOrder >(static_cast< const decltype(tree) & >(tree),
															preOrderExpressions);

	testTreeIteration< TreeTraversal::DepthFirst_InOrder >(tree, inOrderExpressions);
	testTreeIteration< TreeTraversal::DepthFirst_InOrder >(static_cast< const decltype(tree) & >(tree),
														   inOrderExpressions);

	ASSERT_EQ(evaluate(tree, variables), 19);
}

TEST(Core, ExpressionTree_evaluate) {
	std::unordered_map< std::string, int > variables;
	variables["a"] = -3;
	variables["b"] = 12;

	// a + (b + -2)
	ExpressionTree< Variable > tree = treeFromPostfix("a b -2 + +");
	ASSERT_EQ(evaluate(tree, variables), 7);

	// 2 + -1 * b
	tree = treeFromPostfix("2 -1 b * +");
	ASSERT_EQ(evaluate(tree, variables), -10);

	// (2 + a) * (4 * (2 + -1 * b))
	tree = treeFromPostfix("2 a + 4 2 -1 b * + * *");
	ASSERT_EQ(evaluate(tree, variables), 40);

	// 2 + (4 * (a * (2 + -3) + b) + -1 * b) + (4 * 3 * 2 * 1)
	tree = treeFromPostfix("2 4 a 2 -3 + * b + * -1 b * + + 4 3 * 2 * 1 * +");
	ASSERT_EQ(evaluate(tree, variables), 74);
}

TEST(Core, ExpressionTree_iterator_convertability) {
	ExpressionTree< Variable > tree = treeFromPostfix("a b +");

	// This is a compile-time check that produces a compiler error, if it fails
	auto mutIter   = tree.begin();
	auto constIter = tree.cbegin();
	constIter      = mutIter; // mutable to const iterator must be possible

	(void) constIter;
}
