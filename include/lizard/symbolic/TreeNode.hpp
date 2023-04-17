// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#pragma once

#include "lizard/core/ConstexprMath.hpp"
#include "lizard/core/MultiEnum.hpp"
#include "lizard/core/Numeric.hpp"
#include "lizard/symbolic/ExpressionCardinality.hpp"
#include "lizard/symbolic/ExpressionOperator.hpp"
#include "lizard/symbolic/ExpressionType.hpp"

#include <iosfwd>
#include <type_traits>

namespace lizard {

/*
 * This class represents a node in a binary expression tree. Most of this class's interface is not
 * intended for use by non-specialized code that is familiar with the inner workings of how Nodes
 * represent data.
 * For this reason, anyone else will probably find the provided functions and their arguments as
 * rather confusing.
 * If you have to do anything but constructing Nodes, you should look into the Expression class
 * instead
 *
 * @see Expression
 */
class TreeNode {
public:
	/**
	 * Unspecialized constructor. Intended for internal (and testing) use only. Its usage requires knowledge of
	 * the internal representation of a TreeNode's content!
	 */
	explicit TreeNode(ExpressionType type, Numeric left = {}, Numeric right = {}) noexcept;
	/**
	 * Constructor for TreeNodes representing a binary operator
	 */
	explicit TreeNode(ExpressionOperator operatorType) noexcept;
	/**
	 * Constructor for TreeNodes representing a literal value
	 */
	explicit TreeNode(std::make_signed_t< Numeric::numeric_type > numerator,
					  std::make_signed_t< Numeric::numeric_type > denominator = 1) noexcept;

	/**
	 * @returns The cardinality of the expression represented by this TreeNode
	 */
	[[nodiscard]] auto getCardinality() const -> ExpressionCardinality;

	/**
	 * @returns The type of the expression represented by this TreeNode
	 */
	[[nodiscard]] auto getType() const -> ExpressionType;

	/**
	 * @returns The represented operator
	 *
	 * Note: Calling this function on a TreeNode whose type is not Operator is undefined behavior
	 */
	[[nodiscard]] auto getOperator() const -> ExpressionOperator;

	/**
	 * @returns Whether this TreeNode has a parent TreeNode
	 */
	[[nodiscard]] auto hasParent() const -> bool;
	/**
	 * @returns The ID of the parent node
	 */
	[[nodiscard]] auto getParent() const -> Numeric;
	/**
	 * Reparents this TreeNode to the TreeNode with the given ID
	 */
	void setParent(Numeric parentID);

	/**
	 * @returns Whether this TreeNode has a (left) child node. If it only has a single child TreeNode, then
	 *  	that TreeNode is considered to be a left child TreeNode.
	 *
	 * Note: Calling this function on a TreeNode whose cardinality is Nullary is undefined behavior
	 */
	[[nodiscard]] auto hasLeftChild() const -> bool;
	/**
	 * @returns The ID of the left child TreeNode
	 *
	 * Note: Calling this function on a TreeNode whose cardinality is Nullary is undefined behavior
	 */
	[[nodiscard]] auto getLeftChild() const -> Numeric;
	/**
	 * Sets the left (or only) child of this TreeNode to the one with the given ID. Note that the
	 * referenced TreeNode will NOT automatically be reparented to this TreeNode!
	 *
	 * @param childID The ID of the new (left) child TreeNode
	 *
	 * Note: Calling this function on a TreeNode whose cardinality is Nullary is undefined behavior
	 */
	void setLeftChild(Numeric childID);

	/**
	 * @returns Whether this TreeNode has a right child TreeNode
	 *
	 * Note: Calling this function on a TreeNode whose cardinality is Nullary or Unary is undefined behavior
	 */
	[[nodiscard]] auto hasRightChild() const -> bool;
	/**
	 * @returns The ID of this TreeNode's right child
	 *
	 * Note: Calling this function on a TreeNode whose cardinality is Nullary or Unary is undefined behavior
	 */
	[[nodiscard]] auto getRightChild() const -> Numeric;
	/**
	 * Sets the right child of this TreeNode to the one with the given ID. Note that the
	 * referenced TreeNode will NOT automatically be reparented to this TreeNode!
	 *
	 * Note: Calling this function on a TreeNode whose cardinality is Nullary or Unary is undefined behavior
	 */
	void setRightChild(Numeric childID);

	friend auto operator==(const TreeNode &lhs, const TreeNode &rhs) -> bool;
	friend auto operator!=(const TreeNode &lhs, const TreeNode &rhs) -> bool;

private:
	MultiEnum< ExpressionType, ExpressionOperator > m_flags;
	Numeric m_parentID;
	Numeric m_left;
	Numeric m_right;
};

auto operator<<(std::ostream &stream, const TreeNode &node) -> std::ostream &;

} // namespace lizard
