// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#include "lizard/core/ExpressionTree.hpp"
#include "lizard/core/Expression.hpp"
#include "lizard/core/ExpressionException.hpp"
#include "lizard/core/ExpressionOperator.hpp"
#include "lizard/core/Node.hpp"
#include "lizard/core/TreeTraversal.hpp"
#include "lizard/core/details/ExpressionTreeIteratorCore.hpp"
#include "lizard/core/type_traits.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <array>
#include <cassert>
#include <map>
#include <sstream>
#include <stack>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

#include <hedley.h>

using namespace ::lizard;


////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////// UTILITY CLASSES /////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

struct Variable {
	std::string name;
};

auto operator==(const Variable &lhs, const Variable &rhs) -> bool {
	return lhs.name == rhs.name;
}

auto operator!=(const Variable &lhs, const Variable &rhs) -> bool {
	return !(lhs == rhs);
}

////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////// UTILITY FUNCTIONS ////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

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
					resultStack.push(expression.getLiteral().getNumerator());
				}
				break;
		}
	}

	return resultStack.top();
}

template< typename Iterator > auto iterate(Iterator begin, Iterator end) -> std::vector< ConstExpression< Variable > > {
	std::vector< ConstExpression< Variable > > visitedExpressions;

	for (auto iter = begin; iter != end; ++iter) {
		visitedExpressions.push_back(*iter);
	}

	return visitedExpressions;
}


template< typename TreeType >
auto iterate(TreeType &tree, TreeTraversal traversalOrder) -> std::vector< ConstExpression< Variable > > {
	switch (traversalOrder) {
		case TreeTraversal::DepthFirst_InOrder:
			return iterate(tree.template begin< TreeTraversal::DepthFirst_InOrder >(),
						   tree.template end< TreeTraversal::DepthFirst_InOrder >());
		case TreeTraversal::DepthFirst_PreOrder:
			return iterate(tree.template begin< TreeTraversal::DepthFirst_PreOrder >(),
						   tree.template end< TreeTraversal::DepthFirst_PreOrder >());
		case TreeTraversal::DepthFirst_PostOrder:
			return iterate(tree.template begin< TreeTraversal::DepthFirst_PostOrder >(),
						   tree.template end< TreeTraversal::DepthFirst_PostOrder >());
	}

	HEDLEY_UNREACHABLE();
}

auto getNodeName(const ConstExpression< Variable > &expression) -> std::string {
	switch (expression.getType()) {
		case ExpressionType::Variable:
			return expression.getVariable().name;
		case ExpressionType::Literal:
			return static_cast< std::string >(expression.getLiteral());
		case ExpressionType::Operator:
			switch (expression.getOperator()) {
				case ExpressionOperator::Plus:
					return "+";
				case ExpressionOperator::Times:
					return "*";
			}
	}

	HEDLEY_UNREACHABLE();
}

template< typename TreeType >
auto iteratedNodeNames(TreeType &tree, TreeTraversal traversalOrder) -> std::vector< std::string > {
	std::vector< std::string > nodeNames;

	for (ConstExpression< Variable > &current : iterate(tree, traversalOrder)) {
		nodeNames.push_back(getNodeName(current));
	}

	return nodeNames;
}

template< typename Iterator > auto iteratedNodeNames(Iterator begin, Iterator end) -> std::vector< std::string > {
	std::vector< std::string > nodeNames;

	for (ConstExpression< Variable > &current : iterate(begin, end)) {
		nodeNames.push_back(getNodeName(current));
	}

	return nodeNames;
}


////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// TEST FIXTURES //////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

class IterationTest : public ::testing::TestWithParam< std::tuple< TreeTraversal, std::size_t > > {
public:
	using ParamPack = std::tuple< TreeTraversal, std::size_t >;

protected:
	/*
	 *    *
	 *   / \
	 *  2   x
	 */
	ExpressionTree< Variable > m_smallTree = treeFromPostfix("2 x *");
	std::map< TreeTraversal, std::vector< std::string > > m_smallTreeNodeIterationOrder{
		{ TreeTraversal::DepthFirst_PostOrder, { "2", "x", "*" } },
		{ TreeTraversal::DepthFirst_PreOrder, { "*", "2", "x" } },
		{ TreeTraversal::DepthFirst_InOrder, { "2", "*", "x" } },
	};

