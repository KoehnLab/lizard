// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/Krzmbrzl/lizard/blob/main/LICENSE>.

#ifndef LIZARD_CORE_EXPRESSION_HPP_
#define LIZARD_CORE_EXPRESSION_HPP_

#include "lizard/core/ExpressionCardinality.hpp"
#include "lizard/core/ExpressionOperator.hpp"
#include "lizard/core/ExpressionType.hpp"
#include "lizard/core/Node.hpp"
#include "lizard/core/Numeric.hpp"
#include "lizard/core/SignedCast.hpp"
#include "lizard/core/type_traits.hpp"

#include <cassert>
#include <iostream>
#include <utility>
#include <vector>

namespace lizard {

template< typename > class ExpressionTree;

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
	ConstExpression(Numeric nodeIndex, const Node &node, const ExpressionTree< Variable > &tree)
		: m_nodeIndex(std::move(nodeIndex)), m_node(&node), m_tree(&tree) {}
	ConstExpression(const ConstExpression &)     = default;
	ConstExpression(ConstExpression &&) noexcept = default;
	~ConstExpression()                           = default;
	auto operator=(const ConstExpression &) -> ConstExpression & = default;
	auto operator=(ConstExpression &&) noexcept -> ConstExpression & = default;

	/**
	 * @returns The cardinality of this expression
	 */
	[[nodiscard]] auto getCardinality() const -> ExpressionCardinality { return m_node->getCardinality(); }

	/**
	 * @returns The type of this expression
	 */
	[[nodiscard]] auto getType() const -> ExpressionType { return m_node->getType(); }

	/**
	 * @returns The represented variable object
	 *
	 * Note: If this expression doesn't actually represent a variable, calling this function is undefined behavior
	 */
	[[nodiscard]] auto getVariable() const -> const Variable & {
		assert(getCardinality() == ExpressionCardinality::Nullary);
		assert(getType() == ExpressionType::Variable);
		assert(m_node->getLeftChild() < m_tree->m_variables.size());

		return m_tree->m_variables[m_node->getLeftChild()];
	}

	/**
	 * @returns The represented operator type
	 *
	 * Note: If this expression doesn't actually represent an operator, calling this function is undefined behavior!
	 */
	[[nodiscard]] auto getOperator() const -> ExpressionOperator {
		assert(getCardinality() == ExpressionCardinality::Binary);
		assert(getType() == ExpressionType::Operator);

		return m_node->getOperator();
	}

	/**
	 * @returns The numerator of the represented literal
	 *
	 * Note: If this expression doesn't actually represent a literal value, calling this function is undefined behavior!
	 */
	[[nodiscard]] auto getNumerator() const -> std::int32_t {
		assert(getCardinality() == ExpressionCardinality::Nullary);
		assert(getType() == ExpressionType::Literal);

		// For literals the left child's ID really is to be taken as a numeric value representing the literal's
		// numerator
		return signed_cast< std::int32_t >(m_node->getLeftChild());
	}

	/**
	 * @returns The denominator of the represented literal
	 *
	 * Note: If this expression doesn't actually represent a literal value, calling this function is undefined behavior!
	 */
	[[nodiscard]] auto getDenominator() const -> std::int32_t {
		assert(getCardinality() == ExpressionCardinality::Nullary);
		assert(getType() == ExpressionType::Literal);

		// For literals the right child's ID really is to be taken as a numeric value representing the literal's
		// denominator
		return signed_cast< std::int32_t >(m_node->getRightChild());
	}

	/**
	 * @returns The numerical value of the represented literal (as a floating point value)
	 *
	 * Note: If this expression doesn't actually represent a literal value, calling this function is undefined behavior!
	 */
	[[nodiscard]] auto getLiteral() const -> double {
		assert(getCardinality() == ExpressionCardinality::Nullary);
		assert(getType() == ExpressionType::Literal);

		return static_cast< double >(getNumerator()) / getDenominator();
	}

	/**
	 * @returns The Expression representing the left argument of the represented binary expression
	 *
	 * Note: If this expression is not actually binary, calling this function is undefined behavior!
	 */
	[[nodiscard]] auto getLeftArg() const -> ConstExpression {
		assert(getCardinality() == ExpressionCardinality::Binary);
		assert(m_node->getLeftChild() < m_tree->m_nodes.size());

		return { m_node->getLeftChild(), m_tree->m_nodes[m_node->getLeftChild()], *m_tree };
	}

	/**
	 * @returns The Expression representing the right argument of the represented binary expression
	 *
	 * Note: If this expression is not actually binary, calling this function is undefined behavior!
	 */
	[[nodiscard]] auto getRightArg() const -> ConstExpression {
		assert(getCardinality() == ExpressionCardinality::Binary);
		assert(m_node->getRightChild() < m_tree->m_nodes.size());

		return { m_node->getRightChild(), m_tree->m_nodes[m_node->getRightChild()], *m_tree };
	}

	/**
	 * @returns The Expression representing the argument of the represented unary expression
	 *
	 * Note: If this expression is not actually unary, calling this function is undefined behavior!
	 */
	[[nodiscard]] auto getArg() const -> ConstExpression {
		assert(getCardinality() == ExpressionCardinality::Unary);
		assert(m_node->getLeftChild() < m_tree->m_nodes.size());

		return { m_node->getLeftChild(), m_tree->m_nodes[m_node->getLeftChild()], *m_tree };
	}

