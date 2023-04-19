// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#pragma once

#include "lizard/core/Numeric.hpp"
#include "lizard/symbolic/DepthFirst.hpp"
#include "lizard/symbolic/TreeNode.hpp"
#include "lizard/symbolic/TreeTraversal.hpp"

#include <iostream>
#include <iterator>

#include <iterators/is_semantically_const.hpp>

#include <hedley.h>


namespace lizard {

template< typename > class ExpressionTree;
template< typename > class Expression;
template< typename > class ConstExpression;

} // namespace lizard

namespace lizard::details {

/**
 * Implementation of an iterator intended for iterating over an expression tree.
 *
 * @tparam Variable The type for representing variables in the given expression tree
 * @tparam is_const Whether this is supposed to represent a const_iterator
 * @tparam iteration_order The TreeTraversal indicating in what order the given tree shall be traversed
 *
 * @see TreeTraversal
 */
template< typename Variable, bool isConst, TreeTraversal iterationOrder = TreeTraversal::DepthFirst_PreOrder >
class ExpressionTreeIteratorCore {
public:
	using target_iterator_category = std::input_iterator_tag;

	using tree_reference =
		std::conditional_t< isConst, const ExpressionTree< Variable > &, ExpressionTree< Variable > & >;
	using tree_pointer    = std::add_pointer_t< std::remove_reference_t< tree_reference > >;
	using expression_type = std::conditional_t< isConst, ConstExpression< Variable >, Expression< Variable > >;
	using node_type       = std::conditional_t< isConst, const TreeNode, TreeNode >;

	/**
	 * @param tree The ExpressionTree the created core shall be associated with
	 *
	 * @returns An iterator core in a state that is only ever reached, if a all Nodes in the tree have been visited
	 * (regardless of iteration order)
	 */
	static auto end(tree_reference tree) -> ExpressionTreeIteratorCore { return { &tree, Numeric{}, Numeric{} }; }

	/**
	 * @param tree The ExpressionTree the created core shall be associated with
	 * @param nodeID The ID of the referenced TreeNode
	 *
	 * @returns An iterator core in a state that will dereference to the given node
	 */
	static auto at(tree_reference tree, Numeric nodeID) -> ExpressionTreeIteratorCore {
		return { &tree, nodeID, nodeID };
	}

	static auto at(tree_reference tree, const expression_type &expr) -> ExpressionTreeIteratorCore {
		return at(tree, expr.nodeID());
	}

	/**
	 * @param tree The ExpressionTree the created core shall be associated with
	 * @param nodeID The ID of the referenced TreeNode
	 *
	 * @returns An iterator core in a state that will dereference to the TreeNode that will be visited after the
	 * provided TreeNode (if any)
	 */
	static auto after(tree_reference tree, Numeric nodeID) -> ExpressionTreeIteratorCore {
		ExpressionTreeIteratorCore core = at(tree, nodeID);
		core.increment();

		return core;
	}

	static auto after(tree_reference tree, const expression_type &expr) -> ExpressionTreeIteratorCore {
		return after(tree, expr.nodeID());
	}

	/**
	 * @param tree The ExpressionTree the created core shall be associated with
	 * @param nodeID The ID of TreeNode that shall be considered as root TreeNode
	 *
	 * @returns An iterator core in a state that will dereference to the TreeNode has to be visited in order traverse
	 * the tree represented by the given root TreeNode (with its sub-tree) in the respective iteration order
	 */
	static auto fromRoot(tree_reference tree, Numeric nodeID) -> ExpressionTreeIteratorCore {
		switch (iterationOrder) {
			case TreeTraversal::DepthFirst_PreOrder:
				// The root node is in fact the first TreeNode to be visited
				return at(tree, nodeID);
			case TreeTraversal::DepthFirst_PostOrder:
			case TreeTraversal::DepthFirst_InOrder:
				// Start with a step that indicates that we are currently mid-traversal (coming from the TreeNode's
				// parent). Thus, calling increment() on such a state will go and find the TreeNode that shall be
				// visited next.
				assert(nodeID < tree.m_nodes.size());
				const TreeNode &node = tree.m_nodes[nodeID];
				ExpressionTreeIteratorCore core(&tree, nodeID, node.getParent());
				core.increment();
				return core;
		}

		HEDLEY_UNREACHABLE();
	}

