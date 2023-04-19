// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#pragma once

#include "lizard/core/Numeric.hpp"
#include "lizard/symbolic/Expression.hpp"
#include "lizard/symbolic/ExpressionCardinality.hpp"
#include "lizard/symbolic/ExpressionException.hpp"
#include "lizard/symbolic/ExpressionOperator.hpp"
#include "lizard/symbolic/ExpressionType.hpp"
#include "lizard/symbolic/TreeNode.hpp"
#include "lizard/symbolic/TreeTraversal.hpp"
#include "lizard/symbolic/details/ExpressionTreeIteratorCore.hpp"

#include <algorithm>
#include <cassert>
#include <iterator>
#include <stack>
#include <type_traits>
#include <vector>

#include <iterators/iterator_facade.hpp>
#include <iterators/type_traits.hpp>

namespace lizard {

/**
 * A class representing an expression tree. Its goal is to represent the tree in a way that makes
 * common operations (e.g. iterating over it, replacing nodes) as efficient as (reasonably) possible
 */
template< typename Variable > class ExpressionTree {
public:
	template< typename > friend class ConstExpression;
	template< typename, bool, TreeTraversal > friend class details::ExpressionTreeIteratorCore;

	template< bool is_const, TreeTraversal iteration_order >
	using iterator_template =
		iterators::iterator_facade< details::ExpressionTreeIteratorCore< Variable, is_const, iteration_order > >;

	using post_order_iterator       = iterator_template< false, TreeTraversal::DepthFirst_PostOrder >;
	using const_post_order_iterator = iterator_template< true, TreeTraversal::DepthFirst_PostOrder >;
	using pre_order_iterator        = iterator_template< false, TreeTraversal::DepthFirst_PreOrder >;
	using const_pre_order_iterator  = iterator_template< true, TreeTraversal::DepthFirst_PreOrder >;
	using in_order_iterator         = iterator_template< false, TreeTraversal::DepthFirst_InOrder >;
	using const_in_order_iterator   = iterator_template< true, TreeTraversal::DepthFirst_InOrder >;

	using iterator       = post_order_iterator;
	using const_iterator = const_post_order_iterator;

	static_assert(!iterators::is_const_iterator_facade_v< iterator >, "Inconsistency in iterator implementation");
	static_assert(iterators::is_const_iterator_facade_v< const_iterator >, "Inconsistency in iterator implementation");


	ExpressionTree()                           = default;
	ExpressionTree(const ExpressionTree &)     = default;
	ExpressionTree(ExpressionTree &&) noexcept = default;
	virtual ~ExpressionTree()                  = default;

	auto operator=(const ExpressionTree &) -> ExpressionTree & = default;
	auto operator=(ExpressionTree &&) noexcept -> ExpressionTree & = default;

	/**
	 * @returns Whether the represented tree is currently empty
	 */
	[[nodiscard]] auto isEmpty() const -> bool { return m_nodes.empty(); }

	/**
	 * @returns The amount of nodes currently in this tree
	 */
	[[nodiscard]] auto size() const -> Numeric::numeric_type { return m_size; }

	/**
	 * @returns Whether this tree is in a valid state
	 */
	[[nodiscard]] auto isValid() const -> bool { return isEmpty() || m_rootID.isValid(); }

	/**
	 * Ensures that the internally used buffers are big enough to hold at least the given amount of nodes and variables
	 * without reallocating in between.
	 */
	void reserve(std::size_t nodeCount, std::size_t variableCount = 0) {
		m_nodes.reserve(nodeCount);
		m_variables.reserve(variableCount);
	}

	/**
	 * Clears this tree of any content that it is currently holding. After this operation, the tree will be empty
	 */
	void clear() {
		m_variables.clear();
		m_nodes.clear();
		m_consumableNodes = {};
		m_rootID.reset();
		m_size = 0;
	}

	/**
	 * @returns The root expression in this tree
	 */
	auto getRoot() const -> ConstExpression< Variable > { return { m_rootID, m_nodes[m_rootID], *this }; }

	/**
	 * @returns The root expression in this tree
	 */
	auto getRoot() -> Expression< Variable > { return { m_rootID, m_nodes[m_rootID], *this }; }

	/**
	 * Adds the given Variable object as a nullary expression to this tree. The general rules for adding nodes
	 * to the tree apply.
	 */
	void add(Variable var) { add(addVariable(std::move(var))); }

	/**
	 * Adds the given node to this tree. TreeNodes have to be added in the order in which they
	 * would appear in postfix notation. Thus, when an expression of the form a*b shall be added,
	 * the nodes have to be added in the order "a", "b", "*".
	 */
	void add(TreeNode node) {
		static_assert(static_cast< int >(ExpressionCardinality::Binary) == 2,
					  "Cardinality is expected to numerically represent argument count");
		if (m_consumableNodes.size() < static_cast< std::size_t >(node.getCardinality())) {
			throw ExpressionException(
				"Added expression node requires " + std::to_string(static_cast< int >(node.getCardinality()))
				+ " arguments, but only " + std::to_string(m_consumableNodes.size()) + " arguments are available");
		}

		Numeric nodeID(static_cast< Numeric::numeric_type >(m_nodes.size()));
		switch (node.getCardinality()) {
			case ExpressionCardinality::Binary: {
				Numeric rhs = std::move(m_consumableNodes.top());
				m_consumableNodes.pop();
				Numeric lhs = std::move(m_consumableNodes.top());
				m_consumableNodes.pop();

				m_nodes[rhs].setParent(nodeID);
				m_nodes[lhs].setParent(nodeID);

				node.setLeftChild(std::move(lhs));
				node.setRightChild(std::move(rhs));

				break;
			}
			case ExpressionCardinality::Unary: {
				Numeric arg = m_consumableNodes.top();
				m_consumableNodes.pop();

				m_nodes[arg].setParent(nodeID);

				node.setLeftChild(std::move(arg));

				break;
			}
			case ExpressionCardinality::Nullary:
				break;
		}

		// Store the expression as a new node
		m_nodes.push_back(std::move(node));
		m_size++;

		if (m_consumableNodes.empty()) {
			// The node that has consumed the last arguments available thus far must be the new root node
			m_rootID = nodeID;
		} else {
			// At the moment there is no root node as the expression is unfinished
			m_rootID = {};
		}

		// Every expression is understood to create (return) a value and therefore, it can be used
		// as an argument to a subsequent expression
		m_consumableNodes.push(std::move(nodeID));
	}

	template< TreeTraversal iteration_order = TreeTraversal::DepthFirst_PostOrder >
	auto begin() -> iterator_template< false, iteration_order > {
		return getRoot().template begin< iteration_order >();
	}

	template< TreeTraversal iteration_order = TreeTraversal::DepthFirst_PostOrder >
	auto end() -> iterator_template< false, iteration_order > {
		return getRoot().template end< iteration_order >();
	}

	template< TreeTraversal iteration_order = TreeTraversal::DepthFirst_PostOrder >
	auto begin() const -> iterator_template< true, iteration_order > {
		return getRoot().template begin< iteration_order >();
	}

	template< TreeTraversal iteration_order = TreeTraversal::DepthFirst_PostOrder >
	auto end() const -> iterator_template< true, iteration_order > {
		return getRoot().template end< iteration_order >();
	}

	template< TreeTraversal iteration_order = TreeTraversal::DepthFirst_PostOrder >
	auto cbegin() const -> iterator_template< true, iteration_order > {
		return getRoot().template cbegin< iteration_order >();
	}

	template< TreeTraversal iteration_order = TreeTraversal::DepthFirst_PostOrder >
	auto cend() const -> iterator_template< true, iteration_order > {
		return getRoot().template cend< iteration_order >();
	}

	friend auto operator==(const ExpressionTree &lhs, const ExpressionTree &rhs) -> bool {
		if (lhs.size() != rhs.size()) {
			return false;
		}

		auto lhsIt  = lhs.begin< TreeTraversal::DepthFirst_PreOrder >();
		auto lhsEnd = lhs.begin< TreeTraversal::DepthFirst_PreOrder >();
		auto rhsIt  = rhs.begin< TreeTraversal::DepthFirst_PreOrder >();
		auto rhsEnd = rhs.begin< TreeTraversal::DepthFirst_PreOrder >();

		while (lhsIt != lhsEnd && rhsIt != rhsEnd) {
			if (*lhsIt != *rhsIt) {
				return false;
			}
			if (lhsIt->getType() == ExpressionType::Variable) {
				if (lhsIt->getVariable() != rhsIt->getVariable()) {
					return false;
				}
			}

			lhsIt++;
			rhsIt++;
		}

		return lhsIt == lhsEnd && rhsIt == rhsEnd;
	}

	friend auto operator!=(const ExpressionTree &lhs, const ExpressionTree &rhs) -> bool { return !(lhs == rhs); }

private:
	std::vector< Variable > m_variables;
	std::vector< TreeNode > m_nodes;
	std::stack< Numeric > m_consumableNodes;
	Numeric m_rootID;
	Numeric::numeric_type m_size = 0;

	friend class ConstExpression< Variable >;
	friend class Expression< Variable >;

	/**
	 * Adds the given Variable to this tree by appending the Variable to the variable store and creating a new
	 * TreeNode that points to this newly appended Variable. However, the TreeNode itself is not added to the tree.
	 *
	 * @returns The created TreeNode
	 */
	auto addVariable(Variable var) -> TreeNode {
		m_variables.push_back(std::move(var));

		// Create an Expression object the references the variable
		return TreeNode{ ExpressionType::Variable,
						 Numeric(static_cast< Numeric::numeric_type >(m_variables.size() - 1)) };
	}

	/**
	 * Substitutes the TreeNode with the given ID with a new TreeNode representing the provided Variable
	 * (which will be stored inside this tree).
	 */
	void substitute(const Numeric &nodeID, Variable variable) {
		assert(nodeID < m_nodes.size());

		// TODO: Mark space currently occupied by sub-tree under m_nodes[nodeID] as reusable
		TreeNode node = addVariable(variable);
		node.setParent(m_nodes[nodeID].getParent());

		m_nodes[nodeID] = std::move(node);
	}

	/**
	 * Substitutes the TreeNode with the given ID with the sub-tree iterated over by means of the provided
	 * iterator pair. The iteration order is expected to be depth-first, post-order. Using any other
	 * iteration / tree traversal order is not supported!
	 */
	template< typename Iterator > void substitute(const Numeric &nodeID, Iterator iter, const Iterator end) {
		static_assert(
			std::is_base_of_v< ConstExpression< Variable >, typename std::iterator_traits< Iterator >::value_type >,
			"ExpressionTree::substitute expects iterators working on Expressions");

		if (iter == end) {
			return;
		}

		assert(nodeID < m_nodes.size());

		Numeric parentID = m_nodes[nodeID].getParent();
		Numeric::numeric_type replacedSize =
			ConstExpression< Variable >(nodeID, std::move(m_nodes[nodeID]), *this).size();
		// TODO: Mark space currently occupied by the sub-tree under m_nodes[nodeID] as reusable

		assert(m_size >= replacedSize);
		m_size -= replacedSize;

		// Save state and clear it for the duration of the substitution
		decltype(m_consumableNodes) copy = std::move(m_consumableNodes);
		m_consumableNodes                = {};
		decltype(m_rootID) rootCopy      = std::move(m_rootID);
		m_rootID.reset();

		for (; iter != end; ++iter) {
			if (iter->getType() == ExpressionType::Variable) {
				add(iter->getVariable());
			} else {
				add(*iter->m_node);
			}
		}

		// Perform post-substitution verification and postprocessing
		assert(isValid());
		assert(m_consumableNodes.size() == 1);
		assert(m_rootID.isValid());
		assert(m_rootID < m_nodes.size());
		if (!isValid() || m_consumableNodes.size() != 1 || !m_rootID.isValid()) {
			throw ExpressionException("Substitution lead to an inconsistent tree state");
		}

		m_nodes[m_rootID].setParent(parentID);

		if (parentID.isValid()) {
			TreeNode &parent = m_nodes[parentID];
			if (parent.getLeftChild() == nodeID) {
				parent.setLeftChild(std::move(m_rootID));
			} else {
				parent.setRightChild(std::move(m_rootID));
			}
		}

		// Restore previous state
		m_consumableNodes = std::move(copy);
		m_rootID          = std::move(rootCopy);
	}
};



} // namespace lizard