	/**
	 * @returns Whether this expression represents the root of the overall expression (tree)
	 */
	[[nodiscard]] auto isRoot() const -> bool { return !m_node->hasParent(); }

	friend auto operator==(const ConstExpression &lhs, const ConstExpression &rhs) -> bool {
		return lhs.m_nodeIndex == rhs.m_nodeIndex && *lhs.m_node == *rhs.m_node && lhs.m_tree == rhs.m_tree;
	}

	friend auto operator!=(const ConstExpression &lhs, const ConstExpression &rhs) -> bool { return !(lhs == rhs); }

	friend auto operator<<(std::ostream &stream, const ConstExpression &expr) -> std::ostream & {
		return stream << "Node " << expr.m_nodeIndex << ": " << *expr.m_node << " (" << expr.m_tree << ")";
	}

protected:
	[[nodiscard]] auto nodeIndex() const -> const Numeric & { return m_nodeIndex; }
	[[nodiscard]] auto node() const -> const Node & { return *m_node; }
	[[nodiscard]] auto tree() const -> const ExpressionTree< Variable > & { return *m_tree; }

private:
	Numeric m_nodeIndex;
	const Node *m_node;
	const ExpressionTree< Variable > *m_tree;
};


/**
 * Class providing a rich API for accessing individual (sub-)expressions in an expression tree.
 * It should essentially be thought of as a wrapper class around a Node object that makes its
 * data accessible in a more straight forward manner.
 *
 * This subclass implements all functionality that can only be performed on mutable Trees and
 * mutable Nodes therein.
 *
 * Note: Since we ensure that this class can only be constructed from mutable types, it is okay
 * to use const_cast to cast away the constness of the member variables (which are part of the
 * const base class definition and therefore themselves const).
 *
 * @see Node
 */
template< typename Variable > class Expression : public ConstExpression< Variable > {
public:
	Expression(Numeric nodeIndex, Node &node, ExpressionTree< Variable > &tree)
		: ConstExpression< Variable >(std::move(nodeIndex), node, tree) {}

	// Inherit constructors from base class
	using ConstExpression< Variable >::ConstExpression;

	/**
	 * @returns The represented variable object
	 *
	 * Note: If this expression doesn't actually represent a variable, calling this function is undefined behavior!
	 */
	[[nodiscard]] auto getVariable() -> Variable & {
		// NOLINTNEXTLINE(*-const-cast)
		return const_cast< Variable & >(std::as_const(*this).getVariable());
	}


	/**
	 * Sets the represented literal value of this expression.
	 *
	 * Note: If this expression doesn't actually represent a literal value, calling this function is undefined behavior!
	 */
	void setLiteral(std::int32_t numerator, std::int32_t denominator = 1) {
		assert(this->getCardinality() == ExpressionCardinality::Nullary);
		assert(this->getType() == ExpressionType::Literal);

		node().setLeftChild(Numeric(signed_cast< Numeric::numeric_type >(numerator)));
		node().setRightChild(Numeric(signed_cast< Numeric::numeric_type >(denominator)));
	}

	/**
	 * @returns The Expression representing the left argument of the represented binary expression
	 *
	 * Note: If this expression is not actually binary, calling this function is undefined behavior!
	 */
	[[nodiscard]] auto getLeftArg() -> Expression {
		assert(this->getCardinality() == ExpressionCardinality::Binary);
		assert(node().getLeftChild() < tree().m_nodes.size());

		return { node().getLeftChild(), tree().m_nodes[node().getLeftChild()], tree() };
	}

	/**
	 * @returns The Expression representing the right argument of the represented binary expression
	 *
	 * Note: If this expression is not actually binary, calling this function is undefined behavior!
	 */
	[[nodiscard]] auto getRightArg() -> Expression {
		assert(this->getCardinality() == ExpressionCardinality::Binary);
		assert(node().getLeftChild() < tree().m_nodes.size());

		return { node().getRightChild(), tree().m_nodes[node().getRightChild()], tree() };
	}

	/**
	 * @returns The Expression representing the argument of the represented unary expression
	 *
	 * Note: If this expression is not actually unary, calling this function is undefined behavior!
	 */
	[[nodiscard]] auto getArg() -> Expression {
		assert(this->getCardinality() == ExpressionCardinality::Unary);
		assert(node().getLeftChild() < tree().m_nodes.size());

		return { node().getLeftChild(), tree().m_nodes[node().getLeftChild()], tree() };
	}

protected:
	[[nodiscard]] auto nodeIndex() -> Numeric & {
		// NOLINTNEXTLINE(*-const-cast)
		return const_cast< Numeric & >(std::as_const(*this).nodeIndex());
	}

	[[nodiscard]] auto node() -> Node & {
		// NOLINTNEXTLINE(*-const-cast)
		return const_cast< Node & >(std::as_const(*this).node());
	}

	[[nodiscard]] auto tree() -> ExpressionTree< Variable > & {
		// NOLINTNEXTLINE(*-const-cast)
		return const_cast< ExpressionTree< Variable > & >(std::as_const(*this).tree());
	}
};


} // namespace lizard


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
auto operator==(const std::vector< Expr1 > &lhs, const std::vector< Expr2 > &rhs) -> bool {
	if (lhs.size() != rhs.size()) {
		return false;
	}
	for (std::size_t i = 0; i < lhs.size(); ++i) {
		if (lhs[i] != rhs[i]) {
			return false;
		}
	}
	return true;
}

#endif // LIZARD_CORE_EXPRESSION_HPP_
