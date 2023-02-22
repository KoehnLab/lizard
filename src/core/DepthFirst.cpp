// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#include "lizard/core/DepthFirst.hpp"
#include "lizard/core/ExpressionCardinality.hpp"

#include <hedley.h>

namespace lizard::depth_first {

auto stepTraversalPostOrder(const Node &node, const Numeric &currentID, const Numeric &previousNodeID)
	-> TraversalStep {
	TraversalStep step;

	const ExpressionCardinality cardinality = node.getCardinality();
	const bool isBinary                     = cardinality == ExpressionCardinality::Binary;
	const bool isNullary                    = cardinality == ExpressionCardinality::Nullary;

	const bool cameFromParent     = node.getParent() == previousNodeID;
	const bool cameFromLeftChild  = !isNullary && node.getLeftChild() == previousNodeID;
	const bool cameFromRightChild = isBinary && node.getRightChild() == previousNodeID;

	if (cameFromParent) {
		step.nextNodeID    = isNullary ? currentID : node.getLeftChild();
		step.visitNextNode = isNullary;
	} else if (cameFromLeftChild) {
		step.nextNodeID    = isBinary ? node.getRightChild() : currentID;
		step.visitNextNode = !isBinary;
	} else if (cameFromRightChild) {
		step.nextNodeID    = currentID;
		step.visitNextNode = true;
	} else {
		// Coming from the node itself (happens after the node has been visited)
		step.nextNodeID = node.getParent();
	}

	return step;
}

auto stepTraversalPreOrder(const Node &node, const Numeric &currentID, const Numeric &previousNodeID) -> TraversalStep {
	TraversalStep step;

	const ExpressionCardinality cardinality = node.getCardinality();
	const bool isBinary                     = cardinality == ExpressionCardinality::Binary;
	const bool isNullary                    = cardinality == ExpressionCardinality::Nullary;

	const bool cameFromParent     = node.getParent() == previousNodeID;
	const bool cameFromLeftChild  = !isNullary && node.getLeftChild() == previousNodeID;
	const bool cameFromRightChild = isBinary && node.getRightChild() == previousNodeID;

	if (cameFromParent) {
		step.nextNodeID    = currentID;
		step.visitNextNode = true;
	} else if (cameFromLeftChild) {
		step.nextNodeID = isBinary ? node.getRightChild() : node.getParent();
	} else if (cameFromRightChild) {
		step.nextNodeID = node.getParent();
	} else {
		// Coming from the node itself (happens after the node has been visited)
		step.nextNodeID = isNullary ? node.getParent() : node.getLeftChild();
	}

	return step;
}

auto stepTraversalInOrder(const Node &node, const Numeric &currentID, const Numeric &previousNodeID) -> TraversalStep {
	TraversalStep step;

	const ExpressionCardinality cardinality = node.getCardinality();
	const bool isBinary                     = cardinality == ExpressionCardinality::Binary;
	const bool isNullary                    = cardinality == ExpressionCardinality::Nullary;

	const bool cameFromParent     = node.getParent() == previousNodeID;
	const bool cameFromLeftChild  = !isNullary && node.getLeftChild() == previousNodeID;
	const bool cameFromRightChild = isBinary && node.getRightChild() == previousNodeID;

	if (cameFromParent) {
		step.nextNodeID    = !isNullary ? node.getLeftChild() : currentID;
		step.visitNextNode = isNullary;
	} else if (cameFromLeftChild) {
		step.nextNodeID    = currentID;
		step.visitNextNode = true;
	} else if (cameFromRightChild) {
		step.nextNodeID = node.getParent();
	} else {
		// Coming from the node itself (happens after the node has been visited)
		step.nextNodeID = isBinary ? node.getRightChild() : node.getParent();
	}

	return step;
}


auto stepTraversal(const Node &node, const Numeric &currentID, const Numeric &previousID, Order order)
	-> TraversalStep {
	switch (order) {
		case Order::Post:
			return stepTraversalPostOrder(node, currentID, previousID);
		case Order::Pre:
			return stepTraversalPreOrder(node, currentID, previousID);
		case Order::In:
			return stepTraversalInOrder(node, currentID, previousID);
	}

	HEDLEY_UNREACHABLE();
}

} // namespace lizard::depth_first