	/*
	 *      +
	 *     / \
	 *    *   3
	 *   / \
	 *  2   x
	 */
	ExpressionTree< Variable > m_mediumTree = treeFromPostfix("2 x * 3 +");
	std::map< TreeTraversal, std::vector< std::string > > m_mediumTreeNodeIterationOrder{
		{ TreeTraversal::DepthFirst_PostOrder, { "2", "x", "*", "3", "+" } },
		{ TreeTraversal::DepthFirst_PreOrder, { "+", "*", "2", "x", "3" } },
		{ TreeTraversal::DepthFirst_InOrder, { "2", "*", "x", "+", "3" } },
	};

	std::array< ExpressionTree< Variable > *, 2 > m_trees                        = { &m_smallTree, &m_mediumTree };
	std::array< decltype(m_smallTreeNodeIterationOrder) *, 2 > m_iterationOrders = { &m_smallTreeNodeIterationOrder,
																					 &m_mediumTreeNodeIterationOrder };
};

class EvaluationTest : public ::testing::TestWithParam< std::tuple< std::string, int > > {
public:
	using ParamPack = std::tuple< std::string, int >;

protected:
	const std::unordered_map< std::string, int > m_variables = {
		{ "a", -3 },
		{ "b", 12 },
	};
};

class SubstitutionTest : public ::testing::TestWithParam< std::tuple< Node, std::string, int > > {
public:
	using ParamPack = std::tuple< Node, std::string, int >;

protected:
	// (1 * 2) + (a + b)
	ExpressionTree< Variable > m_tree = treeFromPostfix("1 2 * a b + +");

	const std::unordered_map< std::string, int > m_variableDefinitions = {
		{ "a", -2 },
		{ "b", 4 },
		{ "c", 5 },
	};

	const std::array< Variable, 3 > m_variables = { Variable{ "a" }, Variable{ "b" }, Variable{ "c" } };
};



////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// TEST CASES ////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////


