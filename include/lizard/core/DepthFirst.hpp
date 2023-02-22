// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#pragma once

#include "lizard/core/Node.hpp"
#include "lizard/core/Numeric.hpp"

#include <utility>

namespace lizard::depth_first {

/**
 * The possible orders in which to perform a depth-first tree traversal
 */
enum class Order {
	Pre,
	Post,
	In,
};

/**
 * A struct representing a step in a depth-first traversal. It contains the ID of the next node that needs to be
 * processed in the current traversal. Furthermore, does it contain information on whether or not the respective
 * node should actually be visited instead of only processed to obtain the next node in the traversal chain.
 *
 * An invalid ID for the next node indicates that the end of the traversal has been reached.
 */
struct TraversalStep {
	Numeric nextNodeID;
	bool visitNextNode = false;
};

/**
 * Performs a single step in a depth-first traversal of the tree belonging to the given node.
 *
 * @param node The current node from which to start the traversal
 * @param currentID The current node's ID
 * @param previousID The ID of the node that has been visited before the current node. This may be Numeric::Invalid
 *  	to indicate that the traversal has just been started at the current node. It may also be the same as
 *  	currentID indicating that the last visited node was the node itself.
 * @param order The desired order of the traversal
 * @returns A TraversalStep object indicating the next node in the traversal
 */
auto stepTraversal(const Node &node, const Numeric &currentID, const Numeric &previousID, Order order) -> TraversalStep;

} // namespace lizard::depth_first
