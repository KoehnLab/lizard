// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/Krzmbrzl/lizard/blob/main/LICENSE>.

#pragma once

#include <iosfwd>

namespace lizard {

/**
 * An enum holding the possible ways how a given tree can be traversed
 */
enum class TreeTraversal {
	DepthFirst_PostOrder,
	DepthFirst_PreOrder,
	DepthFirst_InOrder,
};

auto operator<<(std::ostream &stream, const TreeTraversal &traversal) -> std::ostream &;

} // namespace lizard