TEST(ExpressionTree, construction) {
	/*
	 *    *
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


template< TreeTraversal traversalOrder, bool isConst > void test_iteration_anchors() {
	/*
	 *      +
	 *     / \
	 *    *   3
	 *   / \
	 *  2   x
	 */
	std::conditional_t< isConst, const ExpressionTree< Variable >, ExpressionTree< Variable > > tree =
		treeFromPostfix("2 x * 3 +");

	using Expression = std::conditional_t< isConst, ConstExpression< Variable >, Expression< Variable > >;

	Expression plusExpr  = tree.getRoot();
	Expression timesExpr = tree.getRoot().getLeftArg();
	Expression threeExpr = tree.getRoot().getRightArg();
	Expression twoExpr   = timesExpr.getLeftArg();
	Expression varExpr   = timesExpr.getRightArg();

	ASSERT_EQ(plusExpr.getOperator(), ExpressionOperator::Plus);
	ASSERT_EQ(timesExpr.getOperator(), ExpressionOperator::Times);
	ASSERT_EQ(threeExpr.getLiteral(), Fraction(3));
	ASSERT_EQ(twoExpr.getLiteral(), Fraction(2));
	ASSERT_EQ(varExpr.getVariable(), Variable{ "x" });

	using Core = details::ExpressionTreeIteratorCore< Variable, isConst, traversalOrder >;

	// From root node
	ASSERT_EQ(Core::at(tree, plusExpr).dereference(), plusExpr);
	ASSERT_EQ(Core::after(tree, plusExpr), [&]() {
		switch (traversalOrder) {
			case TreeTraversal::DepthFirst_InOrder:
				return Core::at(tree, threeExpr);
			case TreeTraversal::DepthFirst_PostOrder:
				return Core::end(tree);
			case TreeTraversal::DepthFirst_PreOrder:
				return Core::at(tree, timesExpr);
		}
		HEDLEY_UNREACHABLE();
	}());
	ASSERT_EQ(Core::fromRoot(tree, plusExpr).dereference(), [&]() {
		switch (traversalOrder) {
			case TreeTraversal::DepthFirst_InOrder:
			case TreeTraversal::DepthFirst_PostOrder:
				return twoExpr;
			case TreeTraversal::DepthFirst_PreOrder:
				return plusExpr;
		}
		HEDLEY_UNREACHABLE();
	}());
	ASSERT_EQ(Core::afterRoot(tree, plusExpr), Core::end(tree));

	// From binary, non-root Node
	ASSERT_EQ(Core::at(tree, timesExpr).dereference(), timesExpr);
	ASSERT_EQ(Core::after(tree, timesExpr).dereference(), [&]() {
		switch (traversalOrder) {
			case TreeTraversal::DepthFirst_InOrder:
				return varExpr;
			case TreeTraversal::DepthFirst_PostOrder:
				return threeExpr;
			case TreeTraversal::DepthFirst_PreOrder:
				return twoExpr;
		}
		HEDLEY_UNREACHABLE();
	}());
	ASSERT_EQ(Core::fromRoot(tree, timesExpr).dereference(), [&]() {
		switch (traversalOrder) {
			case TreeTraversal::DepthFirst_InOrder:
			case TreeTraversal::DepthFirst_PostOrder:
				return twoExpr;
			case TreeTraversal::DepthFirst_PreOrder:
				return timesExpr;
		}
		HEDLEY_UNREACHABLE();
	}());
	ASSERT_EQ(Core::afterRoot(tree, timesExpr).dereference(), [&]() {
		switch (traversalOrder) {
			case TreeTraversal::DepthFirst_InOrder:
				return plusExpr;
			case TreeTraversal::DepthFirst_PostOrder:
			case TreeTraversal::DepthFirst_PreOrder:
				return threeExpr;
		}
		HEDLEY_UNREACHABLE();
	}());

	// From non-root, leaf Node
	ASSERT_EQ(Core::at(tree, twoExpr).dereference(), twoExpr);
	ASSERT_EQ(Core::after(tree, twoExpr).dereference(), [&]() {
		switch (traversalOrder) {
			case TreeTraversal::DepthFirst_InOrder:
				return timesExpr;
			case TreeTraversal::DepthFirst_PostOrder:
			case TreeTraversal::DepthFirst_PreOrder:
				return varExpr;
		}
		HEDLEY_UNREACHABLE();
	}());
	ASSERT_EQ(Core::fromRoot(tree, twoExpr).dereference(), twoExpr);
	ASSERT_EQ(Core::afterRoot(tree, twoExpr).dereference(), [&]() {
		switch (traversalOrder) {
			case TreeTraversal::DepthFirst_InOrder:
				return timesExpr;
			case TreeTraversal::DepthFirst_PostOrder:
			case TreeTraversal::DepthFirst_PreOrder:
				return varExpr;
		}
		HEDLEY_UNREACHABLE();
	}());

	// From root, leaf Node
	decltype(tree) leafTree = treeFromPostfix("x");
	ASSERT_EQ(Core::at(leafTree, leafTree.getRoot()).dereference(), leafTree.getRoot());
	ASSERT_EQ(Core::after(leafTree, leafTree.getRoot()), Core::end(leafTree));
	ASSERT_EQ(Core::fromRoot(leafTree, leafTree.getRoot()).dereference(), leafTree.getRoot());
	ASSERT_EQ(Core::afterRoot(leafTree, leafTree.getRoot()), Core::end(leafTree));
}