	static auto fromRoot(tree_reference tree, const expression_type &expr) -> ExpressionTreeIteratorCore {
		return fromRoot(tree, expr.nodeID());
	}

	/**
	 * @param tree The ExpressionTree the created core shall be associated with
	 * @param nodeID The ID of TreeNode that shall be considered as root TreeNode
	 *
	 * @returns An iterator core in a state that will dereference to whatever TreeNode (if any) is visited after the
	 * sub-tree under (and including) the given root TreeNode has been visited.
	 */
	static auto afterRoot(tree_reference tree, Numeric nodeID) -> ExpressionTreeIteratorCore {
		assert(nodeID < tree.m_nodes.size());

		ExpressionTreeIteratorCore core = [&]() -> ExpressionTreeIteratorCore {
			// Create a state that will lead to the first TreeNode that is outside the given sub-tree upon increment
			const TreeNode &node = tree.m_nodes[nodeID];
			switch (iterationOrder) {
				case TreeTraversal::DepthFirst_InOrder:
					// State: Either just finished visiting the TreeNode's right sub-tree or - if there is no right
					// sub-tree - just visited the current TreeNode
					return node.hasRightChild() ? at(tree, node.getRightChild()) : at(tree, nodeID);
				case TreeTraversal::DepthFirst_PostOrder:
					// State: just visited Root node
					return at(tree, nodeID);
				case TreeTraversal::DepthFirst_PreOrder:
					if (node.hasRightChild()) {
						// State: Just visited TreeNode's right sub-tree
						return at(tree, node.getRightChild());
					} else if (node.hasLeftChild()) {
						// State: Just visited TreeNode's left sub-tree
						return at(tree, node.getLeftChild());
					} else {
						// State: Just visited the TreeNode itself
						return at(tree, nodeID);
					}
			}
			HEDLEY_UNREACHABLE();
		}();

		core.increment();

		return core;
	}

	static auto afterRoot(tree_reference tree, const ConstExpression< Variable > &expr) -> ExpressionTreeIteratorCore {
		return afterRoot(tree, expr.nodeID());
	}

	ExpressionTreeIteratorCore(const ExpressionTreeIteratorCore &)     = default;
	ExpressionTreeIteratorCore(ExpressionTreeIteratorCore &&) noexcept = default;
	~ExpressionTreeIteratorCore()                                      = default;
	auto operator=(const ExpressionTreeIteratorCore &) -> ExpressionTreeIteratorCore & = default;
	auto operator=(ExpressionTreeIteratorCore &&) noexcept -> ExpressionTreeIteratorCore & = default;

	// Conversion constructor for converting from mutable to const core
	template< typename Core,
			  typename = std::enable_if_t<
				  isConst && std::is_same_v< Core, ExpressionTreeIteratorCore< Variable, false, iterationOrder > > > >
	ExpressionTreeIteratorCore(const Core &other)
		: m_tree(other.m_tree), m_currentID(other.m_currentID), m_previousID(other.m_previousID) {}

	/**
	 * This function will be called if the iterator constructed from this core will be dereferenced
	 */
	[[nodiscard]] auto dereference() const -> expression_type { return { m_currentID, toNode(m_currentID), *m_tree }; }

	/**
	 * This function will be called if the iterator constructed from this core is incremented
	 */
	void increment() { skipToNext(); }

	/**
	 * This function will be called if the iterator constructed from this core is compared to another one (equality of
	 * the cores determines equality of the iterators)
	 */
	[[nodiscard]] auto equals(const ExpressionTreeIteratorCore &other) const -> bool {
		return other.m_tree == m_tree && other.m_currentID == m_currentID && other.m_previousID == m_previousID;
	}


