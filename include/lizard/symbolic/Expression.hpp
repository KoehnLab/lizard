// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#pragma once

#include "lizard/core/Fraction.hpp"
#include "lizard/core/Numeric.hpp"
#include "lizard/core/SignedCast.hpp"
#include "lizard/core/TypeTraits.hpp"
#include "lizard/symbolic/ExpressionCardinality.hpp"
#include "lizard/symbolic/ExpressionOperator.hpp"
#include "lizard/symbolic/ExpressionTree.hpp"
#include "lizard/symbolic/ExpressionType.hpp"
#include "lizard/symbolic/Expression_fwd.hpp"
#include "lizard/symbolic/TreeNode.hpp"
#include "lizard/symbolic/details/ExpressionTreeIteratorCore.hpp"

#include <cassert>
#include <iostream>
#include <stack>
#include <type_traits>
#include <utility>
#include <vector>

#include <hedley.h>

namespace lizard {

template< typename Variable >
ConstExpression< Variable >::ConstExpression(Numeric nodeID, const TreeNode &node,
											 const ExpressionTree< Variable > &tree)
	: m_nodeID(std::move(nodeID)), m_node(&node), m_tree(&tree) {
}

template< typename Variable > auto ConstExpression< Variable >::getCardinality() const -> ExpressionCardinality {
	return m_node->getCardinality();
}

template< typename Variable > auto ConstExpression< Variable >::getType() const -> ExpressionType {
	return m_node->getType();
}

template< typename Variable > auto ConstExpression< Variable >::getParent() const -> ConstExpression< Variable > {
	assert(!isRoot());

	return { m_node->getParent(), m_tree->m_nodes[m_node->getParent()], *m_tree };
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

template< typename Variable > auto ConstExpression< Variable >::getLiteral() const -> Fraction {
	assert(getCardinality() == ExpressionCardinality::Nullary);
	assert(getType() == ExpressionType::Literal);

	static_assert(std::is_same_v< Fraction::field_type, std::make_signed_t< Numeric::numeric_type > >,
				  "Expected integers to be of same width in order for signed_cast to work");

	// For literals, the left and right child's ID is the numerator and denominator of the represented literal value
	// respectively
	return { signed_cast< Fraction::field_type >(m_node->getLeftChild()),
			 signed_cast< Fraction::field_type >(m_node->getRightChild()) };
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

template< typename Variable > auto ConstExpression< Variable >::size() const -> Numeric::numeric_type {
	if (m_nodeID == m_tree->m_rootID) {
		// The represented expression is the same as the one represented by the associated expression tree
		// -> Use it to obtain the size as that's more efficient
		return m_tree->size();
	}

	return computeSize();
}

template< typename Variable >
auto ConstExpression< Variable >::getContainingTree() const -> const ExpressionTree< Variable > & {
	assert(m_tree);
	return *m_tree;
}

template< typename Variable >
auto ConstExpression< Variable >::isSame(const ConstExpression< Variable > &other) const -> bool {
	return m_tree == other.m_tree && m_nodeID == other.m_nodeID && m_node == other.m_node;
}

template< typename Variable >
template< TreeTraversal iteration_order >
auto ConstExpression< Variable >::begin() const -> iterator_template< iteration_order > {
	return cbegin< iteration_order >();
}

template< typename Variable >
template< TreeTraversal iteration_order >
auto ConstExpression< Variable >::end() const -> iterator_template< iteration_order > {
	return cend< iteration_order >();
}

template< typename Variable >
template< TreeTraversal iteration_order >
auto ConstExpression< Variable >::cbegin() const -> iterator_template< iteration_order > {
	return details::ExpressionTreeIteratorCore< Variable, true, iteration_order >::fromRoot(tree(), nodeID());
}

template< typename Variable >
template< TreeTraversal iteration_order >
auto ConstExpression< Variable >::cend() const -> iterator_template< iteration_order > {
	return details::ExpressionTreeIteratorCore< Variable, true, iteration_order >::afterRoot(tree(), nodeID());
}

template< typename Variable >
template< TreeTraversal iteration_order >
auto ConstExpression< Variable >::afterRoot() const -> iterator_template< iteration_order > {
	// In our case, end() == afterRoot()
	return end();
}

template< typename Variable >
template< TreeTraversal iteration_order >
auto ConstExpression< Variable >::afterRootEnd() const -> iterator_template< iteration_order > {
	if (!isRoot()) {
		return details::ExpressionTreeIteratorCore< Variable, true, iteration_order >::afterRoot(tree(),
																								 afterRoot()->nodeID());
	}

	return end();
}

template< typename Variable > auto ConstExpression< Variable >::nodeID() const -> const Numeric & {
	return m_nodeID;
}

template< typename Variable > auto ConstExpression< Variable >::node() const -> const TreeNode & {
	return *m_node;
}

template< typename Variable > auto ConstExpression< Variable >::tree() const -> const ExpressionTree< Variable > & {
	return *m_tree;
}

template< typename Variable > auto ConstExpression< Variable >::computeSize() const -> Numeric::numeric_type {
	switch (getCardinality()) {
		case ExpressionCardinality::Nullary:
			return 1;
		case ExpressionCardinality::Unary:
			return getLeftArg().computeSize() + 1;
		case ExpressionCardinality::Binary:
			return getLeftArg().computeSize() + getRightArg().computeSize() + 1;
	}

	HEDLEY_UNREACHABLE();
}



template< typename Variable >
auto operator==(const ConstExpression< Variable > &lhs, const ConstExpression< Variable > &rhs) -> bool {
	auto lhsIt  = lhs.cbegin();
	auto lhsEnd = lhs.cend();
	auto rhsIt  = rhs.cbegin();
	auto rhsEnd = rhs.cend();

	while (lhsIt != lhsEnd && rhsIt != rhsEnd) {
		const ConstExpression< Variable > &currentLHS = *lhsIt;
		const ConstExpression< Variable > &currentRHS = *rhsIt;

		if (currentLHS.getType() != currentRHS.getType()) {
			return false;
		}

		switch (currentLHS.getType()) {
			case ExpressionType::Literal:
				if (currentLHS.getLiteral() != currentRHS.getLiteral()) {
					return false;
				}
				break;
			case ExpressionType::Operator:
				if (currentLHS.getOperator() != currentRHS.getOperator()) {
					return false;
				}
				break;
			case ExpressionType::Variable:
				if (currentLHS.getVariable() != currentRHS.getVariable()) {
					return false;
				}
				break;
		}

		lhsIt++;
		rhsIt++;
	}

	return lhsIt == lhsEnd && rhsIt == rhsEnd;
}

template< typename Variable >
auto operator!=(const ConstExpression< Variable > &lhs, const ConstExpression< Variable > &rhs) -> bool {
	return !(lhs == rhs);
}

template< typename Variable >
auto operator<<(std::ostream &stream, const ConstExpression< Variable > &expr) -> std::ostream & {
	return stream << "TreeNode " << expr.m_nodeID << ": " << *expr.m_node << " (" << expr.m_tree << ")";
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
Expression< Variable >::Expression(Numeric nodeID, TreeNode &node, ExpressionTree< Variable > &tree)
	: ConstExpression< Variable >(std::move(nodeID), node, tree) {
}

template< typename Variable > auto Expression< Variable >::getVariable() -> Variable & {
	// NOLINTNEXTLINE(*-const-cast)
	return const_cast< Variable & >(ConstExpression< Variable >::getVariable());
}


template< typename Variable > void Expression< Variable >::setLiteral(const Fraction &fraction) {
	assert(this->getCardinality() == ExpressionCardinality::Nullary);
	assert(this->getType() == ExpressionType::Literal);
	static_assert(std::is_same_v< Fraction::field_type, std::make_signed_t< Numeric::numeric_type > >,
				  "Expected integers to be of same width in order for signed_cast to work");

	node().setLeftChild(Numeric(signed_cast< Numeric::numeric_type >(fraction.getNumerator())));
	node().setRightChild(Numeric(signed_cast< Numeric::numeric_type >(fraction.getDenominator())));
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

template< typename Variable > auto Expression< Variable >::node() -> TreeNode & {
	// NOLINTNEXTLINE(*-const-cast)
	return const_cast< TreeNode & >(ConstExpression< Variable >::node());
}

template< typename Variable > auto Expression< Variable >::tree() -> ExpressionTree< Variable > & {
	// NOLINTNEXTLINE(*-const-cast)
	return const_cast< ExpressionTree< Variable > & >(ConstExpression< Variable >::tree());
}

template< typename Variable > void Expression< Variable >::substituteWith(Variable variable) {
	tree().substitute(this->nodeID(), std::move(variable));
}

template< typename Variable > void Expression< Variable >::substituteWith(const ConstExpression< Variable > &expr) {
	using Iterator = typename ExpressionTree< Variable >::const_post_order_iterator;
	using Core     = details::ExpressionTreeIteratorCore< Variable, true, TreeTraversal::DepthFirst_PostOrder >;

	Iterator begin(Core::fromRoot(expr.tree(), expr.nodeID()));
	Iterator end(Core::afterRoot(expr.tree(), expr.nodeID()));

	tree().substitute(this->nodeID(), begin, end);
}

template< typename Variable >
template< TreeTraversal iteration_order >
auto Expression< Variable >::begin() -> iterator_template< iteration_order > {
	return details::ExpressionTreeIteratorCore< Variable, false, iteration_order >::fromRoot(
		tree(), ConstExpression< Variable >::nodeID());
}

template< typename Variable >
template< TreeTraversal iteration_order >
auto Expression< Variable >::end() -> iterator_template< iteration_order > {
	return details::ExpressionTreeIteratorCore< Variable, false, iteration_order >::afterRoot(
		tree(), ConstExpression< Variable >::nodeID());
}


template< typename Variable >
template< TreeTraversal iteration_order >
auto Expression< Variable >::afterRoot() -> iterator_template< iteration_order > {
	// In our case, end() == afterRoot()
	return end();
}

template< typename Variable >
template< TreeTraversal iteration_order >
auto Expression< Variable >::afterRootEnd() -> iterator_template< iteration_order > {
	if (!this->isRoot()) {
		return details::ExpressionTreeIteratorCore< Variable, true, iteration_order >::afterRoot(tree(),
																								 afterRoot()->nodeID());
	}

	return end();
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