TEST_P(IterationTest, iteration_anchors) {
	const TreeTraversal traversalOrder = std::get< 0 >(GetParam());
	const bool accessTreeAsConst       = static_cast< bool >(std::get< 1 >(GetParam()));

	// Map from runtime arguments to compile-time arguments
	switch (traversalOrder) {
		case TreeTraversal::DepthFirst_InOrder:
			if (accessTreeAsConst) {
				test_iteration_anchors< TreeTraversal::DepthFirst_InOrder, true >();
			} else {
				test_iteration_anchors< TreeTraversal::DepthFirst_InOrder, false >();
			}
			break;
		case TreeTraversal::DepthFirst_PreOrder:
			if (accessTreeAsConst) {
				test_iteration_anchors< TreeTraversal::DepthFirst_PreOrder, true >();
			} else {
				test_iteration_anchors< TreeTraversal::DepthFirst_PreOrder, false >();
			}
			break;
		case TreeTraversal::DepthFirst_PostOrder:
			if (accessTreeAsConst) {
				test_iteration_anchors< TreeTraversal::DepthFirst_PostOrder, true >();
			} else {
				test_iteration_anchors< TreeTraversal::DepthFirst_PostOrder, false >();
			}
			break;
	}
}

TEST_P(IterationTest, full_iteration) {
	const TreeTraversal traversalOrder    = std::get< 0 >(GetParam());
	const ExpressionTree< Variable > tree = *m_trees.at(std::get< 1 >(GetParam()));
	const std::vector< std::string > expectedIterationOrder =
		(*m_iterationOrders.at(std::get< 1 >(GetParam())))[traversalOrder];

	const std::vector< std::string > visitedNodes = iteratedNodeNames(tree, traversalOrder);
	const std::vector< std::string > constVisitedNodes =
		iteratedNodeNames(static_cast< const decltype(tree) >(tree), traversalOrder);

	EXPECT_EQ(visitedNodes, expectedIterationOrder);
	EXPECT_EQ(constVisitedNodes, expectedIterationOrder);
}

template< TreeTraversal traversalOrder, bool isConst > void test_partial_iteration() {
	/*
	 *      +
	 *     / \
	 *    *   3
	 *   / \
	 *  2   x
	 */
	std::conditional_t< isConst, const ExpressionTree< Variable >, ExpressionTree< Variable > > tree =
		treeFromPostfix("2 x * 3 +");

	using Core     = details::ExpressionTreeIteratorCore< Variable, isConst, traversalOrder >;
	using Iterator = ExpressionTree< Variable >::iterator_template< isConst, traversalOrder >;

	std::vector< std::string > expectedNodeVisits;
	std::vector< std::string > visitedNodes;

	{
		// Start iterating as normal, but stop just before the root node is visited
		auto begin = tree.template begin< traversalOrder >();
		auto end   = Iterator(Core::at(tree, tree.getRoot()));

		visitedNodes       = iteratedNodeNames(begin, end);
		expectedNodeVisits = []() -> std::vector< std::string > {
			switch (traversalOrder) {
				case TreeTraversal::DepthFirst_InOrder:
					return { "2", "*", "x" };
				case TreeTraversal::DepthFirst_PostOrder:
					return { "2", "x", "*", "3" };
				case TreeTraversal::DepthFirst_PreOrder:
					return {};
			}
			HEDLEY_UNREACHABLE();
		}();
		EXPECT_EQ(visitedNodes, expectedNodeVisits);
	}
	{
		// Start iterating as normal, but stop just after the root node has been visited
		auto begin = tree.template begin< traversalOrder >();
		auto end   = Iterator(Core::after(tree, tree.getRoot()));

		visitedNodes       = iteratedNodeNames(begin, end);
		expectedNodeVisits = []() -> std::vector< std::string > {
			switch (traversalOrder) {
				case TreeTraversal::DepthFirst_InOrder:
					return { "2", "*", "x", "+" };
				case TreeTraversal::DepthFirst_PostOrder:
					return { "2", "x", "*", "3", "+" };
				case TreeTraversal::DepthFirst_PreOrder:
					return { "+" };
			}
			HEDLEY_UNREACHABLE();
		}();
		EXPECT_EQ(visitedNodes, expectedNodeVisits);
	}
	{
		// Iterate only over the sub-tree representing the multiplication
		auto begin = Iterator(Core::fromRoot(tree, tree.getRoot().getLeftArg()));
		auto end   = Iterator(Core::afterRoot(tree, tree.getRoot().getLeftArg()));

		visitedNodes       = iteratedNodeNames(begin, end);
		expectedNodeVisits = [&]() -> std::vector< std::string > {
			switch (traversalOrder) {
				case TreeTraversal::DepthFirst_InOrder:
					EXPECT_EQ(begin->getType(), ExpressionType::Literal);
					return { "2", "*", "x" };
				case TreeTraversal::DepthFirst_PostOrder:
					EXPECT_EQ(begin->getType(), ExpressionType::Literal);
					return { "2", "x", "*" };
				case TreeTraversal::DepthFirst_PreOrder:
					EXPECT_EQ(begin->getType(), ExpressionType::Operator);
					return { "*", "2", "x" };
			}
			HEDLEY_UNREACHABLE();
		}();

		EXPECT_EQ(visitedNodes, expectedNodeVisits);
	}
}