	friend auto operator<<(std::ostream &stream, const ExpressionTreeIteratorCore &core) -> std::ostream & {
		stream << "IteratorCore{";
		if (!core.m_currentID.isValid()) {
			// This is the end tag
			assert(!core.m_previousID.isValid());
			return stream << "}";
		}
		if (core.m_previousID == core.m_currentID) {
			stream << " >";
		}
		stream << " " << core.toNode(core.m_currentID);
		if (core.m_previousID == core.m_currentID) {
			stream << " <";
		} else if (core.m_previousID.isValid()) {
			stream << " <- " << core.toNode(core.m_previousID);
		}
		return stream << " }";
	}

private:
	tree_pointer m_tree;
	Numeric m_currentID;
	Numeric m_previousID;

	/**
	 * Ctor for internal use
	 */
	ExpressionTreeIteratorCore(tree_pointer tree, Numeric currentID, Numeric previousID)
		: m_tree(tree), m_currentID(currentID), m_previousID(previousID) {}

	[[nodiscard]] auto toNode(const Numeric &nodeID) -> node_type & {
		assert(nodeID < m_tree->m_nodes.size());
		return m_tree->m_nodes[nodeID];
	}

	[[nodiscard]] auto toNode(const Numeric &nodeID) const -> const node_type & {
		// NOLINTNEXTLINE(*-const-cast)
		return const_cast< ExpressionTreeIteratorCore * >(this)->toNode(nodeID);
	}

	/**
	 * Converts the TreeTraversal enum to the depth_first::Order enum
	 */
	static auto orderToDepthFirstOrder(TreeTraversal order) -> depth_first::Order {
		switch (order) {
			case TreeTraversal::DepthFirst_PostOrder:
				return depth_first::Order::Post;
			case TreeTraversal::DepthFirst_PreOrder:
				return depth_first::Order::Pre;
			case TreeTraversal::DepthFirst_InOrder:
				return depth_first::Order::In;
		}

		HEDLEY_UNREACHABLE();
	}

	/**
	 * Moves this iterator to the next element according to the chosen iteration order
	 */
	void skipToNext() {
		bool done = false;

		do {
			const TreeNode &currentNode = toNode(m_currentID);

			depth_first::TraversalStep step = depth_first::stepTraversal(currentNode, m_currentID, m_previousID,
																		 orderToDepthFirstOrder(iterationOrder));

			m_previousID = m_currentID;
			m_currentID  = step.nextNodeID;

			if (step.visitNextNode) {
				break;
			}
		} while (!done && m_currentID.isValid());

		if (!m_currentID.isValid()) {
			// Ensure that in this case, we equal the end iterator
			m_previousID.reset();
		}
	}

	// Make all ExpressionTreeIteratorCore instantiations friends to each other. This is required for our conversion
	// constructor to be able to access the internal fields.
	template< typename, bool, TreeTraversal > friend class ExpressionTreeIteratorCore;
};

template< typename Variable, bool isConst, TreeTraversal traversal >
auto operator==(const ExpressionTreeIteratorCore< Variable, isConst, traversal > &lhs,
				const ExpressionTreeIteratorCore< Variable, isConst, traversal > &rhs) {
	return lhs.equals(rhs);
}
template< typename Variable, bool isConst, TreeTraversal traversal >
auto operator!=(const ExpressionTreeIteratorCore< Variable, isConst, traversal > &lhs,
				const ExpressionTreeIteratorCore< Variable, isConst, traversal > &rhs) {
	return !lhs.equals(rhs);
}


// A few consistency checks that ensure above implementation is reasonable
static_assert(
	std::is_convertible_v< ExpressionTreeIteratorCore< int, false >, ExpressionTreeIteratorCore< int, true > >,
	"Iterator cores must be convertible from mutable to const");
static_assert(
	!std::is_convertible_v< ExpressionTreeIteratorCore< int, true >, ExpressionTreeIteratorCore< int, false > >,
	"Iterator cores must NOT be convertible from const to mutable");

} // namespace lizard::details


namespace iterators {

// Specialize is_semantically_const for our ConstExpression type in order to let the iterators library
// know that this type should be considered as a const type
template< typename Variable >
struct is_semantically_const< lizard::ConstExpression< Variable >, void > : std::true_type {};

} // namespace iterators
