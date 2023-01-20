// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/Krzmbrzl/lizard/blob/main/LICENSE>.

#pragma once

#include "Expression_fwd.hpp"

#include "lizard/core/ExpressionCardinality.hpp"
#include "lizard/core/ExpressionOperator.hpp"
#include "lizard/core/ExpressionTree.hpp"
#include "lizard/core/ExpressionType.hpp"
#include "lizard/core/Node.hpp"
#include "lizard/core/Numeric.hpp"
#include "lizard/core/SignedCast.hpp"
#include "lizard/core/type_traits.hpp"

#include <cassert>
#include <iostream>
#include <stack>
#include <type_traits>
#include <utility>
#include <vector>

namespace lizard {

template< typename Variable >
ConstExpression< Variable >::ConstExpression(Numeric nodeIndex, const Node &node,
											 const ExpressionTree< Variable > &tree)
	: m_nodeIndex(std::move(nodeIndex)), m_node(&node), m_tree(&tree) {
}

template< typename Variable > auto ConstExpression< Variable >::getCardinality() const -> ExpressionCardinality {
	return m_node->getCardinality();
}

template< typename Variable > auto ConstExpression< Variable >::getType() const -> ExpressionType {
	return m_node->getType();
}

template< typename Variable > auto ConstExpression< Variable >::getVariable() const -> const Variable & {
	assert(getCardinality() == ExpressionCardinality::Nullary);
	assert(getType() == ExpressionType::Variable);
	assert(m_node->getLeftChild() < m_tree->m_variables.size());

	return m_tree->m_variables[m_node->getLeftChild()];
}

template< typename Variable > auto ConstExpression< Variable >::getOperator() const -> ExpressionOperator {
	assert(getCardinality() == ExpressionCardinality::Binary);
	assert(getType() == ExpressionType::Operator);

	return m_node->getOperator();
}

template< typename Variable > auto ConstExpression< Variable >::getNumerator() const -> std::int32_t {
	assert(getCardinality() == ExpressionCardinality::Nullary);
	assert(getType() == ExpressionType::Literal);

	// For literals the left child's ID really is to be taken as a numeric value representing the literal's
	// numerator
	return signed_cast< std::int32_t >(m_node->getLeftChild());
}

template< typename Variable > auto ConstExpression< Variable >::getDenominator() const -> std::int32_t {
	assert(getCardinality() == ExpressionCardinality::Nullary);
	assert(getType() == ExpressionType::Literal);

	// For literals the right child's ID really is to be taken as a numeric value representing the literal's
	// denominator
	return signed_cast< std::int32_t >(m_node->getRightChild());
}

template< typename Variable > auto ConstExpression< Variable >::getLiteral() const -> double {
	assert(getCardinality() == ExpressionCardinality::Nullary);
	assert(getType() == ExpressionType::Literal);

	return static_cast< double >(getNumerator()) / getDenominator();
}

template< typename Variable > auto ConstExpression< Variable >::getLeftArg() const -> ConstExpression< Variable > {
	assert(getCardinality() == ExpressionCardinality::Binary);
	assert(m_node->getLeftChild() < m_tree->m_nodes.size());

	return { m_node->getLeftChild(), m_tree->m_nodes[m_node->getLeftChild()], *m_tree };
}

template< typename Variable > auto ConstExpression< Variable >::getRightArg() const -> ConstExpression< Variable > {
	assert(getCardinality() == ExpressionCardinality::Binary);
	assert(m_node->getRightChild() < m_tree->m_nodes.size());

	return { m_node->getRightChild(), m_tree->m_nodes[m_node->getRightChild()], *m_tree };
}

template< typename Variable > auto ConstExpression< Variable >::getArg() const -> ConstExpression< Variable > {
	assert(getCardinality() == ExpressionCardinality::Unary);
	assert(m_node->getLeftChild() < m_tree->m_nodes.size());

	return { m_node->getLeftChild(), m_tree->m_nodes[m_node->getLeftChild()], *m_tree };
}

template< typename Variable > auto ConstExpression< Variable >::isRoot() const -> bool {
	return !m_node->hasParent();
}

template< typename Variable > auto ConstExpression< Variable >::nodeIndex() const -> const Numeric & {
	return m_nodeIndex;
}

template< typename Variable > auto ConstExpression< Variable >::node() const -> const Node & {
	return *m_node;
}

template< typename Variable > auto ConstExpression< Variable >::tree() const -> const ExpressionTree< Variable > & {
	return *m_tree;
}



template< typename Variable >
auto operator==(const ConstExpression< Variable > &lhs, const ConstExpression< Variable > &rhs) -> bool {
	return lhs.m_nodeIndex == rhs.m_nodeIndex && *lhs.m_node == *rhs.m_node && lhs.m_tree == rhs.m_tree;
}

template< typename Variable >
auto operator!=(const ConstExpression< Variable > &lhs, const ConstExpression< Variable > &rhs) -> bool {
	return !(lhs == rhs);
}

template< typename Variable >
auto operator<<(std::ostream &stream, const ConstExpression< Variable > &expr) -> std::ostream & {
	return stream << "Node " << expr.m_nodeIndex << ": " << *expr.m_node << " (" << expr.m_tree << ")";
}


//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////// EXPRESSION ////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

/*
 * Note: Since we ensure that this class can only be constructed from mutable types, it is okay
 * to use const_cast to cast away the constness of the member variables (which are part of the
 * const base class definition and therefore themselves const).
 */

template< typename Variable >
Expression< Variable >::Expression(Numeric nodeIndex, Node &node, ExpressionTree< Variable > &tree)
	: ConstExpression< Variable >(std::move(nodeIndex), node, tree) {
}

template< typename Variable > auto Expression< Variable >::getVariable() -> Variable & {
	// NOLINTNEXTLINE(*-const-cast)
	return const_cast< Variable & >(std::as_const(*this).getVariable());
}


template< typename Variable >
void Expression< Variable >::setLiteral(std::int32_t numerator, std::int32_t denominator) {
	assert(this->getCardinality() == ExpressionCardinality::Nullary);
	assert(this->getType() == ExpressionType::Literal);

	node().setLeftChild(Numeric(signed_cast< Numeric::numeric_type >(numerator)));
	node().setRightChild(Numeric(signed_cast< Numeric::numeric_type >(denominator)));
}

template< typename Variable > auto Expression< Variable >::getLeftArg() -> Expression< Variable > {
	assert(this->getCardinality() == ExpressionCardinality::Binary);
	assert(node().getLeftChild() < tree().m_nodes.size());

	return { node().getLeftChild(), tree().m_nodes[node().getLeftChild()], tree() };
}

template< typename Variable > auto Expression< Variable >::getRightArg() -> Expression< Variable > {
	assert(this->getCardinality() == ExpressionCardinality::Binary);
	assert(node().getLeftChild() < tree().m_nodes.size());

	return { node().getRightChild(), tree().m_nodes[node().getRightChild()], tree() };
}

template< typename Variable > auto Expression< Variable >::getArg() -> Expression< Variable > {
	assert(this->getCardinality() == ExpressionCardinality::Unary);
	assert(node().getLeftChild() < tree().m_nodes.size());

	return { node().getLeftChild(), tree().m_nodes[node().getLeftChild()], tree() };
}

template< typename Variable > auto Expression< Variable >::nodeIndex() -> Numeric & {
	// NOLINTNEXTLINE(*-const-cast)
	return const_cast< Numeric & >(std::as_const(*this).nodeIndex());
}

template< typename Variable > auto Expression< Variable >::node() -> Node & {
	// NOLINTNEXTLINE(*-const-cast)
	return const_cast< Node & >(std::as_const(*this).node());
}

template< typename Variable > auto Expression< Variable >::tree() -> ExpressionTree< Variable > & {
	// NOLINTNEXTLINE(*-const-cast)
	return const_cast< ExpressionTree< Variable > & >(std::as_const(*this).tree());
}



// Consistency checks
static_assert(std::is_convertible_v< Expression< int >, ConstExpression< int > >,
			  "A mutable expression must be implicitly convertible to a constant expression");
static_assert(!std::is_convertible_v< ConstExpression< int >, Expression< int > >,
			  "A constant expression must NOT be convertible to a mutable expression");


} // namespace lizard


// Support operator== between expressions and Variable types
template< typename Variable >
auto operator==(const lizard::ConstExpression< Variable > &lhs, const Variable &rhs) -> bool {
	return lhs.getType() == lizard::ExpressionType::Variable && lhs.getVariable() == rhs;
}

template< typename Variable >
auto operator==(const Variable &lhs, const lizard::ConstExpression< Variable > &rhs) -> bool {
	return rhs == lhs;
}


// Support operator== for containers holding ConstExpression and Expression

template< typename Expr1, typename Expr2, typename >
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