TEST_P(IterationTest, partial_iteration) {
	const TreeTraversal traversalOrder = std::get< 0 >(GetParam());
	const bool accessTreeAsConst       = static_cast< bool >(std::get< 1 >(GetParam()));

	// Map from runtime arguments to compile-time arguments
	switch (traversalOrder) {
		case TreeTraversal::DepthFirst_InOrder:
			if (accessTreeAsConst) {
				test_partial_iteration< TreeTraversal::DepthFirst_InOrder, true >();
			} else {
				test_partial_iteration< TreeTraversal::DepthFirst_InOrder, false >();
			}
			break;
		case TreeTraversal::DepthFirst_PreOrder:
			if (accessTreeAsConst) {
				test_partial_iteration< TreeTraversal::DepthFirst_PreOrder, true >();
			} else {
				test_partial_iteration< TreeTraversal::DepthFirst_PreOrder, false >();
			}
			break;
		case TreeTraversal::DepthFirst_PostOrder:
			if (accessTreeAsConst) {
				test_partial_iteration< TreeTraversal::DepthFirst_PostOrder, true >();
			} else {
				test_partial_iteration< TreeTraversal::DepthFirst_PostOrder, false >();
			}
			break;
	}
}

TEST(ExpressionTree, iterator_convertability) {
	ExpressionTree< Variable > tree = treeFromPostfix("a b +");

	// This is a compile-time check that produces a compiler error, if it fails
	auto mutIter   = tree.begin();
	auto constIter = tree.cbegin();
	constIter      = mutIter; // mutable to const iterator must be possible

	static_assert(!std::is_same_v< decltype(mutIter), decltype(constIter) >,
				  "Mutable and const iterators should be different types");

	(void) constIter;
}

TEST_P(EvaluationTest, evaluate) {
	const std::string treePostfix = std::get< 0 >(GetParam());
	const int expectedResult      = std::get< 1 >(GetParam());

	ExpressionTree< Variable > tree = treeFromPostfix(treePostfix);
	ASSERT_EQ(evaluate(tree, m_variables), expectedResult);
}

TEST(ExpressionTree, size) {
	/**
	 *   *
	 *  / \
	 * 1   +
	 *    / \
	 *   2   3
	 */
	ExpressionTree< Variable > tree = treeFromPostfix("1 2 3 + *");

	EXPECT_EQ(tree.size(), static_cast< std::size_t >(5));
	EXPECT_EQ(tree.getRoot().size(), static_cast< std::size_t >(5));
	EXPECT_EQ(tree.getRoot().getLeftArg().size(), static_cast< std::size_t >(1));
	EXPECT_EQ(tree.getRoot().getRightArg().size(), static_cast< std::size_t >(3));
}

