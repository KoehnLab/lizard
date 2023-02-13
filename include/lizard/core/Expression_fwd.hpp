// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/Krzmbrzl/lizard/blob/main/LICENSE>.

#pragma once

#include "lizard/core/ExpressionCardinality.hpp"
#include "lizard/core/ExpressionOperator.hpp"
#include "lizard/core/ExpressionType.hpp"
#include "lizard/core/Fraction.hpp"
#include "lizard/core/Numeric.hpp"
#include "lizard/core/TreeTraversal.hpp"
#include "lizard/core/type_traits.hpp"

#include <cstdint>
#include <iosfwd>
#include <type_traits>
#include <vector>

namespace lizard {

template< typename > class ExpressionTree;
class Node;

namespace details {
	template< typename, bool, TreeTraversal > class ExpressionTreeIteratorCore;
}

/**
 * Class providing a rich API for accessing individual (sub-)expressions in an expression tree.
 * It should essentially be thought of as a wrapper class around a Node object that makes its
 * data accessible in a more straight forward manner.
 *
 * This version of the Expression class is the base class for expressions and incorporates all
 * operations that can be performed on a const Tree (and a const Node therein).
 *
 * @see Node
 */
template< typename Variable > class ConstExpression {
public:
	ConstExpression(Numeric nodeID, const Node &node, const ExpressionTree< Variable > &tree);
	ConstExpression(const ConstExpression &)     = default;
	ConstExpression(ConstExpression &&) noexcept = default;
	~ConstExpression()                           = default;
	auto operator=(const ConstExpression &) -> ConstExpression & = default;
	auto operator=(ConstExpression &&) noexcept -> ConstExpression & = default;

	/**
	 * @returns The cardinality of this expression
	 */
	[[nodiscard]] auto getCardinality() const -> ExpressionCardinality;

	/**
	 * @returns The type of this expression
	 */
	[[nodiscard]] auto getType() const -> ExpressionType;

	/**
	 * @returns The parent of the currently represented expression
	 *
	 * Note: Calling this function on a root expression (isRoot() returns true) is undefined behavior!
	 */
	[[nodiscard]] auto getParent() const -> ConstExpression;

	/**
	 * @returns The represented variable object
	 *
	 * Note: If this expression doesn't actually represent a variable, calling this function is undefined behavior
	 */
	[[nodiscard]] auto getVariable() const -> const Variable &;

	/**
	 * @returns The represented operator type
	 *
	 * Note: If this expression doesn't actually represent an operator, calling this function is undefined behavior!
	 */
	[[nodiscard]] auto getOperator() const -> ExpressionOperator;

	/**
	 * @returns The literal value this expression represents
	 *
	 * Note: If this expression doesn't actually represent a literal value, calling this function is undefined behavior!
	 */
	[[nodiscard]] auto getLiteral() const -> Fraction;

	/**
	 * @returns The Expression representing the left argument of the represented binary expression
	 *
	 * Note: If this expression is not actually binary, calling this function is undefined behavior!
	 */
	[[nodiscard]] auto getLeftArg() const -> ConstExpression;

	/**
	 * @returns The Expression representing the right argument of the represented binary expression
	 *
	 * Note: If this expression is not actually binary, calling this function is undefined behavior!
	 */
	[[nodiscard]] auto getRightArg() const -> ConstExpression;

	/**
	 * @returns The Expression representing the argument of the represented unary expression
	 *
	 * Note: If this expression is not actually unary, calling this function is undefined behavior!
	 */
	[[nodiscard]] auto getArg() const -> ConstExpression;

	/**
	 * @returns Whether this expression represents the root of the overall expression (tree)
	 */
	[[nodiscard]] auto isRoot() const -> bool;

	/**
	 * @returns The size of the expression rooted at the currently represented element
	 */
	[[nodiscard]] auto size() const -> Numeric::numeric_type;

	template< typename V >
	friend auto operator==(const ConstExpression< V > &lhs, const ConstExpression< V > &rhs) -> bool;
	template< typename V >
	friend auto operator!=(const ConstExpression< V > &lhs, const ConstExpression< V > &rhs) -> bool;
	template< typename V >
	friend auto operator<<(std::ostream &stream, const ConstExpression< V > &expr) -> std::ostream &;

protected:
	[[nodiscard]] auto nodeID() const -> const Numeric &;
	[[nodiscard]] auto node() const -> const Node &;
	[[nodiscard]] auto tree() const -> const ExpressionTree< Variable > &;

private:
	Numeric m_nodeID;
	const Node *m_node;
	const ExpressionTree< Variable > *m_tree;

	template< typename > friend class ExpressionTree;
	template< typename, bool, TreeTraversal > friend class details::ExpressionTreeIteratorCore;

	/**
	 * @returns The computed size of this expression
	 */
	[[nodiscard]] auto computeSize() const -> Numeric::numeric_type;
};


/**
 * Class providing a rich API for accessing individual (sub-)expressions in an expression tree.
 * It should essentially be thought of as a wrapper class around a Node object that makes its
 * data accessible in a more straight forward manner.
 *
 * This subclass implements all functionality that can only be performed on mutable Trees and
 * mutable Nodes therein.
 *
 *
 * @see Node
 */
template< typename Variable > class Expression : public ConstExpression< Variable > {
public:
	Expression(Numeric nodeID, Node &node, ExpressionTree< Variable > &tree);

	// Inherit constructors from base class
	using ConstExpression< Variable >::ConstExpression;

	/**
	 * @returns The represented variable object
	 *
	 * Note: If this expression doesn't actually represent a variable, calling this function is undefined behavior!
	 */
	[[nodiscard]] auto getVariable() -> Variable &;

	/**
	 * Sets the represented literal value of this expression.
	 *
	 * Note: If this expression doesn't actually represent a literal value, calling this function is undefined behavior!
	 */
	void setLiteral(const Fraction &fraction);

	/**
	 * @returns The Expression representing the left argument of the represented binary expression
	 *
	 * Note: If this expression is not actually binary, calling this function is undefined behavior!
	 */
	[[nodiscard]] auto getLeftArg() -> Expression;

	/**
	 * @returns The Expression representing the right argument of the represented binary expression
	 *
	 * Note: If this expression is not actually binary, calling this function is undefined behavior!
	 */
	[[nodiscard]] auto getRightArg() -> Expression;

	/**
	 * @returns The Expression representing the argument of the represented unary expression
	 *
	 * Note: If this expression is not actually unary, calling this function is undefined behavior!
	 */
	[[nodiscard]] auto getArg() -> Expression;

protected:
	[[nodiscard]] auto node() -> Node &;

	[[nodiscard]] auto tree() -> ExpressionTree< Variable > &;

	template< typename, bool, TreeTraversal > friend class details::ExpressionTreeIteratorCore;
};

} // namespace lizard


// Support operator== between expressions and Variable types
template< typename Variable >
auto operator==(const lizard::ConstExpression< Variable > &lhs, const Variable &rhs) -> bool;

template< typename Variable >
auto operator==(const Variable &lhs, const lizard::ConstExpression< Variable > &rhs) -> bool;


// Support operator== for containers holding ConstExpression and Expression

// clang-format off
template< typename Expr1, typename Expr2,
		  typename = std::enable_if_t<
			  !std::is_same_v< Expr1, Expr2 >
			  && lizard::is_expression_v< Expr1 >
			  && lizard::is_expression_v< Expr2 >
			  && std::is_same_v< lizard::expression_variable_t< Expr1 >, lizard::expression_variable_t< Expr2 > >
			>
		>
// clang-format on
auto operator==(const std::vector< Expr1 > &lhs, const std::vector< Expr2 > &rhs) -> bool;
