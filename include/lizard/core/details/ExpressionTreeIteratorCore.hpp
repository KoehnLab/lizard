// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/Krzmbrzl/lizard/blob/main/LICENSE>.

#pragma once

#include "lizard/core/DepthFirst.hpp"
#include "lizard/core/Expression_fwd.hpp"
#include "lizard/core/Node.hpp"
#include "lizard/core/Numeric.hpp"
#include "lizard/core/TreeTraversal.hpp"

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

	/**
	 * Creates an end() iterator - aka: an iterator that indicates the end of iteration
	 *
	 * @param tree The expression tree this iterator is supposed to be associated with
	 */
	static auto createEnd(tree_reference tree) { return ExpressionTreeIteratorCore(tree, Numeric{}); }

	/**
	 * @param tree The expression tree this iterator is supposed to be iterating / supposed to be associated with
	 * @param rootID The ID of the root node from where to start the iteration
	 */
	ExpressionTreeIteratorCore(tree_reference tree, Numeric rootID) : m_tree(&tree), m_currentID(std::move(rootID)) {
		if (m_currentID.isValid()) {
			skipToFirst();
		}
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
	[[nodiscard]] auto dereference() const -> expression_type {
		return { m_currentID, m_tree->m_nodes[m_currentID], *m_tree };
	}

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

private:
	tree_pointer m_tree;
	Numeric m_currentID;
	Numeric m_previousID;

	/**
	 * Ensures that this iterator will dereference to the correct element (depending on the chosen iteration order)
	 */
	void skipToFirst() {
		switch (iterationOrder) {
			case TreeTraversal::DepthFirst_PreOrder:
				// In these cases, we actually start visiting the root node before doing anything else
				m_previousID = m_currentID;
				break;
			case TreeTraversal::DepthFirst_PostOrder:
			case TreeTraversal::DepthFirst_InOrder:
				// In these cases, the root node is not the first node visited
				skipToNext();
				break;
		}
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
			const Node &currentNode = m_tree->m_nodes[m_currentID];

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


// A few consistency checks that ensure above implementation is reasobale
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