TEST_P(SubstitutionTest, substitutions) {
	ASSERT_EQ(evaluate(m_tree, m_variableDefinitions), 4);

	const Node &replaceTarget                    = std::get< 0 >(GetParam());
	const ExpressionTree< Variable > replacement = treeFromPostfix(std::get< 1 >(GetParam()));
	const int expectedResult                     = std::get< 2 >(GetParam());

	ASSERT_TRUE(replacement.isValid());

	auto iter = std::find_if(m_tree.begin(), m_tree.end(), [&](const ConstExpression< Variable > &expr) {
		// Compare only the Node itself (not its parent and not its children)
		if (expr.getCardinality() != replaceTarget.getCardinality() || expr.getType() != replaceTarget.getType()) {
			return false;
		}
		switch (replaceTarget.getType()) {
			case ExpressionType::Literal:
				return expr.getLiteral().getNumerator()
						   == signed_cast< Fraction::field_type >(replaceTarget.getLeftChild())
					   && expr.getLiteral().getDenominator()
							  == signed_cast< Fraction::field_type >(replaceTarget.getRightChild());
			case ExpressionType::Operator:
				return expr.getOperator() == replaceTarget.getOperator();
			case ExpressionType::Variable:
				return expr.getVariable() == m_variables.at(replaceTarget.getLeftChild());
		}

		HEDLEY_UNREACHABLE();
	});

	ASSERT_NE(iter, m_tree.end());

	if (replacement.getRoot().getType() == ExpressionType::Variable) {
		// Substitute with a single Variable
		iter->substituteWith(replacement.getRoot().getVariable());
	} else {
		// Substitute with entire expression
		iter->substituteWith(replacement.getRoot());
	}

	EXPECT_EQ(evaluate(m_tree, m_variableDefinitions), expectedResult);
}


////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// TEST SUITE INSTANTIATIONS ////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

INSTANTIATE_TEST_SUITE_P(ExpressionTree, IterationTest,
						 ::testing::Combine(::testing::Values(TreeTraversal::DepthFirst_InOrder,
															  TreeTraversal::DepthFirst_PostOrder,
															  TreeTraversal::DepthFirst_PreOrder),
											::testing::Values(0, 1)));

INSTANTIATE_TEST_SUITE_P(ExpressionTree, EvaluationTest,
						 ::testing::Values(
							 // a + (b + -2)
							 EvaluationTest::ParamPack{ "a b -2 + +", 7 },
							 // 2 + -1 * b
							 EvaluationTest::ParamPack{ "2 -1 b * +", -10 },
							 // (2 + a) * (4 * (2 + -1 * b))
							 EvaluationTest::ParamPack{ "2 a + 4 2 -1 b * + * *", 40 },
							 // 2 + (4 * (a * (2 + -3) + b) + -1 * b) + (4 * 3 * 2 * 1)
							 EvaluationTest::ParamPack{ "2 4 a 2 -3 + * b + * -1 b * + + 4 3 * 2 * 1 * +", 74 }));

INSTANTIATE_TEST_SUITE_P(ExpressionTree, SubstitutionTest,
						 ::testing::Values(
							 // Replace literal "1" with "2 * 4"
							 SubstitutionTest::ParamPack{ Node(1), "2 4 *", 18 },
							 // Replace variable "a" with "1 + 1"
							 SubstitutionTest::ParamPack{ Node(ExpressionType::Variable, Numeric{ 0 }), "1 1 +", 8 },
							 // Replace multiplication with "(4 + 2) * -1"
							 SubstitutionTest::ParamPack{ Node(ExpressionOperator::Times), "4 2 + -1 *", -4 },
							 // Replace multiplication with literal "7"
							 SubstitutionTest::ParamPack{ Node(ExpressionOperator::Times), "7", 9 },

							 // Replace literal "1" with variable "c"
							 SubstitutionTest::ParamPack{ Node(1), "c", 12 },
							 // Replace variable "a" with variable "c"
							 SubstitutionTest::ParamPack{ Node(ExpressionType::Variable, Numeric{ 0 }), "c", 11 },
							 // Replace multiplication with variable "c"
							 SubstitutionTest::ParamPack{ Node(ExpressionOperator::Times), "c", 7 }));
