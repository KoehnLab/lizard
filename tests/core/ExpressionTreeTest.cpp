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
#include <tuple>
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


TEST(ExpressionTree, construction) {
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

class IterationTest
	: public ::testing::TestWithParam< std::tuple< std::string, TreeTraversal, std::vector< std::string > > > {
public:
	using ParamPack = std::tuple< std::string, TreeTraversal, std::vector< std::string > >;

protected:
	template< typename TreeType >
	auto visitNodes(TreeTraversal traversalOrder, TreeType &tree) -> std::vector< std::string > {
		std::vector< ConstExpression< Variable > > visitedExpressions;

		switch (traversalOrder) {
			case TreeTraversal::DepthFirst_InOrder: {
				for (auto iter = tree.template begin< TreeTraversal::DepthFirst_InOrder >();
					 iter != tree.template end< TreeTraversal::DepthFirst_InOrder >(); ++iter) {
					visitedExpressions.push_back(*iter);
				}
			} break;
			case TreeTraversal::DepthFirst_PreOrder: {
				for (auto iter = tree.template begin< TreeTraversal::DepthFirst_PreOrder >();
					 iter != tree.template end< TreeTraversal::DepthFirst_PreOrder >(); ++iter) {
					visitedExpressions.push_back(*iter);
				}
			} break;
			case TreeTraversal::DepthFirst_PostOrder: {
				for (auto iter = tree.template begin< TreeTraversal::DepthFirst_PostOrder >();
					 iter != tree.template end< TreeTraversal::DepthFirst_PostOrder >(); ++iter) {
					visitedExpressions.push_back(*iter);
				}
			} break;
		}

		std::vector< std::string > visitedNodeContents;
		for (ConstExpression< Variable > &current : visitedExpressions) {
			switch (current.getType()) {
				case ExpressionType::Variable:
					visitedNodeContents.push_back(current.getVariable().name);
					break;
				case ExpressionType::Literal:
					// We expect to only find integer literals in our tests
					visitedNodeContents.push_back(std::to_string(static_cast< int >(current.getLiteral())));
					break;
				case ExpressionType::Operator:
					switch (current.getOperator()) {
						case ExpressionOperator::Plus:
							visitedNodeContents.emplace_back("+");
							break;
						case ExpressionOperator::Times:
							visitedNodeContents.emplace_back("*");
							break;
					}
					break;
			}
		}

		return visitedNodeContents;
	}
};

TEST_P(IterationTest, iteration) {
	ExpressionTree< Variable > tree = treeFromPostfix(std::get< 0 >(GetParam()));

	std::vector< std::string > visitedNodes = visitNodes(std::get< 1 >(GetParam()), tree);
	std::vector< std::string > constVisitedNodes =
		visitNodes(std::get< 1 >(GetParam()),
				   static_cast< std::add_lvalue_reference_t< std::add_const_t< decltype(tree) > > >(tree));

	EXPECT_EQ(visitedNodes, std::get< 2 >(GetParam()));
	EXPECT_EQ(constVisitedNodes, std::get< 2 >(GetParam()));
}

INSTANTIATE_TEST_SUITE_P(
	ExpressionTree, IterationTest,
	::testing::Values(
		/*
		 *    *
		 *   / \
		 *  2   x
		 */
		IterationTest::ParamPack{ "2 x *", TreeTraversal::DepthFirst_PostOrder, { "2", "x", "*" } },
		IterationTest::ParamPack{ "2 x *", TreeTraversal::DepthFirst_PreOrder, { "*", "2", "x" } },
		IterationTest::ParamPack{ "2 x *", TreeTraversal::DepthFirst_InOrder, { "2", "*", "x" } },
		/*
		 *      +
		 *     / \
		 *    *   3
		 *   / \
		 *  2   x
		 */
		IterationTest::ParamPack{ "2 x * 3 +", TreeTraversal::DepthFirst_PostOrder, { "2", "x", "*", "3", "+" } },
		IterationTest::ParamPack{ "2 x * 3 +", TreeTraversal::DepthFirst_PreOrder, { "+", "*", "2", "x", "3" } },
		IterationTest::ParamPack{ "2 x * 3 +", TreeTraversal::DepthFirst_InOrder, { "2", "*", "x", "+", "3" } }));


class EvaluationTest : public ::testing::TestWithParam< std::tuple< std::string, int > > {};

TEST_P(EvaluationTest, evaluate) {
	std::unordered_map< std::string, int > variables;
	variables["a"] = -3;
	variables["b"] = 12;

	const std::string treePostfix = std::get< 0 >(GetParam());
	int expectedResult            = std::get< 1 >(GetParam());

	ExpressionTree< Variable > tree = treeFromPostfix(treePostfix);
	ASSERT_EQ(evaluate(tree, variables), expectedResult);
}

INSTANTIATE_TEST_SUITE_P(ExpressionTree, EvaluationTest,
						 ::testing::Values(
							 // a + (b + -2)
							 std::tuple< std::string, int >{ "a b -2 + +", 7 },
							 // 2 + -1 * b
							 std::tuple< std::string, int >{ "2 -1 b * +", -10 },
							 // (2 + a) * (4 * (2 + -1 * b))
							 std::tuple< std::string, int >{ "2 a + 4 2 -1 b * + * *", 40 },
							 // 2 + (4 * (a * (2 + -3) + b) + -1 * b) + (4 * 3 * 2 * 1)
							 std::tuple< std::string, int >{ "2 4 a 2 -3 + * b + * -1 b * + + 4 3 * 2 * 1 * +", 74 }));


TEST(ExpressionTree, iterator_convertability) {
	ExpressionTree< Variable > tree = treeFromPostfix("a b +");

	// This is a compile-time check that produces a compiler error, if it fails
	auto mutIter   = tree.begin();
	auto constIter = tree.cbegin();
	constIter      = mutIter; // mutable to const iterator must be possible

	(void) constIter;
}
